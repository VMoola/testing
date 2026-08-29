# Prerequisites
* mkosi
* qemu
* Any requisites to build the linux kernel & modules
* This repo to be located in "~/testing"
* A Fedora or Ubuntu System - for macs see [the mac instructions](instructions/setup/macs.md).

# Simple Start
For those of you who have no interest in the granularity of these scripts,
as long as you are on Fedora with the prerequisites, you can simply run:

If you are x86_64 instead of arm64/aarch64:
```
cp -r examples/example_configs/script_configs/ configs
cp configs/build_configs/x86_64.conf configs/build_script.conf
cp configs/qemu_configs/x86_64.conf configs/qemu_test.conf
```

If you are on Ubuntu instead of Fedora:
```
cp examples/example_configs/mkosi/building/mkosi_ubuntu.conf mkosi.conf
```

Then run:

```
./bringup
./qemu_test
```

For anything more control over the setup, read further.

# Setup

If you just want to get something going, this set of commands should work:
```
cp examples/example_configs/mkosi/mkosi_fstests.conf mkosi.conf
cp -r examples/example_configs/script_configs/ configs

mkdir mkosi.cache mkosi.builddir mkosi.extra mkosi.repart
cp examples/example_configs/mkosi/mkosi_repart.conf mkosi.repart/.
./setup
```

If you want to cross compile images, take a look at the [buildroot](instructions/setup/buildroot.md) and [mkosi](instructions/setup/mkosi.md).

mkosi is prepped for supporting distributions (and package managers).
This is handy when we want to do extensive testing that is made easier
with external packages.

Buildroot is better for rapid iteration testing with custom testcases/drivers
that have minimal package dependencies.

# Workflow

0) Setup the mkosi image for qemu. This only needs to be done once.
1) Build a kernel. See [setup instructions](instructions/setup)
2) Boot into the kernel using the qemu script ```./qemu_test or ./qemu_menuconfig.py```. Example default configs can be found in [configs](examples/example_configs/script_configs)
3) Test the kernel. See [testing instructions](instructions/test_suites)

# Reasoning
Figuring out how to boot a custom kernel on Fedora took me a while - this is so I don't have to remember.
