#![allow(missing_docs)]
#![allow(unused)]
#![allow(clippy::undocumented_unsafe_blocks)]
#![allow(clippy::ref_as_ptr)]
#![allow(clippy::ptr_as_ptr)]
#![allow(non_snake_case)]

use kernel::{
    bindings,
    prelude::*,
    pci,
    io::{
        register,
        Io,
    },
    module_pci_driver,
    pci_device_table,
    device,
    page::{
        Page,
    },
};

// Define a bunch of registers, similar to C includes
mod registers {
    kernel::register! {
        pub(super) ID(u32) @ 0x0 {
            31:24 Major;
            23:16 Minor;
        }
    }
}


const VENDOR : pci::Vendor = unsafe { core::mem::transmute::<u16, pci::Vendor>(0x1234) };
const DEVICE : u32 = 0x11e8;

module_pci_driver! {
    type: Edu,
    name: "Edu",
    authors: ["Vishal Moola"],
    description: "Edu Driver",
    license: "Dual MIT/GPL",
}

struct Edu;

pci_device_table!(
    PCI_TABLE,
    <Edu as pci::Driver>::IdInfo,
    [
        (
            pci::DeviceId::from_id(VENDOR, DEVICE),
            (),
        )
    ]
    );

struct EduData<'bound> {
    pdev: &'bound pci::Device,
    // Ensure the page exists for the life of the module
    dma_src: Page,
    dma_dst: Page,
}

impl pci::Driver for Edu {
    type IdInfo = ();
    type Data<'bound> = EduData<'bound>;
    const ID_TABLE : pci::IdTable<Self::IdInfo> = &PCI_TABLE;

    fn probe<'bound>(
        pdev: &'bound pci::Device<device::Core<'_>>,
        id_info: Option<&'bound Self::IdInfo>,
    ) -> impl PinInit<Self::Data<'bound>, Error> + 'bound {
        pr_info!("Hello World!");

        pdev.enable_device_mem();
        let bar = pdev.iomap_region_sized::<0x8>(0, c"educational")?;

        // We can call our bar functions, and the register! macro has
        // already mapped our specific bits into a way we can call too
        let major: usize = bar.read(registers::ID).Major().into();

        // Also important we specified the usize type above, otherwise the
        // compiler complains about into() being unable to assume the type.
        // Aka, the compiler does (eval -> usize).
        // The following however is ok because the compiler knows the exact
        // type into_raw() will leave us as.
        // Aka, the compiler does (eval -> intermediary -> cast to usize).
        let full = bar.read(registers::ID).into_raw() as usize;
        pr_info!("Major bytes: {:#x}, Full read: {:#x}", major, full);


        // DMA test
        let dma_src = Page::alloc_page(GFP_KERNEL)?;
        let dma_dst = Page::alloc_page(GFP_KERNEL)?;
        let message: usize = 777;
        // Assuming we've implemented "rust_helper_[fxn]", we can do this
        // to r/w from a page. page_address is not upstream right now.
        let va = unsafe { bindings::page_address(dma_src.as_ptr()) };

        // This cast works because we take the reference and cast that to
        // a rust pointer, then cast it to a C pointer. We copy the data
        // into the page + a message, then read it back and print that.
        unsafe {
            // Example of the 'silenced' warning way vs the nitty gritty
            core::ptr::copy((&full as *const usize).cast(), va, size_of::<usize>());
            core::ptr::copy(&message as *const usize
                as *const c_void, va.byte_add(size_of::<usize>()),
                size_of::<usize>());
        };
        let mut val: usize = 0;
        let mut data = core::mem::MaybeUninit::<usize>::uninit();
        unsafe {
            core::ptr::copy(va, &mut val as *mut usize
                as *mut c_void, size_of::<usize>());
            core::ptr::copy(va.byte_add(8), &mut data as *mut _ as *mut usize
                as *mut c_void, size_of::<usize>());
        };
        // Tell rust its been initialized, AND set the value. Otherwise
        // rust will assumes it isn't
            let data = unsafe {
                data.assume_init()
            };

        pr_info!("Hi! This is our major data, read from the backing page we stored it in {:#x}, and message: {:?}", val, data);

        Ok(EduData {
            pdev : pdev,
            dma_src: dma_src,
            dma_dst: dma_dst,
        } )
    }
}

fn test(
) {
}
