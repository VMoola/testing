# MM tools
These are found in `[kernel_dir]/tools/mm/`. Note that these are NOT
found in the build tree, only the source tree.

```
make -C tools/mm
cp tools/mm/[binary] [shared_dir]/.
```

Then simply run on guest.

# Note
Any in-tree modules WILL be found in the build tree as their insertable
`.ko` and NOT the source tree. For those, see [modules tips](/instructions/tools/modules.md).


