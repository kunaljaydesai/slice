use crate::scanner::Token;
use std::vec::Vec;

pub struct Parser {
    idx: usize,
    tokens: Vec<Token>,
}

impl Parser {
    pub fn new(tokens: Vec<Token>) -> Parser {
        Parser {
            tokens: tokens,
            idx: 0,
        }
    }
}
