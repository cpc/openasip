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
    0x00730e8b, /* .insn r 0x0B, 0x00, 0x00, t4, t1, t2  # oa_crc_xor_shift*/
    0x01d3138b, /* .insn r 0x0B, 0x01, 0x00, t2, t1, t4  # oa_reflect32 */
    0x00732e8b  /* .insn r 0x0B, 0x02, 0x00, t4, t1, t2  # oa_reflect8 */
};

const char* opNames[] = {"crc_xor_shift", "reflect32", "reflect8"};
const uint32_t inputs32[] = {50, 20, 1280};
const uint64_t inputs64[] = {50, 20, 1280};
const uint32_t inputCount = 3;

class InstructionExecutorTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();
    void testinitializeMachine();
    void testunpackInstruction();
    void testExecuteInstruction32();
    void testExecuteInstruction64();

private:
    const char* machine_file_path = "data/start.adf";
};

void
InstructionExecutorTest::setUp() {
    char* error = nullptr;

    int status = initializeMachine(machine_file_path, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(error, nullptr);

    if (error) {
        free(error);
    }
}

void
InstructionExecutorTest::tearDown() {}

void
InstructionExecutorTest::testinitializeMachine() {
    char* error = nullptr;

    int status = initializeMachine(machine_file_path, &error);

    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(error, nullptr);

    status = initializeMachine("/improper/path/to/file", &error);
    TS_ASSERT_EQUALS(status, -1);
    TS_ASSERT_DIFFERS(error, nullptr);

    if (error) {
        free(error);
    }
}
void
InstructionExecutorTest::testunpackInstruction() {
    char* error = nullptr;
    char* operationName = nullptr;

    int status = unpackInstruction(opcodes[0], &operationName, &error);
    TS_ASSERT_EQUALS(status, 0);
    std::string resultStr(operationName);
    TS_ASSERT_EQUALS(resultStr, "crc_xor_shift");

    status = unpackInstruction(opcodes[1], &operationName, &error);
    TS_ASSERT_EQUALS(status, 0);
    resultStr = operationName;
    TS_ASSERT_EQUALS(resultStr, "reflect32");

    status = unpackInstruction(opcodes[2], &operationName, &error);
    TS_ASSERT_EQUALS(status, 0);
    resultStr = operationName;
    TS_ASSERT_EQUALS(resultStr, "reflect8");

    TS_ASSERT_EQUALS(error, nullptr);

    status = unpackInstruction(123456789, &operationName, &error);
    TS_ASSERT_EQUALS(status, -1);

    status = unpackInstruction(-558, &operationName, &error);
    TS_ASSERT_EQUALS(status, -1);
    TS_ASSERT_DIFFERS(error, nullptr);

    if (error) {
        free(error);
    }

    if (operationName) {
        free(operationName);
    }
}
void
InstructionExecutorTest::testExecuteInstruction32() {
    char* error = nullptr;

    uint32_t result;

    int status = executeInstruction32(
        opNames[0], inputs32, inputCount, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 12820);

    status = executeInstruction32(
        opNames[1], inputs32, inputCount, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 1275068416);

    status = executeInstruction32(
        opNames[2], inputs32, inputCount, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 76);

    TS_ASSERT_EQUALS(error, nullptr);

    status = executeInstruction32(
        "unknown_instruction", inputs32, inputCount, &result, &error);
    TS_ASSERT_EQUALS(status, -1);
    TS_ASSERT_DIFFERS(error, nullptr);

    error = nullptr;
    status = executeInstruction32(opNames[2], inputs32, 0, &result, &error);
    TS_ASSERT_EQUALS(status, -1);
    TS_ASSERT_DIFFERS(error, nullptr);

    if (error) {
        free(error);
    }
}
void
InstructionExecutorTest::testExecuteInstruction64() {
    char* error = nullptr;

    uint64_t result;

    int status = executeInstruction64(
        opNames[0], inputs64, inputCount, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 12820);

    status = executeInstruction64(
        opNames[1], inputs64, inputCount, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 1275068416);

    status = executeInstruction64(
        opNames[2], inputs64, inputCount, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 76);

    TS_ASSERT_EQUALS(error, nullptr);

    status = executeInstruction64(
        "unknown_instruction", inputs64, inputCount, &result, &error);
    TS_ASSERT_EQUALS(status, -1);
    TS_ASSERT_DIFFERS(error, nullptr);

    error = nullptr;
    status = executeInstruction64(opNames[2], inputs64, 0, &result, &error);
    TS_ASSERT_EQUALS(status, -1);
    TS_ASSERT_DIFFERS(error, nullptr);

    if (error) {
        free(error);
    }
}

#endif /* INSTRUCTION_EXECUTOR_TEST_HH */