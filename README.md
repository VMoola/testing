# Prerequisites
* mkosi
* qemu

# Setup

If you just want to get something going, this set of commands should work:
```
cp examples/example_configs/mkosi/mkosi_fstests.conf mkosi.conf

mkdir mkosi.cache mkosi.builddir mkosi.extra mkosi.repart
cp examples/example_configs/mkosi/mkosi_repart.conf mkosi.repart/.
./setup
```

# Workflow

0) Setup the mkosi image for qemu. This only needs to be done once.
1) Build a kernel. See [setup instructions](instructions/setup)
2) Boot into the kernel using the qemu script ```./qemu_test or ./qemu_menuconfig.py```
3) Test the kernel. See [testing instructions](instructions/test_suites)

# Reasoning
Figuring out how to boot a custom kernel on Fedora took me a while - this is so I don't have to remember.
