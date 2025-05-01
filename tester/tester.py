import subprocess
import difflib
import re
from termcolor import colored

def run_command(cmd):
    process = subprocess.Popen(
        cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
    )
    stdout, stderr = process.communicate()
    return stdout, stderr

def strip_address(line):
    """Remove address from nm output line, keeping type and symbol name."""
    match = re.match(r'^[0-9a-fA-F]+\s+([A-Za-z])\s+(.+)$', line.strip())
    return f"{match.group(1)} {match.group(2)}" if match else line.strip()

def diff_output(reference, test):
    ref_lines = [strip_address(line) for line in reference.splitlines()]
    test_lines = [strip_address(line) for line in test.splitlines()]
    return list(difflib.unified_diff(
        ref_lines,
        test_lines,
        lineterm=''
    ))

def print_diff(tag, diff_lines):
    if not diff_lines:
        return
    print(colored(f"[DIFF][{tag}]", "yellow"))
    for line in diff_lines:
        color = "green" if line.startswith("+") else "red" if line.startswith("-") else "white"
        print(colored(line.rstrip(), color))

def main(file_path):
    with open(file_path, 'r') as f:
        commands = [line.strip() for line in f if line.strip()]

    passed = 0
    failed = 0

    for idx, command in enumerate(commands, 1):
        print(colored(f"[test {idx}] [command: \"{command}\"]", "cyan"))

        cmd_or = f"nm {command}"
        cmd_ft = f"./ft_nm {command}"

        stdout_or, stderr_or = run_command(cmd_or)
        stdout_ft, stderr_ft = run_command(cmd_ft)

        stderr_diff = diff_output(stderr_or, stderr_ft)
        stdout_diff = diff_output(stdout_or, stdout_ft)

        if not stderr_diff and not stdout_diff:
            print(colored("[OK]", "green"))
            passed += 1
        else:
            print(colored("[KO]", "red"))
            if stderr_diff:
                print_diff("stderr", stderr_diff)
            if stdout_diff:
                print_diff("stdout", stdout_diff)
            failed += 1
        print()

    print(colored(f"Tests passed: {passed}", "green"))
    print(colored(f"Tests failed: {failed}", "red"))

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 2:
        print("Usage: python3 test_diff.py commands.txt")
    else:
        main(sys.argv[1])
