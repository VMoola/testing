# Disk Images

We can attach custom disk images to our custom kernel. This is incredibly
straightforward. All we need is a disk, and an appropriate filesystem on
that disk.

```
qemu-img create [name] [size]
mkfs -t [type] [name]
```

These can be attached to qemu as drives, and will populate the `/dev` directory
for mounting. Because they are raw drives, they will populate alphabetically.
If a drive has a partition table, it will be suffixed with numbers correspondingly:
`/dev/sda[IF has partition table] -> /dev/sdb[IF has partition table] ...`.

We should mount using `mount [src] [dest] -t [fs]`.
