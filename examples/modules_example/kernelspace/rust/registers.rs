kernel::register! {
        base: kernel::io::Region<0xA0>;

        pub(super) ID(u32) @ 0x0 {
            31:24 Major;
            23:16 Minor;
        }
        pub(super) FACTORIAL(u32) @0x08 {}
        pub(super) STATUS(u32) @0x20 {
            0:0 Compute;
            7:7 Interrupt;
        }
        pub(super) ISR(u32) @0x24 {}
        pub(super) ISR_ACK(u32) @0x64 {}
        pub(super) DMA_SRC(u64) @ 0x80 {}
        pub(super) DMA_DST(u64) @ 0x88 {}
        pub(super) DMA_SIZE(u64) @ 0x90 {}
        pub(super) DMA_CMD(u64) @ 0x98 {
            0:0 Start;
            1:1 Direction;
            2:2 Interrupt;
        }
    }
