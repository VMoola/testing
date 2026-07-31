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

In our rdinit program, we can call the rest:

```
#include <unistd.h>

// For for each binary we want to run
int pid = fork();
if (pid == 0)
    execl("/[binary]", "[binary]", (char *)NULL);
```

# Usage

We can do this with any of the [examples](/examples/modules_example/userspace).

# Notes

This gets finnicky if we try to use [custom entry functions](/examples/modules_example/userspace/custom_entry.c)
because there's some magic behind the scenes by default. Just stick to
a standard main function.

CPIO generation works best when we do it directly from the parent. Extra
filesystem links in between our location and the parent (share) directory
appear to break the hierarchy.
