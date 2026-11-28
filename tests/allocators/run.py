#!/usr/bin/env python3
"""Build and run allocator tests on the host, isolating each case."""
import os
import pathlib
import signal
import subprocess
import sys
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
TESTS = pathlib.Path(__file__).resolve().parent
CASES = {
    "pfa": ("single", "ranges", "boundaries", "exhaustion"),
    "slab": ("initial_lists", "creation", "small", "tiny", "large", "generic", "failure", "hash_growth", "reap_live"),
}

def main():
    selected = sys.argv[1:]
    known = {f"{suite}.{case}" for suite, cases in CASES.items() for case in cases}
    if selected and not set(selected) <= known:
        print("Usage: tests/allocators/run.py [" + " | ".join(sorted(known)) + "] ...", file=sys.stderr)
        return 2
    cc = os.environ.get("CC", "cc")
    common = [cc, "-std=gnu11", "-g", "-O0", "-ffunction-sections", "-fdata-sections",
              "-I", str(ROOT / "kernel/include"), "-Wl,--gc-sections"]
    failed = 0
    with tempfile.TemporaryDirectory(prefix="nyaos-alloc-tests-") as tmp:
        binaries = {}
        for suite in CASES:
            if selected and not any(name.startswith(suite + ".") for name in selected):
                continue
            binary = pathlib.Path(tmp) / suite
            sources = [TESTS / (suite + "_test.c")]
            if suite == "slab":
                sources.append(ROOT / "kernel/arch/x86_64/slab.c")
            command = common + [str(source) for source in sources] + ["-o", str(binary)]
            try:
                subprocess.run(command, check=True, cwd=ROOT)
            except (OSError, subprocess.CalledProcessError) as exc:
                print(f"Build failed: {exc}", file=sys.stderr)
                return 2
            binaries[suite] = binary
        for suite, cases in CASES.items():
            for case in cases:
                name = f"{suite}.{case}"
                if selected and name not in selected:
                    continue
                try:
                    result = subprocess.run([str(binaries[suite]), case], cwd=tmp,
                                            capture_output=True, text=True, timeout=5)
                    if result.returncode == 0:
                        print(f"PASS {name}", flush=True)
                    else:
                        failed += 1
                        detail = (f"signal {signal.Signals(-result.returncode).name}"
                                  if result.returncode < 0 else f"exit {result.returncode}")
                        print(f"FAIL {name} ({detail})", flush=True)
                        if result.stderr:
                            print(result.stderr.strip(), flush=True)
                except subprocess.TimeoutExpired:
                    failed += 1
                    print(f"FAIL {name} (timed out after 5s)", flush=True)
    print(f"{failed} failed", flush=True)
    return 1 if failed else 0

if __name__ == "__main__":
    raise SystemExit(main())
