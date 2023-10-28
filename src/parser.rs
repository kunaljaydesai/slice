use crate::scanner::{Token, TokenType};
use std::vec::Vec;

struct Function {}

impl Function {
    pub fn new() -> Function {
        Function {}
    }
}

pub struct Program {
    functions: Vec<Function>,
}

pub struct Parser {
    idx: usize,
    tokens: Vec<Token>,
}

impl Program {
    pub fn new(functions: Vec<Function>) -> Program {
        Program { functions }
    }
}

impl Parser {
    pub fn new(tokens: Vec<Token>) -> Parser {
        Parser {
            tokens: tokens,
            idx: 0,
        }
    }

    pub fn parse(&mut self) -> Program {
        let mut functions = vec![];
        while let Some(token) = self.get_current_token() {
            match token.token_type {
                TokenType::TokEof => return Program::new(functions),
                TokenType::TokDef => functions.push(Function::new()),
                _ => panic!("Invalid token found"),
            }
        }
        return Program::new(functions);
    }

    fn get_current_token(&self) -> Option<&Token> {
        if self.idx >= self.tokens.len() {
            None
        } else {
            Some(&self.tokens[self.idx])
        }
    }
}
