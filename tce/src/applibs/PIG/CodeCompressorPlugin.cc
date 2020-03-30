/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file CodeCompressorPlugin.cc
 *
 * Implementation of CodeCompressorPlugin class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#include <string>
#include <boost/format.hpp>

#include "CodeCompressorPlugin.hh"
#include "InstructionBitVector.hh"
#include "PIGTextGenerator.hh"

#include "BinaryEncoding.hh"
#include "ImmediateControlField.hh"
#include "MoveSlot.hh"
#include "ImmediateSlotField.hh"
#include "LImmDstRegisterField.hh"
#include "SourceField.hh"
#include "DestinationField.hh"
#include "GuardField.hh"
#include "NullUnconditionalGuardEncoding.hh"
#include "NullFUGuardEncoding.hh"
#include "GPRGuardEncoding.hh"
#include "SocketEncoding.hh"
#include "SocketCodeTable.hh"
#include "IUPortCode.hh"
#include "RFPortCode.hh"
#include "FUPortCode.hh"
#include "NullImmediateEncoding.hh"
#include "NullNOPEncoding.hh"
#include "NullGPRGuardEncoding.hh"

#include "Machine.hh"
#include "ControlUnit.hh"
#include "Guard.hh"
#include "FUPort.hh"
#include "HWOperation.hh"

#include "CodeSection.hh"
#include "RelocSection.hh"
#include "Binary.hh"
#include "ImmediateElement.hh"
#include "MoveElement.hh"
#include "RelocElement.hh"
#include "ResourceSection.hh"
#include "TPEFTools.hh"

#include "TPEFProgramFactory.hh"
#include "NullInstruction.hh"
#include "Immediate.hh"
#include "TerminalFUPort.hh"
#include "TerminalImmediate.hh"
#include "Move.hh"
#include "MoveGuard.hh"
#include "TCEString.hh"
#include "Program.hh"
#include "Procedure.hh"
#include "Operation.hh"

#include "tce_config.h"
#include "MathTools.hh"

using std::string;
using std::vector;
using std::pair;
using std::map;
using boost::format;

using namespace TTAMachine;
using namespace TPEF;
using namespace TTAProgram;

/**
 * The constructor.
 */
CodeCompressorPlugin::CodeCompressorPlugin() :
    tpefPrograms_(), currentTPEF_(NULL), currentPOM_(NULL),
    currentProgram_(), bem_(NULL), machine_(NULL),
    parameters_(), programBits_(NULL), allStartsAtBeginningOfMAU_(false),
    mau_(0), addressSpaceOffset_(0), instructionsAtBeginningOfMAU_(),
    immediatesToRelocate_(), terminalsToRelocate_(), relocMap_(),
    indexTable_(), instructionAddresses_() {
}


/**
 * The destructor.
 */
CodeCompressorPlugin::~CodeCompressorPlugin() {
    if (currentPOM_ != NULL) {
        delete currentPOM_;
    }
    if (programBits_ != NULL) {
        delete programBits_;
    }
}


/**
 * Sets the plugin parameters.
 *
 * @param parameters The parameters.
 */
void
CodeCompressorPlugin::setParameters(ParameterTable parameters) {
    parameters_ = parameters;
}


/**
 * Sets the programs to be executed in the machine.
 *
 * @param program Map of programs.
 */
void
CodeCompressorPlugin::setPrograms(
    std::map<std::string, TPEF::Binary*>& programs) {
    tpefPrograms_ = programs;
}


/**
 * Sets the machine.
 *
 * Machine must be set before generating the program image.
 * BEM must be set before calling this
 *
 * @param machine The machine.
 * @exception InvalidData If the machine does not have control unit.
 * @exception InvalidData If bem is not set
 */
void
CodeCompressorPlugin::setMachine(const TTAMachine::Machine& machine) {
    ControlUnit* gcu = machine.controlUnit();
    if (gcu == NULL) {
        string errorMsg = "The machine does not have GCU.";
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }
    machine_ = &machine;
    AddressSpace* iMem = gcu->addressSpace();
    if (bem_ == NULL) {
        string errorMsg = "Bem not set!";
            throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }
    mau_ = bem_->width();
    //fix machines imem width if adf has wrong value
    if (mau_ != iMem->width()) {
       setImemWidth(mau_);
    }
    addressSpaceOffset_ = iMem->start();
}    


/**
 * Sets the binary encoding map.
 *
 * @param bem The BEM.
 */
void
CodeCompressorPlugin::setBEM(const BinaryEncoding& bem) {
    bem_ = &bem;
}


/**
 * Returns the bit image of the program encoded as the BEM determines.
 *
 * @param program The program.
 * @return The program image.
 * @exception InvalidData If the BEM is erroneous.
 */
InstructionBitVector*
CodeCompressorPlugin::bemBits(const TTAProgram::Program& program) {
    relocMap_.clear();
    indexTable_.clear();

    InstructionBitVector* imageBits = new InstructionBitVector();
    for (int i = 0; i < program.procedureCount(); i++) {
	Procedure& p = program.procedure(i);
	for (int j = 0; j < p.instructionCount(); j++) {
	    Instruction& instruction = p.instructionAtIndex(j);
	    InstructionBitVector* bits = bemInstructionBits(instruction);
	    assert(bits->size() == static_cast<size_t>(bem_->width()));
	    imageBits->pushBack(*bits);
	}
    }
    return imageBits;
}

/**
 * Returns the number of move slots
 *
 */
int CodeCompressorPlugin::moveSlotCount() const {
    return bem_->moveSlotCount();
}

/**
 * Returns the width of the move slot in the given index
 *
 * @param index Index of move slot
 */
int CodeCompressorPlugin::moveSlotWidth(int index) const {
    return bem_->moveSlot(index).width();
}

/**
 * Returns the index of first move slot defined in bem
 */
int CodeCompressorPlugin::firstMoveSlotIndex() const {
    int moveSlotIndex = 0;
    if (bem_->hasImmediateControlField()) {
        moveSlotIndex = bem_->immediateControlField().width();
        for (int i = 0; i < bem_->longImmDstRegisterFieldCount(); i++) {
            moveSlotIndex += bem_->longImmDstRegisterField(i).width();
        }
    }
    return moveSlotIndex;
}

/**
 * Tells the memory address of the given instruction.
 *
 * @param instruction The instruction.
 * @return The memory address.
 * @exception InstanceNotFound If the program does not contain the given
 *                             instruction.
 */
unsigned int
CodeCompressorPlugin::memoryAddress(
    const TTAProgram::Instruction& instruction) const {
    if (!MapTools::containsKey(instructionAddresses_, &instruction)) {
        const string procName = "CodeCompressorPlugin::memoryAddress";
        throw InstanceNotFound(__FILE__, __LINE__, procName);
    } else {
        return MapTools::valueForKey<unsigned int>(
            instructionAddresses_, &instruction);
    }
}

/**
 * Returns the binary encoding map.
 *
 * @return The binary encoding map.
 * @exception NotAvailable If the BEM is not set.
 */
const BinaryEncoding&
CodeCompressorPlugin::binaryEncoding() const {
    if (bem_ == NULL) {
        throw NotAvailable(__FILE__, __LINE__, __func__);
    }

    return *bem_;
}

/**
 * Returns the program (POM) being processed currently.
 *
 * @return The program.
 * @exception NotAvailable If there is no program being processed.
 */
