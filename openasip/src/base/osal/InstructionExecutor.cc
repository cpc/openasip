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
 * Helper function for the extern C functions.
 * Takes in a Machine object and returns a map of instruction names and their
 * encodings.
 */
std::map<std::string, int>
findRISCVCustomOps(const TTAMachine::Machine &mach)
{
    std::map<std::string, int> customOps;
    BinaryEncoding *bem = BEMGenerator(mach).generate();

    assert(bem != NULL);

    const std::vector<std::string> formatsToSearch = {
        RISCVFields::RISCV_R_TYPE_NAME,
        RISCVFields::RISCV_R1R_TYPE_NAME,
        RISCVFields::RISCV_R1_TYPE_NAME,
        RISCVFields::RISCV_R3R_TYPE_NAME};

    for (const std::string &fName : formatsToSearch)
    {
        InstructionFormat *format = bem->instructionFormat(fName);
        if (format == NULL)
        {
            continue;
        }
        for (int i = 0; i < format->operationCount(); i++)
        {
            const std::string op = format->operationAtIndex(i);
            if (RISCVFields::RISCVRTypeOperations.find(op) == RISCVFields::RISCVRTypeOperations.end())
            {
                customOps.insert({op, format->encoding(op)});
            }
        }
    }

    delete bem;
    return customOps;
}

extern "C"
{

    static std::unique_ptr<OperationPool> pool = nullptr;
    static std::map<std::string, int> custom_ops;
    static std::unique_ptr<TTAMachine::Machine> machine;

    int
    InitializeMachine(const char *machine_path, char **error) {
        machine.reset();

        try {
            machine = std::unique_ptr<TTAMachine::Machine>(
                TTAMachine::Machine::loadFromADF(machine_path));
        } catch (const SerializerException &e) {
            if (error != nullptr) {
                std::string error_msg =
                    std::string("InitializeMachine error: ") +
                    e.errorMessage();
                *error = strdup(error_msg.c_str());
            }
            return -1;
        }

        if (pool == nullptr)
        {
            pool = std::make_unique<OperationPool>();
        }

        custom_ops = findRISCVCustomOps(*machine);

        return 0;
    }

    int
    UnpackInstruction(const uint32_t opcode, char **output, char **error) {
        if (machine == nullptr)
        {
            *error = strdup(
                "UnpackInstruction error: Machine not initialized. Call "
                "InitializeMachine first");
            return -1;
        }

        if (output == nullptr) {
            if (error != nullptr) {
                *error = strdup(
                    "UnpackInstruction error: Output parameter is null");
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

        if (!isCustom0 && !isCustom1)
        {
            if (error != nullptr)
            {
                *error =
                    strdup("UnpackInstruction error: Unknown baseopcode");
            }
            return -1;
        }

        std::string opName = "";
        for (const auto &op : custom_ops)
        {
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

            if (isMatch)
            {
                *output = strdup(op.first.c_str());
                return 0;
            }
        }

        if (error != nullptr)
        {
            *error = strdup(
                "UnpackInstruction error: Could not identify operation for "
                "the given opcode");
        }
        return -1;
    }

    int
    ExecuteInstruction32(
        const char *opName, const uint32_t *inputs, uint32_t *output,
        char **error_msg)

    {
        const int width = 32;

        if (pool == nullptr)
        {
            pool = std::make_unique<OperationPool>();
        }

        if (output == nullptr) {
            if (error_msg != nullptr) {
                *error_msg = strdup(
                    "ExecuteInstruction32 error: Output parameter is null");
            }
            return -1;
        }

        Operation *op = &(pool->operation(opName));

        if (op == &NullOperation::instance())
        {
            if (error_msg != nullptr)
            {
                *error_msg =
                    strdup("ExecuteInstruction32 error: Operation not found");
            }
            return -1;
        }

        OperationBehavior &behavior = op->behavior();
        OperationContext opContext(opName);
        behavior.createState(opContext);

        size_t total_operands = op->numberOfInputs() + op->numberOfOutputs();
        std::vector<SimValue> sim_values;
        sim_values.reserve(total_operands);

        SimValue **sim_value_ptrs = new SimValue *[total_operands];

        for (int i = 0; i < op->numberOfInputs(); i++) {
            sim_values.push_back(SimValue(inputs[i], width));
            sim_value_ptrs[i] = &sim_values.back();
        }

        for (int i = 0; i < op->numberOfOutputs(); i++) {
            sim_values.push_back(SimValue(0, width));
            sim_value_ptrs[op->numberOfInputs() + i] = &sim_values.back();
        }

        if (!behavior.simulateTrigger(sim_value_ptrs, opContext))
        {
            if (error_msg != nullptr)
            {
                *error_msg = strdup(
                    "ExecuteInstruction32 error: simulating the execution, "
                    "simulateTrigger failed.");
            }
            delete[] sim_value_ptrs;
            behavior.deleteState(opContext);
            return -1;
        }

        for (int i = 0; i < op->numberOfOutputs(); i++) {
            output[i] = sim_value_ptrs[op->numberOfInputs() + i]->uIntWordValue();
        }

        delete[] sim_value_ptrs;
        behavior.deleteState(opContext);
        return 0;
    }

    int
    ExecuteInstruction64(
        const char *opName, const uint64_t *inputs, uint64_t *output,
        char **error_msg) {
        const int width = 32;

        if (pool == nullptr)
        {
            pool = std::make_unique<OperationPool>();
        }

        if (output == nullptr) {
            if (error_msg != nullptr) {
                *error_msg = strdup(
                    "ExecuteInstruction64 error: Output parameter is null");
            }
            return -1;
        }

        Operation *op = &(pool->operation(opName));

        if (op == &NullOperation::instance())
        {
            if (error_msg != nullptr)
            {
                *error_msg =
                    strdup("ExecuteInstruction64 error: operation not found");
            }
            return -1;
        }

        OperationBehavior &behavior = op->behavior();
        OperationContext opContext(opName);
        behavior.createState(opContext);

        size_t total_operands = op->numberOfInputs() + op->numberOfOutputs();
        std::vector<SimValue> sim_values;
        sim_values.reserve(total_operands);

        SimValue **sim_value_ptrs = new SimValue *[total_operands];

        for (int i = 0; i < op->numberOfInputs(); i++) {
            sim_values.push_back(SimValue(inputs[i], width));
            sim_value_ptrs[i] = &sim_values.back();
        }

        for (int i = 0; i < op->numberOfOutputs(); i++) {
            sim_values.push_back(SimValue(0, width));
            sim_value_ptrs[op->numberOfInputs() + i] = &sim_values.back();
        }

        if (!behavior.simulateTrigger(sim_value_ptrs, opContext))
        {
            if (error_msg != nullptr)
            {
                *error_msg = strdup(
                    "ExecuteInstruction64 error: simulateTrigger failed.");
            }
            delete[] sim_value_ptrs;
            behavior.deleteState(opContext);
            return -1;
        }

        for (int i = 0; i < op->numberOfOutputs(); i++) {
            output[i] = sim_value_ptrs[op->numberOfInputs() + i]->uLongWordValue();
        }

        delete[] sim_value_ptrs;
        behavior.deleteState(opContext);
        return 0;
    }
}
