#!/usr/bin/env python3
"""
hid_log.py - read InfoLog() text that the mouse sends over USB.

Firmware side (EM_HapticMouse/inc/my_debug.h):
  MYDEBUG_ENABLE = 1, output = USB. Text goes out on the vendor HID
  interface (interface 2, usage page 0xFF01), 64-byte reports, no report ID,
  unused bytes padded with 0.

Install once:   pip install hidapi
Run:            python tools/hid_log.py              (print to screen)
                python tools/hid_log.py -o log.txt   (also save to file)
                python tools/hid_log.py --list       (show HID interfaces)

Close the GUI (MouseTest.exe) first: it uses the same interface.
Stop with Ctrl+C.
"""
import argparse
import datetime
import sys
import time

try:
    import hid
except ImportError:
    sys.exit("missing module: run  pip install hidapi")

VID = 0x1A81          # inc/usbdesc.h USB_VID
PID = 0x2071          # inc/usbdesc.h USB_PID
USAGE_PAGE = 0xFF01   # src/usbdesc.c vendor report descriptor
INTERFACE = 2         # src/usbdesc.c bInterfaceNumber of the vendor interface


def find_path(vid, pid):
    cands = [d for d in hid.enumerate(vid, pid)]
    for d in cands:                      # Windows / macOS report usage_page
        if d.get("usage_page") == USAGE_PAGE:
            return d["path"]
    for d in cands:                      # Linux: fall back to interface number
        if d.get("interface_number") == INTERFACE:
            return d["path"]
    return None


def list_devices(vid, pid):
    devs = hid.enumerate(vid, pid)
    if not devs:
        print("no device %04X:%04X found" % (vid, pid))
    for d in devs:
        print("if=%s usage_page=0x%04X usage=0x%04X path=%s" % (
            d.get("interface_number"), d.get("usage_page", 0),
            d.get("usage", 0), d["path"]))


def open_device(vid, pid):
    path = find_path(vid, pid)
    if path is None:
        return None
    dev = hid.device()
    dev.open_path(path)
    dev.set_nonblocking(False)
    return dev


def main():
    ap = argparse.ArgumentParser(description="print mouse InfoLog() over USB")
    ap.add_argument("-o", "--out", help="also append the log to this file")
    ap.add_argument("--list", action="store_true", help="list HID interfaces")
    ap.add_argument("--vid", type=lambda x: int(x, 16), default=VID)
    ap.add_argument("--pid", type=lambda x: int(x, 16), default=PID)
    args = ap.parse_args()

    if args.list:
        list_devices(args.vid, args.pid)
        return

    out = open(args.out, "a", encoding="utf-8") if args.out else None
    line = ""
    dev = None
    print("waiting for %04X:%04X ... (Ctrl+C to stop)" % (args.vid, args.pid))
    try:
        while True:
            if dev is None:
                dev = open_device(args.vid, args.pid)
                if dev is None:
                    time.sleep(0.5)
                    continue
                print("--- connected ---")
            try:
                data = dev.read(64, 1000)        # timeout 1 s
            except (IOError, OSError):
                print("--- disconnected ---")
                dev.close()
                dev = None
                continue
            if not data:
                continue
            text = bytes(b for b in data if b).decode("ascii", "replace")
            line += text
            while "\n" in line:
                one, line = line.split("\n", 1)
                one = one.rstrip("\r")
                stamp = datetime.datetime.now().strftime("%H:%M:%S.%f")[:-3]
                msg = "%s  %s" % (stamp, one)
                print(msg, flush=True)
                if out:
                    out.write(msg + "\n")
                    out.flush()
    except KeyboardInterrupt:
        pass
    finally:
        if dev is not None:
            dev.close()
        if out:
            out.close()


if __name__ == "__main__":
    main()
