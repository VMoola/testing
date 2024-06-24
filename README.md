# Prerequisites
* IMPORTANT: ON FEDORA, UNINSTALL THE CURRENT VERSION OF MKOSI AND INSTALL THE LASTEST, FULLEST BUILD.

* Make an mkosi.default file to suit your needs. Look at the mkosi manpage for details on formatting and arguments. An example is also included.

* ```mkdir mkosi.cache``` - if we want faster image builds - although we shouldn't need to rebuild this all that frequently

* A custom kernel bzImage - This can be located wherever you want as long as you update the path in "qemu_test"

# Workflow
The "setup" script runs everything needed to setup the image. It will leave us in a container spawned by systemd-nspawn after initializing the Fedora distro. Press Crtl+] 3 times to leave the generated image, or alternatively shut it down.

after that your image is ready to go, set the custom kernel pathway in qemu_test then run it. For testing, redirect to the root directory of "/".

If on wsl (or likely any other virtualizer):

The mounting command for the shared file is: "mount -t 9p -o trans=virtio,version=9p2000.L host0 /mnt/[anymountdir]".

# Reasoning
Figuring out how to boot a custom kernel on Fedora took me a while - this is so I don't have to remember. Especially that Fedora's built in mkosi is outdated and doesn't work with custom kernels.
