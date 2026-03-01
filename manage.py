import os
import subprocess
import sys
import shutil
import pathlib
import multiprocessing
import json

PROJECT_NAME = "pawspective-client"

CONFIG = {
    "NPROCS": multiprocessing.cpu_count()
}

try: 
    import local_config
    for key in CONFIG:
        if hasattr(local_config, key):
            CONFIG[key] = getattr(local_config, key)
except ImportError:
    pass

NPROCS = CONFIG["NPROCS"]

def run_command(command, cwd=None, env=None):
    custom_env = os.environ.copy()
    if env:
        custom_env.update(env)
    
    custom_env["CLICOLOR_FORCE"] = "1"
    custom_env["GTEST_COLOR"] = "1"
    custom_env["PYTEST_ADDOPTS"] = "--color=yes"
    custom_env["CMAKE_COLOR_DIAGNOSTICS"] = "ON"

    print(f"\033[94m--> Running: {' '.join(command)}\033[0m") 
    
    try:
        process = subprocess.Popen(
            command, 
            cwd=cwd, 
            env=custom_env, 
            shell=(os.name == 'nt')
        )
        process.wait()
        
        if process.returncode != 0:
            print(f"\033[91mError: Command failed with exit code {process.returncode}\033[0m")
            sys.exit(process.returncode)
            
    except Exception as e:
        print(f"\033[91mUnexpected error: {e}\033[0m")
        sys.exit(1)

def get_cpp_files():
    extensions = {'.cpp', '.hpp'}
    files = []
    for path in ['src', 'include']:
        if os.path.exists(path):
            for p in pathlib.Path(path).rglob('*'):
                if p.suffix in extensions:
                    files.append(str(p.as_posix()))
    return files

def filter_compile_commands(preset="debug"):
    """Remove MOC files from compile_commands.json while creating a backup."""
    compile_commands_path = pathlib.Path(f"build-{preset}/compile_commands.json")
    backup_path = pathlib.Path(f"build-{preset}/compile_commands.json.backup")
    
    if not compile_commands_path.exists():
        return
    
    if not backup_path.exists():
        with open(compile_commands_path, 'r', encoding='utf-8') as f:
            commands = json.load(f)
        
        with open(backup_path, 'w', encoding='utf-8') as f:
            json.dump(commands, f, indent=2)
        
        filtered = [
            cmd for cmd in commands
            if not any(pattern in cmd['file'] for pattern in [
                '_autogen',
                'moc_',
                'qrc_',
                'ui_',
                '_qmltyperegistrations',
                'qmlcache',             
                '.rcc',
            ])
        ]
        
        with open(compile_commands_path, 'w', encoding='utf-8') as f:
            json.dump(filtered, f, indent=2)
        
        print(f"✓ Filtered compile_commands.json: {len(commands)} -> {len(filtered)} entries")

def restore_compile_commands(preset="debug"):
    """Restore the original compile_commands.json from the backup."""
    compile_commands_path = pathlib.Path(f"build-{preset}/compile_commands.json")
    backup_path = pathlib.Path(f"build-{preset}/compile_commands.json.backup")
    
    if backup_path.exists():
        shutil.copy(backup_path, compile_commands_path)
        backup_path.unlink()
        print("✓ Restored compile_commands.json")

def build(preset="debug"):
    """Configure and build the project."""
    run_command(["cmake", "--preset", preset])
    run_command(["cmake", "--build", "--preset", preset, "-j", str(NPROCS)])

def run(preset="debug"):
    """Run the application."""
    ext = ".exe" if os.name == 'nt' else ""
    exe_path = pathlib.Path(f"build-{preset}") / f"{PROJECT_NAME}{ext}"
    
    if not exe_path.exists():
        print(f"Binary not found at {exe_path}. Building first...")
        build(preset)

    if os.name == 'nt':
        run_command(["windeployqt", str(exe_path)])
        
    run_command([str(exe_path)])

def clean():
    """Clean temporary files."""
    dirs_to_remove = list(pathlib.Path().glob("build*")) + [pathlib.Path(".cache")]
    for d in dirs_to_remove:
        if d.is_dir():
            print(f"Removing {d}...")
            shutil.rmtree(d, ignore_errors=True)
    
    file_to_remove = pathlib.Path("compile_commands.json")
    if file_to_remove.exists():
        file_to_remove.unlink()

