# GDB notes

GDB can be a useful tool for kernel debugging. For example, if we BUG()
or deadlock, we are now hanging. GDB can connect and uncover everything
about the current state of the kernel.

GDB works after we've compiled a kernel, and from that source tree ran:
```
"make scripts_gdb"
```

# Useful commands

help [command] - shows details and usage about a command
lx-dmesg
bt (backtrace) - stack backtrace
frame (#) - focus on a location in the backtrace
list - show the lines around current
info - show variable values
s - step one line
n - execute one line, will skip over functions
p [var] - print variable (might need to dereference)
