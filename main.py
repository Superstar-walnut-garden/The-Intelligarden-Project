import argparse
from pathlib import Path

from code_gen import run_codegen
from metadata_gen import generate_metadata


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generate enum metadata and the EnumCrafter header."
    )
    parser.add_argument(
        "config_file",
        nargs="?",
        type=Path,
        default=Path("config.json"),
        help="Path to the enum metadata config JSON file (default: config.json)",
    )
    parser.add_argument(
        "--metadata-only",
        action="store_true",
        help="Generate enum metadata without generating the EnumCrafter header",
    )
    args = parser.parse_args()

    if args.metadata_only:
        generate_metadata(args.config_file)
    else:
        run_codegen(args.config_file)


if __name__ == "__main__":
    main()
