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
 * @file RISCVInstructionExecutor.cc
 *
 * Definition of RISCVInstructionExecutor class.
 *
 * @author Eetu Soronen 2025 (eetu.soronen@tuni.fi)
 * @note rating: red
 */

#include <string>
#include <vector>

#include "OperationContext.hh"
#include "OperationPool.hh"
#include "Operation.hh"
#include "InstructionFormat.hh"

#include "RISCVTools.hh"
#include "Machine.hh"
#include "BEMGenerator.hh"
#include "BinaryEncoding.hh"
#include "RISCVFields.hh"

/**
 * Takes in a Machine object and returns a map of instruction names and their
 * encodings.
 * 
 * Helper function for the extern C functions.
 */
std::map<std::string, int>
findRISCVCustomOps(const TTAMachine::Machine &mach) {
    std::map<std::string, int> customOps;
    BinaryEncoding *bem = BEMGenerator(mach).generate();

    assert(bem != NULL);

    const std::vector<std::string> formatsToSearch = {
        RISCVFields::RISCV_R_TYPE_NAME,
        RISCVFields::RISCV_R1R_TYPE_NAME,
        RISCVFields::RISCV_R1_TYPE_NAME,
        RISCVFields::RISCV_R3R_TYPE_NAME};

    for (const std::string &fName : formatsToSearch) {
        InstructionFormat *format = bem->instructionFormat(fName);
        if (format == NULL) {
            continue;
        }
        for (int i = 0; i < format->operationCount(); i++) {
            const std::string op = format->operationAtIndex(i);
            if (RISCVFields::RISCVRTypeOperations.find(op) ==
                RISCVFields::RISCVRTypeOperations.end()) {
                customOps.insert({op, format->encoding(op)});
            }
        }
    }

    delete bem;
    return customOps;
}