TTAProgram::Program&
CodeCompressorPlugin::currentProgram() const {
    if (currentPOM_ == NULL) {
        throw NotAvailable(__FILE__, __LINE__, __func__);
    }

    return *currentPOM_;
}

/**
 * Returns the machine.
 *
 * @return The machine.
 * @exception NotAvailable If the machine is not set.
 */
const Machine&
CodeCompressorPlugin::machine() const {
    if (machine_ == NULL) {
        throw NotAvailable(__FILE__, __LINE__, __func__);
    }

    return *machine_;
}

/**
 * Starts generating new program image.
 *
 * This function must be called always before starting to add instructions
 * to the program image.
 *
 * @param programName The program of which to generate the image.
 * @exception InvalidData If the program object model cannot be created.
 */
void
CodeCompressorPlugin::startNewProgram(const string& programName) {
    if (programBits_ != NULL) {
        delete programBits_;
        programBits_ = NULL;
    }

    if (tpefPrograms_.find(programName) == tpefPrograms_.end()) {
        string errorMsg = "Program " + programName + " not found from "
            + "compressor plugin";
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }

    TPEF::Binary* program = tpefPrograms_.find(programName)->second;

    if (program == NULL) {
        string errorMsg = "Program bits for program " + programName
            + " not found";
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }

    programBits_ = new InstructionBitVector();
    currentTPEF_ = program;

    if (currentPOM_ != NULL) {
        delete currentPOM_;
        currentPOM_ = NULL;
    }

    TPEFProgramFactory factory(*program, *machine_);
    try {
        currentPOM_ = factory.build();
    } catch (const Exception& e) {
        throw InvalidData(__FILE__, __LINE__, __func__, e.errorMessage());
    }
    
    // clear previous relocations
    if (immediatesToRelocate_.size() != 0) {
        immediatesToRelocate_.clear();
    }
    if (terminalsToRelocate_.size() != 0) {
        terminalsToRelocate_.clear();
    }
    initializeRelocations();
}

/**
 * Adds the given instruction to the program.
 *
 * @param instruction The instruction.
 * @param bits The bits that models the instruction.
 * @exception InvalidData If new program is not started by startNewProgram.
 * @exception OutOfRange If some immediate would become too large to fit
 *                       in its space due to the address of the instruction
 *                       being added.
 */
void
CodeCompressorPlugin::addInstruction(
    const Instruction& instruction, InstructionBitVector* bits) {
    if (programBits_ == NULL) {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }

    unsigned int firstFree = programBits_->size();
    unsigned int instructionPosition = firstFree;

    if (startsAtBeginningOfMAU(instruction)) {
        instructionPosition = nextAddressablePositionFrom(firstFree);
    }

    // fill the memory from last instruction to this instruction with 0's
    for (unsigned int i = firstFree; i < instructionPosition; i++) {
        programBits_->push_back(false);
    }

    // add the instruction bits
    programBits_->pushBack(*bits);
    delete bits;

    // mark the instruction starting point
    programBits_->markInstructionStartingPoint(instructionPosition);

    unsigned int memoryAddress = addressSpaceOffset_ + 
        (instructionPosition / mau_);
    programBits_->fixInstructionAddress(instruction, memoryAddress);
    instructionAddresses_.insert(
        std::pair<const Instruction*, unsigned int>(
            &instruction, memoryAddress));
}

/**
 * Sets the given instruction to start at the beginning of MAU.
 *
 * This method must be called before the instruction is added to the bit
 * vector.
 *
 * @param instruction The instruction.
 */
void
CodeCompressorPlugin::setInstructionToStartAtBeginningOfMAU(
    const TTAProgram::Instruction& instruction) {

    instructionsAtBeginningOfMAU_.insert(&instruction);
}


/**
 * Sets all instructions to start at the beginning of MAU.
 *
 * This method must be called before any instructions are added to the bit
 * vector.
 */
void
CodeCompressorPlugin::setAllInstructionsToStartAtBeginningOfMAU() {
    allStartsAtBeginningOfMAU_ = true;
}


/**
 * Returns bit vector that represents the given instruction as the BEM
 * determines.
 *
 * @param instruction The instruction.
 * @return The newly created bit vector containing the instruction bits.
 * @exception InvalidData If the BEM is erroneous.
 */
InstructionBitVector*
CodeCompressorPlugin::bemInstructionBits(
    const TTAProgram::Instruction& instruction) {
    if (bem_ == NULL) {
        throw InvalidData(__FILE__, __LINE__, __func__);
    }

    InstructionBitVector* instructionBits = new InstructionBitVector();

    // insert the bits to the vector
    for (int i = binaryEncoding().childFieldCount() - 1; i >= 0; i--) {
        InstructionField& field = binaryEncoding().childField(i);
        ImmediateControlField* icField = 
            dynamic_cast<ImmediateControlField*>(&field);
        MoveSlot* moveSlot = dynamic_cast<MoveSlot*>(&field);
        ImmediateSlotField* immField = 
            dynamic_cast<ImmediateSlotField*>(&field);
        LImmDstRegisterField* dstRegField = 
            dynamic_cast<LImmDstRegisterField*>(&field);
        unsigned int oldSize = instructionBits->size();
        if (icField != NULL) {
            addBitsForICField(
                *icField, instruction, *instructionBits);
            assert(
                instructionBits->size() - oldSize == 
                static_cast<size_t>(icField->width()));
        } else if (moveSlot != NULL) {
            addBitsForMoveSlot(
                *moveSlot, instruction, *instructionBits);
            assert(
                instructionBits->size() - oldSize == 
                static_cast<size_t>(moveSlot->width()));
        } else if (immField != NULL) {
            addBitsForImmediateSlot(
                *immField, instruction, *instructionBits);
            assert(instructionBits->size() - oldSize == 
                   static_cast<size_t>(immField->width()));
        } else if (dstRegField != NULL) {
            addBitsForDstRegisterField(
                *dstRegField, instruction, *instructionBits);
            assert(
                instructionBits->size() - oldSize ==
                static_cast<size_t>(dstRegField->width()));
        } else {
            assert(false);
        }
    }

    // mark the instruction references to the bit vector
    for (RelocMap::const_iterator iter = relocMap_.begin(); 
         iter != relocMap_.end(); iter++) {
        const Immediate* immediate = (*iter).first;
        const Instruction* instruction = (*iter).second;
        assert(MapTools::containsKey(indexTable_, immediate));
        vector<IndexBound> indexes = MapTools::valueForKey<
            vector<IndexBound> >(indexTable_, immediate);
        instructionBits->startSettingInstructionReference(*instruction);
        for (vector<IndexBound>::const_iterator iter = indexes.begin();
             iter != indexes.end(); iter++) {
            instructionBits->addIndexBoundsForReference(*iter);
        }
    }

    relocMap_.clear();
    indexTable_.clear();

    return instructionBits;
}

/**
 * Returns the bits of the program constructed by calling addInstruction 
 * method several times.
 *
 * This method creates a copy of the constructed bit vector and add
 * extra bits to the end to make sure the length of the vector is
 * divisible by MAU.
 *
 * @return The program bits.
 */
