# Mkosi

For Fedora x86 systems, the minimal [setup](/README.md) is known to
work out of the box. If you're unsure, thats probably what you want.

mkosi is prepped for supporting distributions (and package managers).
This is handy when we want to do extensive testing that is made easier
with external packages.

If you want a minimal image size and faster boots, see [buildroot](/instructions/setup/buildroot.md) instead.

# Cross-compilation
We can cross compile images with mkosi as well. This depends on:
* `qemu-user-static`
* In mkosi.conf: `ARCHITECTURE=[arch]`
* A distribution that supports the architecture: `DISTRIBUTION=[supports_arch]`

# Supported Architectures
Fedora: https://fedoraproject.org/wiki/Architectures.

# Reproduceability
`Incremental=yes` tells mkosi to prioritize building images from the
mkosi.cache/ if it exists.

Usefulness:
* Guarantee multiple images have identical distro/packages
* Worried about image corruption during testing (speeds up rebuilds,
especially for cross-architecture)

Cons:
* Takes up disk space (~1.6G for x86 fstests)
