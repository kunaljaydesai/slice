#include "codegen.h"
#include "parser.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/ValueSymbolTable.h"
#include <iostream>
#include <map>

static std::unordered_map<std::string, llvm::AllocaInst *> NamedValues;

void CodegenVisitor::visitProgramNode(const Program *node) {
  for (const auto &function : node->getFunctions()) {
    function->accept(this);
  }
}

void CodegenVisitor::visitFunctionNode(const FunctionNode *node) {
  node->getFunctionDeclaration()->accept(this);
  node->getBody()->accept(this);
}

void CodegenVisitor::visitFunctionDeclarationNode(
    const FunctionDeclarationNode *node) {

  std::vector<llvm::Type *> doubles(node->getArgs().size(),
                                    llvm::Type::getDoubleTy(*context_));
  llvm::FunctionType *function_type = llvm::FunctionType::get(
      llvm::Type::getDoubleTy(*context_), doubles, false);
  llvm::Function *function =
      llvm::Function::Create(function_type, llvm::Function::ExternalLinkage,
                             node->getName(), module_.get());
  size_t i = 0;
  for (auto &arg : function->args()) {
    arg.setName(node->getArgs()[i++]);
  }
  auto entry = llvm::BasicBlock::Create(*context_, "entry", function);
  builder_->SetInsertPoint(entry);
}

void CodegenVisitor::visitBodyNode(const BodyNode *node) {
  for (const auto &block : node->getBlocks()) {
    block->accept(this);
  }
}

void CodegenVisitor::visitBinaryExprNode(const BinaryExprNode *node) {
  node->getLHS()->accept(this);
  auto lhs = ret_;
  node->getRHS()->accept(this);
  auto rhs = ret_;
  switch (node->getOperator()) {
  case tok_add: {
    ret_ = builder_->CreateFAdd(lhs, rhs, "addtmp");
    break;
  }
  case tok_sub: {
    ret_ = builder_->CreateFSub(lhs, rhs, "subtmp");
  }
  case tok_mul: {
    ret_ = builder_->CreateFMul(lhs, rhs, "multmp");
  }
  case tok_div: {
    ret_ = builder_->CreateFDiv(lhs, rhs, "divtmp");
  }
  }
}
void CodegenVisitor::visitNumberLiteralNode(const NumberLiteralNode *node) {
  llvm::Value *literal =
      llvm::ConstantFP::get(*context_, llvm::APFloat(node->getValue()));
  ret_ = literal;
}
void CodegenVisitor::visitIdentifierExprNode(const IdentifierExprNode *node) {
  const auto val = NamedValues[node->getName()];
  if (!val) {
    std::cout << "did not find identifier" << std::endl;
    exit(1);
  }
  ret_ = builder_->CreateLoad(val->getAllocatedType(), val, node->getName());
}

void CodegenVisitor::visitFunctionCallExprNode(
    const FunctionCallExprNode *node) {}
void CodegenVisitor::visitConditionalNode(const ConditionalNode *node) {}
void CodegenVisitor::visitDefinitionNode(const DefinitionNode *node) {
  node->getRHS()->accept(this);
  const auto &var_name = node->getLValue();
  auto alloca = builder_->CreateAlloca(llvm::Type::getDoubleTy(*context_),
                                       nullptr, var_name);
  NamedValues[var_name] = alloca;
  builder_->CreateStore(ret_, alloca);
}

void CodegenVisitor::visitReturnNode(const ReturnNode *node) {
  node->getExpr()->accept(this);
  builder_->CreateRet(ret_);
}