InstructionBitVector*
CodeCompressorPlugin::programBits() const {

    InstructionBitVector* bits = new InstructionBitVector(*programBits_);

    // add extra bits to the end if the length of program image is not
    // divisible by MAU
    int bitCount = mau_ - (bits->size() % mau_);
    if (bitCount == mau_) {
        bitCount = 0;
    }
    for (int i = 0; i < bitCount; i++) {
        bits->push_back(false);
    }

    return bits;
}


/**
 * Tells whether the plugin has the given parameter defined.
 *
 * @param paramName Name of the parameter.
 * @return True if the given parameter is defined, otherwise false.
 */
bool
CodeCompressorPlugin::hasParameter(const std::string& paramName) const {
    for (ParameterTable::const_iterator iter = parameters_.begin();
         iter != parameters_.end(); iter++) {
        Parameter param = *iter;
        if (param.name == paramName) {
            return true;
        }
    }
    return false;
}


/**
 * Returns the value of the given parameter.
 *
 * @param paramName Name of the parameter.
 * @return The value.
 * @exception NotAvailable If the given parameter is not defined.
 */
std::string
CodeCompressorPlugin::parameterValue(const std::string& paramName) const {
    for (ParameterTable::const_iterator iter = parameters_.begin();
         iter != parameters_.end(); iter++) {
        Parameter param = *iter;
        if (param.name == paramName) {
            return param.value;
        }
    }

    throw NotAvailable(__FILE__, __LINE__, __func__);
}

/**
 * Returns the mau of instruction memory. 
 *
 * Before compression this is the
 * original width and after compression it returns the compressed instruction
 * width
 *
 * @return mau width
 */
int
CodeCompressorPlugin::imemMauWidth() const {
    return mau_;
}


/**
 * Sets the imem width to the given mau * widthInMaus and mau to given mau
 *
 * @param mau Mau of instruction memory
 * @param widthInMaus Memory width in maus (default = 1)
 */
void
CodeCompressorPlugin::setImemWidth(int mau, int widthInMaus) {
    machine_->controlUnit()->addressSpace()->setWidth(mau*widthInMaus);
    mau_ = mau;
}

/**
 * Returns the number of programs to be compressed
 *
 * @return number of programs
 */
int
CodeCompressorPlugin::numberOfPrograms() const {
    return tpefPrograms_.size();
}

/**
 * Returns a const iterator to program map element at the given index
 *
 * @return const iterator to program map element
 * @exception OutOfRange If index is out of range
 */
CodeCompressorPlugin::TPEFMap::const_iterator
CodeCompressorPlugin::programElement(int index) const {
    if (index >= static_cast<int>(tpefPrograms_.size())) {
        string errorMsg("Tried to overindex program map");
        throw OutOfRange(__FILE__, __LINE__, __func__, errorMsg);
    }
    TPEFMap::const_iterator iter = tpefPrograms_.begin();
    for (int i = 0; i < index; i++) {
        iter++;
    }
    return iter;
}

/**
 * Returns the indentation string of the given level.
 *
 * @param level The indentation level.
 */
std::string 
CodeCompressorPlugin::indentation(int level) {
    string ind;
    for (int i = 0; i < level; i++) {
        ind += "  ";
    }
    return ind;
}

/**
 * Adds bits for immediate control field to the given bit vector.
 *
 * @param icField The immediate control field that determines the encoding.
 * @param instruction The instruction that is being encoded.
 * @param bitVector The bit vector to which the bits are added.
 * @exception InvalidData If the BEM or machine is erroneous.
 */
void
CodeCompressorPlugin::addBitsForICField(
    const ImmediateControlField& icField,
    const TTAProgram::Instruction& instruction, BitVector& bitVector) const {
    string instructionTemplate("");

    // find the instruction template of the instruction
    try {
        instructionTemplate = 
            this->instructionTemplate(instruction);
    } catch (const InstanceNotFound& exception) {
        throw InvalidData(
            __FILE__, __LINE__, __func__, exception.errorMessage());
    }

    // get the encoding
    int encoding(0);
    try {
        encoding = icField.templateEncoding(instructionTemplate);
    } catch (const InstanceNotFound& exception) {
        PIGTextGenerator textGenerator;
        format text = textGenerator.text(
            PIGTextGenerator::TXT_TEMPLATE_ENCODING_MISSING);
        text % instructionTemplate;
        const string procName = "CodeCompressorPlugin::addBitsForICField";
        throw InvalidData(__FILE__, __LINE__, procName, text.str());
    }

    // add the encoding
    bitVector.pushBack(encoding, icField.width());
}

/**
 * Adds bits for a move slot to the given bit vector.
 *
 * @param slot The move slot that determines the encoding.
 * @param instruction The instruction that is being encoded.
 * @param bitVector The bit vector to which the bits are added.
 * @exception InvalidData If the BEM is erroneous.
 */
void
CodeCompressorPlugin::addBitsForMoveSlot(
    const MoveSlot& slot, const TTAProgram::Instruction& instruction,
    InstructionBitVector& bitVector) {
    // find the correct bus from the machine
    string busName = slot.name();
    Machine::BusNavigator busNav = machine().busNavigator();
    if (!busNav.hasItem(busName)) {
        PIGTextGenerator textGenerator;
        format text = textGenerator.text(
            PIGTextGenerator::TXT_BEM_DEFINES_SLOT_FOR_NONEXISTING_BUS);
        text % busName;
        const string procName = "CodeCompressorPlugin::addBitsForMoveSlot";
        throw InvalidData(__FILE__, __LINE__, procName, text.str());
    }

    Bus* bus = busNav.item(busName);

    // get the correct instruction template
    string instructionTemplate = "";
    try {
        instructionTemplate = this->instructionTemplate(
            instruction);
    } catch (const InstanceNotFound& exception) {
        if (machine().immediateUnitNavigator().count() == 0) {
            // if no immediate units, no instruction templates either
            unsigned int oldSize = bitVector.size();
            encodeMove(slot, instruction, bitVector);
            assert(
                bitVector.size() - oldSize == 
                static_cast<size_t>(slot.width()));
            return;
        } else {
            assert(false);
        }
    }

    Machine::InstructionTemplateNavigator itNav = 
        machine().instructionTemplateNavigator();
    if (!itNav.hasItem(instructionTemplate)) {
        assert(false);
    }

    // check whether there should be a long immediate on the move slot
    InstructionTemplate* iTemp = itNav.item(instructionTemplate);
    if (iTemp->usesSlot(bus->name())) {
        encodeLongImmediate(
            bus->name(), slot.width(), instruction, bitVector);        
    } else {
        encodeMove(slot, instruction, bitVector);
    }
}

/**
 * Adds bits for a source field to the given bit vector.
 *
 * @param srcField The source field that determines the encoding rules.
 * @param move The move that is being encoded.
 * @param bitVector The bit vector to which the bits are added.
 * @exception InvalidData If the BEM is erroneous.
 */
void
CodeCompressorPlugin::addBitsForSourceField(
    const SourceField& srcField, const TTAProgram::Move& move,
    InstructionBitVector& bitVector) const {
    unsigned int oldSize = bitVector.size();
    assert(move.bus().name() == srcField.parent()->name());
    Terminal& source = move.source();
    
    if (source.isImmediateRegister()) {
        encodeIUTerminal(srcField, source, bitVector);
    } else if (source.isGPR()) {
        encodeRFTerminal(srcField, source, bitVector);
    } else if (source.isFUPort()) {
        encodeFUTerminal(srcField, source, bitVector);
    } else if (source.isImmediate()) {
        encodeImmediateTerminal(srcField, source, bitVector);
    }

    assert(
        bitVector.size() - oldSize == static_cast<size_t>(srcField.width()));
}

