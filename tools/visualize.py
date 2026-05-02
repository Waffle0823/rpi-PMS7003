#!/usr/bin/env python3
"""Visualize PMS7003 binary logs produced by `rpi-finedust -o <file>`.

Binary log format (little-endian):
  File header (16 bytes):
    char     magic[8]      = b"PMSLOG\0\0"
    uint32_t version       = 1
    uint32_t record_size   = 32
  Record (32 bytes), repeated:
    int64_t  timestamp_ns
    uint16_t pm1_0_cf1, pm2_5_cf1, pm10_cf1
    uint16_t pm1_0_atm, pm2_5_atm, pm10_atm
    uint16_t air_0_3, air_0_5, air_1_0, air_2_5, air_5_0, air_10_0
"""
from __future__ import annotations

import argparse
import struct
import sys
from datetime import datetime, timezone
from pathlib import Path

import numpy as np

MAGIC = b"PMSLOG\0\0"
HEADER_STRUCT = struct.Struct("<8sII")  # magic, version, record_size
RECORD_STRUCT = struct.Struct("<q12H")  # int64 ts_ns + 12 uint16
EXPECTED_RECORD_SIZE = 32

FIELDS = [
    "pm1_0_cf1", "pm2_5_cf1", "pm10_cf1",
    "pm1_0_atm", "pm2_5_atm", "pm10_atm",
    "air_0_3", "air_0_5", "air_1_0",
    "air_2_5", "air_5_0", "air_10_0",
]


def load_log(path: Path):
    raw = path.read_bytes()
    if len(raw) < HEADER_STRUCT.size:
        raise ValueError(f"file too small: {len(raw)} bytes")

    magic, version, record_size = HEADER_STRUCT.unpack_from(raw, 0)
    if magic != MAGIC:
        raise ValueError(f"bad magic: {magic!r}")
    if version != 1:
        raise ValueError(f"unsupported version: {version}")
    if record_size != EXPECTED_RECORD_SIZE:
        raise ValueError(f"unexpected record size: {record_size}")

    body = raw[HEADER_STRUCT.size:]
    n, rem = divmod(len(body), record_size)
    if rem != 0:
        print(
            f"warning: trailing {rem} bytes ignored (partial record)",
            file=sys.stderr,
        )

    timestamps = np.empty(n, dtype="datetime64[ns]")
    values = np.empty((n, 12), dtype=np.uint16)
    for i in range(n):
        rec = RECORD_STRUCT.unpack_from(body, i * record_size)
        timestamps[i] = np.datetime64(rec[0], "ns")
        values[i] = rec[1:]

    return timestamps, values


def summarize(timestamps, values):
    if len(timestamps) == 0:
        print("no records.")
        return
    t0 = timestamps[0].astype("datetime64[us]").astype(datetime).replace(tzinfo=timezone.utc)
    t1 = timestamps[-1].astype("datetime64[us]").astype(datetime).replace(tzinfo=timezone.utc)
    print(f"records: {len(timestamps)}")
    print(f"range:   {t0.isoformat()}  ->  {t1.isoformat()}")
    print("field            min     mean      max")
    for i, name in enumerate(FIELDS):
        col = values[:, i]
        print(f"  {name:<12} {col.min():>6}  {col.mean():>7.1f}  {col.max():>6}")


def plot(timestamps, values, output: Path | None):
    import matplotlib.dates as mdates
    import matplotlib.pyplot as plt

    t = timestamps.astype("datetime64[ms]").astype("O")

    fig, axes = plt.subplots(3, 1, figsize=(12, 9), sharex=True)

    ax = axes[0]
    ax.plot(t, values[:, 0], label="PM1.0")
    ax.plot(t, values[:, 1], label="PM2.5")
    ax.plot(t, values[:, 2], label="PM10")
    ax.set_ylabel("ug/m^3 (CF=1)")
    ax.set_title("PMS7003 log")
    ax.legend(loc="upper right")
    ax.grid(True, alpha=0.3)

    ax = axes[1]
    ax.plot(t, values[:, 3], label="PM1.0")
    ax.plot(t, values[:, 4], label="PM2.5")
    ax.plot(t, values[:, 5], label="PM10")
    ax.set_ylabel("ug/m^3 (atm)")
    ax.legend(loc="upper right")
    ax.grid(True, alpha=0.3)

    ax = axes[2]
    sizes = ["0.3um", "0.5um", "1.0um", "2.5um", "5.0um", "10um"]
    for i, label in enumerate(sizes):
        ax.plot(t, values[:, 6 + i], label=label)
    ax.set_ylabel("count / 0.1L")
    ax.set_xlabel("time")
    ax.set_yscale("symlog")
    ax.legend(loc="upper right", ncol=3)
    ax.grid(True, alpha=0.3)

    locator = mdates.AutoDateLocator()
    axes[-1].xaxis.set_major_locator(locator)
    axes[-1].xaxis.set_major_formatter(mdates.ConciseDateFormatter(locator))

    fig.tight_layout()

    if output is None:
        plt.show()
    else:
        fig.savefig(output, dpi=120)
        print(f"saved plot to {output}")


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("input", type=Path, help="path to .pmslog binary file")
    parser.add_argument(
        "-o", "--output", type=Path, default=None,
        help="save figure to this path instead of showing it",
    )
    parser.add_argument(
        "--no-plot", action="store_true",
        help="only print a summary, do not plot",
    )
    args = parser.parse_args()

    timestamps, values = load_log(args.input)
    summarize(timestamps, values)
    if args.no_plot:
        return
    plot(timestamps, values, args.output)


if __name__ == "__main__":
    main()
