# BPF Instructions

Firstly, we need to compile our kernel with the required configs:

```
CONFIG_BPF=y
CONFIG_BPF_SYSCALL=y
```

Second, we need to generate our headers:

```
bpftool btf dump file [path_to_vmlinux] format c > vmlinux.h
```

Third, we need to compile our program (see the [example](/examples/modules_example/userspace/bpf.c)):
The main point of note here, is we need our SEC command to wrap a valid ELF section.

The easiest way to find our valid tracepoints is `cat /sys/kernel/tracing/available_events` and
`cat /sys/kernel/tracing/available_filter_functions`.

```
# We NEED all these extra flags to suppress things
clang -target bpf -Wall -O2 -g -c [input] -o [output]
```

# Usage

Now we're all setup. Inside the kernel, we can use bpftool to control the program.

```
bpftool prog load [path_to_program] /sys/fs/bpf/[name] autoattach
rm /sys/fs/bpf/[name] #for removal
```

Now we can check the logs:
```
cat /sys/kernel/tracing/trace

# OR if we're ok losing the information we can do these
bpftool prog tracelog
# This is essentially the same as:
cat /sys/kernel/tracing/trace_pipe
```

We can also manually run the tracepoint on its own:
```
bpftool prog list
bpftool prog run pinned /sys/fs/bpf/[name] repeat [times]
```