/**
 * Adds bits for a destination field to the given bit vector.
 *
 * @param dstField The destination field that determines the encoding rules.
 * @param move The move that is being encoded.
 * @param bitVector The bit vector to which the bits are added.
 * @exception InvalidData If the BEM is erroneous.
 */
void
CodeCompressorPlugin::addBitsForDestinationField(
    const DestinationField& dstField, const TTAProgram::Move& move,
    BitVector& bitVector) {
    assert(move.bus().name() == dstField.parent()->name());
    Terminal& destination = move.destination();

    if (destination.isGPR()) {
        encodeRFTerminal(dstField, destination, bitVector);
    } else if (destination.isFUPort()) {
        encodeFUTerminal(dstField, destination, bitVector);
    } else {
        assert(false);
    }
}

/**
 * Adds bits for a guard field to the given bit vector.
 *
 * @param guardField The guard field that determines the encoding rules.
 * @param move The move that is being encoded.
 * @param bitVector The bit vector to which the bits are added.
 * @exception InvalidData If the BEM is erroneous.
 */
void
CodeCompressorPlugin::addBitsForGuardField(
    const GuardField& guardField, const TTAProgram::Move& move,
    BitVector& bitVector) {
    unsigned int encodingValue(0);
    const string procName = "CodeCompressorPlugin::addBitsForGuardField";
    string busName = guardField.parent()->name();

    // find the correct encoding value
    if (move.isUnconditional()) {
        UnconditionalGuardEncoding& encoding = guardField.
            unconditionalGuardEncoding(false);
        if (&encoding == &NullUnconditionalGuardEncoding::instance()) {
            PIGTextGenerator textGenerator;
            format text = textGenerator.text(
                PIGTextGenerator::TXT_ALWAYS_TRUE_GUARD_ENCODING_MISSING);
            text % busName;
            throw InvalidData(__FILE__, __LINE__, procName, text.str());
        }
        encodingValue = encoding.encoding();

    } else {
        const Guard& guard = move.guard().guard();
        const PortGuard* portGuard = dynamic_cast<const PortGuard*>(&guard);
        const RegisterGuard* registerGuard =
            dynamic_cast<const RegisterGuard*>(&guard);
	
        if (portGuard != NULL) {
            const Port& port = *portGuard->port();
            string portName = port.name();
            string fuName = port.parentUnit()->name();
            FUGuardEncoding& encoding = guardField.fuGuardEncoding(
                fuName, portName, guard.isInverted());
            if (&encoding == &NullFUGuardEncoding::instance()) {
                PIGTextGenerator textGenerator;
                format text = textGenerator.text(
                    PIGTextGenerator::TXT_FU_GUARD_ENCODING_MISSING);
                if (guard.isInverted()) {
                    text % "inverted" % portName % fuName;
                } else {
                    text % "non-inverted" % portName % fuName;
                }
                throw InvalidData(
                    __FILE__, __LINE__, procName, text.str());
            }
            encodingValue = encoding.encoding();

        } else if (registerGuard != NULL) {
            string rfName = registerGuard->registerFile()->name();
            int regIndex = registerGuard->registerIndex();
            GPRGuardEncoding& encoding = guardField.gprGuardEncoding(
                rfName, regIndex, guard.isInverted());
            if (&encoding == &NullGPRGuardEncoding::instance()) {
                PIGTextGenerator textGenerator;
                format text = textGenerator.text(
                    PIGTextGenerator::TXT_GPR_GUARD_ENCODING_MISSING);
                if (guard.isInverted()) {
                    text % "inverted" % regIndex % rfName;
                } else {
                    text % "non-inverted" % regIndex % rfName;
                }
                throw InvalidData(
                    __FILE__, __LINE__, procName, text.str());
            }
            encodingValue = encoding.encoding();

        } else {
            assert(false);
        }
    }
    
    // push back the encoding value to the bit vector
    unsigned int oldSize = bitVector.size();
    bitVector.pushBack(encodingValue, guardField.width());
    assert(
        bitVector.size() - oldSize == 
        static_cast<size_t>(guardField.width()));
}

/**
 * Adds the immediate bits of the given immediate slot to the bit vector.
 *
 * @param immSlot The immediate slot.
 * @param instruction The instruction that contains the immediate.
 * @param bitVector The bit vector to which the bits are added.
 * @exception InvalidData If the BEM is erroneous.
 */
void
CodeCompressorPlugin::addBitsForImmediateSlot(
    const ImmediateSlotField& immSlot,
    const TTAProgram::Instruction& instruction, BitVector& bitVector) {
    // get the correct instruction template
    string instructionTemplate = "";
    try {
        instructionTemplate = this->instructionTemplate(
            instruction);
    } catch (const InstanceNotFound& exception) {
        if (machine().immediateUnitNavigator().count() == 0) {
            // if no immediate units, no instruction templates either
            PIGTextGenerator textGenerator;
            format text = textGenerator.text(
                PIGTextGenerator::
                TXT_BEM_DEFINES_IMM_SLOT_THAT_IS_NEVER_USED);
            const string procName = 
                "CodeCompressorPlugin::addBitsForImmediateSlot";
            throw InvalidData(__FILE__, __LINE__, procName, text.str());
        } else {
            assert(false);
        }
    }

    Machine::InstructionTemplateNavigator itNav = 
        machine().instructionTemplateNavigator();
    if (!itNav.hasItem(instructionTemplate)) {
        assert(false);
    }

    // check whether there should be a long immediate on the immediate slot
    InstructionTemplate* iTemp = itNav.item(instructionTemplate);
    if (iTemp->usesSlot(immSlot.name())) {
        encodeLongImmediate(
            immSlot.name(), immSlot.width(), instruction, bitVector);        
    } else {
        // fill the slot with zeros
        bitVector.pushBack(0, immSlot.width());
    }
}

/**
 * Adds the bits of the given destination register field to the bit vector.
 *
 * @param field The long immediate destination register field.
 * @param instruction The instruction.
 * @param bitVector The bit vector to which the bits are added.
 * @exception InvalidData If the instruction or BEM is erroneous.
 */
void
CodeCompressorPlugin::addBitsForDstRegisterField(
    const LImmDstRegisterField& field,
    const TTAProgram::Instruction& instruction, BitVector& bitVector) {
    for (int i = 0; i < instruction.immediateCount(); i++) {
        TTAProgram::Immediate& immediate = instruction.immediate(i);
        const TTAProgram::Terminal& dstTerminal = immediate.destination();
        const ImmediateUnit& dstIU = dstTerminal.immediateUnit();
        if (dstIU.numberOfRegisters() == 1) {
            continue;
        }
        string instructionTemplate = "";
        try {
            instructionTemplate = this->instructionTemplate(instruction);
        } catch (const Exception& e) {
            throw InvalidData(
                __FILE__, __LINE__, __func__, e.errorMessage());
        }

        string iuName = field.immediateUnit(instructionTemplate);
        if (dstIU.name() == iuName) {
            bitVector.pushBack(dstTerminal.index(), field.width());
            return;
        }
    }

    // the field is not used by this instruction template
    bitVector.pushBack(0, field.width());
}

