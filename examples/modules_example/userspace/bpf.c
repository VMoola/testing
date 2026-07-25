// Include our kernel source code
#include "vmlinux.h"

// BPF definitions
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

// Map the program to the intercept point - See bpf SEC
SEC("raw_tp/sys_enter")
int BPF_PROG(helloworld, struct pt_regs *regs, long syscall)
{
	/*
	 * TP_PROTO defines what we can grab from ctx->args
	 * BPF_PROG does this for us in a clean readable way.
	 */

	// See arch/[arch]/entry/syscalls/syscall.tbl for ids
	if (syscall != 9) {
		// filter out for mmaps only
		return 0;
	}

	bpf_printk("Hello world from mmap! %lu\n", syscall);

	/*
	 * We only have the data passed in to us as arguments.
	 * Anytime we want to access MORE data than passed to us
	 * we must use bpf_probe_read_kernel(dest, size, src).
	 */

	return 0;
}

// Tell the kernel we are GPL compatible
char _license[] SEC("license") = "GPL";
