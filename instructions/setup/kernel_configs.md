# Kernel Configuration for a bootable custom kernel

To make a basic configuration file for a kernel, run "make defconfig"
This is the smallest bootable kernel by qemu.

When compiling our kernel, we can optionally pass in `KCONFIG_CONFIG=[path]`
which will point the build at a precise preconfigured config for that invocation.

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

# Internet Functionality

For internet + ssh functionality, we need the following config options enabled:
```
CONFIG_VIRTIO_NET=y #for ssh (and makes console consistent?)
CONFIG_FUSE=y
CONFIG_VIRTIO_FS=y
CONFIG_VIRTIO_PCI=y
```

# Shared Filesystem Directory

For a shared filesystem directory, we need the following:
```
CONFIG_NET_9P=y
CONFIG_NET_9P_VIRTIO=y
CONFIG_9P_FS=y
```

# BPF Support

The bare minimum:

```
CONFIG_BPF=y
CONFIG_BPF_SYSCALL=y
```

We can look through the kernel hacking tracers section to enable more tracepoints such as:
```
CONFIG_FUNCTION_TRACER=y
```

# QEMU Controls

To change between the qemu window and qemu monitor, use "ALT + {1,2}".
We can shutdown qemu using the ```poweroff``` command.

# Debugging Userspace

Useful config dependencies are:
```
PAGE_OWNER - for tracking page allocation information
MEMORY_ALLOCATION_PROFILING - lightweight page allocation information
MODULES_FORCE_UNLOAD - for unloading test modules even if kernel is unsafe
*_SELFTESTS - enables selftest support for that feature
```

# GDB

```
GDB_SCRIPTS - debugging symbols for gdb (must run "make scripts_gdb")
DEBUG_INFO_REDUCED=n - Ensures that we have the full debug symbols
```

For proper debugging, all we need is the vmlinux and the bzImage.
bzImage boots the kernel, while the vmlinux has all the linkage +
instructions.
