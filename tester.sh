#!/usr/bin/env bash
# bash_tester.sh: compare output of system 'nm' vs custom './ft_nm' with colored output and summary
# Usage: bash bash_tester.sh [path_to_ft_nm] [path_to_binaries]

set -euo pipefail
IFS=$'\n\t'

FT_NM=${1:-./ft_nm}
BIN_DIR=${2:-./bin}

# Ensure ft_nm exists and is executable
if [[ ! -x "$FT_NM" ]]; then
  echo "Error: '$FT_NM' not found or not executable." >&2
  exit 1
fi

# Color codes
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# List of flags to test (empty means no flag)
FLAGS=( "" "-a" "-g" "-u" "-r" "-p" )

# Counters and lists
TOTAL=0
PASS=0
FAIL=0
FAIL_LIST=()

# Function: run diff between nm and ft_nm for given file and flag
run_test() {
  local file="$1"
  local flag="$2"
  local label

  label="file='$file' flags='$flag'"
  (( TOTAL++ ))
  echo "=== Testing: $label ==="

  # Capture outputs
  local sys_out ft_out
  sys_out=$(nm $flag "$file" | awk '{print $1, $2}')
  ft_out=$($FT_NM $flag "$file" | awk '{print $1, $2}')

  # Compare outputs
  if diff <(echo "$sys_out") <(echo "$ft_out") &>/dev/null; then
    (( PASS++ ))
    echo -e "[${GREEN}OK${NC}] $label"
  else
    (( FAIL++ ))
    FAIL_LIST+=("$label")
    echo -e "[${RED}FAIL${NC}] $label"
    echo "--- unified diff ---"
    diff -u <(echo "$sys_out") <(echo "$ft_out") || true
  fi
  echo
}

# Main: Mandatory pass (no flags)
for file in "$BIN_DIR"/*; do
  if [[ -f "$file" && -x "$file" ]]; then
    run_test "$file" ""
  fi
done

# Bonus: run with each flag
printf "\n===== Bonus: flag combinations =====\n"
for file in "$BIN_DIR"/*; do
  if [[ -f "$file" && -x "$file" ]]; then
    for flag in "${FLAGS[@]}"; do
      [[ -z "$flag" ]] && continue # skip empty flag
      run_test "$file" "$flag"
    done
  fi
done

# Summary
echo "=========================================="
echo "Total tests: $TOTAL"
echo -e "Passed: ${GREEN}$PASS${NC}"
echo -e "Failed: ${RED}$FAIL${NC}"
if (( FAIL > 0 )); then
  echo "\nFailed cases:"
  for case in "${FAIL_LIST[@]}"; do
    echo -e "  - ${RED}$case${NC}"
  done
fi
