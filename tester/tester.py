import subprocess
import difflib
from termcolor import colored

def run_command(cmd):
    process = subprocess.Popen(
        cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
    )
    stdout, stderr = process.communicate()
    return stdout, stderr

def diff_output(reference, test):
    return list(difflib.unified_diff(
        reference.splitlines(keepends=True),
        test.splitlines(keepends=True),
        lineterm=''
    ))

def print_diff(tag, diff_lines):
    if not diff_lines:
        return
    print(colored(f"[stderr][{tag}]", "yellow"))
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
                print_diff("or_nm", diff_output(stderr_or, stderr_ft))
                print_diff("ft_nm", diff_output(stderr_ft, stderr_or))
            if stdout_diff:
                print_diff("or_nm stdout", diff_output(stdout_or, stdout_ft))
                print_diff("ft_nm stdout", diff_output(stdout_ft, stdout_or))
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
