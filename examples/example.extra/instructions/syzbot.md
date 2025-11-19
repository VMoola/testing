# Debugging Using Syzbot

For syzbot kernels its as simple as:
1) Grab the bzImage (for booting)
2) Grab the vmlinux (for tracing back addresses)
3) Pass "-serial stdio" into QEMU
4) Wait for the boot
5) Ssh in
6) Grab and run the C reproducer

Syzbot will include the kernel config, so you can build your own kernel
to test your patch locally.

Alternatively, you can mail the patch upstream using #sys-test and inline
the patch to let syzbot do the building and testing for you.

# Using the Syzkaller

We can also use the Syzkaller if we want to perfectly replicate what syzbot
does. This is most useful if syzbot does not have a C reproducer, but has
a syz reproducer.

1) Setup the Syzkaller on host:
```
git clone https://github.com/google/syzkaller
cd syzkaller
make TARGETARCH=[arch]
```

1.5) Optionally, generate the C reproducer:
```
[syzcaller_path]/bin/syz-prog2c -prog [syz_repro] > [shared_dir]/[repro].c
```

2) Give qemu access to the requisites:
```
cp [syzkaller_path]/bin/[arch]/syz-exe{cprog, cutor} [shared_dir]
```

3) Run the reproducer:
```
[shared_dir]/sys-execprog -executor [shared_dir]/syz-executor [syz_repro]
```
