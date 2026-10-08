#!/usr/bin/env python3

#  Copyright (C) 2025-2026 Tampere University.

#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
#  
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
#  
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
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
import shlex
import subprocess
import multiprocessing
import argparse


def parse_args():
    parser = argparse.ArgumentParser(
        description="Clone and build QEMU-OpenASIP for RISC-V instruction emulation"
    )
    parser.add_argument(
        "qemu_path",
        help="Path to clone/build QEMU-OpenASIP"
    )
    return parser.parse_args()

args = parse_args()

QEMU_PATH = args.qemu_path
qemu_build_dir = os.path.join(QEMU_PATH, 'build')
qemu_executable = os.path.join(qemu_build_dir, 'qemu-system-riscv32')
configure_cmd = "./configure --target-list=riscv32-softmmu,riscv64-softmmu"


def clone_qemu_openasip():    
    if os.path.exists(QEMU_PATH):
        try:
            result = subprocess.run(
                ["git", "pull"],
                check=True,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                cwd=QEMU_PATH
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
    
    try:
        result = subprocess.run(
            ["git", "clone", "https://github.com/cpc/qemu-openasip.git", QEMU_PATH],
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
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
    if not os.path.exists(QEMU_PATH):
        print(f"QEMU directory not found at {QEMU_PATH}")
        return False
    
    if os.path.exists(qemu_executable):
        return True
    
    try:
        result = subprocess.run(
            shlex.split(configure_cmd),
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            cwd=QEMU_PATH
        )
        
        num_cpus = multiprocessing.cpu_count()
        make_cmd = f"make -j{num_cpus}"
        result = subprocess.run(
            shlex.split(make_cmd),
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            cwd=QEMU_PATH
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

def install_qemu():
    if not clone_qemu_openasip():
        return False
    return build_qemu()

if __name__ == "__main__":
    success = install_qemu()
    exit(0 if success else 1)
    

