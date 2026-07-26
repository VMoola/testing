# Perf

The prerequisites for this are included with BPF support. Realistically,
if doing performance comparisons, we might want the BPF support anyway.

Perf is best used for over-arching observability (i.e. seeing the
breakdown of latencies by function call and performance analysis).

BPF is best used for targeted observability (i.e. analyzing specific
pieces of a target function and conditional reporting).

# Usage

The overall layout is consistent, but the options change based on needs.
Honestly, just check the man page for perf-ftrace. Theres a LOT to it.
