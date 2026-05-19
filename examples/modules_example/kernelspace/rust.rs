#![allow(missing_docs)]

use kernel::prelude::*;
use kernel::page::Page;

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
        let _x = hi();
        let x: Result<u32, _> = Err(0);
        if x.is_ok() {
            // This path won't happen because we've hardset x to fail
            pr_info!("Its done! {:?} \n", x.unwrap());
        } else {
            // Using :? lets us skip the unwrap()
            // For debugging, we should always do this to dump the whole type
            pr_info!("{:?}\n", x);
        }
        Ok(Self)
    }
}

impl Drop for Hello {
    fn drop(&mut self) {
        pr_info!("Goodbye world\n");
    }
}

#[inline(never)]
fn hi() -> kernel::error::Result<u32>{
    let x = 10;
    let page = Page::alloc_page(GFP_KERNEL)?;

    //We should always print these using the .as_ptr() values
    pr_emerg!("This simply prints the address {:?}", page.as_ptr());
    //unsafe { kernel::bindings::dump_page(page.as_ptr(), "example".as_ptr()) };
    //unsafe { kernel::bindings::dump_stack() };
    //unsafe { kernel::bindings::WARN_ON(true) };
    drop(page);

    //The point of the Ok is to return the expected value of the function
    //If we fail earlier, Error populates and lets the caller handle it
    Ok(x)
}
