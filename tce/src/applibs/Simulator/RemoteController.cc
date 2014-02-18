/*
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
 * @file RemoteController.cc
 * @author Kalle Raiskila 2014
 */

#include "RemoteController.hh"
#include "SimValue.hh"
#include "ProgramImageGenerator.hh"
#include "BinaryEncoding.hh"
#include "BEMGenerator.hh"
#include "Binary.hh"
#include "ProgramWriter.hh"

#define DEBUG_REMOTE_DEBUGGER_CONTROLLER
#ifdef DEBUG_REMOTE_DEBUGGER_CONTROLLER
#include <cstdio>
#define _DPRINT(...) printf(__VA_ARGS__)
#define _WHERE() printf("%s:%d - %s\n", __FILE__, __LINE__, __func__)
#else
#define _DPRINT(...) 
#define _WHERE()
#endif

#include <iostream>

RemoteController::RemoteController( 
        SimulatorFrontend& frontend,
        const TTAMachine::Machine& machine, 
        const TTAProgram::Program& program) : 
        TTASimulationController(frontend, machine, program),
        imemImage(std::ostringstream::binary) {
    exitPoints = findProgramExitPoints(program, machine);
}

void
RemoteController::loadIMemImage() {

    ProgramImageGenerator thePIG;
    BinaryEncoding *theBE;
    TPEF::Binary *binary;
    // TODO. better name from somewhere?
    std::string programName("the program"); 
    // clear the old image
    imemImage.str("");

    // Binary encoding of the instruction sream
    // TODO: loading of instruction compressors would happen here?
    BEMGenerator theBEMGen(sourceMachine_);
    theBE = theBEMGen.generate(); 

    TTAProgram::ProgramWriter writer(program_);
    binary = writer.createBinary();
    ProgramImageGenerator::TPEFMap tmap;
    // TODO: have some actual name for this program?
    tmap[programName] = binary;

    thePIG.loadBEM(*theBE);
    thePIG.loadMachine(sourceMachine_); 
    thePIG.loadPrograms(tmap);
    // clear the old image
    
    // put the instructions into imemImage
    thePIG.generateProgramImage(
        programName, imemImage,
        ProgramImageGenerator::BINARY);

    writeIMem(imemImage.str().c_str(), imemImage.str().size());

    delete binary;
    delete theBE;
        
    return;
   
}
/* vim: set ts=4 expandtab: */
