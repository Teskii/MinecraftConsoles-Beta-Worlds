#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
from dataclasses import asdict, dataclass
from pathlib import Path


TABLE_OFFSET = 0xC000
ENTRY_SIZE = 0x40
BLOCK_SIZE = 0x1000
LEVEL0_HASH_INTERVAL = 0xAA


@dataclass
class Entry:
    index: int
    name: str
    is_dir: bool
    parent_id: int
    first_block: int
    block_count: int
    size: int


def read_u24le(data: bytes) -> int:
    return data[0] | (data[1] << 8) | (data[2] << 16)


def is_empty_entry(raw: bytes) -> bool:
    return not raw.strip(b"\x00")


def parse_entry(index: int, raw: bytes) -> Entry | None:
    if is_empty_entry(raw):
        return None

    name_len_flags = raw[0x28]
    name_len = name_len_flags & 0x3F
    if name_len == 0:
        raw_name = raw[:0x28].split(b"\x00", 1)[0]
        if not raw_name:
            return None
        name = raw_name.decode("ascii", errors="replace")
    else:
        name = raw[:name_len].decode("ascii", errors="replace")

    is_dir = (name_len_flags & 0xC0) == 0xC0
    return Entry(
        index=index,
        name=name,
        is_dir=is_dir,
        parent_id=int.from_bytes(raw[0x32:0x34], "big"),
        first_block=read_u24le(raw[0x2F:0x32]),
        block_count=read_u24le(raw[0x29:0x2C]),
        size=int.from_bytes(raw[0x34:0x38], "big"),
    )


def block_to_offset(block: int) -> int:
    # Small STFS packages like this TU need only the level-0 hash table skips.
    hash_blocks_before = (block + (LEVEL0_HASH_INTERVAL - 2)) // LEVEL0_HASH_INTERVAL
    return TABLE_OFFSET + (block + hash_blocks_before) * BLOCK_SIZE


def parse_table(package: bytes, max_entries: int = 512) -> list[Entry]:
    entries: list[Entry] = []
    empty_run = 0

    for index in range(max_entries):
        start = TABLE_OFFSET + index * ENTRY_SIZE
        raw = package[start : start + ENTRY_SIZE]
        entry = parse_entry(index, raw)
        if entry is None:
            empty_run += 1
            if entries and empty_run >= 16:
                break
            continue

        empty_run = 0
        entries.append(entry)

    return entries


def build_relative_path(entry: Entry, entries_by_index: dict[int, Entry]) -> Path:
    if entry.parent_id == 0:
        return Path(entry.name)

    parent = entries_by_index.get(entry.parent_id)
    if parent is None:
        return Path(entry.name)

    return build_relative_path(parent, entries_by_index) / entry.name


def extract_file(package: bytes, entry: Entry) -> bytes:
    remaining = entry.size
    block = entry.first_block
    parts: list[bytes] = []

    while remaining > 0:
        offset = block_to_offset(block)
        take = min(BLOCK_SIZE, remaining)
        parts.append(package[offset : offset + take])
        remaining -= take
        block += 1

    return b"".join(parts)


def main() -> int:
    parser = argparse.ArgumentParser(description="Extract a small Xbox 360 TU/STFS package.")
    parser.add_argument("package", type=Path, help="Path to the TU/STFS package")
    parser.add_argument("output", type=Path, help="Directory to extract into")
    args = parser.parse_args()

    package_path = args.package
    output_dir = args.output
    package = package_path.read_bytes()

    if package[:4] not in {b"LIVE", b"PIRS", b"CON "}:
        raise SystemExit(f"{package_path} does not look like an Xbox 360 STFS package")

    entries = parse_table(package)
    if not entries:
        raise SystemExit("No file table entries were found")

    output_dir.mkdir(parents=True, exist_ok=True)
    entries_by_index = {entry.index: entry for entry in entries}
    manifest: list[dict[str, object]] = []

    for entry in entries:
        rel_path = build_relative_path(entry, entries_by_index)
        dest = output_dir / rel_path

        manifest.append(
            {
                **asdict(entry),
                "relative_path": rel_path.as_posix(),
            }
        )

        if entry.is_dir:
            dest.mkdir(parents=True, exist_ok=True)
            continue

        dest.parent.mkdir(parents=True, exist_ok=True)
        data = extract_file(package, entry)
        dest.write_bytes(data)

    (output_dir / "_manifest.json").write_text(
        json.dumps(
            {
                "package": str(package_path),
                "type": package[:4].decode("ascii", errors="replace"),
                "entry_count": len(entries),
                "entries": manifest,
            },
            indent=2,
        ),
        encoding="utf-8",
    )

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
