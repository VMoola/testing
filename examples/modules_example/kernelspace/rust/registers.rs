kernel::register! {
        pub(super) ID(u32) @ 0x0 {
            31:24 Major;
            23:16 Minor;
        }
        pub(super) FACTORIAL(u32) @0x08 {}
        pub(super) STATUS(u32) @0x20 {
            0:0 Compute;
        }
    }
