use crate::parser::{Expression, ScopeStatement};
use crate::visitor::{ASTVisitor, Walkable};

pub struct PrintAST {
    pub call_depth: usize,
}

impl PrintAST {
    fn get_padding(&self) -> usize {
        self.call_depth * 4
    }
}

impl ASTVisitor for PrintAST {
    fn visit_function_declaration(
        &mut self,
        function_declaration: &crate::parser::FunctionDeclaration,
    ) {
        println!(
            "{: >width$} {}",
            "",
            "Function Declaration",
            width = self.get_padding()
        );
        println!(
            "{: >width$} {}: {}",
            "",
            "Name",
            function_declaration.name,
            width = self.get_padding() + 4
        );
        println!(
            "{: >width$} {}: {:?}",
            "",
            "Args",
            function_declaration.args,
            width = self.get_padding() + 4
        );
    }

    fn visit_binary_expression(&mut self, binary_expression: &crate::parser::BinaryExpression) {
        println!(
            "{: >width$} {}",
            "",
            "BinaryExpression",
            width = self.get_padding()
        );
        self.call_depth += 1;
        match &*binary_expression.lhs {
            Expression::Binary(binary) => binary.accept(self),
            Expression::NumberLiteral(number_literal) => number_literal.accept(self),
            Expression::Identifier(identifier) => identifier.accept(self),
            Expression::FunctionCall(function_call) => function_call.accept(self),
        }

        match &*binary_expression.rhs {
            Expression::Binary(binary) => binary.accept(self),
            Expression::NumberLiteral(number_literal) => number_literal.accept(self),
            Expression::Identifier(identifier) => identifier.accept(self),
            Expression::FunctionCall(function_call) => function_call.accept(self),
        }
        self.call_depth -= 1;
    }

    fn visit_number_literal(&mut self, number_literal: &crate::parser::NumberLiteralExpression) {
        println!(
            "{: >width$} {}",
            "",
            "NumberLiteralExpr",
            width = self.get_padding()
        );
        println!(
            "{: >width$} {}: {}",
            "",
            "Value",
            number_literal.value,
            width = self.get_padding() + 4
        );
    }

    fn visit_identifier(&mut self, identifier: &crate::parser::IdentifierExpression) {
        println!(
            "{: >width$} {}",
            "",
            "IdentifierExpression",
            width = self.get_padding()
        );
        println!(
            "{: >width$} {}: {}",
            "",
            "Name",
            identifier.name,
            width = self.get_padding()
        );
    }

    fn visit_function_call(&mut self, function_call: &crate::parser::FunctionCallExpression) {
        todo!("implement printable for function call expr");
    }

    fn visit_definition_statement(
        &mut self,
        definition_statement: &crate::parser::DefinitionScopeStatement,
    ) {
        println!(
            "{: >width$} {}",
            "",
            "DefinitionScopeStatement",
            width = self.get_padding()
        );
        println!(
            "{: >width$} {}: {}",
            "",
            "Lvalue Identifier",
            definition_statement.lvalue_identifier,
            width = self.get_padding() + 4
        );
        self.call_depth += 1;
        match &definition_statement.rvalue_expression {
            Expression::Binary(binary) => binary.accept(self),
            Expression::NumberLiteral(number_literal) => number_literal.accept(self),
            Expression::Identifier(identifier) => identifier.accept(self),
            Expression::FunctionCall(function_call) => function_call.accept(self),
        }
        self.call_depth -= 1
    }

    fn visit_return_statement(&mut self, return_statement: &crate::parser::ReturnScopeStatement) {
        println!(
            "{: >width$} {}",
            "",
            "ReturnScopeStatement",
            width = self.get_padding()
        );
        self.call_depth += 1;
        match &return_statement.return_expr {
            Expression::Binary(binary) => binary.accept(self),
            Expression::NumberLiteral(number_literal) => number_literal.accept(self),
            Expression::Identifier(identifier) => identifier.accept(self),
            Expression::FunctionCall(function_call) => function_call.accept(self),
        }
        self.call_depth -= 1;
    }

    fn visit_conditional_statement(
        &mut self,
        conditional_statement: &crate::parser::ConditionalScopeStatement,
    ) {
        println!(
            "{: >width$} {}",
            "",
            "ConditionalScopeStatement",
            width = self.get_padding()
        );
        todo!("Implement conditional statement");
    }

    fn visit_scope(&mut self, scope: &crate::parser::Scope) {
        println!("{: >width$} {}", "", "Scope", width = self.get_padding());
        self.call_depth += 1;
        for block in scope.blocks.iter() {
            match block {
                ScopeStatement::Definition(definition) => definition.accept(self),
                ScopeStatement::Return(ret) => ret.accept(self),
                ScopeStatement::Conditional(conditional) => conditional.accept(self),
            }
        }
        self.call_depth -= 1;
    }

    fn visit_function(&mut self, function: &crate::parser::Function) {
        println!("{: >width$} {}", "", "Function", width = self.get_padding());
        self.call_depth += 1;
        function.declaration.accept(self);
        function.body.accept(self);
        self.call_depth -= 1;
    }

    fn visit_program(&mut self, program: &crate::parser::Program) {
        println!("{: >width$} {}", "", "Program", width = self.get_padding());
        self.call_depth += 1;
        for function in program.functions.iter() {
            function.accept(self);
        }
        self.call_depth -= 1
    }
}
