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
 * @file StackFrameManagementTest.hh
 *
 * A test suite for Stack frame management routines.
 *
 * @author Heikki Kultala 2006 (heikki.kultala@tut.fi)
 * @note rating: red
 */

#ifndef STACKFRAME_MANAGEMENT_TEST_HH
#define STACKFRAME_MANAGEMENT_TEST_HH

#include <string>
#include <vector>

#include "Program.hh"
#include "Procedure.hh"
#include "TPEFProgramFactory.hh"
#include "BinaryStream.hh"
#include "BinaryReader.hh"
#include "UniversalMachine.hh"
#include "RegisterFile.hh"
#include "TerminalRegister.hh"

#include "OldFrontendStackAnalyzer.hh"
#include "StackCodeRemover.hh"
#include "StackFrameCodeCreator.hh"

class StackFrameManagementTest : public CxxTest::TestSuite {
public:
    void setUp();
    void tearDown();

    StackFrameManagementTest();
    ~StackFrameManagementTest();
    
    TTAProgram::Program* readProgram(const std::string& tpefFileName);

    void annotate(TTAProgram::Program& program) throw (Exception);
    void remove(
        TTAProgram::Program& program, std::vector<StackFrameData*>& sfd) 
        throw (Exception);

    void updateRefs(
        TTAProgram::Program& program, std::vector <StackFrameData*>& sfd) 
        throw (Exception);

    void createSF(
        TTAProgram::Program& program, std::vector <StackFrameData*>& sfd) 
        throw (Exception);

    // gets immediate which is source of the only move of an instruction
    int getImmediateValue(const TTAProgram::Instruction& instruction);

    void testAnnotating1();
    void testAnnotating2();
    void testRemoving1();
    void testRemoving2();
    void testRefUpdates1();
    void testRefUpdates2();
    void testRefUpdates3();
    void testRefUpdates4();
    
    void testRecreating1();
    void testRecreating2();
    void testRecreating3();
    void testRecreating4();


    private:
    TTAProgram::Program* program1_;
    TTAProgram::Program* program2_;
    TTAProgram::Program* program3_;
    TTAProgram::Program* program4_;
    
    
    UniversalMachine umach_;
    std::vector <StackFrameData*> sfd1_;
    std::vector <StackFrameData*> sfd2_;
    std::vector <StackFrameData*> sfd3_;
    std::vector <StackFrameData*> sfd4_;

};


StackFrameManagementTest::StackFrameManagementTest() {
    program1_ = readProgram("../../../ProgramRepresentations/DataDependenceGraph/DataDependenceGraphTest/data/arrmul.tpef");
    program2_ = readProgram("data/multipar.tpef");
    
    program3_ = program1_->copy();
    program4_ = program2_->copy();
}

void 
StackFrameManagementTest::setUp() {
}

void 
StackFrameManagementTest::tearDown() {
}

StackFrameManagementTest::~StackFrameManagementTest() {
    if( program1_ != NULL) {
        delete program1_;
        program1_ = NULL;
    }
    if ( program2_ != NULL ) {
        delete program2_;
        program2_ = NULL;
    }
    for( int i = 0; i < sfd1_.size(); i++ ) {
        if(sfd1_.at(i) != 0 ) {
            delete(sfd1_.at(i)); 
            sfd1_.at(i) = NULL;
        }
    }

    for( int i = 0; i < sfd2_.size(); i++ ) {
        if(sfd2_.at(i) != 0 ) {
            delete(sfd2_.at(i)); 
            sfd2_.at(i) = NULL;
        }
    }

}

void StackFrameManagementTest::annotate(TTAProgram::Program& program) 
    throw (Exception) {
    OldFrontendStackAnalyzer analyzer;

    for( int i = 0; i < program.procedureCount(); i++ ) {
        analyzer.analyzeAndAnnotateProcedure(program.procedure(i));
    }
}


void StackFrameManagementTest::testAnnotating1() {
    try {
        annotate(*program1_);
        annotate(*program3_);
        TS_ASSERT_EQUALS(program1_->procedure(0).instructionCount(),15 );
    } catch (Exception &e) {
        std::cerr << e.fileName() << ":" << e.lineNum()
                  << " " << e.errorMessage() << std::endl;
        
//        delete program1_;
        TS_ASSERT(0);
    }
}

void StackFrameManagementTest::testAnnotating2() {
    try {
        annotate(*program2_);
        annotate(*program4_);
        TS_ASSERT_EQUALS(program2_->procedure(0).instructionCount(),15 )


    } catch (Exception &e) {
        std::cerr << e.fileName() << ":" << e.lineNum()
                  << " " << e.errorMessage() << std::endl;
        
        TS_ASSERT(0);
    }
}

void StackFrameManagementTest::remove(
    TTAProgram::Program& program, std::vector<StackFrameData*>& sfd) 
    throw (Exception) {

    StackCodeRemover remover;

    for( int i = 0; i < program.procedureCount(); i++ ) {
        sfd.push_back(
                remover.removeStackFrameCode(program.procedure(i)));
    }

}

