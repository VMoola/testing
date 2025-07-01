# Kernel Configuration for a bootable custom kernel

To make a basic configuration file for a kernel, run "make defconfig"
This is the smallest bootable kernel by qemu.

Then go into the config and enable anything we might want based on testing.
Common dependencies are:
```
LOCKDEP (if anything lock related)
DEBUG_VM - Everything in general
DEBUG_INFO - debugging info for pahole
DWARF_TOOLCHAIN and BTF - pahole for modifying structures
MODULES_FORCE_UNLOAD - for unloading test modules even if kernel is unsafe
GDB_SCRIPTS - debugging symbols for gdb (must run "make scripts_gdb")
```
"make debug.config" is good for thorough intensive testing, just sometimes
bigger/more excessive than necessary.

# Controls

To change between the qemu window and qemu monitor, use "ALT + {1,2}"
