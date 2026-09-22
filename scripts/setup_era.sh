#!/usr/bin/env bash
# scripts/setup_era.sh — fetch the era-accurate PS1 compiler toolchain used for
# functions that GCC 14.2 cannot match (Psy-Q ccpsx = GCC 2.7.2 fingerprint).
#
# Installs into tools/era/ (git-ignored, EXCEPT the locally patched maspsx
# files in MASPSX_TRACKED below — see .gitignore negations):
#   tools/era/gcc-2.7.2-psx/{cpp,cc1,gcc,...}   from decompals/old-gcc (0.17)
#   tools/era/gcc-2.8.1-psx/{cpp,cc1,gcc,...}   from decompals/old-gcc (0.17); selected
#                                                per leaf by ERA_CC1_VER=2.8.1
#   tools/era/maspsx/                            from mkst/maspsx (assembler-macro layer)
#
# Idempotent: skips downloads that are already present. Requires network + curl + git.
# These are open-source community rebuilds / tools — NOT proprietary Psy-Q SDK files.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ERA="$ROOT/tools/era"
GCC_RELEASE="https://github.com/decompals/old-gcc/releases/download/0.17"
GCC_URL="$GCC_RELEASE/gcc-2.7.2-psx.tar.gz"
GCC_DIR="$ERA/gcc-2.7.2-psx"
MASPSX_REPO="https://github.com/mkst/maspsx"

mkdir -p "$ERA"

# fetch_gcc <version>: install tools/era/gcc-<version>-psx from the same
# old-gcc release (idempotent; skipped when cc1+cpp are already executable).
fetch_gcc() {
    local ver="$1" dir="$ERA/gcc-$1-psx" url="$GCC_RELEASE/gcc-$1-psx.tar.gz" tmp
    if [[ -x "$dir/cc1" && -x "$dir/cpp" ]]; then
        echo "OK  era gcc present: $dir"
        return 0
    fi
    echo "Fetching gcc-$ver-psx ..."
    mkdir -p "$dir"
    tmp="$(mktemp)"
    curl -fsSL -o "$tmp" "$url"
    tar xzf "$tmp" -C "$dir"
    rm -f "$tmp"
    [[ -x "$dir/cc1" ]] || { echo "ERROR: cc1 missing after extract ($dir)" >&2; exit 1; }
    echo "OK  installed $dir"
}

fetch_gcc 2.7.2   # default era cc1 (Psy-Q ccpsx fingerprint)
fetch_gcc 2.8.1   # per-leaf ERA_CC1_VER=2.8.1 (fills the delay slots 2.7.2 leaves to a nop)

# Repo-tracked files under tools/era/maspsx (un-ignored via .gitignore
# negations) that carry LOCAL patches. A (re)clone copies the upstream working
# tree AROUND these and restores them from git if absent — upstream must never
# overwrite them, and no .git may be left behind (an embedded repo would make
# them untrackable by the parent repo).
MASPSX_TRACKED=(
    "maspsx/__init__.py"
    "tests/test_fill_store_delay_slot.py"
    "tests/test_fill_epilogue_delay_slot.py"
    "tests/test_three_word_symbol_store.py"
    "tests/test_dispatch_fold.py"
    "tests/test_symbol_load_dest_temp.py"
    "tests/test_symbol_at_temp.py"
    "tests/test_lo_store_passthrough.py"
    "tests/test_fill_branch_delay_slot.py"
    "tests/test_extern_gp_load_delay.py"
    "tests/test_fill_call_delay_slot.py"
)

if [[ -f "$ERA/maspsx/maspsx.py" ]]; then
    echo "OK  maspsx present: $ERA/maspsx"
else
    echo "Cloning maspsx ..."
    tmp="$(mktemp -d)"
    git clone -q --depth 1 "$MASPSX_REPO" "$tmp/maspsx"
    mkdir -p "$ERA/maspsx"
    excludes=(--exclude='./.git')
    for f in "${MASPSX_TRACKED[@]}"; do excludes+=("--exclude=./$f"); done
    ( cd "$tmp/maspsx" && tar cf - "${excludes[@]}" . ) | ( cd "$ERA/maspsx" && tar xf - )
    rm -rf "$tmp"
    for f in "${MASPSX_TRACKED[@]}"; do
        if [[ ! -f "$ERA/maspsx/$f" ]]; then
            if git -C "$ROOT" ls-files --error-unmatch "tools/era/maspsx/$f" >/dev/null 2>&1; then
                git -C "$ROOT" checkout -- "tools/era/maspsx/$f"
                echo "OK  restored tracked file from git: tools/era/maspsx/$f"
            else
                echo "WARNING: tools/era/maspsx/$f is un-ignored but not in git; local patches NOT restored" >&2
            fi
        fi
    done
    [[ -f "$ERA/maspsx/maspsx.py" ]] || { echo "ERROR: maspsx.py missing" >&2; exit 1; }
    echo "OK  cloned $ERA/maspsx"
fi

# Smoke test: era cc1 must reproduce the lui;ori const fingerprint GCC 14.2 can't.
echo "Smoke test: era const-synthesis fingerprint ..."
t="$(mktemp -d)"; printf 'int f(void){return 0x7F7F7F;}\n' >"$t/x.c"
"$GCC_DIR/cpp" "$t/x.c" >"$t/x.i" 2>/dev/null
"$GCC_DIR/cc1" -quiet -O2 -G0 "$t/x.i" -o "$t/x.s" 2>/dev/null
if grep -q 'ori' "$t/x.s"; then echo "OK  era emits lui;ori (matches retail)"; else
    echo "ERROR: era cc1 did not emit ori — wrong compiler?" >&2; exit 1; fi
# The 2.8.1 pair must accept the same cc1 flag set the build profiles use.
"$ERA/gcc-2.8.1-psx/cpp" "$t/x.c" >"$t/y.i" 2>/dev/null
"$ERA/gcc-2.8.1-psx/cc1" -quiet -O2 -G0 "$t/y.i" -o "$t/y.s" 2>/dev/null \
    && echo "OK  gcc-2.8.1-psx cc1 compiles (ERA_CC1_VER=2.8.1 available)" \
    || { echo "ERROR: gcc-2.8.1-psx cc1 failed the smoke compile" >&2; exit 1; }
rm -rf "$t"
echo "Era toolchain ready. The YAML-derived build plan will use it for era-profile leaves."
