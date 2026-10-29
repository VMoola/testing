#![allow(missing_docs)]
#![allow(unused)]
#![allow(clippy::undocumented_unsafe_blocks)]
#![allow(clippy::redundant_field_names)]
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
    irq,
    drm,
};

// Define a bunch of registers, similar to C includes
mod registers;


const VENDOR : pci::Vendor = unsafe { core::mem::transmute::<u16, pci::Vendor>(0x1234) };
const DEVICE : u32 = 0x11e8;
const BAR_SIZE : usize = 0xA0;

// define a type for our specific bar so the compiler has guarantees
type Bar<'bound> = pci::Bar<'bound, BAR_SIZE>;

// The important part here is this structure's address mut not move
// after handing it to drm via drm_dev_alloc()
const DRV: bindings::drm_driver = bindings::drm_driver {
    name: c"mydrm".as_ptr().cast_mut().cast(),
    desc: c"mydrm".as_ptr().cast_mut().cast(),
    fops: &FOPS,
    // The default path is /dev/dri/cardN. Lets use ACCEL if we have it
    #[cfg(CONFIG_DRM_ACCEL)]
    driver_features: bindings::drm_driver_feature_DRIVER_COMPUTE_ACCEL,
    // Tell rust to make the rest of the fields 0
    ..unsafe { core::mem::zeroed() }
};

const FOPS: bindings::file_operations = bindings::file_operations {
    open: Some(open),
    release: Some(close),
    mmap: Some(mmap),
    ..unsafe { core::mem::zeroed() }
};

// The proper rust way
const DR: drm::DriverInfo = drm::DriverInfo {
    major: 0,
    minor: 0,
    patchlevel: 0,
    name: c"Edu Drm Driver registration",
    desc: c"Educational example driver",
};

// The following is required by the compiler for drm::Driver (impl AllocImpl)
struct EduObject;

impl drm::gem::DriverObject for EduObject {
    type Driver = EduDriverData;
    type Args = ();

    fn new(
        _dev: &drm::Device<Self::Driver>,
        _size: usize,
        _args: Self::Args,
    ) -> impl PinInit<Self, Error> {
        Ok::<_, Error>(EduObject)
    }
}

// We need a different fileops for rust, adhering to the trait
impl drm::file::DriverFile for EduClient<'_> {
    type Driver = EduDriverData;

    fn open(_dev: &drm::Device<Self::Driver>) -> Result<Pin<KBox<Self>>> {
        pr_info!("You have opened Rust's file descriptor registration\n");
        let data = Page::alloc_page(GFP_KERNEL)?;
        let message: u64 = 777;

        //unsafe { data.write_raw(&message as *const u64 as *const u8, 0, 8) };
        Ok(KBox::pin(EduClient { data, _bound:core::marker::PhantomData }, GFP_KERNEL)?)
    }
}

struct EduDriverData;

#[vtable]
impl drm::Driver for EduDriverData {
    type Data = ();
    type RegistrationData<'a> = ();
    type Object = drm::gem::Object<EduObject>;
    type ParentDevice<Ctx: device::DeviceContext> = pci::Device<Ctx>;
    type File = EduClient<'static>;

    const IOCTLS: &'static [drm::ioctl::DrmIoctlDescriptor] = &[];
    const INFO: drm::DriverInfo = DR;
    const FEAT_ACCEL: bool = true;
}

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

struct EduIrq<'bound> {
    bar: Bar<'bound>,
    pad: usize,
    // So these values don't get dropped
    dma_dst: Page,
    fxn: usize,
}

struct EduData<'bound> {
    pdev: &'bound pci::Device,
    _reg: Pin<KBox<drm::Registration<'bound, EduDriverData>>>,
    // Ensure the page exists for the life of the module
    handler: Pin<KBox<irq::Registration<'bound, EduIrq<'bound>>>>,
    // vec dropped after the handler
    vec: KBox<pci::IrqVectorRegistration<'bound>>,
    dma_src: Page,
    // Store device as a usize to avoid compiler
    // We could get around this 'cleaner' by implementing Send/Sync instead
    device: usize,
}

