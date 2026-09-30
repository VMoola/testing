#![allow(missing_docs)]
#![allow(unused)]
#![allow(clippy::undocumented_unsafe_blocks)]

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
        Ok(EduData { pdev : pdev } )
    }
}

fn test(
) {
}
