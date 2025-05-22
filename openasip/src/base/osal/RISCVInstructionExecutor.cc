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


static std::unique_ptr<OperationPool> pool;
static std::map<std::string, int> customOps;
static std::unique_ptr<TTAMachine::Machine> machine;


/**
 * Takes in a Machine object and returns a map of instruction names and their
 * encodings.
 * 
 * Helper function for the extern C functions.
 */
std::map<std::string, int>
findRISCVCustomOps(const TTAMachine::Machine& mach) {
    std::map<std::string, int> customOps;
    BinaryEncoding* bem = BEMGenerator(mach).generate();

    assert(bem != NULL);

    const std::vector<std::string> formatsToSearch = {
        RISCVFields::RISCV_R_TYPE_NAME,
        RISCVFields::RISCV_R1R_TYPE_NAME,
        RISCVFields::RISCV_R1_TYPE_NAME,
        RISCVFields::RISCV_R3R_TYPE_NAME};

    for (const std::string& fName : formatsToSearch) {
        InstructionFormat* format = bem->instructionFormat(fName);
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

/**
 * Takes in the opName, instruction width, and the inputs, and returns the output vector.
 * 
 * Helper function to the extern C functions, and therefore not shown in the header.
 */
std::vector<SimValue>
executeInstructionHelper(
    const char* opName, uint8_t width, const uint64_t* inputs) {

    if (pool == nullptr) {
        pool = std::make_unique<OperationPool>();
    }

    Operation op = pool->operation(opName);

    if (op.isNull()) {
        throw std::runtime_error("ExecuteInstruction error: operation not found");
    }

    OperationBehavior& behavior = op.behavior();
    OperationContext opContext(opName);
    behavior.createState(opContext);

    size_t totalOperands = op.numberOfInputs() + op.numberOfOutputs();
    std::vector<SimValue> simValues(totalOperands);

    for (int i = 0; i < op.numberOfInputs(); i++) {
        simValues.push_back(SimValue(inputs[i], width));
    }
    
    for (int i = 0; i < op.numberOfOutputs(); i++) {
        simValues.push_back(SimValue(0, width));
    }
    
    SimValue* simValuePtrs = simValues.data();

    if (!behavior.simulateTrigger(&simValuePtrs, opContext)) {
        behavior.deleteState(opContext);
        throw std::runtime_error("ExecuteInstruction error: failed to execute the instruction.");
    }

    std::vector<SimValue> results(op.numberOfOutputs());
    for (int i = 0; i < op.numberOfOutputs(); i++) {
        results.push_back(simValues.at(op.numberOfInputs() + i));
    }

    behavior.deleteState(opContext);
    return results;
}

extern "C" {

int
initializeMachine(const char* machinePath, char** error) {
    machine.reset();

    try {
        machine = std::unique_ptr<TTAMachine::Machine>(
            TTAMachine::Machine::loadFromADF(machinePath));
    } catch (const SerializerException& e) {
        if (error != nullptr) {
            std::string errorStr =
                std::string("InitializeMachine error: ") + e.errorMessage();
            *error = strdup(errorStr.c_str());
        }
        return -1;
    }

    pool = std::make_unique<OperationPool>();
    customOps = findRISCVCustomOps(*machine);

    return 0;
}

int
unpackInstruction(const uint32_t instruction, char** output, char** error) {
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

    R4Instruction decodedInstruction = RISCVTools::decodeR4Instruction(instruction);

    const unsigned OPC_CUSTOM_0 = 0b0001011;
    const unsigned OPC_CUSTOM_1 = 0b0101011;

    bool isCustom0 = (decodedInstruction.baseopcode == OPC_CUSTOM_0);
    bool isCustom1 = (decodedInstruction.baseopcode == OPC_CUSTOM_1);

    if (!isCustom0 && !isCustom1) {
        if (error != nullptr) {
            *error = strdup("UnpackInstruction error: Unknown baseopcode");
        }
        return -1;
    }

    std::string opName = "";
    for (const auto& op : customOps) {
        uint32_t encoding = op.second;
        bool isMatch = false;

        if (isCustom1 &&
            RISCVTools::getFunc2Int(encoding) == decodedInstruction.funct2 &&
            RISCVTools::getFunc3Int(encoding) == decodedInstruction.funct3) {
            isMatch = true;
        } else if (
            isCustom0 &&
            RISCVTools::getFunc7Int(encoding) == decodedInstruction.funct7 && 
            RISCVTools::getFunc3Int(encoding) == decodedInstruction.funct3) {
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
    const char* opName, const uint32_t *inputs, uint32_t *output,
    char** error) {

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
    
    Operation* op = &(pool->operation(opName));
    std::vector<uint64_t> inputsVector(op->numberOfInputs());
    for (int i = 0; i < op->numberOfInputs(); i++) {
        inputsVector[i] = static_cast<uint64_t>(inputs[i]);
    }
    
    try {
        std::vector<SimValue> results = executeInstructionHelper(opName, 32, inputsVector.data());
        for(int i = 0; i < op->numberOfOutputs(); i++) {
            output[i] = results.at(i).uIntWordValue();
        }
        return 0;
    } catch (std::runtime_error& e) {
        *error = strdup(e.what());
        return -1;
    }
}

int
executeInstruction64(
    const char* opName, const uint64_t* inputs, uint64_t* output,
    char** error) {

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
    
    Operation* op = &(pool->operation(opName));
    std::vector<uint64_t> inputsVector(op->numberOfInputs());
    for (int i = 0; i < op->numberOfInputs(); i++) {
        inputsVector[i] = inputs[i];
    }
    
    try {
        std::vector<SimValue> results = executeInstructionHelper(opName, 64, inputsVector.data());
        for(int i = 0; i < op->numberOfOutputs(); i++) {
            output[i] = results.at(i).uLongWordValue();
        }
        return 0;
    } catch (std::runtime_error& e) {
        *error = strdup(e.what());
        return -1;
    }
}
}
