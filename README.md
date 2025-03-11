# Prerequisites
* A custom kernel bzImage

* A mkosi.conf file to suit your needs

# Workflow
Run the setup script

Run *qemu_test*

The mounting command for the shared file is: "mount -t 9p -o trans=virtio,version=9p2000.L host0 /mnt/[anymountdir]".

# Reasoning
Figuring out how to boot a custom kernel on Fedora took me a while - this is so I don't have to remember.
