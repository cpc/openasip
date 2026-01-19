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

#include <memory>
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

namespace RISCVInstructionExecutor {
constexpr unsigned OPC_CUSTOM_0 = 0b0001011;
constexpr unsigned OPC_CUSTOM_1 = 0b0101011;

std::map<std::string, int> customOps = {};
std::unique_ptr<OperationPool> pool = nullptr;

/**
 * Helper function that executes an OSAL instruction.
 *
 * Takes in the opName, instruction width, and the inputs, and returns the
 * output vector.
 *
 * @param opName operation to be executed
 * @param width instruction width (typically 32 or 64)
 * @param inputs array of input values
 * @param inputsCount number of values in the inputs array
 * @return vector of SimValue objects.
 */
std::vector<SimValue>
executeInstructionHelper(
    const char* opName, uint8_t width, const uint64_t* inputs,
    int inputsCount) {
    if (RISCVInstructionExecutor::pool == nullptr) {
        RISCVInstructionExecutor::pool = std::make_unique<OperationPool>();
    }

    Operation& op = RISCVInstructionExecutor::pool->operation(opName);
    if (&op == &NullOperation::instance()) {
        THROW_EXCEPTION(
            IllegalOperationBehavior,
            std::string("ExecuteInstruction error: No behavior "
                        "implementation found for operation '") +
                opName + "'");
    }
    OperationBehavior& behavior = op.behavior();

    if (&behavior == &NullOperationBehavior::instance()) {
        throw std::logic_error(
            std::string("ExecuteInstruction error: No behavior "
                        "implementation found for operation '") +
            opName + "'");
    }

    OperationContext opContext(opName);
    behavior.createState(opContext);

    if (inputsCount < op.numberOfInputs()) {
        behavior.deleteState(opContext);
        THROW_EXCEPTION(
            IllegalParameters,
            std::string("ExecuteInstruction error: Not enough input values"));
    }

    const int opInputs = op.numberOfInputs();
    const int opOutputs = op.numberOfOutputs();
    const int opValues = opInputs + opOutputs;

    std::vector<std::unique_ptr<SimValue>> simValues(opValues);

    for (int i = 0; i < opInputs; ++i) {
        simValues[i] = std::make_unique<SimValue>(inputs[i], width);
    }
    for (int i = 0; i < opOutputs; ++i) {
        simValues[opInputs + i] = std::make_unique<SimValue>(0, width);
    }

    std::vector<SimValue*> simValPtrs(opValues);
    for (int i = 0; i < opValues; ++i) {
        simValPtrs[i] = simValues[i].get();
    }

    if (!behavior.simulateTrigger(simValPtrs.data(), opContext)) {
        behavior.deleteState(opContext);
        THROW_EXCEPTION(
            ModuleRunTimeError,
            std::string(
                "ExecuteInstruction error: operation execution failed"));
    }

    std::vector<SimValue> results(opOutputs);
    for (int i = 0; i < opOutputs; i++) {
        results[i] = *simValues[opInputs + i];
    }

    behavior.deleteState(opContext);
    return results;
}

}  // namespace RISCVInstructionExecutor

