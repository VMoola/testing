# Custom Init Script

This is useful for testing kernels. We can prepare a binary that will run
on kernel boot in place of the standard init function.

```
gcc [input.c] -static -o [binary] //The static is important!
echo [binary] >> list
//Do this for every File we want access to

cat list | cpio -H newc -o > init.cpio //newc is the type for Linux!
qemu_test -u "-initrd init.cpio"
```

Make sure at least one binary is exactly named `init`.
In our init program, we can call the rest:

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
naming the primary binary "init" and having a main function.
