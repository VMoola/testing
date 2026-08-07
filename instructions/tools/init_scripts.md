# Custom Init Script

This is useful for testing kernels. We can prepare a binary that will run
on kernel boot in place of the standard init function.

```
gcc [input.c] -static -o [share_dir][binary] //The static is important!
find [share_dir] > list
//Do this for every File we want access to

cat list | cpio -H newc -o > [cpio] //newc is the type for Linux!
cpio -it < [cpio]
qemu_test -u "-initrd [cpio]" -p "rdinit=/[path]"  // The slash is important!
```

# Usage

See the [rdinit example](/examples/modules_example/userspace/rdinit.c).
For custom disks (i.e. no proper system layout), see [disk images](/instructions/setup/disks.md).

# Notes

This gets finnicky if we try to use [custom entry functions](/examples/modules_example/userspace/custom_entry.c)
because there's some magic behind the scenes by default. Just stick to
a standard main function.

CPIO generation works best when we do it directly from the parent. Extra
filesystem links in between our location and the parent (share) directory
appear to break the hierarchy.

# Useful
Bash is the core of all of our scripts, so we can use a statically linked
bash in our init here. The whole preparation only takes a couple minutes.

```
wget https://ftp.gnu.org/gnu/bash/bash-[ver].tar.gz
tar -xvzf bash-[ver].tar.gz [extracted]
cd [extracted]
./configure --enable-static-link
make -j$(nproc)
# bash is an executable found here
```

BusyBox is a nice executable that aggregates many other basic executeables.

```
wget https://busybox.net/downloads/busybox-[ver].tar.bz2
tar -xpf busybox-[ver].tar.bz2 [extracted]
cd [extracted]
make menuconfig #enable static compilation
make -j$(nproc)
#busybox is an executable found here
```
