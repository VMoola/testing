# Compiler

We write human readable kernel code. We compile this human readable code
into computer parseable machine code. There are intermediary steps
in between that we care about seeing:

| STEP | C | RUST |
|------|---|------|
| The human readable file | `.c` | `.rs` |
| The human readable, post pre-processor | `.i` | `.rsi` |
| The LLVM intermediate representation | `.ll` | `.ll` |
| The assembly code | `.s` | `.s` |
| The machine code binary | `.o` | `.o` |
