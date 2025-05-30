#!/usr/bin/env python3
import os
import subprocess
import shlex
import time
import multiprocessing

QEMU_PATH = os.path.expandvars("$HOME/qemu-openasip/build")
TEST_ROOT = os.path.dirname(os.path.abspath(__file__))
DATA_PATH = os.path.join(TEST_ROOT, "data")
MACHINE_FILE = os.path.join(DATA_PATH, "machine.adf")
PROGRAM_COMPILE_COMMAND = "riscv64-unknown-elf-gcc -T link.ld -nostdlib -march=rv32imac -mabi=ilp32 -o crc_program.elf start.S main.c crc.c -g"
KERNEL = os.path.join(DATA_PATH, "crc_program.elf")
OUTPUT_FILE = os.path.join(DATA_PATH, "qemu_test_result.txt")
EXPECTED_RESULT = """CHECK_VALUE: 0x62488E82
Slow CRC: 0x62488E82
Fast CRC: 0x62488E82
"""

def clone_qemu_openasip():
    qemu_dir = os.path.expandvars("$HOME/qemu-openasip")
    
    if os.path.exists(qemu_dir):
        original_dir = os.getcwd()
        try:
            os.chdir(qemu_dir)
            result = subprocess.run(
                ["git", "pull"],
                check=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True
            )
            return True
        except subprocess.CalledProcessError as e:
            print(f"Failed to update repository: {e}")
            print(f"stdout: {e.stdout}")
            print(f"stderr: {e.stderr}")
            return False
        except Exception as e:
            print(f"Error updating repository: {e}")
            return False
        finally:
            os.chdir(original_dir)
    
    try:
        result = subprocess.run(
            ["git", "clone", "https://github.com/cpc/qemu-openasip.git", qemu_dir],
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        return True
    except subprocess.CalledProcessError as e:
        print(f"Failed to clone repository: {e}")
        print(f"stdout: {e.stdout}")
        print(f"stderr: {e.stderr}")
        return False
    except Exception as e:
        print(f"Error in clone_qemu_openasip: {e}")
        return False

def build_qemu():
    qemu_dir = os.path.expandvars("$HOME/qemu-openasip")
    qemu_executable = os.path.join(QEMU_PATH, "qemu-system-riscv32")
    
    if not os.path.exists(qemu_dir):
        print(f"QEMU OpenASIP directory not found at {qemu_dir}")
        return False
    
    if os.path.exists(qemu_executable):
        return True
    
    original_dir = os.getcwd()
    try:
        os.chdir(qemu_dir)        
        configure_cmd = "./configure --target-list=riscv32-softmmu"
        result = subprocess.run(
            shlex.split(configure_cmd),
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        
        num_cpus = multiprocessing.cpu_count()
        make_cmd = f"make -j{num_cpus}"
        result = subprocess.run(
            shlex.split(make_cmd),
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        
        return True
    except subprocess.CalledProcessError as e:
        print(f"Failed to build QEMU OpenASIP: {e}")
        print(f"stdout: {e.stdout}")
        print(f"stderr: {e.stderr}")
        return False
    except Exception as e:
        print(f"Error in build_qemu: {e}")
        return False
    finally:
        os.chdir(original_dir)

def compile_program():
    original_dir = os.getcwd()
    try:
        os.chdir(DATA_PATH)        
        result = subprocess.run(
            shlex.split(PROGRAM_COMPILE_COMMAND),
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        return True
    except subprocess.CalledProcessError as e:
        print(f"Compilation failed: {e}")
        print(f"stdout: {e.stdout}")
        print(f"stderr: {e.stderr}")
        return False
    finally:
        os.chdir(original_dir)


def run_qemu():
    qemu_executable = os.path.join(QEMU_PATH, "qemu-system-riscv32")
    qemu_cmd = f"{qemu_executable} " \
               f"-machine virt,openasip_machine_path={MACHINE_FILE} " \
               f"-bios none " \
               f"-serial file:{OUTPUT_FILE} " \
               f"-nographic " \
               f"-no-reboot " \
               f"-kernel {KERNEL}"
    
    try:
        if os.path.exists(OUTPUT_FILE):
            os.remove(OUTPUT_FILE)
            
        process = subprocess.Popen(
            shlex.split(qemu_cmd),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE
        )
        
        start_time = time.time()
        timeout = 15
        
        while time.time() - start_time < timeout:
            if os.path.exists(OUTPUT_FILE):
                with open(OUTPUT_FILE, 'r') as f:
                    content = f.read()
                    if EXPECTED_RESULT.strip() in content:
                        process.terminate()
                        process.wait(timeout=2)
                        return True
            time.sleep(0.1)
        
        print(f"Timeout after {timeout} seconds - terminating QEMU")
        process.terminate()
        process.wait(timeout=2)    
        return False
        
    except Exception as e:
        print(f"QEMU execution failed: {e}")
        try:
            process.terminate()
        except:
            pass
        return False
    
def compare_output():
    try:
        with open(OUTPUT_FILE, 'r') as f:
            actual_output = f.read()
        
        if actual_output.startswith(EXPECTED_RESULT):
            return True
        else:
            print("Test FAILED: Output does not match expected result")
            print(f"Expected:\n{EXPECTED_RESULT}")
            print(f"Actual:\n{actual_output}...")
            return False
    except Exception as e:
        print(f"Error comparing output: {e}")
        return False

def run_test():
    if not clone_qemu_openasip():
        return False
    if not build_qemu():
        return False
    if not compile_program():
        return False
    run_qemu()
    return compare_output()

if __name__ == "__main__":
    SYSTEM_TCE_DEVL_MODE = os.environ.get('TCE_DEVEL_MODE') 
    os.environ['TCE_DEVEL_MODE'] = '0' # 🤷
    success = run_test()
    os.environ['TCE_DEVEL_MODE'] = SYSTEM_TCE_DEVL_MODE
    exit(0 if success else 1)