#!/usr/bin/env python3
### TCE TESTCASE 
### title: Evaluating OSAL instructions with QEMU. Remember to build QEMU first
#  Copyright (C) 2025 Tampere University.

#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.

#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.

#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
#  
#  
#  @file QEMUSystemtest.py
#  
#  System test for RISCVInstructionExecutor and QEMU-OpenASIP integration
#  
#  @author Eetu Soronen 2025 (eetu.soronen@tuni.fi)
#  @note rating: red
#  

import os
import subprocess
import shlex

QEMU_PATH = os.environ.get("OPENASIP_QEMU_PATH", os.path.expandvars("$HOME/qemu-openasip"))
qemu_build_dir =os.path.join(QEMU_PATH, 'build')

TEST_ROOT = os.path.dirname(os.path.abspath(__file__))
DATA_PATH = os.path.join(TEST_ROOT, "data")
MACHINE_FILE = os.path.join(DATA_PATH, "machine.adf")
PROGRAM_COMPILE_COMMAND = "riscv64-unknown-elf-gcc -T link.ld -nostdlib -march=rv32imac -mabi=ilp32 -o crc_program.elf start.S main.c crc.c -g"
KERNEL = os.path.join(DATA_PATH, "crc_program.elf")
EXPECTED_RESULT = """CHECK_VALUE: 0x62488E82
Slow CRC: 0x62488E82
Fast CRC: 0x62488E82
"""

def qemu_available():
    qemu_executable = os.path.join(qemu_build_dir, "qemu-system-riscv32")
    if not os.path.isfile(qemu_executable) or not os.access(qemu_executable, os.X_OK):
        return False

    return True


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
    qemu_executable = os.path.join(qemu_build_dir, "qemu-system-riscv32")
    qemu_cmd = f"{qemu_executable} " \
               f"-machine virt,oasip_machine={MACHINE_FILE} " \
               f"-bios none " \
               f"-nographic " \
               f"-no-reboot " \
               f"-kernel {KERNEL}"
    
    try:
        process = subprocess.Popen(
            shlex.split(qemu_cmd),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        output_lines = []
        stderr_lines = []
        found_expected_result = False
        timeout = 10

        try:
            stdout, stderr = process.communicate(timeout=timeout)
            output_lines = stdout.splitlines(True)
            stderr_lines = stderr.splitlines(True)
            found_expected_result = EXPECTED_RESULT.strip() in stdout
        except subprocess.TimeoutExpired:
            process.terminate()
            stdout, stderr = process.communicate()
            output_lines = stdout.splitlines(True)
            stderr_lines = stderr.splitlines(True)
            found_expected_result = EXPECTED_RESULT.strip() in stdout

                
        if not found_expected_result:
            print("STDOUT:")
            print(''.join(output_lines))
            print("\nSTDERR:")
            print(''.join(stderr_lines))

        return found_expected_result
        
    except Exception as e:
        print(f"QEMU execution failed: {e}")
        try:
            process.terminate()
        except:
            pass
        return False
    
def run_test():
    if not qemu_available():
        with open(os.path.join(TEST_ROOT, "tcetest_qemu-openasip.py.disabled"), 'w') as f:
            f.write("qemu is not available, assuming this is by intention and skipping...\n")
        exit(0) 
    if not compile_program():
        return False
    return run_qemu()

if __name__ == "__main__":
    SYSTEM_TCE_DEVL_MODE = os.environ.get('TCE_DEVEL_MODE', '') 
    os.environ['TCE_DEVEL_MODE'] = '0' # averts a linking error that causes the behavior to not load properly
    success = run_test()
    os.environ['TCE_DEVEL_MODE'] = SYSTEM_TCE_DEVL_MODE
    exit(0 if success else 1)