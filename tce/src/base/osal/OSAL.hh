/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file OSAL.hh
 *
 * Macro definitions for defining the behavior of operations in
 * the Operation Set Abstraction Layer. 
 *
 * Macros expand to derived OperationBehavior class definitions and factory 
 * functions that instantiate these custom classes. The factory functions 
 * constitute the plugin interface.
 *
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 * @note rating: red
 */

#ifndef TTA_OSAL_LANGUAGE_DEFINITIONS_HH
#define TTA_OSAL_LANGUAGE_DEFINITIONS_HH

#include "tce_config.h"
#include "OperationContext.hh"
#include "SimValue.hh"
#include "OperationBehavior.hh"
#include "Operation.hh"
#include "Memory.hh"
#include "OperationPool.hh"
#include "OperationState.hh"
#include "OperationGlobals.hh"

#include "SimulateTriggerWrappers.icc"


/**
 * OPERATION and OPERATION_WITH_STATE start a definition block for operation
 * behavior. They expand to a definition of a new OperationBehavior class
 * with name OPERATION_Behavior (e.g., ADD4_Behavior).
 *
 * Argument overloading is not supported by the preprocessor macros, thus we 
 * need two differently named OPERATION macros, OPERATION() and 
 * OPERATION_WITH_STATE().
 *
 */

/**
 * OPERATION_COMMON provides definitions that are common to both OPERATION
 * and OPERATION_WITH_STATE:
 *
 * - Constructor with Operation reference (points to the operation the
 *   behavior is defined for).
 */
#define OPERATION_COMMON(OPNAME) \
public: \
OPNAME##_Behavior(const Operation& parent) : parent_(parent) {}; \
    const Operation& parent_; \
private: \
    mutable OperationPool opPool_;

/**
 * Operation behavior without state.
 *
 * fetchState() is called in the simulation functions to fetch instance of 
 * state (using STATE macro).
 *
 * In an operation definition without state, the STATE is set point to
 * an instance of NullOperationState.
 */
#define OPERATION(OPNAME) \
class OPNAME##_Behavior : public OperationBehavior { \
public:                                              \
    typedef NullOperationState StateType; \
private: \
    StateType* fetchState(const OperationContext&) const { \
        return NullOperationState::instance();             \
    }; \
OPERATION_COMMON(OPNAME) \
public:

/**
 * Ends the definition of operation behavior without state.
 *
 * This macro expands to a factory/destructor functions for the newly defined
 * behavior class type. The functions are exported using C symbols so they
 * are accesible without needing C++ name demangling. These two functions
 * constitute the plugin interface which are used as an access point to the
 * definitions in an operation behavior plugin module.
 *
 * Factory function returns an instance of the newly defined behavior class. 
 * A reference to the operation the behavior belongs to is given as an 
 * argument. The function is named createOpBehavior_OPERATIONNAME(), e.g.,
 * createOpBehavior_Add4().
 *
 * Destructor function needs to be defined because delete operator can be 
 * overridden in the plugin module and the deletion of the custom operation 
 * behavior instance has to be done using the correct delete operator. The
 * instance cannot be simply deallocated using delete operator in the client's
 * code. Function takes a pointer to the object that should be deleted as an
 * argument. It's named deleteOpBehavior_OPERATIONNAME(), for example,
 * deleteOpBehavior_Add4().
 *
 * The dummy variable in the last line is to force addition of semicolon after
 * the END_OPERATION() statement in user's code because semicolon is not 
 * allowed after an "extern" block (at least with all compilers). 
 *
 * The semicolon "policy" is wanted to be consistent with all statements: 
 * the statement that starts the block (e.g., OPERATION()) does not require 
 * semicolon after it, but the block ender (e.g., END_OPERATION()) does. 
 */
#define END_OPERATION(OPNAME) \
};\
extern "C" { \
    OperationBehavior* createOpBehavior_##OPNAME(const Operation& parent) {\
        return new OPNAME##_Behavior(parent);\
    }\
    void deleteOpBehavior_##OPNAME(OperationBehavior* target) {\
        delete target;\
    }\
}