/**
 * Returns the name of the instruction template of the given instruction.
 *
 * @param instruction The instruction.
 * @exception InstanceNotFound If the machine does not have such an 
 *                             instruction template.
 */
std::string
CodeCompressorPlugin::instructionTemplate(
    const TTAProgram::Instruction& instruction) const {
    if (instruction.immediateCount() == 0) {
        Machine::InstructionTemplateNavigator itNav = 
            machine().instructionTemplateNavigator();
        for (int i = 0; i < itNav.count(); i++) {
            InstructionTemplate* iTemp = itNav.item(i);
            if (iTemp->isEmpty()) {
                return iTemp->name();
            }
        }
        const string procName = "CodeCompressorPlugin::instructionTemplate";
        PIGTextGenerator textGen;
        format text = textGen.text(PIGTextGenerator::TXT_EMPTY_ITEMP_MISSING);
        throw InstanceNotFound(__FILE__, __LINE__, procName, text.str());
    } else {
        return instruction.instructionTemplate().name();
    }
}

/**
 * Tells whether the given instruction starts at the beginning of MAU.
 *
 * @param instruction The instruction.
 * @return True if the instruction start at the beginning of MAU, otherwise
 *         false.
 */
bool
CodeCompressorPlugin::startsAtBeginningOfMAU(
    const TTAProgram::Instruction& instruction) const {

    if (allStartsAtBeginningOfMAU_) {
        return true;
    } else {
        return AssocTools::containsKey(
            instructionsAtBeginningOfMAU_, &instruction);
    }
}


/**
 * Returns the next addressable position in the bit vector calculated from 
 * the given position.
 *
 * @param position The position.
 * @return The next addressable position.
 */
unsigned int
CodeCompressorPlugin::nextAddressablePositionFrom(
    unsigned int position) const {

    if (position % mau_ == 0) {
        return position;
    } else {
        return position + mau_ - (position % mau_);
    }
}


/**
 * Encodes the instruction's long immediate bits that belong to the
 * given (move or immediate) slot to the bit vector.
 *
 * @param slotName Name of the slot.
 * @param width Bit width of the slot.
 * @param instruction The instruction that is being encoded.
 * @param bitVector The bit vector to which the bits are added.
 */
void
CodeCompressorPlugin::encodeLongImmediate(
    const std::string& slotName,
    int slotWidth,
    const TTAProgram::Instruction& instruction,
    BitVector& bitVector) {
        
    // find the correct immediate from the instruction
    for (int i = 0; i < instruction.immediateCount(); i++) {
        Immediate& imm = instruction.immediate(i);
        const InstructionTemplate& iTemp = instruction.instructionTemplate();

        if (iTemp.usesSlot(slotName)) {
            // correct immediate
            ImmediateUnit* dstUnit = iTemp.destinationOfSlot(slotName);

            // find the correct boundary of the long immediate which is to
            // be encoded in the given (immediate or move) slot
            int rightmostBitToEncode = 0;
            for (int i = iTemp.numberOfSlots(*dstUnit) - 1; i >= 0; i--) {
                string slotOfImm = iTemp.slotOfDestination(*dstUnit, i);
                if (slotOfImm != slotName) {
                    rightmostBitToEncode += iTemp.supportedWidth(slotOfImm);
                } else {
                    break;
                }
            }
            unsigned int limmSlotWidth = iTemp.supportedWidth(slotName);
            int leftmostBitToEncode = 
                rightmostBitToEncode + limmSlotWidth - 1;
            assert(leftmostBitToEncode >= rightmostBitToEncode);
            assert(
                leftmostBitToEncode - rightmostBitToEncode < slotWidth);
            
            // need to add zero bits to the move slot if the immediate
            // width is smaller than the width of the move slot
            int zerosToAdd = slotWidth - (
                leftmostBitToEncode - rightmostBitToEncode) - 1;
            assert(zerosToAdd >= 0);
            for (int i = 0; i < zerosToAdd; i++) {
                bitVector.push_back(false);
            }

            // push back the immediate value
            UIntWord immediateValue = imm.value().value().uIntWordValue();
            addBits(
                immediateValue, leftmostBitToEncode, rightmostBitToEncode, 
                bitVector);

            // if the long immediate is address of an instruction, mark it
            // to relocMap_ and indexTable_
            if (AssocTools::containsKey(immediatesToRelocate_, &imm)) {
                Instruction& referenced = currentPOM_->instructionAt(
                    immediateValue);
                unsigned int startIndex = bitVector.size() - slotWidth;
                unsigned int endIndex = bitVector.size() - 1;
                relocMap_.insert(
                    pair<const Immediate*, const Instruction*>(
                        &imm, &referenced));
                vector<IndexBound> indices;
                if (MapTools::containsKey(indexTable_, &imm)) {
                    indices = MapTools::valueForKey<vector<IndexBound> >(
                        indexTable_, &imm);
                }
                IndexBound bounds(startIndex, endIndex, limmSlotWidth,
                                  leftmostBitToEncode, rightmostBitToEncode);
                indices.push_back(bounds);
                indexTable_[&imm] = indices;
            }

            return;
        }
    }
    assert(false);
}


/**
 * Encodes a move to the given bit vector.
 *
 * @param slot The move slot that determines the encoding rules.
 * @param instruction The instruction that is being encoded.
 * @param bitVector The bit vector to which the bits are added.
 * @exception InvalidData If the BEM is erroneous.
 */
void
CodeCompressorPlugin::encodeMove(
    const MoveSlot& slot, const TTAProgram::Instruction& instruction,
    InstructionBitVector& bitVector) const {
    string busName = slot.name();
    Machine::BusNavigator busNav = machine().busNavigator();
    Bus* bus = busNav.item(busName);

    for (int i = 0; i < instruction.moveCount(); i++) {
        Move& move = instruction.move(i);
        if (&move.bus() == bus) {
            // correct move
            SourceField& srcField = slot.sourceField();
            DestinationField& dstField = slot.destinationField();
            GuardField& guardField = slot.guardField();
            for (int i = slot.childFieldCount() - 1; i >= 0; i--) {
                InstructionField& childField = slot.childField(i);
                unsigned int oldSize = bitVector.size();
                if (dynamic_cast<GuardField*>(&childField) != NULL) {
                    addBitsForGuardField(guardField, move, bitVector);
                    assert(bitVector.size() - oldSize == 
                           static_cast<size_t>(guardField.width()));
                } else if (dynamic_cast<SourceField*>(&childField) != NULL) {
                    addBitsForSourceField(srcField, move, bitVector);
                    assert(bitVector.size() - oldSize == 
                           static_cast<size_t>(srcField.width()));

                } else if (dynamic_cast<DestinationField*>(&childField) !=
                           NULL) {
                    addBitsForDestinationField(dstField, move, bitVector);
                    assert(bitVector.size() - oldSize == 
                           static_cast<size_t>(dstField.width()));
                } else {
                    assert(false);
                }
            }
            return;
        }
    }

    // no move on the bus
    unsigned int oldSize = bitVector.size();
    encodeNOP(slot, bitVector);
    assert(bitVector.size() - oldSize == static_cast<size_t>(slot.width()));
}

