#ifndef INSTRUCTION_EXECUTOR_HH
#define INSTRUCTION_EXECUTOR_HH

#include <stdint.h>

extern "C" {

/**
 * Should be called before other functions. Initializes the Machine object and
 * loads the custom ops map.
 * @param machine_path path to the .adf machine file.
 * @param error error messages in case of failure. Can also be a nullptr if
 * desired.
 * @return 0 on success, -1 on failure.
 */
int InitializeMachine(const char *machine_path, char **error);

/**
 * Unpacks a Risc-V opcode and returns its string representation if found from
 * the machine file. Remember to call Initialize machine first.
 * @param opcode full RISC-V opcode. Register values are ignored.
 * @param output The char* representation of the opcode, if it is found.
 * @param error Error messages in case of failure.
 * @return 0 on success, -1 on failure.
 */
int UnpackInstruction(const uint32_t opcode, char **output, char **error);

/**
 * Executes a custom instruction. The instruction behavior is by default
 * located in ~/.openasip/opset 32 refers to the operation width, as well as
 * input and output sizes.
 * @param opName The operation name as it is in the machine file.
 * @param inputs Input value(s) of the operation. Can contain more values than
 * the operation needs, in that case only the first values will be used.
 * @param output The result of the operation.
 * @param error_msg  Will not be touched in case of success.
 * @return 0 on success, -1 on failure.
 */
int ExecuteInstruction32(
    const char *opName, const uint32_t *inputs, uint32_t *output,
    char **error_msg);

/**
 * Executes a custom instruction. The instruction behavior is by default
 * located in ~/.openasip/opset 64 refers to the operation width, as well as
 * input and output sizes.
 * @param opName The operation name as it is in the machine file.
 * @param inputs Input value(s) of the operation. Can contain more values than
 * the operation needs, in that case only the first values will be used.
 * @param output The result of the operation.
 * @param error_msg  Will not be touched in case of success.
 * @return 0 on success, -1 on failure.
 */
int ExecuteInstruction64(
    const char *opName, const uint64_t *inputs, uint64_t *output,
    char **error_msg);
}

#endif
