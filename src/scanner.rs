use std::f64;
use std::fmt;
use std::vec::Vec;

#[derive(PartialEq)]
pub enum TokenType {
    TokEof,
    // keywords
    TokDef,
    TokExtern,
    TokIf,
    TokElse,
    TokReturn,

    // variables
    TokIdentifier,

    // literals
    TokNumber,

    // misc
    TokRpar,
    TokLpar,
    TokRbrak,
    TokLbrak,
    TokComma,

    // assignment
    TokEquals,

    // comparators
    TokLt,
    TokGt,
    TokLte,
    TokGte,

    // operations
    TokAdd,
    TokSub,
    TokMul,
    TokDiv,
    TokMod,
}

pub struct Token {
    token_type: TokenType,
    identifier: String, // filled in for type `tok_identifier`
    number: f64,        // filled in for type `tok_number`
}

impl Token {
    pub fn new(token_type: TokenType, identifier: String, number: f64) -> Token {
        Token {
            token_type: token_type,
            identifier: identifier,
            number: number,
        }
    }
}

impl fmt::Display for Token {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        if self.token_type == TokenType::TokIdentifier {
            return write!(f, "{}", self.identifier);
        } else if self.token_type == TokenType::TokNumber {
            return write!(f, "{}", self.number.to_string());
        }

        let token_type_str = match self.token_type {
            TokenType::TokLt => "less_than",
            TokenType::TokGt => "greater_than",
            TokenType::TokLte => "less_than_equal",
            TokenType::TokGte => "greater_than_equal",
            TokenType::TokLbrak => "left_bracket",
            TokenType::TokRbrak => "right_bracket",
            TokenType::TokLpar => "left_parenthesis",
            TokenType::TokRpar => "right_parenthesis",
            TokenType::TokIf => "if",
            _ => "unknown",
        };
        write!(f, "{}", token_type_str)
    }
}

pub struct Scanner {
    source: String,
    idx: usize,
    tokens: Vec<Token>,
}

impl Scanner {
    pub fn new(source: String) -> Scanner {
        Scanner {
            source,
            idx: 0,
            tokens: vec![],
        }
    }

    pub fn scan_tokens(&mut self) {
        while !self.is_at_end() {
            let token = self.get_token();
            if token.is_some() {
                self.tokens.push(token.unwrap())
            }
        }
        self.tokens
            .push(Token::new(TokenType::TokEof, String::from(""), 0.0))
    }

    fn skip_spaces(&mut self) {
        while !self.is_at_end() && (self.is_space() || self.is_new_line()) {
            self.idx += 1
        }
    }

    pub fn get_tokens(self) -> Vec<Token> {
        return self.tokens;
    }

    fn get_token(&mut self) -> Option<Token> {
        self.skip_spaces();
        if self.is_at_end() {
            return None;
        }

        if self.get_char().is_alphabetic() {
            let mut identifier_str = String::from(self.get_char());
            self.idx += 1;
            while !self.is_at_end() && self.get_char().is_alphanumeric() {
                identifier_str.push(self.get_char());
                self.idx += 1;
            }

            return match identifier_str.as_str() {
                "def" => Some(Token::new(TokenType::TokDef, String::from(""), 0.0)),
                "extern" => Some(Token::new(TokenType::TokExtern, String::from(""), 0.0)),
                "if" => Some(Token::new(TokenType::TokIf, String::from(""), 0.0)),
                "else" => Some(Token::new(TokenType::TokElse, String::from(""), 0.0)),
                "return" => Some(Token::new(TokenType::TokReturn, String::from(""), 0.0)),
                _ => Some(Token::new(TokenType::TokIdentifier, identifier_str, 0.0)),
            };
        } else if self.get_char().is_numeric() {
            let mut number_str = String::from(self.get_char());
            self.idx += 1;
            while !self.is_at_end() && self.get_char().is_numeric() {
                number_str.push(self.get_char());
                self.idx += 1;
            }

            if self.get_char() == '.' {
                number_str.push(self.get_char());
                self.idx += 1;
                while !self.is_at_end() && self.get_char().is_numeric() {
                    number_str.push(self.get_char());
                    self.idx += 1;
                }
            }

            return Some(Token::new(
                TokenType::TokNumber,
                String::from(""),
                number_str.parse::<f64>().unwrap(),
            ));
        } else if self.get_char() == '#' {
            self.idx += 1;
            while !self.is_at_end() && !self.is_new_line() {
                self.idx += 1;
            }
            return self.get_token();
        } else if self.get_char() == '(' {
            self.idx += 1;
            return Some(Token::new(TokenType::TokLpar, String::from(""), 0.0));
        } else if self.get_char() == ')' {
            self.idx += 1;
            return Some(Token::new(TokenType::TokRpar, String::from(""), 0.0));
        } else if self.get_char() == '{' {
            self.idx += 1;
            return Some(Token::new(TokenType::TokLbrak, String::from(""), 0.0));
        } else if self.get_char() == '}' {
            self.idx += 1;
            return Some(Token::new(TokenType::TokRbrak, String::from(""), 0.0));
        } else if self.get_char() == '<' {
            self.idx += 1;
            if !self.is_at_end() && self.get_char() == '=' {
                self.idx += 1;
                return Some(Token::new(TokenType::TokLte, String::from(""), 0.0));
            }
            return Some(Token::new(TokenType::TokLt, String::from(""), 0.0));
        } else if self.get_char() == '>' {
            self.idx += 1;
            if !self.is_at_end() && self.get_char() == '=' {
                self.idx += 1;
                return Some(Token::new(TokenType::TokGte, String::from(""), 0.0));
            }
            return Some(Token::new(TokenType::TokGt, String::from(""), 0.0));
        } else if self.get_char() == '+' {
            self.idx += 1;
            return Some(Token::new(TokenType::TokAdd, String::from(""), 0.0));
        } else if self.get_char() == '-' {
            self.idx += 1;
            return Some(Token::new(TokenType::TokSub, String::from(""), 0.0));
        } else if self.get_char() == '*' {
            self.idx += 1;
            return Some(Token::new(TokenType::TokMul, String::from(""), 0.0));
        } else if self.get_char() == '/' {
            self.idx += 1;
            return Some(Token::new(TokenType::TokDiv, String::from(""), 0.0));
        } else if self.get_char() == '%' {
            self.idx += 1;
            return Some(Token::new(TokenType::TokMod, String::from(""), 0.0));
        } else if self.get_char() == ',' {
            self.idx += 1;
            return Some(Token::new(TokenType::TokComma, String::from(""), 0.0));
        } else if self.get_char() == '=' {
            self.idx += 1;
            return Some(Token::new(TokenType::TokEquals, String::from(""), 0.0));
        }
        panic!("Encountered an unhandled token type {}", self.get_char());
    }

    fn get_char(&self) -> char {
        self.source.chars().nth(self.idx).unwrap()
    }
    fn is_space(&self) -> bool {
        self.get_char() == ' '
    }
    fn is_at_end(&self) -> bool {
        self.idx >= self.source.len()
    }
    fn is_new_line(&self) -> bool {
        self.get_char() == 0xA as char
    }
}