/**
 * Encodes a NOP to the given bitVector.
 *
 * @param slot The move slot that determines the encoding rules.
 * @param bitVector The bit vector to which the bits are added.
 * @exception InvalidData If the BEM does not define encoding for NOP.
 */
void
CodeCompressorPlugin::encodeNOP(const MoveSlot& slot, BitVector& bitVector) {
    for (int i = slot.childFieldCount() - 1; i >= 0; i--) {
        InstructionField& childField = slot.childField(i);
        GuardField& guardField = slot.guardField();
        SourceField& srcField = slot.sourceField();
        DestinationField& dstField = slot.destinationField();

        if (&guardField == &childField) {
            if ((!srcField.hasNoOperationEncoding() ||
                 !dstField.hasNoOperationEncoding()) && 
                guardField.hasUnconditionalGuardEncoding(true)) {
                bitVector.pushBack(
                    guardField.unconditionalGuardEncoding(true).encoding(),
                    guardField.width());
            } else {
                bitVector.pushBack(0, guardField.width());
            }

        } else if (&srcField == &childField) {
            if (srcField.width() == 0) {
                continue;
            }

            NOPEncoding& encoding = srcField.noOperationEncoding();
            if (&encoding == &NullNOPEncoding::instance() &&
                !guardField.hasUnconditionalGuardEncoding(true)) {
                PIGTextGenerator textGenerator;
                format text = textGenerator.text(
                    PIGTextGenerator::TXT_NOP_ENCODING_MISSING);
                text % slot.name();
                const string procName = "CodeCompressorPlugin::encodeNOP";
                throw InvalidData(
                    __FILE__, __LINE__, procName, text.str());
            }
            if (&encoding == &NullNOPEncoding::instance()) {
                bitVector.pushBack(0, srcField.width());
            } else {
                if (srcField.componentIDPosition() == BinaryEncoding::LEFT) {
                    bitVector.pushBack(
                        encoding.encoding(), encoding.width());
                    bitVector.pushBack(
                        0, srcField.width() - encoding.width());
                } else {
                    bitVector.pushBack(
                        encoding.encoding(), srcField.width());
                }
            }

        } else if (&dstField == &childField) {
            if (dstField.width() == 0) {
                continue;
            }
            NOPEncoding& encoding = dstField.noOperationEncoding();
            if (&encoding == &NullNOPEncoding::instance() &&
                !guardField.hasUnconditionalGuardEncoding(true)) {
                PIGTextGenerator textGenerator;
                format text = textGenerator.text(
                    PIGTextGenerator::TXT_NOP_ENCODING_MISSING);
                text % slot.name();
                throw InvalidData(__FILE__, __LINE__, __func__, text.str());
            }
            if (&encoding == &NullNOPEncoding::instance()) {
                bitVector.pushBack(0, dstField.width());
            } else {
                if (dstField.componentIDPosition() == BinaryEncoding::LEFT) {
                    bitVector.pushBack(encoding.encoding(), encoding.width());
                    bitVector.pushBack(0, dstField.width() - encoding.width());
                } else {
                    bitVector.pushBack(encoding.encoding(), dstField.width());
                }
            }
        }
    }
}

/**
 * Encodes the immediate unit register terminal to the given bit vector.
 *
 * @param field The slot field that determines the encoding rules.
 * @param terminal The terminal to be encoded.
 * @param bitVector The bit vector to which the bits are added.
 * @exception InvalidData If the BEM is erroneous.
 */
void
CodeCompressorPlugin::encodeIUTerminal(
    const SlotField& field, const TTAProgram::Terminal& terminal,
    BitVector& bitVector) {
    const string procName = "CodeCompressorPlugin::encodeIUTerminal";
    assert(terminal.isImmediateRegister());
    string iuName = terminal.immediateUnit().name();
    const Port& port = terminal.port();
    string socketName("");

    // find the correct socket
    if (dynamic_cast<const SourceField*>(&field) != NULL) {
        socketName = port.outputSocket()->name();
    } else if (dynamic_cast<const DestinationField*>(&field) != NULL) {
        socketName = port.inputSocket()->name();
    } else {
        assert(false);
    }

    if (!field.hasSocketEncoding(socketName)) {
        PIGTextGenerator textGenerator;
        format text = textGenerator.text(
            PIGTextGenerator::TXT_SOCKET_ENCODING_MISSING);
        if (dynamic_cast<const SourceField*>(&field) != NULL) {
            text % socketName % "source" % field.parent()->name();
        } else {
            text % socketName % "destination" % field.parent()->name();
        }
        throw InvalidData(__FILE__, __LINE__, procName, text.str());
    }

    // create socket ID encoding
    SocketEncoding& socketID = field.socketEncoding(socketName);
    BitVector socketIDBits;
    socketIDBits.pushBack(socketID.encoding(), socketID.socketIDWidth());

    // create socket code encoding
    BitVector socketCodeBits;
    if (socketID.hasSocketCodes()) {
        SocketCodeTable& socketCodes = socketID.socketCodes();
        if (!socketCodes.hasIUPortCode(iuName)) {
            PIGTextGenerator textGenerator;
            format text = textGenerator.text(
                PIGTextGenerator::TXT_IU_PORT_CODE_MISSING);
            text % iuName % socketName % field.parent()->name();
            throw InvalidData(__FILE__, __LINE__, procName, text.str());
        }

        IUPortCode& portCode = socketCodes.iuPortCode(iuName);
        unsigned int registerIndex = terminal.index();
        socketCodeBits.pushBack(
            CodeCompressorPlugin::socketCodeBits(
                socketCodes, portCode, registerIndex));
    }
    
    encodeSlotField(field, socketIDBits, socketCodeBits, bitVector);
}

/**
 * Encodes the given register file terminal to the given bit vector.
 *
 * @param field The slot field that determines the encoding rules.
 * @param terminal The terminal to be encoded.
 * @param bitVector The bit vector to which the bits are added.
 * @exception InvalidData If the BEM is erroneous.
 */
void
CodeCompressorPlugin::encodeRFTerminal(
    const SlotField& field, const TTAProgram::Terminal& terminal,
    BitVector& bitVector) {
    const string procName = "CodeCompressorPlugin::encodeRFTerminal";
    assert(terminal.isGPR());
    string rfName = terminal.registerFile().name();
    const Port& port = terminal.port();
    string socketName("");
    
    // find the correct socket
    if (dynamic_cast<const SourceField*>(&field) != NULL) {
        socketName = port.outputSocket()->name();
    } else if (dynamic_cast<const DestinationField*>(&field) != NULL) {
        socketName = port.inputSocket()->name();
    } else {
        assert(false);
    }
    
    if (!field.hasSocketEncoding(socketName)) {
        PIGTextGenerator textGenerator;
        format text = textGenerator.text(
            PIGTextGenerator::TXT_SOCKET_ENCODING_MISSING);
        if (dynamic_cast<const SourceField*>(&field) != NULL) {
            text % socketName % "source" % field.parent()->name();
        } else {
            text % socketName % "destination" % field.parent()->name();
        }
        throw InvalidData(__FILE__, __LINE__, procName, text.str());
    }
 
    // create encoding for socket ID
    SocketEncoding& socketID = field.socketEncoding(socketName);
    BitVector socketIDBits;
    socketIDBits.pushBack(socketID.encoding(), socketID.socketIDWidth());
    
    // create encoding for socket code
    BitVector socketCodeBits;
    if (socketID.hasSocketCodes()) {
        SocketCodeTable& socketCodes = socketID.socketCodes();
        if (!socketCodes.hasRFPortCode(rfName)) {
            PIGTextGenerator textGenerator;
            format text = textGenerator.text(
                PIGTextGenerator::TXT_RF_PORT_CODE_MISSING);
            text % rfName % socketName % field.parent()->name();
            throw InvalidData(__FILE__, __LINE__, procName, text.str());
        }
        RFPortCode& portCode = socketCodes.rfPortCode(rfName);
        unsigned int registerIndex = terminal.index();
        socketCodeBits = CodeCompressorPlugin::socketCodeBits(
            socketCodes, portCode, registerIndex);
    }
    
    encodeSlotField(field, socketIDBits, socketCodeBits, bitVector);
}

