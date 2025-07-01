# LTP instructions
In mkosi.extra:
``` git clone https://github.com/linux-test-project/ltp.git```
Then in qemu:
```
cd /ltp
make autotools
./configure
#now to build and install the tests
make
make install
```
Now you can navigate to /opt/ltp to run the tests.

Take a look at the command found in the example scripts folder ```run_ltp -h```.

# For smaller projects
If we don't want to do a FULL test suite every time, we can simply
change into the test directory and then build and execute the tests from
there.

```
cd testcases/kernel/mem/hugetlb
make
make install
```

Now you can find the testcases in /opt/ltp/testcases/bin to run the tests.
See the included script for running our desired tests (because its a pain
to install all the ltp tests to get runltp happy).

# Advanced Usage
When we don't need a completely fresh testing environment, we can pre-build
the ltp testcases on the host. This saves us time since we can then skip
that (longest) step. We can run our desired test directly from our shared
directory, or proceed to run the suite like normal (just much faster).

For kernel development purposes, this is preferred since it allows us to
keep the testcases consistent over time since a fresh git clone/pull may
change testcase behaviour from under us. I.e. do this for bigger, longer
projects.
