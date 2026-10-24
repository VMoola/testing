# Devices

When we have devices physically connected to our computer, our guest cannot
see them without setting up a passthrough. This allows us to hand our device
to QEMU, and the guest can then use it directly.

# Prerequisites

- iommu is required (duh)! Ensure the iommu is enabled: `iommu=pt intel_iommu=on`.
- The list of all functions in the device's iommu group:
```
ls /sys/bus/pci/devices/[BDF]/iommu_group/devices/
```

# Passthrough

For the passthrough, trivially run the [vfio script](/examples/example_scripts/host/vfio_bind).
Essentially, just bind our io device to the vfio-pci driver, and tell our
shell to use as much memory as it needs to expose it. Then, hand our addresses
to QEMU as devices to attach, and we can see them on our guest.

# Graphics Handoffs

Handing away our device while we're using it to render our display obviously
doesn't work. To get around this, we can make use of the virtual terminals.

Firstly, ensure our iGPU is enabled. This is BIOS dependent. Linux uses that
for rendering the text-only virtual terminals.

Now, switch to one of those text-only terminals (`Ctrl + Alt + Fn[N]`) so
its safe to do the [handoff](/examples/example_scripts/host/gpu_mode).

```
gpu_mode {host, vm}

# if vm
vfio_bind
~/testing/qemu_test
```

# Drivers

As an example, nova-core is for nvidia gpus. Drivers always search the
`/usr/lib/firmware`, so put the firmware in the place the driver will look.
For nova-core, this is:

```
# Pulled from ttabi/linux-firmware-nova (some drivers use linux-firmware though)
# Check our supported versions - drivers/gpu/nova-core/gsp/fw
sudo tar --zstd -xf [firmware].tar.zstd -C <image>/usr/lib/firmware/
```

Now we can load our module, and our guest gets to make use of the hardware.
