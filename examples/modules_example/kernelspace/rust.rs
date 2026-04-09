#![allow(missing_docs)]

use kernel::prelude::*;

module! {
    type: Hello,
    name: "hello_rust",
    authors: ["Vishal Moola"],
    description: "Example Rust kernel module",
    license: "GPL",
}

struct Hello;

impl kernel::Module for Hello {
    fn init(_module: &'static kernel::ThisModule) -> kernel::error::Result<Self> {
        pr_info!("Hello from Rust kernel module!\n");
        Ok(Self)
    }
}

impl Drop for Hello {
    fn drop(&mut self) {
        pr_info!("Goodbye world\n");
    }
}
