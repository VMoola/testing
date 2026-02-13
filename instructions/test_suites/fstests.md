# xFstests
All we need as prerequisites are the fstests dependencies (found in fstests.conf
for mkosi), and the desired filesystems compiled into our kernel.

Then in mkosi.extra: ``` git clone https://git.kernel.org/pub/scm/fs/xfs/xfstests-dev.git.```
Then in qemu:

```
cp [mounted_shared_dir]/xfstests-dev xfstests
cd xfstests
make
make install

//Then place a config in the directory - see example_fstest_local.config
cp [config]/xfstests/local.config
```
Take a look at the command found in the example scripts folder ```run_fstests -h```.

# Advanced Usage
When we don't need a completely fresh testing environment, we can pre-build
the fstest testcases on the host. This saves us time since we can then skip
that (longest) step. We can run our desired test directly from our shared
directory, or proceed to run the suite like normal (just much faster).

For kernel development purposes, this is preferred since it allows us to
keep the testcases consistent over time since a fresh git clone/pull may
change testcase behaviour from under us. I.e. do this for bigger, longer
projects.

The README does a good job of describing the test suite's additional
features and capabilities.