/**
 * Encodes the given function unit terminal to the given bit vector.
 *
 * @param field The slot field that determines the encoding rules.
 * @param terminal The terminal to be encoded.
 * @param bitVector The bit vector to which the bits are added.
 * @exception InvalidData If the BEM is erroneous.
 */
void
CodeCompressorPlugin::encodeFUTerminal(
    const SlotField& field, const TTAProgram::Terminal& terminal,
    BitVector& bitVector) {
    const string procName = "CodeCompressorPlugin::encodeFUTerminal";
    assert(terminal.isFUPort());
    string fuName = terminal.functionUnit().name();
    const Port& port = terminal.port();
    string socketName("");

    // find the correct socket
    if (dynamic_cast<const SourceField*>(&field) != NULL) {
        socketName = port.outputSocket()->name();
    } else if (dynamic_cast<const DestinationField*>(&field) != NULL) {
        socketName = port.inputSocket()->name();
    } else {
        assert(false);
    }

    if (!field.hasSocketEncoding(socketName)) {
        PIGTextGenerator textGenerator;
        format text = textGenerator.text(
            PIGTextGenerator::TXT_SOCKET_ENCODING_MISSING);
        if (dynamic_cast<const SourceField*>(&field) != NULL) {
            text % socketName % "source" % field.parent()->name();
        } else {
            text % socketName % "destination" % field.parent()->name();
        }
        throw InvalidData(__FILE__, __LINE__, procName, text.str());
    }

    // create encoding for socket ID
    SocketEncoding& socketID = field.socketEncoding(socketName);
    BitVector socketIDBits;
    socketIDBits.pushBack(socketID.encoding(), socketID.socketIDWidth());

    // create encoding for socket code
    BitVector socketCodeBits;
    if (socketID.hasSocketCodes()) {
        SocketCodeTable& socketCodes = socketID.socketCodes();
        FUPortCode* portCode = NULL;
        if (terminal.isOpcodeSetting()) {
            if (&terminal.operation() == &NullOperation::instance()) {
                const TTAProgram::TerminalFUPort* fuTerm =
                    dynamic_cast<const TerminalFUPort*>(&terminal);
                PIGTextGenerator textGenerator;
                format text = textGenerator.text(
                    PIGTextGenerator::TXT_OSAL_OC_MISSING);
                text % (fuTerm != NULL ? fuTerm->hwOperation()->name() : "") % fuName;
                throw InvalidData(__FILE__, __LINE__, procName, text.str());
            }
            string opName = terminal.operation().name();
            if (!socketCodes.hasFUPortCode(fuName, port.name(), opName)) {
                PIGTextGenerator textGenerator;
                format text = textGenerator.text(
                    PIGTextGenerator::TXT_FU_OC_PORT_CODE_MISSING);
                text % fuName % opName;
                throw InvalidData(__FILE__, __LINE__, procName, text.str());
            }
            portCode = &socketCodes.fuPortCode(fuName, port.name(), opName);

        } else {
            if (!socketCodes.hasFUPortCode(fuName, port.name())) {
                PIGTextGenerator textGenerator;
                format text = textGenerator.text(
                    PIGTextGenerator::TXT_FU_PORT_CODE_MISSING);
                text % port.name() % fuName % socketName;
                throw InvalidData(__FILE__, __LINE__, procName, text.str());
            }
            portCode = &socketCodes.fuPortCode(fuName, port.name());
        }
        socketCodeBits.pushBack(portCode->encoding(), socketCodes.width());
    }

    encodeSlotField(field, socketIDBits, socketCodeBits, bitVector);
}

/**
 * Encodes the given immediate terminal to the given bit vector.
 *
 * @param field The source field which determines the encoding rules.
 * @param terminal The immediate terminal to be encoded.
 * @param bitVector The bit vector to which the bits are added.
 * @exception InvalidData If the BEM is erroneous.
 */
void
CodeCompressorPlugin::encodeImmediateTerminal(
    const SourceField& field, const TTAProgram::Terminal& terminal,
    InstructionBitVector& bitVector) const {
    assert(terminal.isImmediate());
    unsigned int immValue = terminal.value().uIntWordValue();
    ImmediateEncoding& encoding = field.immediateEncoding();

    if (&encoding == &NullImmediateEncoding::instance()) {
        PIGTextGenerator textGenerator;
        format text = textGenerator.text(
            PIGTextGenerator::TXT_IMMEDIATE_ENCODING_MISSING);
        text % field.parent()->name();
        const string procName = 
            "CodeCompressorPlugin::encodeImmediateTerminal";
        throw InvalidData(__FILE__, __LINE__, procName, text.str());
    }

    // create bits for the immediate value
    InstructionBitVector immediateBits;
    static_cast<BitVector&>(immediateBits).pushBack(
        immValue, field.width() - encoding.encodingWidth());
    if (AssocTools::containsKey(terminalsToRelocate_, &terminal)) {
        Instruction& referenced = currentPOM_->instructionAt(immValue);
        assert(&referenced != &NullInstruction::instance());
        immediateBits.startSettingInstructionReference(referenced);
        IndexBound simmBound(0, (immediateBits.size()-1));
        immediateBits.addIndexBoundsForReference(simmBound);
    }

    // encode the source field
    unsigned int oldSize = bitVector.size();
    if (field.componentIDPosition() == BinaryEncoding::LEFT) {
        static_cast<BitVector&>(bitVector).pushBack(
            encoding.encoding(), encoding.encodingWidth());
        bitVector.pushBack(immediateBits);
    } else {
        bitVector.pushBack(immediateBits);
        static_cast<BitVector&>(bitVector).pushBack(
            encoding.encoding(), encoding.encodingWidth());
    }
    assert(bitVector.size() - oldSize == static_cast<size_t>(field.width()));
}

/**
 * Encodes the given source or destination field with the given bit vectors.
 *
 * @param field The source or destination field which determines the encoding
 *              order of component ID bits and socket code bits.
 * @param componentIDBits The encoding for component ID.
 * @param socketCodeBits The encoding for socket code.
 * @param bitVector The bit vector to which the bits are added.
 */
void
CodeCompressorPlugin::encodeSlotField(
    const SlotField& field,
    const BitVector& componentIDBits,
    const BitVector& socketCodeBits,
    BitVector& bitVector) {

    unsigned int oldSize = bitVector.size();
    unsigned int numberOfZeros = field.width() - componentIDBits.size() - 
        socketCodeBits.size();

    if (field.componentIDPosition() == BinaryEncoding::LEFT) {
        bitVector.pushBack(componentIDBits);
        bitVector.pushBack(0, numberOfZeros);
        bitVector.pushBack(socketCodeBits);
    } else {
        bitVector.pushBack(socketCodeBits);
        bitVector.pushBack(0, numberOfZeros);
        bitVector.pushBack(componentIDBits);
    }
    assert(bitVector.size() - oldSize == static_cast<size_t>(field.width()));
}


