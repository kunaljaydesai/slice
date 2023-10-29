use crate::scanner::{Token, TokenType};
use std::vec::Vec;

trait Visitable {
    fn visit(tokens: &mut TokenStream) -> Self;
}

pub struct TokenStream {
    tokens: Vec<Token>,
}

impl TokenStream {
    pub fn new(tokens: Vec<Token>) -> TokenStream {
        TokenStream { tokens }
    }

    fn is_current_token_binary_operator(&self) -> bool {
        if let Some(current_token) = self.get_current_token() {
            return match current_token.token_type {
                TokenType::TokAdd => true,
                TokenType::TokSub => true,
                TokenType::TokMul => true,
                TokenType::TokDiv => true,
                TokenType::TokMod => true,
                TokenType::TokGt => true,
                TokenType::TokGte => true,
                TokenType::TokLt => true,
                TokenType::TokLte => true,
                _ => false,
            };
        } else {
            false
        }
    }

    fn peek_next_token(&self) -> Option<&Token> {
        if self.tokens.len() > 1 {
            return Some(&self.tokens[1]);
        }
        None
    }

    fn expected_current_token(&mut self, expected_token_type: TokenType) {
        if let Some(curr_token) = self.get_current_token() {
            // TODO: implement fmt::Display for TokenType
            assert!(
                curr_token.token_type == expected_token_type,
                "expected {}, actual {}",
                Token::new(expected_token_type, String::from(""), 0.0),
                curr_token
            );
        } else {
            panic!(
                "Expected token type {} but didn't find it",
                Token::new(expected_token_type, String::from(""), 0.0)
            );
        }
    }

    fn consume(&mut self) -> Option<Token> {
        // TODO: this is inefficient, swap it out with vecdeq
        if self.tokens.len() > 0 {
            Some(self.tokens.remove(0))
        } else {
            None
        }
    }

    fn get_current_token(&self) -> Option<&Token> {
        if self.tokens.len() == 0 {
            None
        } else {
            Some(&self.tokens[0])
        }
    }
}

pub struct FunctionDeclaration {
    pub name: String,
    pub args: Vec<String>,
}

impl Visitable for FunctionDeclaration {
    fn visit(tokens: &mut TokenStream) -> FunctionDeclaration {
        tokens.expected_current_token(TokenType::TokDef); // skipping 'def' token
        tokens.consume();

        tokens.expected_current_token(TokenType::TokIdentifier);
        let fn_name = tokens.consume();
        if fn_name.is_none() {
            panic!("Invalid token after start of function definition");
        }

        tokens.expected_current_token(TokenType::TokLpar);
        tokens.consume();

        // get function args
        let mut args: Vec<String> = vec![];
        while let Some(token) = tokens.consume() {
            if token.token_type == TokenType::TokRpar {
                break;
            }
            if token.token_type == TokenType::TokIdentifier {
                args.push(token.identifier);
                if let Some(current_token) = tokens.get_current_token() {
                    if current_token.token_type == TokenType::TokRpar {
                        break;
                    }
                    tokens.expected_current_token(TokenType::TokComma);
                    tokens.consume();
                } else {
                    panic!("Expected a token in function declaration argument list");
                }
            } else {
                panic!("Failed to parse function declaration {}", token);
            }
        }

        tokens.expected_current_token(TokenType::TokRpar);
        tokens.consume();
        FunctionDeclaration {
            name: fn_name.unwrap().identifier,
            args,
        }
    }
}

pub enum Expression {
    Binary(BinaryExpression),
    NumberLiteral(NumberLiteralExpression),
    Identifier(IdentifierExpression),
    FunctionCall(FunctionCallExpression),
}

pub struct BinaryExpression {
    pub bin_op: TokenType,
    pub lhs: Box<Expression>,
    pub rhs: Box<Expression>,
}

impl Visitable for BinaryExpression {
    fn visit(tokens: &mut TokenStream) -> Self {
        todo!("Implement binary expression");
    }
}

pub struct NumberLiteralExpression {
    pub value: f64,
}

