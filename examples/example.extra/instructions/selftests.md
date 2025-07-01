# Kselftests
This is part of the Linux Kernel API. Take a look at the official
docs: https://docs.kernel.org/dev-tools/kselftest.html.

These binaries MUST be run from within the target kernel. The APIs make this
easy to accomplish:

```
make -C tools/testing/selftests install INSTALL_PATH=[shared_dir_path]/selftests TARGETS=[targets]
```

Then in QEMU:

```
cp [mounted_shared_dir]/selftests selftests
cd selftests
./run_kselftest.sh -h
```

# Advanced Usage
If we don't need loggin tools, we can run the tests directly from the shared
directory. Since the shared directory is readonly, we must follow the steps
above for logging.

For testing found in other directories (not in 'selftests'), we make the tests
in our kernel source tree, then copy the binaries into our guest and run them.

We only need to recompile these binaries whenever the testing source code is
updated. Otherwise we can compile them once, to save time (and eliminate the
hassle of running 'make mrproper' often).

# Testing Requisites
For radix-tree testing, we need libasan and libusan installed on the guest.
