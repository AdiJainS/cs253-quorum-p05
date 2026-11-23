#!/usr/bin/env python3
"""Build and run the Structure A tests with and without operation counters."""

import argparse
from pathlib import Path
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cxx", default="g++", help="C++17 compiler executable")
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    sources = sorted((root / "tests" / "structA").glob("test_*.cpp"))
    for counters in (False, True):
        mode = "counters-on" if counters else "counters-off"
        output = root / "build" / "tests" / "structA" / mode
        output.mkdir(parents=True, exist_ok=True)
        for source in sources:
            if source.stem == "test_counters" and not counters:
                continue
            binary = output / (source.stem + (".exe" if sys.platform == "win32" else ""))
            command = [args.cxx, "-std=c++17", "-O2", "-Wall", "-Wextra", "-pedantic"]
            command.extend(["-I", str(root / "include")])
            if counters:
                command.append("-DSTRUCTA_ENABLE_COUNTERS")
            command.extend([str(source), "-o", str(binary)])
            print(f"[{mode}] {source.stem}", flush=True)
            subprocess.run(command, cwd=root, check=True)
            subprocess.run([str(binary)], cwd=root, check=True)
    print("ALL STRUCTURE A TESTS PASSED", flush=True)


if __name__ == "__main__":
    main()