// Distinctly different lifetime, this is per-open
struct EduClient<'client> {
    data: Page,
    // PhantomData ensures we can bound the lifetime
    _bound: core::marker::PhantomData<&'client ()>,
}

impl<'bound> EduData<'bound> {
    // bar MUST be a reference, otherwise it takes ownership (no Copy)
    // Taking ownership means our probe loses it
    fn read_full(
        bar: &Bar<'bound>,
    ) -> usize {
        let major = Self::read_major(bar);

        // Also important we specified the usize type above, otherwise the
        // compiler complains about into() being unable to assume the type.
        // Aka, the compiler does (eval -> usize).
        // The following however is ok because the compiler knows the exact
        // type into_raw() will leave us as.
        // Aka, the compiler does (eval -> intermediary -> cast to usize).
        let full = bar.read(registers::ID).into_raw() as usize;
        pr_info!("Major bytes: {:#x}, Full read: {:#x}", major, full);

        full
    }
    fn read_major(
        bar: &Bar<'bound>,
    ) -> usize {
        // We can call our bar functions, and the register! macro has
        // already mapped our specific bits into a way we can call too
        let major: usize = bar.read(registers::ID).Major().into();

        major
    }

    fn queue_factorial(
        bar: &Bar<'bound>,
        n: u32,
    ) {
      // The value to compute - we must convert our input to the register's
        // own type (i.e. FACTORIAL). We MUST read first, because we always
        // write the whole register

        let status = bar.read(registers::STATUS);
        bar.write_reg(status.with_Interrupt(true));
        bar.write_reg(registers::FACTORIAL::from(n));
    }

    fn poll_factorial(
        bar: &Bar<'bound>,
    ) {
        let mut done = true;
        while done {
            //let val = bar.read(registers::ISR);
            //pr_info!("{:?}", val.into_raw() as usize);
            done = bar.read(registers::STATUS).Compute().into();
        }
    }

    fn poll_dma(
        bar: &Bar<'bound>,
    ) -> Result {
        let mut done = true;
        while done {
            //let val = bar.read(registers::ISR);
            //pr_info!("{:?}", val.into_raw() as usize);
            done = bar.try_read(registers::DMA_CMD)?.Start().into();
        }
        Ok(())
    }
}

impl EduIrq<'_> {
    // Must be a C function because the C API we call takes that type
    unsafe extern "C" fn factorial_thread(data: *mut c_void) -> c_int {
        pr_info!("Factorial deferred print!\n");
        0
    }
}

// See the irq crate for type implementation details
impl irq::Handler for EduIrq<'_> {
    fn handle(&self) -> irq::IrqReturn {
        let data = self.bar.read(registers::ISR).into_raw();

        // No cleaner way since rust doesn't have a bit checker
        if data & 1 != 0 {
            // Factorial
            pr_info!("Factorial Caught!\n");
            // This read should really be done in the bottom half...
            let message: usize = self.bar.read(registers::FACTORIAL).into_raw() as usize;
            pr_info!("Output value {:?}\n", message);

            // Cast back. Inside our factorial since the kthread will complete
            // so future wakeups will do nothing
            unsafe { bindings::wake_up_process(self.fxn as *mut bindings::task_struct) };
        }

        if data & 256 != 0 {
            // DMA
            pr_info!("DMA Caught!\n");

            let va = unsafe { bindings::page_address(self.dma_dst.as_ptr()) };
            let mut val: usize = 0;
            unsafe {
                core::ptr::copy(va, &mut val as *mut usize
                    as *mut c_void, size_of::<usize>());
            };

            pr_info!("Major data: {:#x}, read from the dma destination page with physical address {:#x}\n", val, self.pad as u64);
        }

        // Handled both types already
        self.bar.write_reg(registers::ISR_ACK::from_raw(data));
        irq::IrqReturn::Handled
    }
}