def format_code():
    """Format code with clang-format."""
    files = get_cpp_files()
    if not files:
        print("No files found to format.")
        return
    print("Formatting C++ files with clang-format...")
    run_command(["clang-format", "-i"] + files)

def format_check():
    """Check code formatting without changes."""
    files = get_cpp_files()
    if not files:
        print("No files found to check.")
        return
    print("Checking C++ code formatting...")
    run_command(["clang-format", "--dry-run", "--Werror"] + files)

def cppcheck_lint():
    """Run cppcheck for static analysis."""
    if not pathlib.Path("build-debug/compile_commands.json").exists():
        print("compile_commands.json not found. Building first...")
        build()
    
    try:
        filter_compile_commands()
        
        print("Running cppcheck...")
        cmd = [
            "cppcheck",
            "--enable=all",
            "--library=qt",
            "--error-exitcode=1",
            "--project=build-debug/compile_commands.json",
            "--file-filter=src/*",
            "--file-filter=include/*",
            "-I", "include",
            "--suppress=normalCheckLevelMaxBranches",
            "--suppress=checkersReport"
        ]
        
        if pathlib.Path(".cppcheck_suppressions").exists():
            cmd.extend(["--suppressions-list=.cppcheck_suppressions"])
        
        run_command(cmd)
    finally:
        restore_compile_commands()

def tidy_lint():
    """Run clang-tidy."""
    if not pathlib.Path("build-debug/compile_commands.json").exists():
        print("compile_commands.json not found. Building first...")
        build()
    
    run_clang_tidy_path = shutil.which("run-clang-tidy")
    if not run_clang_tidy_path and os.name == 'nt':
        paths = os.environ.get("PATH", "").split(os.pathsep)
        for p in paths:
            full_path = os.path.join(p, "run-clang-tidy")
            if os.path.isfile(full_path):
                run_clang_tidy_path = full_path
                break
    if not run_clang_tidy_path:
        print("run-clang-tidy not found in PATH.")
        sys.exit(1)
    
    try:
        filter_compile_commands()
        
        print(f"Running clang-tidy via {run_clang_tidy_path}...")
        cmd = [sys.executable, run_clang_tidy_path,
               "-p", str(pathlib.Path("build-debug").resolve()), 
               "-j", str(NPROCS),
               f"-config-file={pathlib.Path(".clang-tidy").resolve()}",
               "-header-filter=/src/.*",
               "-extra-arg=-Wno-unknown-argument",
               '-extra-arg=-std=c++20',
               "-extra-arg=--target=x86_64-w64-windows-gnu",
               ] 
        
        run_command(cmd)
    finally:
        restore_compile_commands()

def lint(steps="all"):
    """Run all linters."""
    lint_steps = ["format-check", "cppcheck", "tidy"] if steps == "all" else steps.split(",")
    
    for step in lint_steps:
        step = step.strip()
        if step == "format-check":
            format_check()
        elif step == "cppcheck":
            cppcheck_lint()
        elif step == "tidy":
            tidy_lint()
    
    print("\n\033[92m✓ All lint checks passed!\033[0m")

def main():
    if len(sys.argv) < 2:
        print(
            "Usage: python manage.py <command> [options]\n"
            "\nCommands:\n"
            "  build [preset]           - Build project (debug/release, default: debug)\n"
            "  run [preset]             - Run application (default: debug)\n"
            "  clean                    - Clean build artifacts\n"
            "  format                   - Format code with clang-format\n"
            "  format-check             - Check code formatting without changes\n"
            "  cppcheck                 - Run cppcheck static analysis\n"
            "  tidy                     - Run clang-tidy analysis\n"
            "  lint [steps]             - Run all linters (format-check, cppcheck, tidy)\n"
            "                             Optional: specify steps separated by comma\n"
        )
        sys.exit(1)

    cmd = sys.argv[1]
    preset = sys.argv[2] if len(sys.argv) > 2 else "debug"

    if cmd == "build":
        build(preset)
    elif cmd == "run":
        run(preset)
    elif cmd == "clean":
        clean()
    elif cmd == "format":
        format_code()
    elif cmd == "format-check":
        format_check()
    elif cmd == "cppcheck":
        cppcheck_lint()
    elif cmd == "tidy":
        tidy_lint()
    elif cmd == "lint":
        steps = sys.argv[2] if len(sys.argv) > 2 else "all"
        lint(steps)
    else:
        print(f"Unknown command: {cmd}")
        sys.exit(1)

if __name__ == "__main__":
    main()