/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/** 
 * @file LanguageTest.hh 
 *
 * A test suite for C preprocessor definition macros that constitute the
 * operation behavior definition language.
 *
 * @author Jussi Nyk‰nen 2004 (jussi.nykanen-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2004 (pekka.jaaskelainen-no.spam-tut.fi)
 */    

#ifndef TTA_OSAL_LANGUAGE_TEST_HH
#define TTA_OSAL_LANGUAGE_TEST_HH

#include <string>
using std::string;

#include <iostream>
using std::cerr;
using std::endl;

#include <vector>
using std::vector;

#include <TestSuite.h>

#include "PluginTools.hh"
#include "FileSystem.hh"
#include "SimValue.hh"
#include "OperationContext.hh"
#include "OSAL.hh"
#include "Exception.hh"
#include "Operation.hh"
#include "ObjectState.hh"
#include "IdealSRAM.hh"
#include "BaseType.hh"
#include "TCEString.hh"

/**
 * Class for testing the OSAL language.
 */
class LanguageTest : public CxxTest::TestSuite {
public:
    LanguageTest();

    void testIntHandling();
    void testFloatHandling();
    void testDoubleHandling();
    void testStateHandling();
    void testSharedStateHandling();
    void testBWidth();
    void testControl();
    void testContextId();
    void testClockedOperation();
    void testExtend();

private:
    OperationBehavior* loadBehavior( 
        std::string moduleBaseName, 
        std::string operationName,
        const Operation& parentOperation = defaultParentOperation_);

    bool simulateTrigger(
        OperationBehavior* behavior, 
        vector<SimValue*> io, 
        OperationContext& context);

    void deleteBehavior(
        std::string moduleBaseName,
        std::string operationName,
        OperationBehavior* target);

    PluginTools pluginTools_;

    typedef OperationBehavior* (*BuilderFunc)(const Operation&);
    typedef void (*DestroyerFunc)(OperationBehavior*);

    static const Operation defaultParentOperation_;
    
};

const string BUILDER_FUNC_PREFIX = "createOpBehavior_";
const string DESTROYER_FUNC_PREFIX = "deleteOpBehavior_";
const Operation LanguageTest::defaultParentOperation_(
    "default", NullOperationBehavior::instance());

/**
 * A wrapper for the old simulate trigger method which took vector<SimValue*>
 * as io instead of SimValue**.
 */
bool
LanguageTest::simulateTrigger(
    OperationBehavior* behavior, 
    vector<SimValue*> io, 
    OperationContext& context) {
    SimValue** io_ = new SimValue*[io.size()];
    for (std::size_t i = 0; i < io.size(); ++i) {
        io_[i] = io.at(i);
    }
    bool ready = behavior->simulateTrigger(io_, context);
    delete[] io_;
    io_ = NULL;
    return ready;
}

/**
 * Constructor.
 *
 * Initializes the PluginTools instance that is used to load the plugin
 * modules.
 */
inline
LanguageTest::LanguageTest() {
    string pluginPath = FileSystem::currentWorkingDir() +
        FileSystem::DIRECTORY_SEPARATOR + "data";
    pluginTools_.addSearchPath(pluginPath);
}

/**
 * Helper function that loads behavior instance from a dynamic module.
 *
 * @param moduleBaseName Base name of the dynamic module under data/
 * @param operationName  Name of the operation inside the dynamic module.
 * @param parentOperation Operation that is set to be the parent of the
 * behavior definition.
 */
inline OperationBehavior*
LanguageTest::loadBehavior(
    std::string moduleBaseName, 
    std::string operationName,
    const Operation& parentOperation) {

    BuilderFunc loader = NULL;
    try {
	pluginTools_.importSymbol(
	    BUILDER_FUNC_PREFIX + operationName, 
	    loader, moduleBaseName + ".so");
    } catch (const Exception& e) {
	cerr << e.errorMessage() << endl;
	return NULL;
    }
    
    TS_ASSERT_DIFFERS(loader, static_cast<BuilderFunc>(NULL));
           
    return (*loader)(parentOperation);
}