extern "C" {

/**
 * Initializes the OpenASIP Machine.
 *
 * Should be called before other functions. Creates the <instruction name,
 * opcode> map for the custom operations, and the OperationPool instruction
 * cache object.
 *
 * @param machinePath path to the .adf machine file.
 * @param error error messages in case of failure. Can also be a nullptr if
 * desired. Must be freed by the client.
 * @return 0 on success, -1 on failure.
 */
int
initializeMachine(const char* machinePath, char** error) {
    std::unique_ptr<TTAMachine::Machine> machine = nullptr;

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
    BinaryEncoding* bem = BEMGenerator(*machine).generate();
    if (bem == nullptr) {
        *error = strdup("InitializeMachine error: failed to generate bem");
        return -1;
    }

    RISCVTools::findCustomOps(RISCVInstructionExecutor::customOps, bem);
    if (RISCVInstructionExecutor::customOps.empty()) {
        *error = strdup(
            "InitializeMachine error: failed to generate custom ops map");
        return -1;
    }

    if (RISCVInstructionExecutor::pool == nullptr) {
        RISCVInstructionExecutor::pool = std::make_unique<OperationPool>();
    }

    delete bem;
    return 0;
}

/**
 * Resets the machine to uninitialized state.
 *
 * Deletes the customOps map loaded and deletes the
 * OperationPool instruction cache.
 */
int
resetMachine() {
    RISCVInstructionExecutor::customOps = {};
    RISCVInstructionExecutor::pool = nullptr;
    return 0;
}

/**
 * Unpacks a RISC-V R4-type instruction and returns its string representation
 * if found from the machine file.
 *
 * Remember to call Initialize machine first.
 *
 * @param opcode full RISC-V opcode. Register values are ignored.
 * @param output The char* representation of the opcode, if it is found. Must
 * be freed by the client.
 * @param error Error messages in case of failure. Must be freed by the
 * client.
 * @return 0 on success, -1 on failure.
 */
int
unpackInstruction(uint32_t instruction, char** output, char** error) {
    if (RISCVInstructionExecutor::customOps.empty()) {
        *error = strdup(
            "UnpackInstruction error: customOps map is empty. Did you "
            "initialize the machine first?");
        return -1;
    }

    if (output == nullptr) {
        if (error != nullptr) {
            *error =
                strdup("UnpackInstruction error: Output parameter is null");
        }
        return -1;
    }

    R4Instruction decodedInstruction =
        RISCVTools::decodeR4Instruction(instruction);

    bool isCustom0 =
        (decodedInstruction.baseopcode ==
         RISCVInstructionExecutor::OPC_CUSTOM_0);
    bool isCustom1 =
        (decodedInstruction.baseopcode ==
         RISCVInstructionExecutor::OPC_CUSTOM_1);

    if (!isCustom0 && !isCustom1) {
        if (error != nullptr) {
            *error = strdup("UnpackInstruction error: Unknown base opcode");
        }
        return -1;
    }

    std::string opName = "";
    for (const auto& op : RISCVInstructionExecutor::customOps) {
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
        *error = strdup("UnpackInstruction error: Operation not found");
    }
    return -1;
}
/**
 * Executes a custom 32-wide instruction.
 *
 * The instruction behavior is searched automatically by OperationPool
 * from the OSAL search paths. See chapter 4.4 in the manual.
 *
 * @param opName The operation name as it is in the machine file.
 * @param inputs Input value(s) of the operation. Can contain more values
 * than the operation needs, in that case only the first values will be
 * used.
 * @param inputsCount number of inputs
 * @param output The result of the operation.
 * @param error  Will not be touched in case of success. Must be freed by
 * the client.
 * @return 0 on success, -1 on failure.
 */
int
executeInstruction32(
    const char* opName, const uint32_t* inputs, uint32_t inputsCount,
    uint32_t* output, char** error) {
    if (output == nullptr) {
        if (error != nullptr) {
            *error = strdup(
                "ExecuteInstruction32 error: Output parameter is null");
        }
        return -1;
    }

    try {
        std::vector<uint64_t> inputs64(inputsCount);
        for (uint32_t i = 0; i < inputsCount; i++) {
            inputs64[i] = static_cast<uint64_t>(inputs[i]);
        }

        std::vector<SimValue> results =
            RISCVInstructionExecutor::executeInstructionHelper(
                opName, 32, inputs64.data(), inputsCount);
        for (size_t i = 0; i < results.size(); i++) {
            output[i] = results.at(i).uIntWordValue();
        }
        return 0;
    } catch (Exception& e) {
        if (error != nullptr) {
            *error = strdup(e.errorMessage().c_str());
        }
        return -1;
    }
}

/**
 * Executes a custom 64-wide instruction.
 *
 * The instruction behavior is searched automatically by OperationPool
 * from the OSAL search paths. See chapter 4.4 in the manual.
 *
 * @param opName The operation name as it is in the machine file.
 * @param inputs Input value(s) of the operation. Can contain more values
 * than the operation needs, in that case only the first values will be
 * used.
 * @param inputsCount number of inputs
 * @param output The result of the operation.
 * @param error  Will not be touched in case of success. Must be freed by
 * the client.
 * @return 0 on success, -1 on failure.
 */
int
executeInstruction64(
    const char* opName, const uint64_t* inputs, uint32_t inputsCount,
    uint64_t* output, char** error) {
    if (output == nullptr) {
        if (error != nullptr) {
            *error = strdup(
                "ExecuteInstruction64 error: Output parameter is null");
        }
        return -1;
    }

    try {
        std::vector<SimValue> results =
            RISCVInstructionExecutor::executeInstructionHelper(
                opName, 64, inputs, inputsCount);
        for (size_t i = 0; i < results.size(); i++) {
            output[i] = results.at(i).uLongWordValue();
        }
        return 0;
    } catch (Exception& e) {
        if (error != nullptr) {
            *error = strdup(e.errorMessage().c_str());
        }
        return -1;
    }
}
}
