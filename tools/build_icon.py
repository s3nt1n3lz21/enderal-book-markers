#!/usr/bin/env python3
"""Build the original, exported SWF icon used by the DIII rule."""

from __future__ import annotations

import argparse
import struct
from pathlib import Path


class BitWriter:
    def __init__(self) -> None:
        self._bits: list[int] = []

    def write(self, value: int, count: int) -> None:
        if count < 0:
            raise ValueError("bit count cannot be negative")
        if count == 0:
            return
        mask = (1 << count) - 1
        for shift in range(count - 1, -1, -1):
            self._bits.append((value & mask) >> shift & 1)

    def bytes(self) -> bytes:
        bits = self._bits + [0] * ((-len(self._bits)) % 8)
        return bytes(
            sum(bits[start + offset] << (7 - offset) for offset in range(8))
            for start in range(0, len(bits), 8)
        )


def signed_width(value: int) -> int:
    for width in range(1, 32):
        if -(1 << (width - 1)) <= value < (1 << (width - 1)):
            return width
    raise ValueError(f"value is too wide for SWF signed field: {value}")


def rect(x_min: int, x_max: int, y_min: int, y_max: int) -> bytes:
    width = max(signed_width(v) for v in (x_min, x_max, y_min, y_max))
    bits = BitWriter()
    bits.write(width, 5)
    for value in (x_min, x_max, y_min, y_max):
        bits.write(value, width)
    return bits.bytes()


def tag(code: int, data: bytes = b"") -> bytes:
    length = len(data)
    if length < 63:
        return struct.pack("<H", (code << 6) | length) + data
    return struct.pack("<HI", (code << 6) | 63, length) + data


def set_fill_and_move(bits: BitWriter, fill_index: int, x: int, y: int, fill_bits: int) -> None:
    bits.write(0, 1)  # TypeFlag: style-change record
    bits.write(0, 1)  # StateNewStyles
    bits.write(0, 1)  # StateLineStyle
    bits.write(1, 1)  # StateFillStyle1
    bits.write(0, 1)  # StateFillStyle0
    bits.write(1, 1)  # StateMoveTo
    width = max(signed_width(x), signed_width(y))
    bits.write(width, 5)
    bits.write(x, width)
    bits.write(y, width)
    bits.write(fill_index, fill_bits)


def line_to(bits: BitWriter, dx: int, dy: int) -> None:
    width = max(2, signed_width(dx), signed_width(dy))
    bits.write(1, 1)  # TypeFlag: edge record
    bits.write(1, 1)  # StraightFlag
    bits.write(width - 2, 4)
    if dx and dy:
        bits.write(1, 1)  # GeneralLineFlag
        bits.write(dx, width)
        bits.write(dy, width)
    else:
        bits.write(0, 1)  # GeneralLineFlag
        bits.write(1 if dx == 0 else 0, 1)  # VertLineFlag
        bits.write(dy if dx == 0 else dx, width)


def shape() -> bytes:
    bounds = rect(0, 400, 0, 400)
    fill_styles = (
        b"\x02"  # two fills
        + b"\x00\xD8\xA8\x48\xFF"  # warm gold RGBA
        + b"\x00\x3A\x2A\x1A\xFF"  # dark brown RGBA check
    )
    line_styles = b"\x00"
    records = BitWriter()
    records.write(2, 4)  # NumFillBits
    records.write(0, 4)  # NumLineBits

    bookmark = [(55, 25), (345, 25), (345, 375), (200, 285), (55, 375)]
    check = [(100, 190), (140, 150), (190, 200), (270, 115), (310, 155), (190, 280)]
    for points, fill_index in ((bookmark, 1), (check, 2)):
        start_x, start_y = points[0]
        set_fill_and_move(records, fill_index, start_x, start_y, 2)
        previous_x, previous_y = start_x, start_y
        for x, y in points[1:] + [points[0]]:
            line_to(records, x - previous_x, y - previous_y)
            previous_x, previous_y = x, y
    records.write(0, 6)  # EndShapeRecord

    return struct.pack("<H", 1) + bounds + fill_styles + line_styles + records.bytes()


def matrix_identity() -> bytes:
    bits = BitWriter()
    bits.write(0, 1)  # HasScale
    bits.write(0, 1)  # HasRotate
    bits.write(0, 5)  # NTranslateBits
    return bits.bytes()


def movie() -> bytes:
    shape_tag = tag(32, shape())  # DefineShape3
    place = tag(26, b"\x06" + struct.pack("<HH", 1, 1) + matrix_identity())
    sprite = tag(39, struct.pack("<HH", 2, 1) + place + tag(1) + tag(0))
    export = tag(56, struct.pack("<H", 1) + struct.pack("<H", 2) + b"EnderalBookMarker\x00")
    body = rect(0, 400, 0, 400) + struct.pack("<HH", 24 * 256, 1)
    body += shape_tag + sprite + export + tag(1) + tag(0)
    return b"FWS" + bytes([8]) + struct.pack("<I", len(body) + 8) + body


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "output",
        nargs="?",
        default="Data/Interface/EnderalBookMarkers/BookMarker.swf",
        type=Path,
    )
    args = parser.parse_args()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    data = movie()
    args.output.write_bytes(data)
    print(f"Wrote {args.output} ({len(data)} bytes)")


if __name__ == "__main__":
    main()