/**
 * Helper function that deletes a behavior instance.
 *
 * @param moduleBaseName Base name of the dynamic module under data/
 * @param operationName  Name of the operation inside the dynamic module.
 * @param target The instance to delete.
 */
inline
void
LanguageTest::deleteBehavior(
    std::string moduleBaseName, 
    std::string operationName,
    OperationBehavior* target) {

    DestroyerFunc destroyer = NULL;
    try {
	pluginTools_.importSymbol(
	    DESTROYER_FUNC_PREFIX + operationName, 
	    destroyer, moduleBaseName + ".so");
    } catch (const Exception& e) {
	cerr << e.errorMessage() << endl;
    }
    
    TS_ASSERT_DIFFERS(destroyer, static_cast<DestroyerFunc>(NULL));
           
    (*destroyer)(target);
}


/**
 * Creates an operation behavior definition with no state using the language.
 *
 * Tests that INT macro works. Uses a definition that emulates a four-operand
 * add operation.
 */
inline void
LanguageTest::testIntHandling() {

    vector<SimValue*> arguments;
    OperationContext context;

    SimValue input1(32);
    input1 = 1;
    arguments.push_back(&input1);

    SimValue input2(32);
    input2 = 2;
    arguments.push_back(&input2);
    
    SimValue input3(32);
    input3 = 3;
    arguments.push_back(&input3);

    SimValue input4(32);
    input4 = 4;
    arguments.push_back(&input4);

    SimValue result(32);
    arguments.push_back(&result);

    OperationBehavior* simple = loadBehavior("simple", "ADD4");
    TS_ASSERT_EQUALS(simulateTrigger(simple, arguments, context), true);
    deleteBehavior("simple", "ADD4", simple);

    TS_ASSERT_EQUALS(result.intValue(), 1 + 2 + 3 + 4);
}

/**
 * Creates an operation behavior definition with no state using the language.
 * 
 * Tests that the FLT macro works correctly. Uses a definition that emulates a
 * three-operand floating point multiplication.
 */
inline void
LanguageTest::testFloatHandling() {

    vector<SimValue*> arguments;
    OperationContext context;

    SimValue input1(32);
    input1 = FloatWord(1.0);
    arguments.push_back(&input1);

    SimValue input2(32);
    input2 = FloatWord(2.0);
    arguments.push_back(&input2);

    SimValue input3(32);
    input3 = FloatWord(3.0);
    arguments.push_back(&input3);
    
    SimValue result(32);
    arguments.push_back(&result);

    OperationBehavior* simple = loadBehavior("simple", "FLOATMUL3");
    TS_ASSERT_EQUALS(simulateTrigger(simple, arguments, context), true);
    deleteBehavior("simple", "FLOATMUL3", simple);

    TS_ASSERT_EQUALS(result.floatWordValue(), FloatWord(6.0));
}

/**
 * Creates an operation behavior definition with no state using the language.
 *
 * Tests that the DBL macro works correctly. Uses a definition that emulates a
 * three-operand floating point multiplication using double precision floats.
 */
inline void
LanguageTest::testDoubleHandling() {

    vector<SimValue*> arguments;
    OperationContext context;

    SimValue input1(32);
    input1 = DoubleWord(1.0);
    arguments.push_back(&input1);

    SimValue input2(32);
    input2 = DoubleWord(2.0);
    arguments.push_back(&input2);

    SimValue input3(32);
    input3 = DoubleWord(3.0);
    arguments.push_back(&input3);
    
    SimValue result(32);
    arguments.push_back(&result);

    OperationBehavior* simple = loadBehavior("simple", "DOUBLESUB3");
    TS_ASSERT_EQUALS(simulateTrigger(simple, arguments, context), true);
    deleteBehavior("simple", "DOUBLESUB3", simple);

    TS_ASSERT_EQUALS(result.doubleWordValue(), DoubleWord(-4.0));
}

