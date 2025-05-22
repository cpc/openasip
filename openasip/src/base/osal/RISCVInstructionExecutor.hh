/*
    Copyright (c) 2002-2025 Tampere University.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
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
int initializeMachine(const char *machinePath, char **error);

/**
 * Unpacks a Risc-V opcode and returns its string representation if found from
 * the machine file. Remember to call Initialize machine first.
 * 
 * @param opcode full RISC-V opcode. Register values are ignored.
 * @param output The char* representation of the opcode, if it is found. Must be freed by the client.
 * @param error Error messages in case of failure. Must be freed by the client.
 * @return 0 on success, -1 on failure.
 */
int unpackInstruction(const uint32_t opcode, char **output, char **error);

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
    const char *opName, const uint32_t *inputs, uint32_t *output,
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
    const char *opName, const uint64_t *inputs, uint64_t *output,
    char **error);
}

#endif
