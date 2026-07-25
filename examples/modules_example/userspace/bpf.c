// Include our kernel source code
#include "vmlinux.h"

// BPF definitions
#include <bpf/bpf_helpers.h>

// Map the program to the intercept point - See bpf SEC
SEC("raw_tp/sys_enter")
int helloworld(struct bpf_raw_tracepoint_args *ctx)
{
	/*
	 * TP_PROTO defines what we can grab from ctx->args
	 * I.e. for sys_enter, we have pt_regs, id.
	 */
	unsigned long syscall = ctx->args[1];

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
