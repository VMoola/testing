# Mac Users
Mac is fancy. Mac is annoying. After playing around with supporting this
repo on a mac, I have found that we're better off not using a Mac.

If we REALLY must use a mac, then look at the following instructions:

# Pre-Pre Requisites
- Install homebrew from the website.

# Prerequisites
- Install UTM
- QEMU* (this is optional, but does allow us to boot kernels from host)

# Pre-vm Steps

1) Create a new UTM vm.
2) Virtualize -> Other -> None
3) Set image size (this will be the size of the guest)
4) Now, edit the vm entry
5) Under drives, Import pointing to a fedora boot iso (aarch64)
6) Set the drive as Virtio + CD/DVD (ISO) Image
7) Under Sharing, set the share mode as VirtFS
8) Set a share path if desired

# Post-vm steps

1) Install fedora
2) Under drives, delete the iso drive

# Final steps
At this stage, we have now entered a fedora environment that we can use. This
repo works from there on.

# Access
Whenever we are in the vm, we can access our shared path using:
`sudo mount -t 9p -o trans=virtio,version=9p2000.L share [mountpoint]`.

We can copy files to/from the shared directory for use. We can NOT directly
execute binaries due to ABI differences with Mac zsh :/. The files themselves
remain intact and accessible for tools that use them - such as QEMU. We CAN
also nest vms, so there's not a lot of reason to worry about that.

# Notes

QEMU on mac doesn't support gtk as a display type. Replace it with `none` and
the qemu script will work as expected.

In case we need to mount it from the host side, the vms are located in:
`~/Library/Containers/com.utmapp.UTM/Data/Documents/`.
