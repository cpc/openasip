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
        TTASimulationController( frontend, machine, program ),
        imemImage(std::ostringstream::binary)
{
    exitPoints = findProgramExitPoints( program, machine );
}

void RemoteController::loadIMemImage()
{
    ProgramImageGenerator thePIG;
    BinaryEncoding *theBE;
    TPEF::Binary *binary;
    // TODO. better name from somewhere?
    std::string programName("the program"); 
    // clear the old image
    imemImage.str("");

    // Binary encoding of the instruction sream
    // TODO: loading of instruction compressors would happen here?
    BEMGenerator theBEMGen( sourceMachine_ );
    theBE = theBEMGen.generate(); 

    TTAProgram::ProgramWriter writer( program_ );
    binary = writer.createBinary();
    ProgramImageGenerator::TPEFMap tmap;
    // TODO: have some actual name for this program?
    tmap[programName]=binary;

    thePIG.loadBEM( *theBE );
    thePIG.loadMachine( sourceMachine_ ); 
    thePIG.loadPrograms(tmap);
    // clear the old image
    
    // put the instructions into imemImage
    thePIG.generateProgramImage(programName, imemImage,
                                ProgramImageGenerator::BINARY);

    writeIMem( imemImage.str().c_str(), imemImage.str().size() );

    delete binary;
    delete theBE;
        
    return;
   
}
/* vim: set ts=4 expandtab: */
