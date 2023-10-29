mod parser;
mod scanner;

use parser::{Parser, Printable, Program};
use scanner::Scanner;
use std::env;
use std::fs;

fn main() {
    let args: Vec<String> = env::args().collect();

    // first arg is assumed to be file path
    if args.len() <= 1 {
        panic!("Did not specify a file arg")
    }
    let file_path = &args[1];

    let source = fs::read_to_string(file_path).expect("error trying to read source file");
    let mut scanner = Scanner::new(source);
    scanner.scan_tokens();

    let mut parser = Parser::new(scanner.get_tokens());
    let program: Program = parser.parse();
    program.debug(0);
}