void StackFrameManagementTest::testRemoving1() {
    try {
        remove(*program1_, sfd1_);
        remove(*program3_, sfd3_);

        TS_ASSERT_EQUALS(program1_->procedure(0).instructionCount(),2 );
        TS_ASSERT_EQUALS(program1_->procedure(1).instructionCount(),37 );
        TS_ASSERT_EQUALS(program1_->procedure(2).instructionCount(),12 );
        
        TS_ASSERT_EQUALS(sfd1_[0]->localVarSize(), 0);
        TS_ASSERT_EQUALS(sfd1_[1]->localVarSize(), 0);
        TS_ASSERT_EQUALS(sfd1_[2]->localVarSize(), 240);

        TS_ASSERT_EQUALS(sfd1_[0]->outputParamsSize(), 0);
        TS_ASSERT_EQUALS(sfd1_[1]->outputParamsSize(), 0);
        TS_ASSERT_EQUALS(sfd1_[2]->outputParamsSize(), 0);

// TODO: check
        TS_ASSERT_EQUALS(sfd1_[0]->stackInitAddr(), 16777932);
        TS_ASSERT_EQUALS(sfd1_[1]->stackInitAddr(), 0);
        TS_ASSERT_EQUALS(sfd1_[2]->stackInitAddr(), 0);

//        program3_ = program1_->copy();

    } catch (Exception &e) {
        std::cerr << e.fileName() << ":" << e.lineNum()
                  << " " << e.errorMessage() << std::endl;
        
        assert(0);
    }
}

void StackFrameManagementTest::testRemoving2() {
    try {
        remove(*program2_, sfd2_);
        remove(*program4_, sfd4_);

        TS_ASSERT_EQUALS(program2_->procedure(0).instructionCount(),2 )
        TS_ASSERT_EQUALS(program2_->procedure(1).instructionCount(),26 );
        TS_ASSERT_EQUALS(program2_->procedure(2).instructionCount(),1 );
        TS_ASSERT_EQUALS(program2_->procedure(3).instructionCount(),14 );

        TS_ASSERT_EQUALS(sfd2_[0]->localVarSize(), 0);
        TS_ASSERT_EQUALS(sfd2_[1]->localVarSize(), 0);
        TS_ASSERT_EQUALS(sfd2_[2]->localVarSize(), 0);
        TS_ASSERT_EQUALS(sfd2_[3]->localVarSize(), 0);

        TS_ASSERT_EQUALS(sfd2_[0]->outputParamsSize(), 0);
        TS_ASSERT_EQUALS(sfd2_[1]->outputParamsSize(), 0);
        TS_ASSERT_EQUALS(sfd2_[2]->outputParamsSize(), 0);
        TS_ASSERT_EQUALS(sfd2_[3]->outputParamsSize(), 8);

// TODO: check
        TS_ASSERT_EQUALS(sfd2_[0]->stackInitAddr(), 16777948);
        TS_ASSERT_EQUALS(sfd2_[1]->stackInitAddr(), 0);
        TS_ASSERT_EQUALS(sfd2_[2]->stackInitAddr(), 0);
        TS_ASSERT_EQUALS(sfd2_[3]->stackInitAddr(), 0);

        // do some modifications to program4 stacks.

        sfd4_[1]->setLocalVarSize(20);
        sfd4_[2]->setLocalVarSize(32);

        

    } catch (Exception &e) {
        std::cerr << e.fileName() << ":" << e.lineNum()
                  << " " << e.errorMessage() << std::endl;
        
        assert(0);
    }
}

void StackFrameManagementTest::updateRefs(
    TTAProgram::Program& program, std::vector <StackFrameData*>& sfd) 
    throw (Exception) {

    TTAProgram::Terminal* stackPointer = NULL;
    const TTAMachine::Machine::RegisterFileNavigator& rfNav
        = umach_.registerFileNavigator();
    for( int i = 0; i < rfNav.count(); i++ ) {
        const TTAMachine::RegisterFile& rf = *rfNav.item(i);
        if( rf.width() == 32 && rf.numberOfRegisters() > 1 ) {
            stackPointer = new TTAProgram::TerminalRegister(*rf.port(0) , 1);
        }
    }
    assert(stackPointer != NULL);
    
    StackFrameCodeCreator creator(umach_,umach_, *stackPointer);
    delete stackPointer;
    
    for( int i = 0; i < program1_->procedureCount(); i++ ) {
        creator.updateStackReferences(program1_->procedure(i));
    }

}


