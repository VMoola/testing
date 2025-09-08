# Kernel Configuration for a bootable custom kernel

To make a basic configuration file for a kernel, run "make defconfig"
This is the smallest bootable kernel by qemu.

Then go into the config and enable anything we might want based on testing.
Common config dependencies are:
```
DEBUG_INFO - debugging info for function and data structure details
DWARF_TOOLCHAIN and BTF - pahole for viewing structures
LOCKDEP (for anything lock related)
DEBUG_VM - memory related debugging in general
```

"make debug.config" is good for thorough intensive testing, just sometimes
bigger/more excessive than necessary.

# QEMU Controls

To change between the qemu window and qemu monitor, use "ALT + {1,2}".
For syzbot kernels, pass in "-serial stdio" then ssh in.

# Debugging Userspace

Useful config dependencies are:
```
PAGE_OWNER - for tracking page allocation information
MEMORY_ALLOCATION_PROFILING - lightweight page allocation information
MODULES_FORCE_UNLOAD - for unloading test modules even if kernel is unsafe
GDB_SCRIPTS - debugging symbols for gdb (must run "make scripts_gdb")
*_SELFTESTS - enables selftest support for that feature
```

For proper debugging, all we need is the vmlinux and the bzImage.
bzImage boots the kernel, while the vmlinux has all the linkage +
instructions.