fn init_pdev<'bound>(
    pdev: &'bound pci::Device<device::Core<'_>>,
) {
        pdev.enable_device_mem();
        pdev.set_master();
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

        init_pdev(pdev);
        // This will fail (0 byte mask). This showcases how we can
        // handle an error and continue the function execution.
        let mask = kernel::dma::DmaMask::new::<0>();
        unsafe { kernel::dma::Device::dma_set_mask_and_coherent(pdev,mask).unwrap_or_else(
            |e|{pr_info!("Expected failure: error {:?}\n", e)})
        };
        let bar = pdev.iomap_region_sized::<BAR_SIZE>(0, c"educational")?;
        unsafe { BAR_START = (*(pdev as *const _ as *const bindings::pci_dev)).resource[0].start};

        let task = unsafe {
            bindings::kthread_create_on_node(Some(EduIrq::factorial_thread), core::ptr::null_mut(), bindings::NUMA_NO_NODE, c"hello".as_ptr() as *const u8)
        };
        // DMA test - this needs to happen early so rust can organize the memory
        // Depending on how many bits the device supports, we allocate
        // from a region:
        // DMA : 0-16Mb | 24 bits
        // DMA32 : 16Mb-4Gb | 32 bits
        //
        // With QEMU, use -device edu,dma_mask=0xffffffffffffffff
        // gives us all 64-bits of access. For arm64 we need a mask
        // because it has a 1GB floor. x86_64 starts at 0.
        let dma_src = Page::alloc_page(GFP_KERNEL)?;
        let dma_dst = Page::alloc_page(GFP_KERNEL)?;
        let pad = unsafe { bindings::page_to_phys(dma_dst.as_ptr()) } as usize;

        // Setup the interrupts. This must happen before the device does work because
        // MSI will only send on the completion.
        // Allocate the vectors, default is empty, all is everything
        // Rust has better ways to actually use these, but this is the nitty
        // gritty dirty implementation

        let vec_src = KBox::new(pdev.alloc_irq_vectors(1, 1, pci::IrqTypes::default() | (pci::IrqType::Msi))?, GFP_KERNEL)?;
        let vec: &'bound pci::IrqVectorRegistration<'bound> = unsafe { &*(&*vec_src as *const _) };
        // Attach a handler to the vector
        let reg = unsafe { irq::Registration::new(vec.index(0)?.into(), irq::Flags::TRIGGER_NONE,
                c"edu", Ok::<_, Error>(EduIrq { bar, pad, dma_dst, fxn: task as usize })) };
        let handler = KBox::pin_init(reg, GFP_KERNEL)?;

        // Bar and dma_dst have been moved to EduIrq, so we need to get it again
        // The handler method fetchs the handler associated with a registration
        let bar = &handler.handler().bar;
        unsafe { BAR = bar as *const _ as usize };
        let dma_dst = &handler.handler().dma_dst;

        // Because we want the function to tied to an EduData object
        let full = EduData::read_full(&bar);

        let fac = 10;
        EduData::queue_factorial(&bar, fac);
        //EduData::poll_factorial(&bar);
        // Commenting the above out makes the read racy, but its unused

        let message: usize = bar.read(registers::FACTORIAL).into_raw() as usize;
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

        pr_info!("Hi! This is our major data, read from the backing page we stored it in {:#x}, and message: {:?}\n", val, data);

        let pas = unsafe { bindings::page_to_phys(dma_src.as_ptr()) };
        let dma_addr = 0x40000; // Device address in its physical map

        // Must use try_* variant b/c a pci BAR is only guaranteed 32-bit
        // Note no '?'s here, we want to continue if an error occurs. This
        // variant DOES however return a Result that we don't use.
        // from_raw() lets us write exact values
        bar.try_write_reg(registers::DMA_SRC::from_raw(pas as u64));
        bar.try_write_reg(registers::DMA_DST::from_raw(dma_addr));
        bar.try_write_reg(registers::DMA_SIZE::from(8));
        bar.try_write_reg(registers::DMA_CMD::from_raw(0).with_Start(true).with_Direction(false).with_Interrupt(false));

        // Poll until start bit is cleared
        // We don't actually care about the error value yet
        let _ = EduData::poll_dma(&bar);

        // Now take that data into our destination page and check it
        bar.try_write_reg(registers::DMA_SRC::from_raw(dma_addr));
        bar.try_write_reg(registers::DMA_DST::from_raw(pad as u64));
        bar.try_write_reg(registers::DMA_SIZE::from(8));
        bar.try_write_reg(registers::DMA_CMD::from_raw(0).with_Start(true).with_Direction(true).with_Interrupt(true));

        // Allocate and register the device
        let parent = pdev.as_ref() as *const _ as *mut bindings::device;
        let device = unsafe { bindings::drm_dev_alloc(&DRV, parent) };
        unsafe { bindings::drm_dev_register(device, 0) };

        let drm = drm::UnregisteredDevice::<EduDriverData>::new(pdev, Ok::<_, Error>(()))?;
        let reg = unsafe { drm::Registration::new(pdev.as_ref(), drm, Ok::<_, Error>(()))};
        let reg = KBox::pin_init(reg, GFP_KERNEL)?;

        pr_info!("Done with probe!\n");
        Ok(EduData {
            pdev : pdev,
            _reg : reg,
            dma_src: dma_src,
            handler: handler,
            vec: vec_src,
            device: device as usize,
        } )
    }
}

