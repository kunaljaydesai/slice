use std::env;

fn main() {
    let args: Vec<String>  = env::args().collect();

    // first arg is assumed to be filename
    if args.len() <= 1 {
        panic!("Did not specify a file arg")
    }
    let filename = &args[1];


}