impl Visitable for NumberLiteralExpression {
    fn visit(tokens: &mut TokenStream) -> Self {
        tokens.expected_current_token(TokenType::TokNumber);
        if let Some(token) = tokens.consume() {
            NumberLiteralExpression {
                value: token.number,
            }
        } else {
            panic!("Found invalid token type for number literal");
        }
    }
}

pub struct IdentifierExpression {
    pub name: String,
}

impl Visitable for IdentifierExpression {
    fn visit(tokens: &mut TokenStream) -> Self {
        tokens.expected_current_token(TokenType::TokIdentifier);
        if let Some(identifier_token) = tokens.consume() {
            IdentifierExpression {
                name: identifier_token.identifier,
            }
        } else {
            panic!("Expected token in identifier expression but didnt find one");
        }
    }
}

pub struct FunctionCallExpression {}

impl Visitable for FunctionCallExpression {
    fn visit(tokens: &mut TokenStream) -> Self {
        todo!("Implement number literal expression");
    }
}

impl Expression {
    fn handle_lhs(tokens: &mut TokenStream) -> Expression {
        if let Some(token) = tokens.get_current_token() {
            match token.token_type {
                TokenType::TokIdentifier => {
                    if let Some(identifier_token) = tokens.get_current_token() {
                        if let Some(next_token) = tokens.peek_next_token() {
                            // token after means it can be a variable or a function call
                            let is_fn_call = next_token.token_type == TokenType::TokLpar;
                            if is_fn_call {
                                return Expression::FunctionCall(FunctionCallExpression::visit(
                                    tokens,
                                ));
                            } else {
                                return Expression::Identifier(IdentifierExpression::visit(tokens));
                            }
                        } else {
                            // no token after the identifier means its just an identifier
                            // expression
                            return Expression::Identifier(IdentifierExpression::visit(tokens));
                        }
                    } else {
                        panic!("Expected identifier token but didn't find one");
                    }
                }
                TokenType::TokNumber => {
                    return Expression::NumberLiteral(NumberLiteralExpression::visit(tokens))
                }
                _ => panic!("Received unexpected token type in expression"),
            }
        } else {
            panic!("Expected token in expression but didn't find one");
        }
    }

    fn get_bin_op_precedence(token: &Token) -> i32 {
        match token.token_type {
            TokenType::TokAdd => 1000,
            TokenType::TokSub => 2000,
            TokenType::TokMul => 3000,
            TokenType::TokDiv => 4000,
            TokenType::TokMod => 5000,
            TokenType::TokGt => 6000,
            TokenType::TokGte => 7000,
            TokenType::TokLt => 8000,
            TokenType::TokLte => 9000,
            _ => -1,
        }
    }

    fn handle_bin_op_rhs(
        precedence: i32,
        tokens: &mut TokenStream,
        mut lhs: Expression,
    ) -> Expression {
        loop {
            if !tokens.is_current_token_binary_operator() {
                return lhs;
            }

            // dont consume unless we're sure this is a binary operator
            if let Some(bin_op) = tokens.consume() {
                let bin_op_precedence = Expression::get_bin_op_precedence(&bin_op);
                let mut rhs = Expression::handle_lhs(tokens);

                if let Some(next_bin_op) = tokens.get_current_token() {
                    if !tokens.is_current_token_binary_operator() {
                        return Expression::Binary(BinaryExpression {
                            bin_op: bin_op.token_type,
                            rhs: Box::new(rhs),
                            lhs: Box::new(lhs),
                        });
                    }

                    let next_bin_op_precedence = Expression::get_bin_op_precedence(&next_bin_op);
                    if bin_op_precedence < next_bin_op_precedence {
                        rhs =
                            Expression::handle_bin_op_rhs(next_bin_op_precedence + 1, tokens, rhs);
                    }
                    lhs = Expression::Binary(BinaryExpression {
                        bin_op: bin_op.token_type,
                        lhs: Box::new(lhs),
                        rhs: Box::new(rhs),
                    });
                } else {
                    return Expression::Binary(BinaryExpression {
                        bin_op: bin_op.token_type,
                        lhs: Box::new(lhs),
                        rhs: Box::new(rhs),
                    });
                }
            } else {
                return lhs;
            }
        }
    }
}

