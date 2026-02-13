#!/usr/bin/env python3

import curses
import os
import subprocess
import sys

# Defaults, matching qemu_test's defaults as closely as possible
default_config = {
    "ARCH": "x86_64",
    "KERNEL_PATH": os.path.expanduser("~/kernel/compiled/arch/x86_64/boot/bzImage"),
    "BOOT_FILE": "image.raw",
    "ROOTFS": "ext4",
    "MEMORY": "1G",
    "CPUS": "1",
    "NUMA_ID": "0",
    "SERIAL": "none",
    "MONITOR": "vc",
    "DISPLAY": "curses",
    "EXTRA": "mkosi.extra",
    "BOOT_PARAMS": "nokaslr printk.synchronous=1",
    "QEMU": "qemu-kvm",
    "ACCEL": "kvm -accel tcg",
}

config = default_config.copy()

fields = [
    ("ARCH", "Architecture", "x86_64, arm64, ..."),
    ("KERNEL_PATH", "Kernel Path", "Path to bzImage"),
    ("BOOT_FILE", "Disk Image", "image.raw, or path"),
    ("ROOTFS", "Filesystem type", "ext4, xfs, ..."),
    ("MEMORY", "Memory", "e.g., 1G, 4096M"),
    ("CPUS", "CPUs", "number"),
    ("NUMA_ID", "NUMA Node Id", "number"),
    ("SERIAL", "Serial", "none, vc, stdio"),
    ("MONITOR", "Monitor", "vc, stdio, none"),
    ("DISPLAY", "Display", "curses, gtk, none"),
    ("EXTRA", "Extra Path", "path"),
    ("BOOT_PARAMS", "Boot Params", "quoted string if multiples"),
    ("QEMU", "QEMU Executable", "qemu-kvm, qemu-system-x86_64..."),
    ("ACCEL", "Accelerator", "kvm, tcg, ..."),
]

def curses_main(stdscr):
    curses.curs_set(0)
    curses.start_color()
    curses.init_pair(1, curses.COLOR_BLACK, curses.COLOR_CYAN)
    selected = 0
    maxrow = len(fields)
    info_line = "Arrows: move | Enter: edit | r: Run | p: Preview | q: Exit"
    while True:
        stdscr.clear()
        stdscr.addstr(0, 0, "QEMU Menuconfig".center(60), curses.A_BOLD)
        for idx, (key, name, helpstr) in enumerate(fields):
            display = f"{name:18}: {config[key]}"
            if idx == selected:
                stdscr.attron(curses.color_pair(1))
                stdscr.addstr(idx + 2, 2, display.ljust(56))
                stdscr.attroff(curses.color_pair(1))
            else:
                stdscr.addstr(idx + 2, 2, display.ljust(56))
        stdscr.addstr(maxrow + 4, 2, info_line, curses.A_DIM)
        stdscr.addstr(maxrow + 5, 2, f"Help: {fields[selected][2]}", curses.A_ITALIC)
        stdscr.refresh()
        c = stdscr.getch()
        if c in (curses.KEY_DOWN, ord("j")):
            selected = (selected + 1) % maxrow
        elif c in (curses.KEY_UP, ord("k")):
            selected = (selected - 1) % maxrow
        elif c in (curses.KEY_ENTER, ord("\n"), ord("\r")):
            curses.curs_set(1)
            stdscr.addstr(maxrow + 7, 2, " " * 56)
            stdscr.addstr(maxrow + 7, 2, f"Set {fields[selected][1]}: ")
            stdscr.refresh()
            curses.echo()
            value = stdscr.getstr(maxrow + 7, 2 + len(f"Set {fields[selected][1]}: "), 50).decode()
            curses.noecho()
            if value.strip():
                config[fields[selected][0]] = value.strip()
            curses.curs_set(0)
        elif c == ord('q'):
            break
        elif c == ord('r'):
            curses.endwin()
            qemu_cmd = build_qemu_command()
            print("\nRunning QEMU command:\n")
            print(qemu_cmd)
            print()
            os.system(qemu_cmd)
            return
        elif c == ord('p'):
            curses.endwin()
            qemu_cmd = build_qemu_command()
            print("\nPreview QEMU command:\n")
            print(qemu_cmd)
            print("\nPress Enter to return to menu.")
            input()
            curses.wrapper(curses_main)
            return

def build_qemu_command():
    # Compose QEMU command similar to your qemu_test
    s = config
    # Important: wrap BOOT_PARAMS and proper quoting.
    qemu_str = (
        f"{s['QEMU']} "
        f"-kernel {s['KERNEL_PATH']} "
        f"-drive format=raw,file={s['BOOT_FILE']} "
        f"-append 'root=/dev/sda1 rw rootfstype={s['ROOTFS']} console=ttyS0 {s['BOOT_PARAMS']}' "
        f"-accel {s['ACCEL']} "
        f"-m {s['MEMORY']} "
        f"-s "
        f"-serial {s['SERIAL']} "
        f"-monitor {s['MONITOR']} "
        f"-display {s['DISPLAY']} "
        f"-smp cores={s['CPUS']} "
        f"-object memory-backend-ram,size={s['MEMORY']},id=m{s['NUMA_ID']} "
        f"-numa node,cpus=0-{int(s['CPUS']) - 1},nodeid={s['NUMA_ID']},memdev=m{s['NUMA_ID']} "
        f"-net nic -net user,hostfwd=tcp::10022-:22 "
        f"-virtfs local,path={s['EXTRA']},mount_tag=host0,security_model=passthrough,id=host0"
    )
    return qemu_str

def main():
    try:
        curses.wrapper(curses_main)
    except curses.error as e:
        if str(e) == "endwin() returned ERR":
            print("\nWarning: curses did not shut down cleanly (endwin() returned ERR), but this is harmless.")
        else:
            raise

if __name__ == "__main__":
    main()
