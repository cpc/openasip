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
 * @file Program.cc
 *
 * Implementation of Program class.
 *
 * @author Ari Mets‰halme 2005 (ari.metsahalme-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>

#include "Program.hh"
#include "NullInstruction.hh"
#include "Procedure.hh"
#include "NullProcedure.hh"
#include "CodeLabel.hh"
#include "Terminal.hh"
#include "TerminalAddress.hh"
#include "Move.hh"
#include "InstructionReference.hh"
#include "NullProgram.hh"
#include "AddressSpace.hh"
#include "DataMemory.hh"
#include "DataDefinition.hh"
#include "DataInstructionAddressDef.hh"
#include "AssocTools.hh"
#include "ControlUnit.hh"
#include "UniversalMachine.hh"
#include "SequenceTools.hh"
#include "DataLabel.hh"
#include "BinaryStream.hh"
#include "BinaryReader.hh"
#include "TPEFProgramFactory.hh"
#include "UniversalMachine.hh"
#include "Application.hh"
#include "ProgramWriter.hh"
#include "BinaryWriter.hh"
#include "TPEFWriter.hh"

using std::string;

using namespace TTAMachine;

namespace TTAProgram {

/////////////////////////////////////////////////////////////////////////////
// Program
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * Creates an empty program in given address space. A default value
 * zero is used for start and entry addresses. Automatically creates
 * an empty global scope to store all program symbols with external
 * linkage.
 *
 * @param space The address space of the program.
 */
Program::Program(const AddressSpace& space):
    start_(0, space), entry_(0, space) {
    globalScope_ = new GlobalScope();
    refManager_ = new InstructionReferenceManager();
}

/**
 * Constructor.
 *
 * Alternative constructor that takes the start address as a parameter.
 *
 * @param space The address space of the program.
 * @param start The start address of the program.
 */
Program::Program(const AddressSpace& space, Address start):
    start_(start), entry_(0, space) {
    globalScope_ = new GlobalScope();
    refManager_ = new InstructionReferenceManager();
}

/**
 * Constructor.
 *
 * Alternative constructor that takes the start and entry addresses as
 * parameters.
 *
 * @param space The address space of the program.
 * @param start The start address of the program.
 * @param entry The entry address of the program.
 */
Program::Program(
    const AddressSpace&, Address start, Address entry):
    start_(start), entry_(entry) {
    globalScope_ = new GlobalScope();
    refManager_ = new InstructionReferenceManager();
}

/**
 * The destructor.
 */
Program::~Program() {
    cleanup();
    delete globalScope_;
    globalScope_ = NULL;
    delete refManager_;
    refManager_ = NULL;
}

/**
 * Cleans up the program to a state it was at the construction.
 */
void
Program::cleanup() {

    SequenceTools::deleteAllItems(procedures_);
    SequenceTools::deleteAllItems(dataMems_);

    delete globalScope_;
    globalScope_ = NULL;
    delete refManager_;
    refManager_ = NULL;
    globalScope_ = new GlobalScope();
    refManager_ = new InstructionReferenceManager();
}

/**
 * Returns the global scope of the program.
 *
 * @return The global scope of the program.
 */
GlobalScope&
Program::globalScope() {
    return *globalScope_;
}

/**
 * Returns the global scope of the program.
 *
 * Const version.
 *
 * @return The global scope of the program.
 */
const GlobalScope&
Program::globalScopeConst() const {
    return *globalScope_;
}

/**
 * Returns the target TTA processor of the program.
 *
 * @return The target TTA processor of the program.
 */
Machine&
Program::targetProcessor() const {
    return *start_.space().machine();
}

/**
 * Returns the first procedure of the program.
 *
 * @return The first procedure of the prgram.
 * @exception InstanceNotFound if there are no procedures.
 */
Procedure&
Program::firstProcedure() const throw (InstanceNotFound) {
    if (procedureCount() > 0) {
        return *procedures_.at(0);
    } else {
        throw InstanceNotFound(
            __FILE__, __LINE__, __func__,
            "There are no procedures in the program.");
    }
}

/**
 * Returns the last procedure of the program.
 *
 * @return The last procedure of the prgram.
 * @exception InstanceNotFound if there are no procedures.
 */
Procedure&
Program::lastProcedure() const 
    throw (InstanceNotFound) {
    if (procedureCount() > 0) {
        return *procedures_.at(procedureCount() - 1);
    } else {
        throw InstanceNotFound(
            __FILE__, __LINE__, __func__,
            "There are no procedures in the program.");
    }
}

/**
 * Returns the procedure following the given procedure.
 *
 * @return The procedure following the given procedure or a null procedure,
 *         if the given procedure is the last procedure in the program.
 * @param proc The following procedure of this parameter is returned.
 * @exception IllegalRegistration if the given procedure does not belong
 *            to this program.
 */
Procedure&
Program::nextProcedure(const Procedure& proc) const
    throw (IllegalRegistration) {

    if (!proc.isInProgram() || &proc.parent() != this) {
        throw IllegalRegistration(
            __FILE__, __LINE__, __func__,
            "The procedure given in parameter doesn't "
            "belong to this program.");
    }

    if (&proc != &lastProcedure()) {
        int i = 0;
        for (; i < procedureCount(); i++) {
            if (procedures_.at(i) == &proc) {
                break;
            }
        }
        assert(i < procedureCount() - 1);
        return *procedures_.at(i + 1);

    } else {
        return NullProcedure::instance();
    }
}

/**
 * Returns the start (lowest) address of the program. 
 * 
 * The start address of the program is not necessarily the lowest address of 
 * the instruction address space.
 *
 * @note Preserving this value is not implemented in TPEFProgramFactory
 *       and ProgramWriter, however implementation of property is already 
 *       specified in TPEF specs.
 *
 * @return The start (lowest) address of the program.
 */
Address
Program::startAddress() const {
    return start_;
}

/**
 * Sets the start (lowest) address of the program. 
 * 
 * The start address of the program is not necessarily the lowest address of 
 * the instruction address space. Relocates instructions to start from the
 * address.
 *
 * @note Preserving this value is not implemented in TPEFProgramFactory
 *       and ProgramWriter, however implementation of property is already 
 *       specified in TPEF specs.
 *
 * @param start The start (lowest) address of the program.
 */
void
Program::setStartAddress(Address start) {

    start_ = start;

    // move procedures before the new starting address
    if (procedureCount() > 0 && 
        firstProcedure().startAddress().location() < start_.location()) {
        moveProcedure(
            firstProcedure(), 
            start_.location() - firstProcedure().startAddress().location());
    }
}

/**
 * Returns the address of the program entry point.
 *
 * The entry point is not necessarily the instruction of the program with
 * the lowest address.
 *
 * @note Preserving this value is not implemented in TPEFProgramFactory
 *       and ProgramWriter, however implementation of property is already 
 *       specified in TPEF specs.
 *
 * @return The address of the program entry point.
 */
Address
Program::entryAddress() const {
    return entry_;
}

/**
 * Sets the entry address of the program.
 *
 * @note Preserving this value is not implemented in TPEFProgramFactory
 *       and ProgramWriter, however implementation of property is already 
 *       specified in TPEF specs.
 */
void
Program::setEntryAddress(Address address) {
    entry_ = address;
}

/**
 * Returns the first instruction of the program.
 *
 * @return The first instruction of the program.
 * @exception InstanceNotFound There is no instructions in program.
 */
Instruction&
Program::firstInstruction() const 
    throw (InstanceNotFound) {
    if (!procedures_.empty()) {
        return firstProcedure().firstInstruction();
    } else {
        throw InstanceNotFound(
            __FILE__, __LINE__, __func__,
            "There is no instructions in the program.");
    }
}

/**
 * Returns the instruction at a given address.
 *
 * The address space of the address is implied, since there is only
 * one address space for instructions.
 *
 * @param address The address of the instruction.
 * @exception KeyNotFound if given address is illegal.
 * @return The instruction at the given address.
 */
Instruction&
Program::instructionAt(InstructionAddress address) const 
    throw (KeyNotFound) {

    for(ProcIter iter = procedures_.begin(); iter != procedures_.end();
        iter++) {

        if ((*iter)->startAddress().location() <= address &&
            (*iter)->endAddress().location() > address) {
            return (*iter)->instructionAt(address);
        }
    }

    throw KeyNotFound(
        __FILE__, __LINE__, __func__, 
        "No instruction at address: " +
        Conversion::toString(address));
}

/**
 * Returns the next instruction in the instruction stream.
 *
 * @return The next instruction in the instruction stream or a special
 * null instruction if the given instruction is the last instruction
 * of the program.
 *
 * @exception IllegalRegistration If given instruction does not belong
 * to the program.
 */
Instruction&
Program::nextInstruction(const Instruction& ins) const
    throw (IllegalRegistration) {

    if (!ins.isInProcedure() || !ins.parent().isInProgram() ||
        &ins.parent().parent() != this) {

        throw IllegalRegistration(
            __FILE__, __LINE__, __func__,
            "Instruction in parameter doesn't belong to this program.");
    }

    unsigned int insAddress = ins.address().location();

    ProcIter iter = procedures_.begin();
    while (iter != procedures_.end()) {

        // if the current instruction address fits procedure's
        // address space
        if ((*iter)->startAddress().location() <= insAddress &&
            (*iter)->endAddress().location() > insAddress) {

            // return next instruction, if it's in the same procedure
            if ((*iter)->hasNextInstruction(ins)) {
                return (*iter)->nextInstruction(ins);

                // otherwise find the next non-empty procedure and return
                // its first instruction
            } else {
                iter++;
                while (iter != procedures_.end()) {
                    if ((*iter)->instructionCount() != 0) {
                        return (*iter)->instructionAt(
                            (*iter)->startAddress().location());
                    }
                    iter++;
                }

                // if no non-empty procedures found, return null instruction
                return NullInstruction::instance();
            }
        }
        iter++;
    }

    // no procedure where current instruction fits was found
    // (should throw exception?)
    return NullInstruction::instance();
}

/**
 * Returns the last instruction of the program.
 *
 * @return The last instruction of the program.
 * @exception InstanceNotFound There is no instructions in the program.
 */
Instruction&
Program::lastInstruction() const throw (InstanceNotFound) {
    if (!procedures_.empty()) {
        return lastProcedure().lastInstruction();
    } else {
        throw InstanceNotFound(
            __FILE__, __LINE__, __func__,
            "There is no instructions in the program.");
    }
}

/**
 * Returns a move at given location, counting from beginning, zero
 * @param number zero-beginning count of move's location
 * @return a reference to the found move
 * @exception KeyNotFound if the move wasn't found at the given location
 */
const TTAProgram::Move& 
Program::moveAt(int number) const throw (KeyNotFound) {
    if (number < 0 || number >= static_cast<int>(moves_.size())) {
        throw KeyNotFound(__FILE__, __LINE__, __FUNCTION__,
            "Move not found at location: " + Conversion::toString(number));
    }
    return *moves_.at(number);
}

/**
 * Returns the total count of moves used in the program
 * @return the total count of moves used in the program
 */
int 
Program::moveCount() const {
    return static_cast<int>(moves_.size());
}

/**
 * Returns the procedure at a given index in the container of procedures.
 *
 * This method is for quickly traversing through all the procedures within
 * time critical loops. 
 *
 * @param index The procedure index in the list of procedures.
 * @return The Procedure at the given index.
 */
const Procedure&
Program::procedureAtIndex(int index) const {
    return *procedures_.at(index);
}

/**
 * Insert the given procedure as the last procedure of the program.
 *
 * The ownership of the procedure will be passed to the program. Labels 
 * must be added explicitely by client because only it knows the scope
 * where to add the labels.
 *
 * @param proc The procedure to be inserted.
 * @exception IllegalRegistration if the procedure belongs to another
 *                                program.
 */
void
Program::addProcedure(Procedure* proc) throw (IllegalRegistration) {

    if (proc->isInProgram()) {
        throw IllegalRegistration(
            __FILE__, __LINE__, __func__,
            "Procedure is already in program.");
    } else {

        proc->setParent(*this);
        if (!procedures_.empty()) {
            proc->setStartAddress(procedures_.back()->endAddress());
        } else {
            proc->setStartAddress(start_);
        }

        procedures_.push_back(proc);
    }
}

/**
 * Insert the given instruction as the instruction of the last
 * procedure of the program.
 *
 * The ownership of the instruction will be passed to the procedure in which
 * the instruction is inserted.
 *
 * @param ins The instruction to be inserted.
 * @exception IllegalRegistration if the instruction belongs to a
 *            procedure or there are no procedures in the program.
 */
void
Program::addInstruction(Instruction* ins) throw (IllegalRegistration) {

    if (procedures_.empty()) {
        throw IllegalRegistration(
            __FILE__, __LINE__, __func__,
            "No procedures in the program.");
    }
    
    // add all moves of an instruction to the move list
    for (int i = 0; i < ins->moveCount(); ++i) {
        moves_.push_back(&ins->move(i));
    }

    if (!ins->isInProcedure()) {
        procedures_.back()->add(ins);
    } else {
        throw IllegalRegistration(
            __FILE__, __LINE__, __func__,
            "Instruction already belongs to a procedure.");
    }
}

/**
 * Moves the given procedure and its succeeding procedures the given 
 * count of instructions.
 *
 * @note If the given count is negative, does not perform any checking for
 * overlapping procedures, thus the relocated procedure might end up
 * sharing instruction indices with the preceeding procedure,
 * which means a corrupted program.
 *
 * @param proc The procedure to move.
 * @param howMuch How many instructions the procedure should be moved.
 */
void
Program::moveProcedure(Procedure& proc, int howMuch) {
    unsigned int index;
    // skip procedures before given procedure
    for (index = 0; index < procedures_.size(); index++) {
        if (procedures_[index] == &proc) {
            break; 
        }
    } 
    // only update procedures after.
    for (;index < procedures_.size(); index++) {
        Procedure* p2 = procedures_[index];
        UIntWord oldAddr = p2->startAddress().location();
        p2->setStartAddress(Address(oldAddr + howMuch, start_.space()));
    }
}

/**
 * Returns the number of procedures in the program.
 *
 * @return The number of procedures in the program.
 */
int
Program::procedureCount() const {
    return procedures_.size();
}

/**
 * Returns the procedure at the given index.
 *
 * @param index The position index.
 * @return The procedure at the given index.
 * @exception OutOfRange if the index is out of range.
 */
Procedure&
Program::procedure(int index) const throw (OutOfRange) {
    if (index >= 0 &&
        static_cast<unsigned int>(index) < procedures_.size()) {
        return *procedures_.at(index);
    } else {
        throw OutOfRange(
            __FILE__, __LINE__, __func__,
            "There is no procedure by index: " + 
            Conversion::toString(index));
    }
}

/**
 * Returns the procedure with the given name.
 *
 * @param name The procedure name.
 * @return The procedure with given name.
 * @exception KeyNotFound if the procedure cannot be found.
 */
Procedure&
Program::procedure(const std::string& name) const
    throw (KeyNotFound) {
    ProcList::const_iterator i = procedures_.begin();
    while (i != procedures_.end()) {
        if ((*i)->name() == name) {
            return *(*i);
        }
        ++i;
    }

    throw KeyNotFound(
        __FILE__, __LINE__, __func__,
        "No procedure found with name: " +
        name);
}

/**
 * Return the instruction reference manager.
 */
InstructionReferenceManager&
Program::instructionReferenceManager() const {
    return *refManager_;
}

/**
 * Assignment operator.
 *
 * Replaces the contents of this program with the given program.
 *
 * @param newProgram The Program instance to be assigned
 * @return The assigned Program instance.
 */
Program&
Program::operator=(const Program& newProgram) {
    copyFrom(newProgram);
    return *this;
}

/**
 * Creates an exact copy of the program.
 *
 * References to external object models will remain untouched.
 *
 * @return An exact copy of the program.
 */
Program*
Program::copy() const {

    Program* newProgram = new Program(
        startAddress().space(), startAddress(), entryAddress());

    newProgram->copyFrom(*this);
    return newProgram;
}

/**
 * Copies data from another program to itself.
 *
 * References to external object models will remain untouched. The old data
 * is cleaned first. this->copyFrom(another) is equivalent to *this = another.
 *
 * @return source The other program to copy from.
 */
void
Program::copyFrom(const Program& source) {

    // cleanup the old data first
    cleanup();

    for (int k = 0; k < source.procedureCount(); k++) {
        Procedure& proc = source.procedure(k);
        Procedure* newProc =
            new Procedure(
                proc.name(), proc.startAddress().space(),
                proc.startAddress().location());
        addProcedure(newProc);
            
        for (int j = 0; j < proc.instructionCount(); j++) {
            Instruction &ins = proc.instructionAtIndex(j);

            Instruction* newIns = ins.copy();
            newProc->add(newIns);
        }
    }            

    fixInstructionReferences();
    copyDataMemoriesFrom(source);

    // set addresses
    setStartAddress(source.startAddress());
    setEntryAddress(source.entryAddress());

    delete globalScope_;
    globalScope_ = dynamic_cast<GlobalScope*>(
        source.globalScopeConst().copyAndRelocate(*this));
}

/**
 * Fix instruction references to point to the corresponding instructions
 * of the program.
 */
void
Program::fixInstructionReferences() {
    for (int k = 0; k < procedureCount(); k++) {
        Procedure& proc = procedure(k);
        for (int j = 0; j < proc.instructionCount(); j++) {
            Instruction &ins = proc.instructionAtIndex(j);
            for (int i = 0; i < ins.moveCount(); i++) {
                Terminal& source = ins.move(i).source();
                
                if (source.isInstructionAddress()) {
                    Instruction& oldRefIns =
                        source.instructionReference().instruction();
                    
                    Instruction& newRefIns =
                        instructionAt(oldRefIns.address().location());
                    
                    InstructionReference& newRef =
                        instructionReferenceManager().createReference(
                            newRefIns);
                    source.setInstructionReference(newRef);
                }
            }
        }
    }
    
}

/**
 * Copy data memories from source program to destination and fixes 
 * data->code references to use InstructionReferenceManager of the
 * instance.
 *
 * @param srcProg Program from which data is copied.
 */
void
Program::copyDataMemoriesFrom(const Program& srcProg) {

    // copy data memories 
    for (int i = 0; i < srcProg.dataMemoryCount(); i++) {
        DataMemory& currMem = srcProg.dataMemory(i);
        
        DataMemory* newDataMem = new DataMemory(currMem.addressSpace());
        
        for (int j = 0; j < currMem.dataDefinitionCount(); j++) {
            DataDefinition& currDef = currMem.dataDefinition(j);

            DataDefinition* newDef = NULL;
            
            if (currDef.isInstructionAddress()) {
                Instruction& dstInstr = instructionAt(
                    currDef.destinationAddress().location());
                
                InstructionReference& dstRef = 
                    instructionReferenceManager().createReference(
                        dstInstr);
                
                newDef = new DataInstructionAddressDef(
                    currDef.startAddress(), currDef.size(), dstRef);

            } else {
                newDef = currDef.copy();
            }
            
            newDataMem->addDataDefinition(newDef);
        }

        addDataMemory(newDataMem);
    }
}

/**
 * Remove procedure from the program.
 *
 * The procedure becomes independent (it is not deleted). All
 * instructions following the removed procedure are relocated
 * appropriately. All code labels attached to the removed procedure
 * are deleted from the Program.
 *
 * @note Possible references to instructions in this procedure elsewhere
 * in the program are not automatically fixed! It's a responsibility of
 * the caller to fix the Program back to a consistent state.
 *
 * @todo Copy the deleted CodeLabels to the removed procedure's own
 * Scope objects.
 *
 * @param proc Procedure to remove.
 * @exception IllegalRegistration If procedure does not belong to the
 *            program.
 */
void
Program::removeProcedure(Procedure& proc) 
    throw (IllegalRegistration) {

    if (&proc.parent() != this) {
        throw IllegalRegistration(
            __FILE__, __LINE__, __func__, 
            "Procedure doesn't belong to this program.");
    }

    const InstructionAddress firstAddress = proc.startAddress().location();
    const InstructionAddress lastAddress = proc.endAddress().location() - 1;

    for (InstructionAddress addr = firstAddress;
         addr <= lastAddress;
         ++addr) {

        globalScope_->removeCodeLabels(addr);
    }

    if (&proc != &lastProcedure()) {
        moveProcedure(
            nextProcedure(proc),
            proc.startAddress().location() - proc.endAddress().location());
    }

    for (ProcList::iterator iter = procedures_.begin();
         iter != procedures_.end(); iter++) {

        if ((*iter) == &proc) {
            procedures_.erase(iter);
            break;
        }
    }

    proc.setParent(NullProgram::instance());
}

/**
 * Returns the number of data memories accessed by the program.
 *
 * @return The number of data memories accessed by the program.
 */
int
Program::dataMemoryCount() const {
    return dataMems_.size();
}

/**
 * Insert the given data memory to the program.
 *
 * @param dataMem The data memory to be inserted.
 * @exception IllegalRegistration Memory with the same address space is 
 *                                found from the program.
 */
void
Program::addDataMemory(DataMemory* dataMem) 
    throw (IllegalRegistration) {
    // TODO: check that there is no another data mem with same address space...
    dataMems_.push_back(dataMem);
}

/**
 * Returns the data memory at the given index.
 *
 * @param index The position index.
 * @return The data memory at the given index.
 * @exception OutOfRange the index is out of range.
 */
DataMemory&
Program::dataMemory(int index) const 
    throw (OutOfRange) {
    if (index >= 0 &&
        static_cast<unsigned int>(index) < dataMems_.size()) {
        return *dataMems_.at(index);
    } else {
        throw OutOfRange(
            __FILE__, __LINE__, __func__, 
            "There is no data memory with index: " + 
            Conversion::toString(index));
    }
}

/**
 * Returns the data memory with the given address space name.
 *
 * @param aSpaceName The address space name of the memory.
 * @return The data memory with given address space name.
 * @exception KeyNotFound if the data memory cannot be found.
 */
DataMemory&
Program::dataMemory(const std::string& aSpaceName) const
    throw (KeyNotFound) {
    
    for (int i = 0; i < dataMemoryCount(); i++) {
        if (dataMemory(i).addressSpace().name() == aSpaceName) {
            return dataMemory(i);
        }
    }
    
    throw KeyNotFound(
        __FILE__, __LINE__, __func__,
        "No data memory found by address space name: " +
        aSpaceName);
}

/**
 * Replace instruction and data address space of a sequential program
 * with real ones of the target machine.
 *
 * Only the data address space of the target machine that corresponds
 * to the universal data address space is required. Target machine and
 * instruction address space are replaced implicitly.
 *
 * @param space New data address space.
 * @note Use only to convert a sequential program to use a real machine!
 */
void
Program::replaceUniversalAddressSpaces(const TTAMachine::AddressSpace& space) {

    if (dynamic_cast<UniversalMachine*>(start_.space().machine()) == NULL) {
        abortWithError("Address space not connected to a machine.");
    }

    AddressSpace& instructionAS = 
        *space.machine()->controlUnit()->addressSpace();

    start_ = Address(start_.location(), instructionAS);
    entry_ = Address(entry_.location(), instructionAS);


    for (int k = 0; k < procedureCount(); k++) {
        Procedure& proc = procedure(k);
        for (int j = 0; j < proc.instructionCount(); j++) {
            Instruction &ins = proc.instructionAtIndex(j);
            for (int i = 0; i < ins.moveCount(); i++) {
                Move& move = ins.move(i);
                Terminal& source = move.source();
                if (source.isAddress() && !source.isInstructionAddress()) {
                    TerminalAddress* newSource = new TerminalAddress(
                        source.value(), const_cast<AddressSpace&>(space));
                    move.setSource(newSource);
                }
            }
        }
    }
    
    if (dataMemoryCount()  > 1) {
        throw IllegalProgram(
            __FILE__, __LINE__, __func__,
            "There should be less than two data memories. Number of memories: " + 
            Conversion::toString(dataMemoryCount()));
    }

    if (dataMemoryCount() == 0) {
        // TODO: what to do when there is no data used in program...
    } else if (dataMemoryCount() == 1) {
        DataMemory& dataMem = dataMemory(0);
        dataMem.setAddressSpace(const_cast<AddressSpace&>(space));

        // fix dest addresses in data defs to point to the new single data AS
        for (int i = 0; i < dataMem.dataDefinitionCount(); i++) {
            DataDefinition& def = dataMem.dataDefinition(i);
            if (def.isAddress() && !def.isInstructionAddress()) {
                def.setDestinationAddress(
                    Address(def.destinationAddress().location(), space));
            }
        }
        
        assert(dataMemoryCount() == 1);
        globalScope_->setDataLabelAddressSpace(space);

    } else {
        throw IllegalProgram(
            __FILE__, __LINE__, __func__,
            "There should be less than two data memories. Number of memories: " + 
            Conversion::toString(dataMemoryCount()));
    }
}

/**
 * A shortcut for loading a partially scheduled program from a TPEF file.
 *
 * @param tpefFileName The file name of the TPEF.
 * @param theMachine The target machine.
 * @param umach The universal machine for the unscheduled parts.
 * @return Created program.
 * @exception Exception if the TPEF or program in it is somehow broken.
 */
Program*
Program::loadFromTPEF(
    const std::string& tpefFileName,
    const TTAMachine::Machine& theMachine,
    const UniversalMachine& umach) 
    throw (Exception) {

    TPEF::BinaryStream binaryStream(tpefFileName);

    // read to TPEF Handler Module
    TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);

    if (tpef_ == NULL) {
        throw IOException(
            __FILE__, __LINE__, __func__, "Loading TPEF failed.");
    }

    // convert the loaded TPEF to POM
    TTAProgram::TPEFProgramFactory factory(*tpef_, theMachine, umach);
    return factory.build();
}

/**
 * A shortcut for loading a scheduled program from a TPEF file.
 *
 * @param tpefFileName The file name of the TPEF.
 * @param theMachine The target machine.
 * @return Created program.
 * @exception Exception if the TPEF or program in it is somehow broken.
 */
Program*
Program::loadFromTPEF(
    const std::string& tpefFileName,
    const TTAMachine::Machine& theMachine)
    throw (Exception) {

    TPEF::BinaryStream binaryStream(tpefFileName);

    // read to TPEF Handler Module
    TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);

    if (tpef_ == NULL) {
        throw IOException(
            __FILE__, __LINE__, __func__, "Loading TPEF failed.");
    }

    // convert the loaded TPEF to POM
    TTAProgram::TPEFProgramFactory factory(*tpef_, theMachine);
    return factory.build();
}


