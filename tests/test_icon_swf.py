import json
import struct
import unittest
from pathlib import Path

from tools.build_icon import movie


def read_tag(data: bytes, offset: int) -> tuple[int, bytes, int]:
    header = struct.unpack_from("<H", data, offset)[0]
    code, length = header >> 6, header & 0x3F
    offset += 2
    if length == 0x3F:
        length = struct.unpack_from("<I", data, offset)[0]
        offset += 4
    end = offset + length
    return code, data[offset:end], end


def read_tags(data: bytes, offset: int) -> list[tuple[int, bytes]]:
    result = []
    while offset < len(data):
        code, payload, offset = read_tag(data, offset)
        result.append((code, payload))
        if code == 0:
            break
    return result


class IconSwfTests(unittest.TestCase):
    def test_movie_header_and_exported_symbol_are_well_formed(self) -> None:
        data = movie()
        self.assertEqual(data[:3], b"FWS")
        self.assertEqual(struct.unpack_from("<I", data, 4)[0], len(data))
        self.assertEqual(data[3], 8)

        rect_nbits = data[8] >> 3
        rect_size = (5 + rect_nbits * 4 + 7) // 8
        tags_offset = 8 + rect_size + 4
        tags = read_tags(data, tags_offset)
        self.assertEqual([code for code, _ in tags], [32, 39, 56, 1, 0])

        sprite = tags[1][1]
        self.assertEqual(struct.unpack_from("<HH", sprite), (2, 1))
        self.assertEqual([code for code, _ in read_tags(sprite, 4)], [26, 1, 0])

        exports = tags[2][1]
        count, character_id = struct.unpack_from("<HH", exports)
        self.assertEqual((count, character_id), (1, 2))
        self.assertEqual(exports[4:], b"EnderalBookMarker\0")

    def test_diii_rule_adds_marker_without_replacing_native_read_icon(self) -> None:
        config_path = Path("Data/SKSE/Plugins/DIII/EnderalBookMarkers.json")
        config = json.loads(config_path.read_text(encoding="utf-8"))
        rule = config["rules"][0]
        self.assertEqual(rule["match"], {"formType": "Book", "personallyRead": True})
        self.assertNotIn("replace", rule["icon"])


if __name__ == "__main__":
    unittest.main()
