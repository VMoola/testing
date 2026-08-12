# Displays

This doc is a work in progress!

# Prerequisites

A desktop environment.
```
sudo dnf group install "xfce"
```

# Usage
For QEMU, we need to pass extra paramters.
```
-u "-vga virtio" - pass the virtio connection for display
-u "-usb -device usb-tablet" - pass the mouse to it
```

# SSH
For ssh, we need to do this bare minimum set of steps:

```
1 Ssh-keygen -t rsa
2 Press enter for each line
3 cat ~/.ssh/id_rsa.pub >> [target]/.ssh/authorized_keys
4 chmod 700 [target]/.ssh && chmod 600 [target]/.ssh_authorized_keys
```

And for graphics passthrough, we can simply:
```
add -X to the ssh command
```

# Notes
We might need to enable X11 forwarding in `/etc/ssh/sshd_config`, but
the usage documented above works flawlessly with Fedora.

Make sure that the user ownership of the ssh keys matches the ownership
of the user attempting the ssh!