/**
 * A shortcut for loading a sequential program from a TPEF file.
 *
 * @param tpefFileName The file name of the TPEF.
 * @param umach The universal machine for the unscheduled parts.
 * @return Created program.
 * @exception Exception if the TPEF or program in it is somehow broken.
 */
Program*
Program::loadFromTPEF(
    const std::string& tpefFileName,
    const UniversalMachine& umach) 
    throw (Exception) {

    TPEF::BinaryStream binaryStream(tpefFileName);

    // read to TPEF Handler Module
    TPEF::Binary* tpef_ = TPEF::BinaryReader::readBinary(binaryStream);

    if (tpef_ == NULL) {
        throw IOException(
            __FILE__, __LINE__, __func__, "Loading TPEF failed.");
    }

    // convert the loaded TPEF to POM
    TTAProgram::TPEFProgramFactory factory(*tpef_, umach);
    return factory.build();
}

/**
 * A shortcut for writing a program to a TPEF file.
 *
 * @param program The program to write.
 * @param tpefFileName The file name of the TPEF.
 * @exception Exception if the TPEF or program in it is somehow broken.
 */
void
Program::writeToTPEF(
    const TTAProgram::Program& program,
    const std::string& tpefFileName) 
    throw (Exception) {

    TPEF::BinaryStream binaryStream(tpefFileName);

    TTAProgram::ProgramWriter writer(program);
    TPEF::Binary* tpefBin = writer.createBinary();

    TPEF::TPEFWriter::instance().writeBinary(binaryStream, tpefBin);

    delete tpefBin;
    tpefBin = NULL;
}



} // namespace TTAProgram
