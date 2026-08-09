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

And for ssh, we can simply:
```
add -X to the ssh command
```

# Notes
We might need to enable X11 forwarding in `/etc/ssh/sshd_config`, but
the usage documented above works flawlessly with Fedora.
