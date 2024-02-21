# Example
This directory will copy any files placed within. As of right now, this whole setup works to run ltp through qemu. I've also included a sample build script to be run from your kernel directory.

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
