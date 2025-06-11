#!/usr/bin/env python3

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
    

