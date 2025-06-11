/*
 Copyright (C) 2025 Tampere University.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */
/**
 * @file RISCVInstructionExecutor.hh
 *
 * Declaration of RISCVInstructionExecutor class.
 *
 * @author Eetu Soronen 2025 (eetu.soronen@tuni.fi)
 * @note rating: red
 */

#ifndef INSTRUCTION_EXECUTOR_HH
#define INSTRUCTION_EXECUTOR_HH

#include <stdint.h>

extern "C" {

int initializeMachine(const char* machinePath, char** error);

int resetMachine();

int unpackInstruction(uint32_t instruction, char** output, char** error);

int executeInstruction32(
    const char* opName, const uint32_t* inputs, uint32_t inputsCount,
    uint32_t* output, char** error);

int executeInstruction64(
    const char* opName, const uint64_t* inputs, uint32_t inputsCount,
    uint64_t* output, char** error);
}

#endif
