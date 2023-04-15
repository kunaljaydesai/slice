#include "visitor.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include <fstream>
#include <iostream>

class SymbolTableNode {
public:
  SymbolTableNode(const std::string &name, llvm::AllocaInst *alloca)
      : name_(name), alloca_(alloca) {}

  std::string getName() const { return name_; }
  llvm::AllocaInst *getAlloca() const { return alloca_; }

private:
  std::string name_;
  llvm::AllocaInst *alloca_;
};

class SymbolTable {
public:
  void insert(std::shared_ptr<SymbolTableNode> node) {
    table_[node->getName()] = node;
  }

  std::shared_ptr<SymbolTableNode> get(const std::string &name) {
    auto el = table_.find(name);
    return el->second;
  }

  void addChild(const std::string &scope_name,
                std::shared_ptr<SymbolTable> symbol_table) {
    children_[scope_name] = symbol_table;
  }

  void setParent(std::shared_ptr<SymbolTable> symbol_table) {
    parent_ = symbol_table;
  }

  std::shared_ptr<SymbolTable> getParentSymbolTable() const { return parent_; }

private:
  std::unordered_map<std::string, std::shared_ptr<SymbolTableNode>> table_;
  std::shared_ptr<SymbolTable> parent_;
  std::unordered_map<std::string, std::shared_ptr<SymbolTable>> children_;
};

class CodegenVisitor : public Visitor {
public:
  CodegenVisitor() {
    context_ = std::make_unique<llvm::LLVMContext>();
    module_ = std::make_unique<llvm::Module>("slice", *context_);
    builder_ = std::make_unique<llvm::IRBuilder<>>(*context_);
    root_symbol_table_ = std::make_shared<SymbolTable>();
    current_symbol_table_ = root_symbol_table_;
  }
  void visitBinaryExprNode(const BinaryExprNode *node) override;
  void visitNumberLiteralNode(const NumberLiteralNode *node) override;
  void visitIdentifierExprNode(const IdentifierExprNode *node) override;
  void visitFunctionCallExprNode(const FunctionCallExprNode *node) override;
  void visitBodyNode(const BodyNode *node) override;
  void visitConditionalNode(const ConditionalNode *node) override;
  void visitDefinitionNode(const DefinitionNode *node) override;
  void visitReturnNode(const ReturnNode *node) override;
  void
  visitFunctionDeclarationNode(const FunctionDeclarationNode *node) override;
  void visitFunctionNode(const FunctionNode *node) override;
  void visitProgramNode(const Program *node) override;

  void dump() { module_->print(llvm::outs(), nullptr); }

private:
  void setCurrentSymbolTable(std::shared_ptr<SymbolTable> symbol_table) {
    current_symbol_table_ = symbol_table;
  }
  void onEnterBlock(const std::string &block_name) {
    auto new_symbol_table = std::make_shared<SymbolTable>();
    current_symbol_table_->addChild(block_name, new_symbol_table);
    new_symbol_table->setParent(current_symbol_table_);

    current_symbol_table_ = new_symbol_table;
  }
  void onExitBlock() {
    current_symbol_table_ = current_symbol_table_->getParentSymbolTable();
  }

  std::unique_ptr<llvm::LLVMContext> context_;
  std::unique_ptr<llvm::Module> module_;
  std::unique_ptr<llvm::IRBuilder<>> builder_;
  llvm::Value *ret_;
  std::shared_ptr<SymbolTable> root_symbol_table_;
  std::shared_ptr<SymbolTable> current_symbol_table_;
};