/**
 * Creates an operation behavior definition with state using the language.
 *
 * Tests that state is preserved and can be used successfully with a single
 * operation. Also tests that state is preserved in context until 
 * deleteState() call. Tested operation accumulates its internal register with
 * the input operand value.
 */
inline void
LanguageTest::testStateHandling() {

    vector<SimValue*> arguments;
    OperationContext context;
    OperationBehavior* simple = loadBehavior("operationsWithState", "ACCADD");

    SimValue input(32);
    input = 5;
    arguments.push_back(&input);
   
    SimValue result(32);
    arguments.push_back(&result);

    TS_ASSERT_THROWS(context.state("ACCUSTATE"), KeyNotFound);
    simple->createState(context);
    TS_ASSERT_THROWS_NOTHING(context.state("ACCUSTATE"));
    
    // the input should accumulate to state, increasing the value with 5
    TS_ASSERT_EQUALS(simulateTrigger(simple, arguments, context), true);
    TS_ASSERT_EQUALS(result.intValue(), 5);

    // the input should accumulate to state, increasing the value with 5
    TS_ASSERT_EQUALS(simulateTrigger(simple, arguments, context), true);
    TS_ASSERT_EQUALS(result.intValue(), 10);

    // the input should accumulate to state, increasing the value with 5
    TS_ASSERT_EQUALS(simulateTrigger(simple, arguments, context), true);
    TS_ASSERT_EQUALS(result.intValue(), 15);


    TS_ASSERT_THROWS_NOTHING(context.state("ACCUSTATE"));
    simple->deleteState(context);
    TS_ASSERT_THROWS(context.state("ACCUSTATE"), KeyNotFound); 

    deleteBehavior("operationsWithState", "ACCADD", simple);
}

/**
 * Creates two operation behavior definition with shared state.
 *
 * Tests that state is preserved and can be used successfully with both
 * operations. One of the tested operations accumulate the inner register,
 * other decrements.
 */
inline void
LanguageTest::testSharedStateHandling() {

    vector<SimValue*> arguments;
    OperationContext context;
    OperationBehavior* adder = loadBehavior("operationsWithState", "ACCADD");
    OperationBehavior* suber = loadBehavior("operationsWithState", "ACCSUB");

    SimValue input(32);
    input = 5;
    arguments.push_back(&input);
   
    SimValue result(32);
    arguments.push_back(&result);

    adder->createState(context);
    // createState() is idempotent with respect to the context
    TS_ASSERT_THROWS_NOTHING(suber->createState(context));
    
    // input value accumulates to state, increasing the value by 5
    simulateTrigger(adder, arguments, context);
    TS_ASSERT_EQUALS(result.intValue(), 5);

    // the input value decreases the state by 5
    simulateTrigger(suber, arguments, context);
    TS_ASSERT_EQUALS(result.intValue(), 0);

    TS_ASSERT_THROWS_NOTHING(context.state("ACCUSTATE"));
    adder->deleteState(context);
    TS_ASSERT_THROWS(context.state("ACCUSTATE"), KeyNotFound); 
    // deleteState() is idempotent with respect to the context
    TS_ASSERT_THROWS_NOTHING(suber->deleteState(context));

    deleteBehavior("operationsWithState", "ACCADD", adder);
    deleteBehavior("operationsWithState", "ACCSUB", suber);
}

/**
 * Tests that the BWIDTH macro works correctly.
 *
 * Tested operation returns the bit width of the input operand.
 */
inline void
LanguageTest::testBWidth() {
    vector<SimValue*> arguments;
    OperationContext context;

    OperationBehavior* bwidth = loadBehavior("bwidth", "INPUT_BIT_WIDTH");

    SimValue result(32);
    SimValue input1(15);
    arguments.push_back(&input1);
    arguments.push_back(&result);    

    TS_ASSERT_EQUALS(simulateTrigger(bwidth, arguments, context), true);

    TS_ASSERT_EQUALS(result.intValue(), 15);

    SimValue input2(54);
    arguments[0] = &input2;
    TS_ASSERT_EQUALS(simulateTrigger(bwidth, arguments, context), true);
    TS_ASSERT_EQUALS(result.intValue(), 54);

    deleteBehavior("bwidth", "INPUT_BIT_WIDTH", bwidth);
}

