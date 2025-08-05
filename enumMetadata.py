#!/usr/bin/env python3
"""
enum_metadata.py

Generate enum_metadata.json from C++ sources.
"""

import argparse
import json
import logging
from dataclasses import dataclass, asdict
from pathlib import Path
from typing import List, Dict, Optional
from concurrent.futures import ThreadPoolExecutor

from clang import cindex

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s %(levelname)s │ %(message)s",
    datefmt="%Y-%m-%d %H:%M:%S"
)

SOURCE_ROOTS: List[Path] = []  # Populated from config


@dataclass
class Enumerator:
    name: str
    value: int


@dataclass
class EnumMeta:
    qualified_name: str
    file: str
    line: int
    is_scoped: bool
    enumerators: List[Enumerator]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Scan C++ files and emit enum metadata as JSON."
    )
    parser.add_argument(
        "config",
        type=Path,
        help="Path to JSON config file (see sample below)."
    )
    return parser.parse_args()


def load_config(path: Path) -> Dict:
    with path.open() as f:
        cfg = json.load(f)
    assert "sources" in cfg, "'sources' key missing in config"
    assert "output" in cfg, "'output' key missing in config"
    return cfg


def collect_files(sources: List[str]) -> List[Path]:
    global SOURCE_ROOTS
    files: List[Path] = []
    roots: List[Path] = []

    for entry in sources:
        p = Path(entry).resolve()
        if p.is_dir():
            roots.append(p)
            for ext in ("*.cpp", "*.cc", "*.cxx", "*.hpp", "*.h", "*.hxx"):
                files.extend(p.rglob(ext))
        elif p.is_file():
            roots.append(p.parent)
            files.append(p)
        else:
            matched = list(Path(".").glob(entry))
            for m in matched:
                roots.append(m.parent.resolve())
            files.extend(matched)

    SOURCE_ROOTS = sorted(set(roots))
    unique = sorted({f.resolve() for f in files})
    logging.info(f"Found {len(unique)} source files in {len(SOURCE_ROOTS)} root(s)")
    return unique


def is_project_file(file_path: Optional[str]) -> bool:
    if not file_path:
        return False
    try:
        resolved = Path(file_path).resolve()
        return any(resolved.is_relative_to(root) for root in SOURCE_ROOTS)
    except Exception:
        return False


def build_index(include_dirs: List[str]) -> cindex.Index:
    args = [
        "-x", "c++",
        "-std=c++17",
        "-fsyntax-only",
        "-w"
    ] + [f"-I{d}" for d in include_dirs]
    logging.info(f"libclang args: {args}")
    return cindex.Index.create(), args


def visit(cursor: cindex.Cursor,
          namespace: List[str],
          results: List[EnumMeta]) -> None:
    from clang.cindex import CursorKind

    # Skip non-project files
    if cursor.location.file:
        file_path = str(cursor.location.file.name)
        if not is_project_file(file_path):
            return

    if cursor.kind in (CursorKind.NAMESPACE, CursorKind.STRUCT_DECL, CursorKind.CLASS_DECL):
        name = cursor.spelling or "<anon>"
        namespace.append(name)

    if cursor.kind == CursorKind.ENUM_DECL:
        if cursor.spelling:
            qualified = "::".join(namespace + [cursor.spelling])
            is_scoped = cursor.is_scoped_enum()
            file = cursor.location.file.name
            line = cursor.location.line
            enumerators: List[Enumerator] = []
            for enum_constant in cursor.get_children():
                if enum_constant.kind == CursorKind.ENUM_CONSTANT_DECL:
                    val = enum_constant.enum_value
                    enumerators.append(
                        Enumerator(name=enum_constant.spelling, value=val)
                    )
            results.append(EnumMeta(
                qualified_name=qualified,
                file=file,
                line=line,
                is_scoped=is_scoped,
                enumerators=enumerators
            ))

    for child in cursor.get_children():
        visit(child, namespace.copy(), results)


def parse_file(index: cindex.Index,
               args: List[str],
               filepath: Path) -> List[EnumMeta]:
    tu = index.parse(
        str(filepath),
        args=args,
        options=cindex.TranslationUnit.PARSE_SKIP_FUNCTION_BODIES
    )
    results: List[EnumMeta] = []
    visit(tu.cursor, [], results)
    return results


def parse_all(index: cindex.Index,
              args: List[str],
              sources: List[Path]) -> List[EnumMeta]:
    seen = set()
    all_results: List[EnumMeta] = []

    def parse_and_filter(src: Path) -> List[EnumMeta]:
        metas = parse_file(index, args, src)
        filtered = []
        for m in metas:
            key = (m.qualified_name, m.file, m.line)
            if key not in seen:
                seen.add(key)
                filtered.append(m)
        return filtered

    with ThreadPoolExecutor() as executor:
        futures = [executor.submit(parse_and_filter, src) for src in sources]
        for f in futures:
            all_results.extend(f.result())

    return all_results

def main() -> None:
    opts = parse_args()
    cfg = load_config(opts.config)

    sources = collect_files(cfg["sources"])
    inc_dirs = cfg.get("include_dirs", [])
    output = Path(cfg["output"])
    index, clang_args = build_index(inc_dirs)

    all_meta = parse_all(index, clang_args, sources)

    json_out = [asdict(m) for m in all_meta]
    output.parent.mkdir(parents=True, exist_ok=True)
    with output.open("w") as f:
        json.dump(json_out, f, indent=2)
    logging.info(f"Wrote metadata for {len(all_meta)} enums to {output}")


if __name__ == "__main__":
    main()