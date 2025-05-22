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
 * @file RISCVInstructionExecutorTest.hh 
 *
 * A test suite for RISCVInstructionExecutor
 *
 * @author Eetu Soronen 2025 (eetu.soronen@tuni.fi)
 */    

#ifndef INSTRUCTION_EXECUTOR_TEST_HH
#define OPERATION_DAG_TINSTRUCTION_EXECUTOR_TEST_HHEST_HH

#include <TestSuite.h>

#include <RISCVInstructionExecutor.hh>
#include <string>
#include <vector>

#include "BEMGenerator.hh"
#include "BinaryEncoding.hh"
#include "InstructionFormat.hh"
#include "Machine.hh"
#include "Operation.hh"
#include "OperationContext.hh"
#include "OperationPool.hh"
#include "RISCVFields.hh"
#include "RISCVTools.hh"

const uint32_t opcodes[] = {
    0x00730e8b, /* .insn r 0x0B, 0x00, 0x00, t4, t1, t2    # oa_crc_xor_shift
                 */
    0x01d3138b, /* .insn r 0x0B, 0x01, 0x00, t2, t1, t4    # oa_reflect32 */
    0x00732e8b  /* .insn r 0x0B, 0x02, 0x00, t4, t1, t2    # oa_reflect8 */
};

const char *opNames[] = {"crc_xor_shift", "reflect32", "reflect8"};
const uint32_t inputs32[] = {50, 20, 1280};
const uint64_t inputs64[] = {50, 20, 1280};

class InstructionExecutorTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testinitializeMachine();
    void testunpackInstruction();
    void testExecuteInstruction32();
    void testExecuteInstruction64();

private:
    const char *machine_file_path = "data/start.adf";
};

void
InstructionExecutorTest::setUp() {
    char *error = NULL;
    int status = initializeMachine(machine_file_path, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(error, nullptr);
}

void
InstructionExecutorTest::tearDown() {}
void
InstructionExecutorTest::testinitializeMachine() {
    char *error = NULL;
    int status = initializeMachine(machine_file_path, &error);
    if (status == -1) {
        std::cerr << error << std::endl;
    }
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(error, nullptr);

    status = initializeMachine("/improper/path/to/file", &error);
    TS_ASSERT_EQUALS(status, -1);
    TS_ASSERT_DIFFERS(error, nullptr);
}
void
InstructionExecutorTest::testunpackInstruction() {
    char *result = NULL;
    char *error = NULL;
    int status = unpackInstruction(opcodes[0], &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    std::string resultStr(result);
    TS_ASSERT_EQUALS(resultStr, "crc_xor_shift");

    status = unpackInstruction(opcodes[1], &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    resultStr = result;
    TS_ASSERT_EQUALS(resultStr, "reflect32");

    status = unpackInstruction(opcodes[2], &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    resultStr = result;
    TS_ASSERT_EQUALS(resultStr, "reflect8");

    status = unpackInstruction(123456789, &result, &error);
    TS_ASSERT_EQUALS(status, -1);

    status = unpackInstruction(-558, &result, &error);
    TS_ASSERT_EQUALS(status, -1);
}
void
InstructionExecutorTest::testExecuteInstruction32() {
    char *error = NULL;
    uint32_t result;

    int status = executeInstruction32(opNames[0], inputs32, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 12820);

    status = executeInstruction32(opNames[1], inputs32, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 1275068416);

    status = executeInstruction32(opNames[2], inputs32, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 76);
}
void
InstructionExecutorTest::testExecuteInstruction64() {
    char *error = NULL;
    uint64_t result;

    int status = executeInstruction64(opNames[0], inputs64, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 12820);

    status = executeInstruction64(opNames[1], inputs64, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 1275068416);

    status = executeInstruction64(opNames[2], inputs64, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 76);
}

#endif /* INSTRUCTION_EXECUTOR_TEST_HH */