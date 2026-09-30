#![allow(missing_docs)]
#![allow(unused)]
#![allow(clippy::undocumented_unsafe_blocks)]

use kernel::{
    bindings,
    prelude::*,
    pci,
    module_pci_driver,
    pci_device_table,
    device,
};

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

        Ok(EduData { pdev : pdev } )
    }
}

fn test(
) {
}
