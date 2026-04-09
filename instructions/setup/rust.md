# Rust

Following the kernel's rust quick start documentation sets up all our
prerequisites. For Fedora, simply installing the specified packages works
out of the box.

It makes sense to install rustup:
```
rustup-init - ignore the warnings
source ~/.bashrc
rustup default nightly
rustup component add rust-src
```

Once we've installed the prerequisite packages, we simply regenerate
the config and enable:

```
CONFIG_RUST=y
```

*Note: If our toolchain ever breaks, we can delete the rust directory from
our compiled kernel to avoid recompiling the whole thing.

*Note: We NEED to make modules for rust, otherwise we'll be missing critial
linkers
