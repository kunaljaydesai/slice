use crate::parser::{
    BinaryExpression, ConditionalScopeStatement, DefinitionScopeStatement, Function,
    FunctionCallExpression, FunctionDeclaration, IdentifierExpression, NumberLiteralExpression,
    Program, ReturnScopeStatement, Scope,
};

pub trait Walkable {
    fn accept<V: ASTVisitor>(&self, visitor: &mut V);
}

pub trait ASTVisitor {
    fn visit_program(&mut self, program: &Program);
    fn visit_function(&mut self, function: &Function);
    fn visit_function_declaration(&mut self, function_declaration: &FunctionDeclaration);
    fn visit_scope(&mut self, scope: &Scope);
    fn visit_conditional_statement(&mut self, conditional_statement: &ConditionalScopeStatement);
    fn visit_definition_statement(&mut self, definition_statement: &DefinitionScopeStatement);
    fn visit_return_statement(&mut self, return_statement: &ReturnScopeStatement);
    fn visit_binary_expression(&mut self, binary_expression: &BinaryExpression);
    fn visit_identifier(&mut self, identifier: &IdentifierExpression);
    fn visit_number_literal(&mut self, number_literal: &NumberLiteralExpression);
    fn visit_function_call(&mut self, function_call: &FunctionCallExpression);
}

impl Walkable for Program {
    fn accept<V: ASTVisitor>(&self, visitor: &mut V) {
        visitor.visit_program(self);
    }
}

impl Walkable for Function {
    fn accept<V: ASTVisitor>(&self, visitor: &mut V) {
        visitor.visit_function(self);
    }
}

impl Walkable for FunctionDeclaration {
    fn accept<V: ASTVisitor>(&self, visitor: &mut V) {
        visitor.visit_function_declaration(self);
    }
}

impl Walkable for Scope {
    fn accept<V: ASTVisitor>(&self, visitor: &mut V) {
        visitor.visit_scope(self);
    }
}

impl Walkable for ConditionalScopeStatement {
    fn accept<V: ASTVisitor>(&self, visitor: &mut V) {
        visitor.visit_conditional_statement(self);
    }
}

impl Walkable for DefinitionScopeStatement {
    fn accept<V: ASTVisitor>(&self, visitor: &mut V) {
        visitor.visit_definition_statement(self);
    }
}

impl Walkable for ReturnScopeStatement {
    fn accept<V: ASTVisitor>(&self, visitor: &mut V) {
        visitor.visit_return_statement(self);
    }
}

impl Walkable for BinaryExpression {
    fn accept<V: ASTVisitor>(&self, visitor: &mut V) {
        visitor.visit_binary_expression(self);
    }
}

impl Walkable for IdentifierExpression {
    fn accept<V: ASTVisitor>(&self, visitor: &mut V) {
        visitor.visit_identifier(self);
    }
}

impl Walkable for NumberLiteralExpression {
    fn accept<V: ASTVisitor>(&self, visitor: &mut V) {
        visitor.visit_number_literal(self);
    }
}

impl Walkable for FunctionCallExpression {
    fn accept<V: ASTVisitor>(&self, visitor: &mut V) {
        visitor.visit_function_call(self);
    }
}
