// Include our kernel source code
#include "vmlinux.h"

#define __TARGET_ARCH_x86 //Required for kprobes

// BPF definitions
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

// Map the program to the intercept point - See bpf SEC
// "raw_tp/[event]" is for tracepoints
// "fentry/[function]" is for function entries
// "kprobe/[function][+offset]" is for mapping to raw code lines

/*
 * We may not see both the kprobe and retprobe if the compiler optimizes
 * out certain instructions. This can be prevented by passing the
 * [noinline] keyword to the function name.
 */

//Aka assuming our function signature is : noinline int placeholder(int)
SEC("kprobe/placeholder") int BPF_KPROBE(hi, int val)
{
	bpf_printk("Hello world from module function! %d", val);

	/*
	 * We only have the data passed in to us as arguments.
	 * Anytime we want to access MORE data than passed to us
	 * we must use bpf_probe_read_kernel(dest, size, src).
	 */

	return 0;
}

SEC("kretprobe/placeholder") int BPF_KRETPROBE(hey, int ret)
{
	bpf_printk("Goodbye world from module fxn! %d", ret);
	return 0;
}

// Tell the kernel we are GPL compatible
char _license[] SEC("license") = "GPL";
