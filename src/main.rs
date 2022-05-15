use std::{env, process};

mod engine;

fn main()
{
    let args: Vec<String> = env::args().collect();

    if args.len() < 2
    {
        println!("ArgumentError: {} <filename>", args[0]);
        process::exit(1);
    }

    let filename = &args[1];

    engine::exec(filename);

    process::exit(0);
}