/**
 * Tests control register access.
 *
 * Tests operations that emulate traditional call and return operations.
 */
inline void
LanguageTest::testControl() {

    vector<SimValue*> arguments;

    InstructionAddress PC = 0;
    SimValue returnAddress(32);

    OperationContext context(NULL, PC, returnAddress);

    SimValue callTarget(32);
    callTarget = 500;
    arguments.push_back(&callTarget);

    OperationBehavior* callOperation = loadBehavior("control", "CALL");    
    OperationBehavior* retOperation = loadBehavior("control", "RET");    

    TS_ASSERT_EQUALS(simulateTrigger(callOperation, arguments, context), true);

    TS_ASSERT_EQUALS(static_cast<int>(context.programCounter()), 500);
    TS_ASSERT_EQUALS(context.returnAddress(), 1);

    // make sure that we use the PC and returnAddress defined in this function
    TS_ASSERT_EQUALS(context.programCounter(), PC);
    TS_ASSERT_EQUALS(context.returnAddress(), returnAddress);

    TS_ASSERT_EQUALS(simulateTrigger(retOperation, arguments, context), true);

    TS_ASSERT_EQUALS(static_cast<int>(context.programCounter()), 1);
    TS_ASSERT_EQUALS(context.returnAddress(), 1);

    deleteBehavior("control", "CALL", callOperation);
    deleteBehavior("control", "RET", retOperation);
}

/**
 * Tests that the context id generation of OperationContext and access 
 * with CONTEXT_ID works.
 */
inline void 
LanguageTest::testContextId() {
    OperationContext c1, c2, c3, context;
    TS_ASSERT_DIFFERS(c1.contextId(), c2.contextId());
    TS_ASSERT_DIFFERS(c2.contextId(), c3.contextId());
    TS_ASSERT_DIFFERS(c3.contextId(), c1.contextId());

    int dummy = -1;
    dummy = CONTEXT_ID;

    // Note that this test expects that context id is never -1, which might
    // not be the case some day, who nows. Currently context ids start
    // from 0 and increase.
    TS_ASSERT_DIFFERS(dummy, -1);
}

/**
 * Tests a clocked operation.
 */
inline void
LanguageTest::testClockedOperation() {

    vector<SimValue*> outputs;
    SimValue out(32);
    outputs.push_back(&out);

    OperationContext context;

    OperationBehavior* clocked =
	loadBehavior("clockedOperation", "CLOCK", NullOperation::instance());

    clocked->createState(context);

    context.advanceClock();
    context.advanceClock();
    context.advanceClock();

    simulateTrigger(clocked, outputs, context);

    // clock was advanced from 0 three times so it should be 3 now
    TS_ASSERT_EQUALS(out.intValue(), 3);

    clocked->deleteState(context);
    deleteBehavior("clockedOperation", "CLOCK", clocked);
}

/**
 * Tests that sign extend and zero extend works.
 */
inline void
LanguageTest::testExtend() {

    vector<SimValue*> arguments;
    SimValue input(8);
    // 147 = 10010011
    input = 147;
    arguments.push_back(&input);
    
    OperationContext context;

    SimValue result(32);
    arguments.push_back(&result);

    OperationBehavior* sign_extend = loadBehavior("extend", "S_EXTEND");

    OperationBehavior* zero_extend = loadBehavior("extend", "Z_EXTEND");

    simulateTrigger(sign_extend, arguments, context);

    string binResult = Conversion::toBinString(
        result.intValue());
    TS_ASSERT_EQUALS(binResult, "11111111111111111111111110010011b");

    simulateTrigger(zero_extend, arguments, context);

    binResult = Conversion::toBinString(result.intValue());
    TS_ASSERT_EQUALS(binResult, "10010011b");

    deleteBehavior("extend", "S_EXTEND", sign_extend);
    deleteBehavior("extend", "Z_EXTEND", zero_extend);
}

#endif
