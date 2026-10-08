/*
 Copyright (C) 2025-2026 Tampere University.

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
