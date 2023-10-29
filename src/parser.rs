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
        println!("Parsed token {}", self.tokens[0]);
        // TODO: this is inefficient, swap it out with vecdeq
        if self.tokens.len() > 0 {
            Some(self.tokens.remove(0))
        } else {
            None
        }
    }

    fn remove_current_token(&mut self) -> Option<Token> {
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
    name: String,
    args: Vec<String>,
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

pub struct BinaryExpression {}
impl Visitable for BinaryExpression {
    fn visit(tokens: &mut TokenStream) -> Self {
        todo!("Implement binary expression");
    }
}

pub struct NumberLiteralExpression {}
impl Visitable for NumberLiteralExpression {
    fn visit(tokens: &mut TokenStream) -> Self {
        todo!("Implement number literal expression");
    }
}

pub struct IdentifierExpression {
    name: String,
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
    fn handleLHS(tokens: &mut TokenStream) -> Expression {
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
                _ => panic!("Received unexpected token type in expression"),
            }
        } else {
            panic!("Expected token in expression but didn't find one");
        }
    }

    fn handleBinOpRHS(tokens: &mut TokenStream, lhs: Expression) -> Expression {
        todo!("handleBinOpRHS");
    }
}

impl Visitable for Expression {
    fn visit(tokens: &mut TokenStream) -> Expression {
        let lhs = Expression::handleLHS(tokens);
        Expression::handleBinOpRHS(tokens, lhs)
    }
}

enum ScopeStatement {
    Definition(DefinitionScopeStatement),
    Conditional(ConditionalScopeStatement),
    Return(ReturnScopeStatement),
}
pub struct DefinitionScopeStatement {
    lvalue_identifier: String,
    rvalue_expression: Expression,
}
pub struct ReturnScopeStatement {}
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
        todo!("Need to implement return statement");
        ReturnScopeStatement {}
    }
}

impl Visitable for ConditionalScopeStatement {
    fn visit(tokens: &mut TokenStream) -> ConditionalScopeStatement {
        todo!("Need to implement conditional statement");
        tokens.expected_current_token(TokenType::TokIf);
        ConditionalScopeStatement {}
    }
}

pub struct Scope {}
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
        Scope {}
    }
}

pub struct Function {
    declaration: FunctionDeclaration,
    body: Scope,
}

impl Visitable for Function {
    fn visit(tokens: &mut TokenStream) -> Function {
        let declaration = FunctionDeclaration::visit(tokens);
        let body = Scope::visit(tokens);
        Function { body, declaration }
    }
}

pub struct Program {
    functions: Vec<Function>,
}

impl Program {
    pub fn new(functions: Vec<Function>) -> Program {
        Program { functions }
    }
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
                TokenType::TokEof => return Program::new(functions),
                TokenType::TokDef => {
                    let function = Function::visit(&mut self.tokens);
                    functions.push(function)
                }
                _ => panic!("Invalid token found"),
            }
        }
        return Program::new(functions);
    }
}
