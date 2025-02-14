/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file OperationTestGenerator.cc
 *
 * Implementation of OperationTestGenerator class.
 *
 * Created on: 12.3.2015
 * @author: Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "FUOperationTestGenerator.hh"

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include "TestCase.hh"
#include "Program.hh"
#include "ADFSerializer.hh"
#include "Machine.hh"
#include "FunctionUnit.hh"
#include "CompileTools.hh"
#include "TCEString.hh"
#include "OperationPool.hh"
#include "Operation.hh"
#include "Operand.hh"
#include "HWOperation.hh"
#include "Conversion.hh"
#include "RandomNumberGenerator.hh"
#include "LLVMIRTools.hh"
#include "MachineInfo.hh"
#include "MathTools.hh"

//#define DEBUG_OPERATIONTESTGEN
#ifdef DEBUG_OPERATIONTESTGEN
#include <fstream>
#endif

FUOperationTestGenerator::FUOperationTestGenerator()
    : TestGeneratorBase(
        std::string("operation-tests"),
        std::string("Generates tests for basic operations for each Function "
                    "Unit. Operations to be testes are ones without side "
                    "effects and memory accesses.")) {
}

FUOperationTestGenerator::~FUOperationTestGenerator() {
}


/**
 * Generates test cases to try each pure operations of each function unit.
 *
 */
std::vector<TestCase>
FUOperationTestGenerator::generateTestCasesImpl(
    RandomNumberGenerator::SeedType seed) {
    using namespace TTAMachine;
    using namespace TTAProgram;
    using std::endl;
    std::vector<TestCase> testcases;
    if (!Application::shellCommandsExists("tcecc")) {
        return testcases;
    }

    std::stringstream globalStream;
    std::stringstream codeStream;
    OperationPool operPool;

    RandomNumberGenerator throwDice(seed);
    LLVMIRTools llvmir;

    typedef std::vector<Operation::InputOperandVector> TestVectorListType;
    typedef std::map<const HWOperation*, TestVectorListType>
        OperationTestVectorsMapType;
    OperationTestVectorsMapType operationTestVectors;
    std::map<std::string, std::string> definedGlobals;

    // Generate separate test case for each function unit //
    Machine::FunctionUnitNavigator fuNav = machine().functionUnitNavigator();
    for (int fui = 0; fui < fuNav.count(); fui++) {
        const FunctionUnit& fu = *fuNav.item(fui);
        operationTestVectors.clear();
        definedGlobals.clear();
        globalStream.str(std::string());
        codeStream.str(std::string());
        llvmir.resetIdentifiers();
        // Collect test vector for each operation of the function unit //
        for (int opi = 0 ; opi < fu.operationCount(); opi++) {
            const HWOperation& hwOperation = *fu.operation(opi);
            const Operation& osalOperation =
                operPool.operation(hwOperation.name().c_str());
            if (!osalOperation.isPure()
                || osalOperation.numberOfOutputs() == 0) {
                continue;
            }
            TestVectorListType& tmp = operationTestVectors[&hwOperation];
            osalOperation.makeTestVectors(tmp, throwDice());
        }
        if (operationTestVectors.empty()) {
            continue;
        }
        // Write LLVM IR assembly code from the test vectors //
        OperationTestVectorsMapType::const_iterator otv_it;

        for (otv_it = operationTestVectors.begin();
            otv_it != operationTestVectors.end(); otv_it++) {
            const HWOperation& hwOperation = *otv_it->first;
            const TestVectorListType& testVectors = otv_it->second;
            TestVectorListType::const_iterator tv_it;
            for (tv_it = testVectors.begin();
                tv_it != testVectors.end(); tv_it++) {
                const Operation::InputOperandVector& operandValues = *tv_it;
                const Operation& osalOperation =
                    operPool.operation(hwOperation.name().c_str());
                std::string fuName = hwOperation.parentUnit()->name();
                std::string opName = hwOperation.name();
                std::string identifier = llvmir.newIdentifier();

                LLVMIRTools::Type returnType =
                    llvmir.outputTypeListOf(osalOperation);

                // Call tce macro //
                codeStream << "  " <<
                    llvmir.assignment(
                        identifier,
                        llvmir.inlineAssembly(
                            fuName + "." + TCEString::toUpper(opName),
                            returnType.toString(),
                            llvmir.inlineAssemblyArguments(
                                osalOperation, operandValues),
                            "" // = no meta data
                        )
                    ) << endl;
                // Store result to  global variable so the results appears //
                // on bus trace                                            //
                if (returnType.isStructure()) {
                    // Generate extract field instructions
                    for (size_t i = 0; i < returnType.subTypes().size(); i++) {
                        const LLVMIRTools::Type& subType =
                            returnType.subTypes().at(i);
                        std::string extractionIdentifier =
                            llvmir.newIdentifier();
                        codeStream << "  " << llvmir.extractValue(
                            extractionIdentifier, identifier, returnType, i)
                                   << endl;
                        if (definedGlobals.count(subType.toString()) == 0) {
                            globalStream << llvmir.newGlobalDecl(
                                subType.toString() + " zeroinitializer",
                                Conversion::toString(subType.getAlignment()));
                            globalStream << endl;
                            definedGlobals[subType.toString()] =
                                llvmir.lastGlobalIdentifier();
                        }
                        codeStream << "  " << llvmir.store(
                            subType, extractionIdentifier,
                            definedGlobals[subType.toString()],true) << endl;
                    }
                } else {
                    if (definedGlobals.count(returnType.toString()) == 0) {
                        globalStream << llvmir.newGlobalDecl(
                            returnType.toString() + " zeroinitializer",
                            Conversion::toString(returnType.getAlignment()));
                        globalStream << endl;
                        definedGlobals[returnType.toString()] =
                            llvmir.lastGlobalIdentifier();
                    }
                    std::string typeStr = returnType.toString();
                    codeStream << "  "
                        << llvmir.store(returnType, identifier,
                            definedGlobals[typeStr], true) << endl;
                }
            } // for each operation test vectors
        } // for each test vectors set of a operation

        if (codeStream.rdbuf()->in_avail() == 0) {
            continue;
        }

        // Finalize code and try to compile it and convert it to Program //
        std::stringstream completeCode;
        completeCode << llvmir.targetInfo(machine().isLittleEndian())
                     << globalStream.rdbuf()
                     << llvmir.mainBody(codeStream.str()) << std::flush;

#ifdef DEBUG_OPERATIONTESTGEN
        std::string codeDumpStr = completeCode.str();
#endif

        // Sequential schedule, so that operations are not optimized away.
        // Compiling some emulation libs seems to take forever to compile.
        std::string compileOptions = "--sequential-schedule --no-emulationlib";
        Program* compiled =
            CompileTools::compileAsLLVM(machine(), completeCode,
                                        compileOptions);
        if (compiled != NULL) {
            testcases.push_back(TestCase(
                compiled, fu.name() + "-operation-tests"));
            if (Application::spamVerbose()) {
                std::cerr << title() <<": Generated test case for FU \""
                          << fu.name() << "\"" << endl;
            }
        } else {
            if (Application::spamVerbose()) {
                std::cerr << title() <<": Compilation of a test case failed "
                          << "for FU " << fu.name() << endl;
            }
        }
#ifdef DEBUG_OPERATIONTESTGEN
        std::ofstream codeDump(fu.name() + "-operation-tests.ll"
            + TCEString::applyIf(compiled == NULL, ".fail"));
        codeDump << codeDumpStr;
        codeDump.close();
#endif
    } // End of for-each-FU
    return testcases;
}

