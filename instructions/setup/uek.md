# UEK notes

# Building kernels

We can find the official UEK configs in the linux-uek github. The UEK config
found in examples contains the necessary changes to build a bootable UEK
kernel on my x86 system for testing purposes.

# kABI

In linux-uek, the uek-rpm directory has uek specific tools. We can ensure kABI
stability by building with ```KBUILD_SYMTYPES=y [file].o``` then running
```uek-rpm/tools/kabi compare [file].symtypes uek-rpm/ol[ver]/Symtypes.[type]```