impl Visitable for Expression {
    fn visit(tokens: &mut TokenStream) -> Expression {
        let lhs = Expression::handle_lhs(tokens);
        Expression::handle_bin_op_rhs(0, tokens, lhs)
    }
}

pub enum ScopeStatement {
    Definition(DefinitionScopeStatement),
    Conditional(ConditionalScopeStatement),
    Return(ReturnScopeStatement),
}
pub struct DefinitionScopeStatement {
    pub lvalue_identifier: String,
    pub rvalue_expression: Expression,
}
pub struct ReturnScopeStatement {
    pub return_expr: Expression,
}
pub struct ConditionalScopeStatement {}

impl Visitable for DefinitionScopeStatement {
    fn visit(tokens: &mut TokenStream) -> DefinitionScopeStatement {
        if let Some(lvalue_token) = tokens.consume() {
            tokens.expected_current_token(TokenType::TokEquals);
            tokens.consume();
            DefinitionScopeStatement {
                lvalue_identifier: lvalue_token.identifier,
                rvalue_expression: Expression::visit(tokens),
            }
        } else {
            panic!("Expected a token in definition but didn't find one");
        }
    }
}

impl Visitable for ReturnScopeStatement {
    fn visit(tokens: &mut TokenStream) -> ReturnScopeStatement {
        tokens.expected_current_token(TokenType::TokReturn);
        tokens.consume();
        let return_expr = Expression::visit(tokens);
        ReturnScopeStatement { return_expr }
    }
}

impl Visitable for ConditionalScopeStatement {
    fn visit(tokens: &mut TokenStream) -> ConditionalScopeStatement {
        todo!("Need to implement conditional statement");
        tokens.expected_current_token(TokenType::TokIf);
        ConditionalScopeStatement {}
    }
}

pub struct Scope {
    pub blocks: Vec<ScopeStatement>,
}

impl Visitable for Scope {
    fn visit(tokens: &mut TokenStream) -> Scope {
        tokens.expected_current_token(TokenType::TokLbrak);
        tokens.consume();

        let mut blocks: Vec<ScopeStatement> = Vec::new();
        while let Some(token) = tokens.get_current_token() {
            match token.token_type {
                TokenType::TokIf => blocks.push(ScopeStatement::Conditional(
                    ConditionalScopeStatement::visit(tokens),
                )),
                TokenType::TokReturn => {
                    blocks.push(ScopeStatement::Return(ReturnScopeStatement::visit(tokens)))
                }
                TokenType::TokIdentifier => blocks.push(ScopeStatement::Definition(
                    DefinitionScopeStatement::visit(tokens),
                )),
                TokenType::TokRbrak => break,
                _ => panic!("Found unexpected token in scope"),
            }
        }
        tokens.expected_current_token(TokenType::TokRbrak);
        tokens.consume();
        Scope { blocks }
    }
}

pub struct Function {
    pub declaration: FunctionDeclaration,
    pub body: Scope,
}

impl Visitable for Function {
    fn visit(tokens: &mut TokenStream) -> Function {
        let declaration = FunctionDeclaration::visit(tokens);
        let body = Scope::visit(tokens);
        Function { body, declaration }
    }
}

pub struct Program {
    pub functions: Vec<Function>,
}

pub struct Parser {
    tokens: TokenStream,
}

impl Parser {
    pub fn new(tokens: Vec<Token>) -> Parser {
        Parser {
            tokens: TokenStream::new(tokens),
        }
    }

    pub fn parse(&mut self) -> Program {
        let mut functions = vec![];
        while let Some(token) = self.tokens.get_current_token() {
            match token.token_type {
                TokenType::TokEof => return Program { functions },
                TokenType::TokDef => {
                    let function = Function::visit(&mut self.tokens);
                    functions.push(function)
                }
                _ => panic!("Invalid token found"),
            }
        }
        return Program { functions };
    }
}
