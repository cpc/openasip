#ifndef INSTRUCTION_EXECUTOR_TEST_HH
#define OPERATION_DAG_TINSTRUCTION_EXECUTOR_TEST_HHEST_HH

#include <TestSuite.h>

#include <InstructionExecutor.hh>
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
    void testInitializeMachine();
    void testUnpackInstruction();
    void testExecuteInstruction32();
    void testExecuteInstruction64();

private:
    const char *machine_file_path = "data/start.adf";
};

void
InstructionExecutorTest::setUp() {
    char *error = NULL;
    int status = InitializeMachine(machine_file_path, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(error, nullptr);
}

void
InstructionExecutorTest::tearDown() {}
void
InstructionExecutorTest::testInitializeMachine() {
    char *error = NULL;
    int status = InitializeMachine(machine_file_path, &error);
    if (status == -1) {
        std::cerr << error << std::endl;
    }
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(error, nullptr);

    status = InitializeMachine("/improper/path/to/file", &error);
    TS_ASSERT_EQUALS(status, -1);
    TS_ASSERT_DIFFERS(error, nullptr);
}
void
InstructionExecutorTest::testUnpackInstruction() {
    char *result = NULL;
    char *error = NULL;
    int status = UnpackInstruction(opcodes[0], &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    std::string resultStr(result);
    TS_ASSERT_EQUALS(resultStr, "crc_xor_shift");

    status = UnpackInstruction(opcodes[1], &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    resultStr = result;
    TS_ASSERT_EQUALS(resultStr, "reflect32");

    status = UnpackInstruction(opcodes[2], &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    resultStr = result;
    TS_ASSERT_EQUALS(resultStr, "reflect8");

    status = UnpackInstruction(123456789, &result, &error);
    TS_ASSERT_EQUALS(status, -1);

    status = UnpackInstruction(-558, &result, &error);
    TS_ASSERT_EQUALS(status, -1);
}
void
InstructionExecutorTest::testExecuteInstruction32() {
    char *error = NULL;
    uint32_t result;

    int status = ExecuteInstruction32(opNames[0], inputs32, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 12820);

    status = ExecuteInstruction32(opNames[1], inputs32, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 1275068416);

    status = ExecuteInstruction32(opNames[2], inputs32, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 76);
}
void
InstructionExecutorTest::testExecuteInstruction64() {
    char *error = NULL;
    uint64_t result;

    int status = ExecuteInstruction64(opNames[0], inputs64, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 12820);

    status = ExecuteInstruction64(opNames[1], inputs64, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 1275068416);

    status = ExecuteInstruction64(opNames[2], inputs64, &result, &error);
    TS_ASSERT_EQUALS(status, 0);
    TS_ASSERT_EQUALS(result, 76);
}

#endif /* INSTRUCTION_EXECUTOR_TEST_HH */