/**
 * Operation behavior with state.
 *
 * This definition connects user defined state type to an operation behavior
 * definition. Connection is implemented with a typedef that sets StateType
 * type to be the custom type. 
 *
 * createState() is called by the owner of the custom OperationBehavior 
 * instance, it is used to add a custom state instance to the operation
 * context to be accessed later. deleteState() is used to remove the
 * custom state from the context. See the design document for more information.
 * 
 * In an operation definition with state, the STATE is set to point to
 * an instance of an OperationState object of a custom defined OperationState
 * derived type. The instance is fetched from context and dynamic_casted
 * to the correct type.
 *
 * stateExists() helper method tells if an instance of the custom state
 * already exists in the context.
 */
#define OPERATION_WITH_STATE(OPNAME, STATE_NAME) \
class OPNAME##_Behavior : public OperationBehavior { \
public: \
    typedef STATE_NAME##_State StateType;\
\
    void createState(OperationContext& context) const {\
         if (!stateExists(context)) {\
             context.registerState(new STATE_NAME##_State(context));\
         }\
    }\
\
    void deleteState(OperationContext& context) const {\
         if (!stateExists(context)) {\
             return;\
         }\
         delete &context.state(#STATE_NAME);\
         context.unregisterState(#STATE_NAME);\
    }\
\
    const char* stateName() const {\
        return #STATE_NAME;\
    }\
private: \
    StateType* fetchState(const OperationContext& oc) const { \
	return dynamic_cast<StateType*>(&oc.state(#STATE_NAME)); \
    } \
    bool stateExists(OperationContext& context) const {\
         try {\
             context.state(#STATE_NAME);\
         } catch (...) {\
      	     return false;\
         }\
	 return true;\
    }\
OPERATION_COMMON(OPNAME) \
public:

#define END_OPERATION_WITH_STATE(OPNAME) END_OPERATION(OPNAME)

/**
 * Custom state definition.
 *
 * The information of the custom state type used by the OPERATION_WITH_STATE
 * definition is entered using DEFINE_STATE. The custom state must be defined
 * with this macro before using it in an OPERATION_WITH_STATE definition.
 *
 * Macro expands to a class structure which derives itself from OperationState
 * and implements the pure virtual method name() trivially by returning the
 * name of the state as a C string. Rest of the class, which are usually public 
 * member variables, are entered by the user.
 *
 */
#define DEFINE_STATE(STATE_NAME) \
class STATE_NAME##_State : public OperationState { \
    public: \
        const char* name() { return #STATE_NAME; };

#define END_DEFINE_STATE };


/**
 * Custom state initialization.
 *
 * If the state data needs to be initialized before use, INIT_STATE can be used
 * to define a constructor to the state class. isEmpty call is to avoid warning
 * about unused parameter.
 */
#define INIT_STATE(STATE_NAME) \
        public: STATE_NAME##_State(OperationContext& context) { \
            context.isEmpty();

#define END_INIT_STATE }


/**
 * Custom state deallocation.
 *
 * If the state data needs to be deallocated when state instance is deleted, 
 * FINALIZE_STATE can be used to define a destructor to the state class.
 */
#define FINALIZE_STATE(STATE_NAME) \
        public: ~STATE_NAME##_State() { 

#define END_FINALIZE_STATE }


/** 
 * Simulation function definition macros. 
 *
 * Each simulation function fetches the state instance using fetchState() in
 * the beginning of the function definition.
 */

/**
 * Starts the definition block for a trigger simulation function.
 *
 * "if (&context ..." is an ugly hack to avoid compiler warning 
 * when context is not used.
 */
#define TRIGGER \
bool simulateTrigger( \
    SimValue** io, \
    OperationContext& context) const { \
        if (&context == 0 || io == 0) { }

/**
 * Fetches the state instance.
 */
#define STATE (*fetchState(context))


/**
 * Ends the definition block for trigger simulation function.
 */
#define END_TRIGGER return true; } 

/**
 * Starts the definition block for the state clock advancing simulation 
 * function.
 *
 * "if (&context ..." is an ugly hack to avoid compiler warning 
 * when context is not used.
 *
 */
#define ADVANCE_CLOCK \
    void advanceClock(OperationContext& context) { \
        if (&context == 0) { }

/**
 * Ends the definition block for the clock advancing simulation function.
 */
#define END_ADVANCE_CLOCK }

/** 
 * Explicit return statements for simulation function definitions.
 */

/**
 * RETURN_READY is not needed anymore, macro is deprecated.
 */
#define RETURN_READY 

/**
 * Operand value accessor macros for different types.
 *
 * Each accessor macro accesses the corresponding type of value_ union of
 * the operand SimValue. The value is a copy and thus can be only used
 * as a rvalue of an assignment.
 */
#define INT(OPERAND) (io[(OPERAND) - 1]->intValue())
#define UINT(OPERAND) (io[(OPERAND) - 1]->unsignedValue())
#define FLT(OPERAND) (io[(OPERAND) - 1]->floatWordValue())
#define DBL(OPERAND) (io[(OPERAND) - 1]->doubleWordValue())
#define HFLT(OPERAND) (io[(OPERAND) -1]->halfFloatWordValue())

/**
 * Operand accessor macro.
 *
 * This macro must be used as lvalue when assigning values to output operands.
 */
#define IO(OPERAND) (*io[(OPERAND) - 1])

/**
 * Macro for obtaining the bit width of the port the operand is bound to.
 */
#define BWIDTH(OPERAND) (((io[(OPERAND) - 1])->width()))

/**
 * Access the program counter register.
 */
#define PROGRAM_COUNTER (context.programCounter())

/**
 * Instruct the GCU to save the return address register.
 */
#define SAVE_RETURN_ADDRESS context.setSaveReturnAddress(true)

/**
 * Access the return address register.
 */
#define RETURN_ADDRESS (context.returnAddress())

/**
 * Access the unique id of the operation context.
 */
#define CONTEXT_ID (context.contextId())

/**
 * Memory interface.
 */

/**
 * Get the width of the minimum addressable unit of the memory.
 */ 
#define MAU_SIZE (context.memory().MAUSize())

/**
 * Accesses the memory instance connected to the operation's FU.
 */
#define MEMORY context.memory()
         
/**
 * Sign extends the given integer.
 *
 * Sign extension means that the sign bit of the source word is duplicated
 * to the extra bits provided by the wider target destination word.
 * 
 * Example:
 *
 * SRC:    1 0010b (5 bits)
 * DST: 1111 0010b (8 bits)
 *
 * The first argument is the integer that should be sign extended. 
 * The second argument is the bit width of the source argument. 
 * In the previous example, the bit width is 5. This is used to figure out 
 * which is the sign bit. Returns a sign extended (to 32 bits) IntWord.
 */
#define SIGN_EXTEND(VALUE, WIDTH) \
    ((int)(VALUE << (sizeof(SIntWord)*8-(WIDTH))) >> (sizeof(SIntWord)*8-(WIDTH)))   

/**
 * Zero extends the given integer.
 *
 * Zero extension means that the extra bits of the wider target destination
 * word are set to zero.
 * 
 * Example:
 *
 * SRC:    1 0010b (5 bits)
 * DST: 0001 0010b (8 bits)
 *
 * The first argument is the integer that should be zero extended. 
 * Second argument is the bit width of the source argument. 
 * In the previous example, the bit width is 5. This is used to figure out 
 * which bits to reset in the destination. Returns a zero extended 
 * (to 32 bits) IntWord.
 */
#define ZERO_EXTEND(VALUE, WIDTH) \
    ((VALUE << (sizeof(SIntWord)*8-(WIDTH))) >> (sizeof(SIntWord)*8-(WIDTH)))   

/**
 * Provides access to the output stream which can be used to print out debug 
 * information to simulator console, or, for example, to simulate output from 
 * the TTA.
 *
 * Default output stream is std::cout. 
 */
#define OUTPUT_STREAM OperationGlobals::outputStream()

/**
 * Maximum bit width of the inputs and outputs currently fully supported by
 * behaviour models of base operations.
 */
#define OSAL_WORD_WIDTH sizeof(UIntWord) * BYTE_BITWIDTH


/**
 * Causes runtime error with given error explanation message.
 *
 * Can be used to abort simulation in case of illegal operands etc.
 */
#define RUNTIME_ERROR(MESSAGE) OperationGlobals::runtimeError(\
    MESSAGE, __FILE__, __LINE__, parent_);

/**
 * Executes operation.
 */
#define EXEC_OPERATION(OPNAME, ...) \
    const Operation& __used_in_macro_temp__ = opPool_.operation(#OPNAME); \
    executeOperation(__used_in_macro_temp__.behavior(), context, __VA_ARGS__);

#endif

/**
 * Returns the cycle count since the beginning of simulation.
 */
#define CYCLE_COUNT (context.cycleCount())

/**
 * Returns the function unit name associated to the OperationContext.
 */
#define FU_NAME (context.functionUnitName())
