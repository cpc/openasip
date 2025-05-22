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

/**
 * Should be called before other functions. Initializes the Machine object and
 * loads the custom ops map.
 * 
 * @param machinePath path to the .adf machine file.
 * @param error error messages in case of failure. Can also be a nullptr if
 * desired. Must be freed by the client.
 * @return 0 on success, -1 on failure.
 */
int initializeMachine(const char* machinePath, char** error);

/**
 * Unpacks a RISC-V R4-type instruction and returns its string representation if found from
 * the machine file. Remember to call Initialize machine first.
 * 
 * @param opcode full RISC-V opcode. Register values are ignored.
 * @param output The char* representation of the opcode, if it is found. Must be freed by the client.
 * @param error Error messages in case of failure. Must be freed by the client.
 * @return 0 on success, -1 on failure.
 */
int unpackInstruction(const uint32_t instruction, char** output, char** error);

/**
 * Executes a custom instruction. The instruction behavior is by default
 * located in ~/.openasip/opset 32 refers to the operation width, as well as
 * input and output sizes.
 * 
 * @param opName The operation name as it is in the machine file.
 * @param inputs Input value(s) of the operation. Can contain more values than
 * the operation needs, in that case only the first values will be used.
 * @param output The result of the operation.
 * @param error  Will not be touched in case of success. Must be freed by the client.
 * @return 0 on success, -1 on failure.
 */
int executeInstruction32(
    const char *opName, const uint32_t* inputs, uint32_t* output,
    char **error);

/**
 * Executes a custom instruction. The instruction behavior is by default
 * located in ~/.openasip/opset 64 refers to the operation width, as well as
 * input and output sizes.
 * 
 * @param opName The operation name as it is in the machine file.
 * @param inputs Input value(s) of the operation. Can contain more values than
 * the operation needs, in that case only the first values will be used.
 * @param output The result of the operation.
 * @param error  Will not be touched in case of success. Must be freed by the client.
 * @return 0 on success, -1 on failure.
 */
int executeInstruction64(
    const char* opName, const uint64_t* inputs, uint64_t* output,
    char** error);
}

#endif
