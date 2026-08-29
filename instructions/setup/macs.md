# Mac Users
Mac is fancy. Mac is annoying. After playing around with supporting this
repo on a mac, I've found it to be complex but possible.

# Pre-Pre Requisites
- Install homebrew from the website.

# Prerequisites
```
brew install lima qemu
git clone https://github.com/VMoola/testing.git ~/testing
```

# Simple Start
We have 2 main scripts for the mac here.

```
./setup_scripts/mac
exit
./bringup_mac
./qemu_test
```

The first sets up the vm and leaves us in a shell. Run this command
to reenter the vm whenever we want to do development. At this point, we
can have a standalone linux vm, where the core repository works as intended.
Deleting the vm using `limactl delete kdev --force` will cause the script
to rebuild it.

The second command enters the vm to prepare all our files, then copies
them to our mac host and sets it up to launch the guest. `bringup_mac` takes
the same format of arguments as `bringup`, and passes them down directly.

# Notes

Apple's M3+ does support nested hardware virtualization, but because the
chips cannot run linux natively, the performance wins from trying to enable
this are too little for the extra complexity.

Lima does use apples virtualization by default, which is a substantial
improvement over qemu instances for our computationally intensive work.
If you want a UTM setup instead, read further.

# UTM
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

# UTM Notes

QEMU on mac doesn't support gtk as a display type. Replace it with `none` and
the qemu script will work as expected.

In case we need to mount it from the host side, the vms are located in:
`~/Library/Containers/com.utmapp.UTM/Data/Documents/`.