/**
 * Creates a bit vector that encodes the given register file terminal.
 *
 * @param socketCodes The socket code table that contains the port codes.
 * @param portCode The port code that defines the encoding.
 * @param registerIndex The register index to be encoded.
 * @return Bit vector that contains encoding for the register file terminal.
 */
BitVector
CodeCompressorPlugin::socketCodeBits(
    const SocketCodeTable& socketCodes,
    const PortCode& portCode,
    unsigned int registerIndex) {

    BitVector bits;
    unsigned int encodingWidth = portCode.width() - portCode.indexWidth();
    if (portCode.hasEncoding()) {
        bits.pushBack(portCode.encoding(), encodingWidth);
    }
    bits.pushBack(registerIndex, socketCodes.width() - encodingWidth);
    assert(bits.size() == static_cast<size_t>(socketCodes.width()));
    return bits;
}


/**
 * Initializes the immediatesToRelocate_ and terminalsToRelocate_ members.
 */
void
CodeCompressorPlugin::initializeRelocations() {

    typedef std::set<const TPEF::SectionElement*> SectionElemSet;
    SectionElemSet toRelocate;
    RelocSection* relocSection = NULL;

    // find the correct reloc section from tpef
    for (unsigned int sectionIndex = 0; 
         sectionIndex < currentTPEF_->sectionCount(Section::ST_RELOC);
         sectionIndex++) {
        relocSection = dynamic_cast<RelocSection*>(
            currentTPEF_->section(Section::ST_RELOC, sectionIndex));
        Section* codeSection = currentTPEF_->section(Section::ST_CODE, 0);
        if (relocSection->referencedSection() == codeSection) {
            break;
        }
    }

    if (relocSection == NULL) {
        return;
    }

    // collect the section elements that are to be relocated to a set
    for (unsigned int i = 0; i < relocSection->elementCount(); i++) {
        RelocElement* element = dynamic_cast<RelocElement*>(
            relocSection->element(i));
        assert(element != NULL);
        SectionElement* location = element->location();
        SectionElement* destination = element->destination();
        assert(dynamic_cast<InstructionElement*>(location) != NULL);
        if (dynamic_cast<InstructionElement*>(destination) != NULL){
            toRelocate.insert(location);
        }
    }

    // find the corresponding Immediates and Terminals from POM and add
    // them to immediatesToRelocate_ and terminalsToRelocate_ members
    CodeSection* codeSection = dynamic_cast<CodeSection*>(
        currentTPEF_->section(Section::ST_CODE, 0));
    Instruction* instruction = &currentPOM_->firstInstruction();
    unsigned int instStart = 0;
    for (unsigned int i = 0; i < codeSection->elementCount(); i++) {
        InstructionElement* instructionElem = 
            dynamic_cast<InstructionElement*>(codeSection->element(i));     
        if (instructionElem->begin() && i > 0) {
            instruction = &currentPOM_->nextInstruction(*instruction);
            instStart = i;
        }
        if (AssocTools::containsKey(toRelocate, instructionElem)) {
            assert(instructionElem->isImmediate());
            ImmediateElement* immElem = dynamic_cast<ImmediateElement*>(
                instructionElem);                
            Terminal* terminal = immediateTerminal(
                *codeSection, instStart, *immElem, *instruction);
            Immediate* immediate = programImmediate(*immElem, *instruction);
            if (terminal != NULL) {
                terminalsToRelocate_.insert(terminal);
            } else if (immediate != NULL) {
                immediatesToRelocate_.insert(immediate);
            } else {
                assert(false);
            }
        }
    }
}


/**
 * Adds the bits of the given number to the given bit vector.
 *
 * The bits added are determined by leftmostBit and rightmostBit parameters.
 *
 * @param number The number.
 * @param leftmost The leftmost bit of the number which is added to the bit
 *                 vector.
 * @param rightmost The rightmost bit of the number which is added to the
 *                  bit vector.
 * @param bitVector The bit vector to which the bits are added.
 */
void
CodeCompressorPlugin::addBits(
    unsigned int number,
    int leftmostBit,
    int rightmostBit,
    BitVector& bitVector) {
    
    for (int i = leftmostBit; i >= rightmostBit; i--) {
        bitVector.push_back(MathTools::bit(number, i));
    }
}


/**
 * Returns the immediate terminal that matches with the given 
 * ImmediateElement instance.
 *
 * @param codeSection The code section that contains the immediate element.
 * @param elementIndex The index at which the instruction containing the
 *                     immediate element starts.
 * @param immElem The immediate element.
 * @param instruction The instruction that contains the immediate terminal.
 * @return The immediate terminal or NULL if there is no such terminal.
 */
TTAProgram::Terminal*
CodeCompressorPlugin::immediateTerminal(
    const TPEF::CodeSection& codeSection,
    unsigned int elementIndex,
    const TPEF::ImmediateElement& immElem,
    const TTAProgram::Instruction& instruction) const {

    if (!immElem.isInline()) {
        return NULL;
    }

    Byte busIndex = immElem.destinationIndex();
    string busName = "";
    for (unsigned int i = elementIndex; i < codeSection.elementCount(); 
         i++) {
        InstructionElement* element = dynamic_cast<InstructionElement*>(
            codeSection.element(i));
        if (element->begin() && i > elementIndex) {
            assert(false);
        }
        MoveElement* moveElem = dynamic_cast<MoveElement*>(element);
        if (moveElem != NULL && 
            moveElem->sourceType() == MoveElement::MF_IMM &&
            moveElem->sourceIndex() == busIndex) {
            busName = TPEFTools::resourceName(
                *currentTPEF_, ResourceElement::MRT_BUS, moveElem->bus());
            break;
        }
    }
    
    assert(busName != "");
    
    for (int i = 0; i < instruction.moveCount(); i++) {
        Move& move = instruction.move(i);
        if (move.bus().name() == busName) {
            return &move.source();
        }
    }

    assert(false);
    return NULL;
}


/**
 * Returns the immediate in the given instruction that matches with the
 * given immediate element.
 *
 * @param immElem The immediate element.
 * @param instruction The instruction.
 * @return The immediate or NULL if there is no such an immediate.
 */
TTAProgram::Immediate*
CodeCompressorPlugin::programImmediate(
    const TPEF::ImmediateElement& immElem,
    const TTAProgram::Instruction& instruction) const {

    if (immElem.isInline()) {
        return NULL;
    }

    string dstUnitName = TPEFTools::resourceName(
        *currentTPEF_, ResourceElement::MRT_IMM, immElem.destinationUnit());
    
    for (int i = 0; i < instruction.immediateCount(); i++) {
        Immediate& immediate = instruction.immediate(i);
        const Terminal& dstTerminal = immediate.destination();
        assert(dstTerminal.isImmediateRegister());
        const ImmediateUnit& dstUnit = dstTerminal.immediateUnit();
        if (dstUnit.name() == dstUnitName) {
            return &immediate;
        }
    }

    assert(false);
    return NULL;
}
