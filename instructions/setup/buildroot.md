# Buildroot

For more widespread users, we'll use buildroot. This takes longer because it
compiles the libraries and toolchains from scratch.

# Summary
1) Config Setup
2) Build the Image
3) Build the kernel
4) Setup the hardware
5) Test

# Config Setup
```
git clone https://github.com/buildroot/buildroot.git
cd buildroot
make list-defconfigs
make [config]	#i.e. qemu_x86_64_defconfig
```

Now the default buildroot configs have a LOT going on. This makes sense for
vendors, but we don't need this level of scruitiny. To customize:

```
make menuconfig
```

The following config options affect the build time a LOT, so set them like so:
```
BR2_CCACHE=y		 #Makes its own buildroot ccache directory
BR2_LINUX_KERNEL=n	 #Compiles a linux kernel
BR2_TOOLCHAIN_EXTERNAL=y #Uses an external toolchain instead of building one
```

Useful things to enable in menuconfig:
```
1) ext4 (or whatever rootfs we have compiled in our kernel)

2) Any userspace packages we want. See [examples](/examples/example_configs/buildroot_configs/)

* We may need to enable `BR2_PACKAGE_BUSYBOX_SHOW_OTHERS=y` to see them.

```

# Build the Image

We copy the files we want to their desired location.

```
cp [desired_directories] output/target/[path_on_image]
make
```
There are many cached files:
```
dl/         #downloads
package/    #packages
build/      #built tools
```

# Build the Kernel
```
cd [kernel_dir]
cat [buildroot_dir]/board/[hardware]/[arch]/[local.config] >> .config
make menuconfig
make CROSS_COMPILE=[cross-compiler]
```

Once that's all done, we can use the freshly compiled image. We can reuse
that image as many times as we like for the architecture, given a
compatible kernel.

Buildroot will override any images in its output directory, so make sure to
copy them beforehand when testing multiple architectures.

# Booting the rootfs
```
[buildroot_dir]/output/images/start-qemu.sh
```
in case we want to use our more customizable command, we can tweak it using
that as a guide. If we don't need anything fancy, we can stick to the above
script.

For some architectures, Buildroot assumes we can install the appropriate
modules BEFORE booting. In case we hang on boot without a dump/panic, try
following the guide: [installing modules](/instructions/test_suites/modules.md)

# Testing
Prioritize using our own targeted drivers + custom binaries.
Buildroot does everything it can to speedup subsequent builds. One major
speedup is utilizing the BuildBox linker. This is very prone to bugs, so
we're better off writing our own targeted testing drivers.

We can use a cross-compiler to compile the binaries on our host. Alternatively,
we can install the proper compiler into the guest through the config.

# Notes
So long as our `known_hosts` file exists, we will get a warning on one
machine. This doesn't really matter to us and can be safely ignored.

I also tried to play with the rootfs overlay, but ran into issues with keeping
persistent ssh keys. On top of that, this requires a fresh clean build (which
is slow and long). Sticking to `output/targets/[path_on_image]` is easier.

I also wasn't able to get the external toolchain to work with Fedora. The
build speeds up a LOT if we download a full external toolchain and set the
following:
```
BR2_TOOLCHAIN_EXTERNAL=y #Uses an external toolchain instead of building one
BR2_TOOLCHAIN_EXTERNAL_PREINSTALLED=y
BR2_TOOLCHAIN_EXTERNAL_PATH="[toolchain_path]" #Found under Toolchain Origin
BR2_TOOLCHAIN_EXTERNAL_PREFIX="[toolchain_prefixes]"
```

# Issues
If we are rebuilding, we're better off doing clean builds. Its not worth
the headache tracking down all the bits of stale data, and ccache makes it
fast enough.
```
make clean  #dist-clean removes downloads as well!
```