extern "C" {

static std::unique_ptr<OperationPool> pool = nullptr;
static std::map<std::string, int> customOps;
static std::unique_ptr<TTAMachine::Machine> machine;

int
initializeMachine(const char *machinePath, char **error) {
    machine.reset();

    try {
        machine = std::unique_ptr<TTAMachine::Machine>(
            TTAMachine::Machine::loadFromADF(machinePath));
    } catch (const SerializerException &e) {
        if (error != nullptr) {
            std::string errorStr =
                std::string("InitializeMachine error: ") + e.errorMessage();
            *error = strdup(errorStr.c_str());
        }
        return -1;
    }

    if (pool == nullptr) {
        pool = std::make_unique<OperationPool>();
    }

    customOps = findRISCVCustomOps(*machine);

    return 0;
}

int
unpackInstruction(const uint32_t opcode, char **output, char **error) {
    if (machine == nullptr) {
        *error = strdup(
            "UnpackInstruction error: Machine not initialized. Call "
            "InitializeMachine first");
        return -1;
    }

    if (output == nullptr) {
        if (error != nullptr) {
            *error =
                strdup("UnpackInstruction error: Output parameter is null");
        }
        return -1;
    }

    uint32_t baseopcode = opcode & 0x7F;
    uint32_t funct3 = (opcode >> 12) & 0x7;
    uint32_t funct7 = (opcode >> 25) & 0x7F;
    uint32_t funct2 = (opcode >> 25) & 0x3;

    const unsigned OPC_CUSTOM_0 = 0b0001011;
    const unsigned OPC_CUSTOM_1 = 0b0101011;

    bool isCustom0 = (baseopcode == OPC_CUSTOM_0);
    bool isCustom1 = (baseopcode == OPC_CUSTOM_1);

    if (!isCustom0 && !isCustom1) {
        if (error != nullptr) {
            *error = strdup("UnpackInstruction error: Unknown baseopcode");
        }
        return -1;
    }

    std::string opName = "";
    for (const auto &op : customOps) {
        uint32_t encoding = op.second;
        bool isMatch = false;

        if (isCustom1 &&
            (static_cast<uint32_t>(RISCVTools::getFunc2Int(encoding)) ==
             funct2) &&
            (static_cast<uint32_t>(RISCVTools::getFunc3Int(encoding)) ==
             funct3)) {
            isMatch = true;
        } else if (
            isCustom0 &&
            (static_cast<uint32_t>(RISCVTools::getFunc7Int(encoding)) ==
             funct7) &&
            (static_cast<uint32_t>(RISCVTools::getFunc3Int(encoding)) ==
             funct3)) {
            isMatch = true;
        }

        if (isMatch) {
            *output = strdup(op.first.c_str());
            return 0;
        }
    }

    if (error != nullptr) {
        *error = strdup(
            "UnpackInstruction error: Could not identify operation for "
            "the given opcode");
    }
    return -1;
}

int
executeInstruction32(
    const char *opName, const uint32_t *inputs, uint32_t *output,
    char **error) {
    const int width = 32;

    if (pool == nullptr) {
        pool = std::make_unique<OperationPool>();
    }

    if (output == nullptr) {
        if (error != nullptr) {
            *error = strdup(
                "ExecuteInstruction32 error: Output parameter is null");
        }
        return -1;
    }

    Operation *op = &(pool->operation(opName));

    if (op == &NullOperation::instance()) {
        if (error != nullptr) {
            *error =
                strdup("ExecuteInstruction32 error: Operation not found");
        }
        return -1;
    }

    OperationBehavior &behavior = op->behavior();
    OperationContext opContext(opName);
    behavior.createState(opContext);

    size_t total_operands = op->numberOfInputs() + op->numberOfOutputs();
    std::vector<SimValue> simValues;
    simValues.reserve(total_operands);

    SimValue **simValuePtrs = new SimValue *[total_operands];

    for (int i = 0; i < op->numberOfInputs(); i++) {
        simValues.push_back(SimValue(inputs[i], width));
        simValuePtrs[i] = &simValues.back();
    }

    for (int i = 0; i < op->numberOfOutputs(); i++) {
        simValues.push_back(SimValue(0, width));
        simValuePtrs[op->numberOfInputs() + i] = &simValues.back();
    }

    if (!behavior.simulateTrigger(simValuePtrs, opContext)) {
        if (error != nullptr) {
            *error = strdup(
                "ExecuteInstruction32 error: simulating the execution, "
                "simulateTrigger failed.");
        }
        delete[] simValuePtrs;
        behavior.deleteState(opContext);
        return -1;
    }

    for (int i = 0; i < op->numberOfOutputs(); i++) {
        output[i] = simValuePtrs[op->numberOfInputs() + i]->uIntWordValue();
    }

    delete[] simValuePtrs;
    behavior.deleteState(opContext);
    return 0;
}

int
executeInstruction64(
    const char *opName, const uint64_t *inputs, uint64_t *output,
    char **error) {
    const int width = 64;

    if (pool == nullptr) {
        pool = std::make_unique<OperationPool>();
    }

    if (output == nullptr) {
        if (error != nullptr) {
            *error = strdup(
                "ExecuteInstruction64 error: Output parameter is null");
        }
        return -1;
    }

    Operation *op = &(pool->operation(opName));

    if (op == &NullOperation::instance()) {
        if (error != nullptr) {
            *error =
                strdup("ExecuteInstruction64 error: operation not found");
        }
        return -1;
    }

    OperationBehavior &behavior = op->behavior();
    OperationContext opContext(opName);
    behavior.createState(opContext);

    size_t total_operands = op->numberOfInputs() + op->numberOfOutputs();
    std::vector<SimValue> simValues;
    simValues.reserve(total_operands);

    SimValue **simValuePtrs = new SimValue *[total_operands];

    for (int i = 0; i < op->numberOfInputs(); i++) {
        simValues.push_back(SimValue(inputs[i], width));
        simValuePtrs[i] = &simValues.back();
    }

    for (int i = 0; i < op->numberOfOutputs(); i++) {
        simValues.push_back(SimValue(0, width));
        simValuePtrs[op->numberOfInputs() + i] = &simValues.back();
    }

    if (!behavior.simulateTrigger(simValuePtrs, opContext)) {
        if (error != nullptr) {
            *error = strdup(
                "ExecuteInstruction64 error: simulating the execution, "
                "simulateTrigger failed.");
        }
        delete[] simValuePtrs;
        behavior.deleteState(opContext);
        return -1;
    }

    for (int i = 0; i < op->numberOfOutputs(); i++) {
        output[i] =
            simValuePtrs[op->numberOfInputs() + i]->uLongWordValue();
    }

    delete[] simValuePtrs;
    behavior.deleteState(opContext);
    return 0;
}
}