void StackFrameManagementTest::createSF(
    TTAProgram::Program& program, std::vector <StackFrameData*>& sfd) 
    throw (Exception) {

    TTAProgram::Terminal* stackPointer = NULL;
    const TTAMachine::Machine::RegisterFileNavigator& rfNav
        = umach_.registerFileNavigator();
    for( int i = 0; i < rfNav.count(); i++ ) {
        const TTAMachine::RegisterFile& rf = *rfNav.item(i);
        if( rf.width() == 32 && rf.numberOfRegisters() > 1 ) {
            stackPointer = new TTAProgram::TerminalRegister(*rf.port(0) , 1);
        }
    }
    TS_ASSERT_DIFFERS((int)stackPointer,0);
    
    StackFrameCodeCreator creator(umach_,umach_, *stackPointer);
    delete stackPointer;
    
    for( int i = 0; i < program.procedureCount(); i++ ) {
        creator.setParameters(*sfd[i]);
        creator.createStackFrameCode(program.procedure(i));
    }

}



void StackFrameManagementTest::testRecreating1() {
    try {
        createSF(*program1_, sfd1_);

        TS_ASSERT_EQUALS(program1_->procedure(0).instructionCount(),3 )
        TS_ASSERT_EQUALS(program1_->procedure(1).instructionCount(),37 );
        TS_ASSERT_EQUALS(program1_->procedure(2).instructionCount(),22 );

    } catch (Exception &e) {
        std::cerr << e.fileName() << ":" << e.lineNum()
                  << " " << e.errorMessage() << std::endl;
        
        TS_ASSERT(0);
    }
}

void StackFrameManagementTest::testRecreating2() {
    try {
        createSF(*program2_, sfd2_);

        TS_ASSERT_EQUALS(program2_->procedure(0).instructionCount(),3 )
        TS_ASSERT_EQUALS(program2_->procedure(1).instructionCount(),26 );
        TS_ASSERT_EQUALS(program2_->procedure(2).instructionCount(),1 );
        TS_ASSERT_EQUALS(program2_->procedure(3).instructionCount(),30 );

    } catch (Exception &e) {
        std::cerr << e.fileName() << ":" << e.lineNum()
                  << " " << e.errorMessage() << std::endl;
        
        TS_ASSERT(0);
    }
}

void StackFrameManagementTest::testRecreating3() {
    try {
        createSF(*program3_, sfd3_);

        TS_ASSERT_EQUALS(program3_->procedure(0).instructionCount(),3 )
        TS_ASSERT_EQUALS(program3_->procedure(1).instructionCount(),37 );
        TS_ASSERT_EQUALS(program3_->procedure(2).instructionCount(),22 );

    } catch (Exception &e) {
        std::cerr << e.fileName() << ":" << e.lineNum()
                  << " " << e.errorMessage() << std::endl;
        
        TS_ASSERT(0);
    }
}


void StackFrameManagementTest::testRecreating4() {
    try {
        createSF(*program4_, sfd4_);

        TS_ASSERT_EQUALS(program4_->procedure(0).instructionCount(),3 )
        TS_ASSERT_EQUALS(program4_->procedure(1).instructionCount(),32 );
        TS_ASSERT_EQUALS(program4_->procedure(2).instructionCount(),7 );
        TS_ASSERT_EQUALS(program4_->procedure(3).instructionCount(),30 );

    } catch (Exception &e) {
        std::cerr << e.fileName() << ":" << e.lineNum()
                  << " " << e.errorMessage() << std::endl;
        
        TS_ASSERT(0);
    }
}

void StackFrameManagementTest::testRefUpdates1() {
    try {
        updateRefs(*program1_, sfd1_);
    } catch (Exception &e) {
        std::cerr << e.fileName() << ":" << e.lineNum()
                  << " " << e.errorMessage() << std::endl;
        
        assert(0);
    }
}

void StackFrameManagementTest::testRefUpdates2() {
    try {
        updateRefs(*program2_, sfd2_);
    } catch (Exception &e) {
        std::cerr << e.fileName() << ":" << e.lineNum()
                  << " " << e.errorMessage() << std::endl;
        
        assert(0);
    }
}


void StackFrameManagementTest::testRefUpdates3() {
    try {
        updateRefs(*program3_, sfd3_);
    } catch (Exception &e) {
        std::cerr << e.fileName() << ":" << e.lineNum()
                  << " " << e.errorMessage() << std::endl;
        
        assert(0);
    }
}


void StackFrameManagementTest::testRefUpdates4() {
    try {
        updateRefs(*program4_, sfd4_);
    } catch (Exception &e) {
        std::cerr << e.fileName() << ":" << e.lineNum()
                  << " " << e.errorMessage() << std::endl;
        
        assert(0);
    }
}


TTAProgram::Program*
StackFrameManagementTest::readProgram(const std::string& tpefFileName) {

    TPEF::BinaryStream binaryStream(tpefFileName);
    
    // read to TPEF Handler Module
    TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);

    assert(tpef_ != NULL);

    // convert the loaded TPEF to POM
    TTAProgram::TPEFProgramFactory factory(*tpef_, umach_);
    TTAProgram::Program* currentProgram = factory.build();
    
    return currentProgram;
}


#endif
