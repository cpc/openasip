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
 * @file EstimatorCmdLineUI.cc
 *
 * Implementation of estimate.
 *
 * The command line user interface of cost estimator.
 *
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <iostream>
#include <cmath>

#include "EstimatorCmdLineOptions.hh"
#include "Machine.hh"
#include "ADFSerializer.hh"
#include "MachineImplementation.hh"
#include "IDFSerializer.hh"
#include "Program.hh"
#include "NullProgram.hh"
#include "Binary.hh"
#include "FileSystem.hh"
#include "BinaryStream.hh"
#include "BinaryReader.hh"
#include "TPEFProgramFactory.hh"
#include "ExecutionTrace.hh"
#include "Estimator.hh"

using namespace CostEstimator;

/// this is set to true in case energy estimation can be performed with the
/// given input files
bool energyEstimation = false;
/// the architecture definition of the estimated processor
TTAMachine::Machine* machine = NULL;
/// the implementation definition of the estimated processor
IDF::MachineImplementation* implementation = NULL;
/// the estimated program is stored in this variable (in case energy estimation
/// is wanted
TTAProgram::Program* program = NULL;
/// the execution trace database
ExecutionTrace* trace = NULL;

/**
 * Loads the input files using the command line arguments.
 *
 * @param options The command line arguments.
 * @return True in case there was no errors in the arguments.
 */
bool 
loadInputs(EstimatorCmdLineOptions& options) {
    
    if (options.numberOfArguments() < 2) {
        std::cerr << "ADF and IDF are required." << std::endl;
        return false;
    }
    else if (options.numberOfArguments() > 2) {
        std::cerr << "Illegal command line arguments." << std::endl;
        return false;
    }
    
    std::string adfFile = options.argument(1);
    std::string idfFile = options.argument(2);    
    
    try {
        ADFSerializer serializer;
        serializer.setSourceFile(adfFile);
        machine = serializer.readMachine();	
    } catch (const Exception& e) {
        std::cerr << "Error while loading ADF. " << e.errorMessage() 
                  << std::endl;
        return false;
    }

    try {
        IDF::IDFSerializer serializer;
        serializer.setSourceFile(idfFile);
        implementation = serializer.readMachineImplementation();	
    } catch (const Exception& e) {
        std::cerr << "Error while loading IDF. " << e.errorMessage() 
                  << std::endl;
        return false;
    }

    if (options.TPEF() != "") {
        
        if (options.traceDB() == "") {
            std::cerr << "Also TraceDB is required for energy estimation."
                      << std::endl;
            return false;
        }

        TPEF::Binary* tpef = NULL;
        try {
            const std::string tpefFileName = options.TPEF();
            if (!FileSystem::fileExists(tpefFileName)) {
                std::cerr << "Error while loading TPEF. " 
                          << "Cannot open file '" << tpefFileName << "'."
                          << std::endl;
                delete machine;
                machine = NULL;
                delete implementation;
                implementation = NULL;
                return EXIT_FAILURE;
            }
            TPEF::BinaryStream binaryStream(tpefFileName);

            // read first to a TPEF Handler Module
            tpef = TPEF::BinaryReader::readBinary(binaryStream);

            assert(tpef != NULL);
            assert(machine != NULL);

            // convert the loaded TPEF to POM
            TTAProgram::TPEFProgramFactory factory(*tpef, *machine);
            program = factory.build();
            delete tpef;
            tpef = NULL;      
        } catch (const Exception& e) {
            std::cerr << "Error while loading TPEF. " << e.errorMessage()
                      << std::endl;
            delete tpef;
            tpef = NULL;
            return false;
        }
    }

    if (options.traceDB() != "") {
        
        const std::string traceDBFileName = options.traceDB();
        if (!FileSystem::fileExists(traceDBFileName)) {
            std::cerr << "Error while loading TraceDB. " 
                      << "Cannot open file '" << traceDBFileName << "'."
                      << std::endl;
            return false;
        }

        try {
            trace = ExecutionTrace::open(traceDBFileName);
        } catch (const Exception& e) {
            std::cerr << "Error while loading TraceDB. " << e.errorMessage() 
                      << std::endl;
            return false;
        }
        if (program == NULL)
            program = &TTAProgram::NullProgram::instance();
        energyEstimation = true;
    }
    return true;
}

/**
 * Frees all allocated input resources.
 */
void cleanup() {

    delete machine;
    machine = NULL;
    delete implementation;
    implementation = NULL;

    if (energyEstimation) {
        delete program;
        program = NULL;
        delete trace;
        trace = NULL;
    }
}

/**
 * Main function.
 *
 * Parses the command line and executes cost estimation functionality.
 *
 * @param argc The command line argument count.
 * @param argv The command line arguments (passed to the interpreter).
 * @return The return status.
 */
int 
main(int argc, char* argv[]) {

    Application::initialize();    

    EstimatorCmdLineOptions *options = new EstimatorCmdLineOptions();
    try {
        options->parse(argv, argc);
        Application::setCmdLineOptions(options);
    } catch (ParserStopRequest) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& i) {
        std::cerr << i.errorMessage() << std::endl;
        return EXIT_FAILURE;
    }

    if (!loadInputs(*options)) {
        cleanup();
        return EXIT_FAILURE;
    }

    Estimator estimator;
    if (options->totalArea() || !options->runOnlyEstimations()) {
        int totalArea = -1;
        try {
            totalArea = round(estimator.totalArea(*machine, *implementation));
        } catch (const Exception& e) {
            std::cerr << "estimation failed: " + e.errorMessage() 
                      << std::endl;
        }

        if (totalArea >= 0) {
            std::cout << "total area: " << totalArea << " gates"
                      << std::endl;
		}
    }

    if (options->longestPath() || !options->runOnlyEstimations()) {
        std::cout << "delay of the longest path: ";
        try {
            std::cout 
                << round(estimator.longestPath(*machine, *implementation))
                << " ns" << std::endl;
        } catch (const Exception& e) {
            std::cerr << "estimation failed: " + e.errorMessage() 
                      << std::endl;
        }
    }

    if (energyEstimation && 
            (options->totalEnergy() || !options->runOnlyEstimations())) {
        std::cout << "total consumed energy: ";
        try {
            std::cout 
                << estimator.totalEnergy(
                        *machine, *implementation, *program, *trace)
                << " mJ" << std::endl;
        } catch (const Exception& e) {
            std::cerr << "estimation failed: " + e.errorMessage() 
                      << std::endl;
        }
    }

    cleanup();
    return EXIT_SUCCESS;
}