impl Drop for EduData<'_> {
    fn drop(
        &mut self,
    ) {
        // Have to cleanup our drm data here because we allocated
        // with the direct C bindings
        let device = self.device as *mut bindings::drm_device;
        unsafe { bindings::drm_dev_unregister(device) };
        unsafe { bindings::drm_dev_put(device) };
    }
}

// Conceptually, since each 'open' is a client instance, we should
// generate a new structure to represent it, put it on the heap, and
// make it accessible via the file descriptor private data.
unsafe extern "C" fn open(inode: *mut bindings::inode, filp: *mut bindings::file) -> c_int {
    let data = Page::alloc_page(GFP_KERNEL).unwrap();
    let message: u64 = 777;

    unsafe { data.write_raw(&message as *const u64 as *const u8, 0, 8) };
    let client = EduClient { data: data, _bound: core::marker::PhantomData };

    // Just showcasing that we can calculate the bar since we stored it.
    // This should be safe as well, since its inside a pinned KBox
    //EduData::queue_factorial(unsafe { &*(BAR as *const Bar<'_>) } , 5);

    // cast() vs as are virtually the same, cast() is just more restrictive
    let filep: *mut c_void = KBox::into_raw(KBox::new(client, GFP_KERNEL).unwrap()).cast();

    unsafe { (*filp).private_data = filep };
    pr_info!("Ready for the mmap!\n");
    0
}

// Reassociate our stuff with Rust, let rust handle the actual dropping
unsafe extern "C" fn close(inode: *mut bindings::inode, filp: *mut bindings::file) -> c_int {
    let filep = unsafe { (*filp).private_data };

    pr_info!("Freeing descriptor\n");
    // This 'consumes' our heap resource and lets rust handle the dropping
    // KBox's drop first, then each of its members (EduClient)
    unsafe { KBox::from_raw(filep) };
    0
}

unsafe extern "C" fn mmap(file: *mut bindings::file, vma: *mut bindings::vm_area_struct) -> c_int {
    // We need to handhold the compiler to acquire our desired type
    // This line is "access private as a pointer storing an EduClient"
    let client = unsafe { &*((*file).private_data as *const EduClient<'_>)};

    // ugly because the vma bindgen is weird
    unsafe { bindings::vm_insert_page(vma, (*vma).__bindgen_anon_1.__bindgen_anon_1.vm_start, client.data.as_ptr()) };

    unsafe { bindings::vmf_insert_mixed(vma, (*vma).__bindgen_anon_1.__bindgen_anon_1.vm_start + bindings::PAGE_SIZE, (BAR_START >> kernel::page::PAGE_SHIFT) as usize); }

    0
}

static mut BAR: usize = 0;
static mut BAR_START: u64 = 0;

fn test(
) {
}
