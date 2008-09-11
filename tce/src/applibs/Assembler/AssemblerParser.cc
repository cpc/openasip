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
 * @file AssemblerParser.cc
 *
 * Syntax declarations and callbacks of assembler language.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 *
 * @note rating: yellow
 */

#include "AssemblerParser.hh"

#include "Binary.hh"
#include "CodeSection.hh"
#include "ResourceSection.hh"
#include "RelocSection.hh"
#include "SymbolSection.hh"

using namespace TPEF;

/////////////////////////////////////////////////////////////////////////////
/// PrintString actor
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor of actor.
 *
 * @param aStr String to print.
 */
PrintString::PrintString(const char* aStr) : str_(aStr) {
}


/**
 * Constructor of actor.
 *
 * @param aStr String to print.
 */
PrintString::PrintString(std::string& aStr) : str_(aStr.c_str()) {
}

/**
 * Prints out string of actor.
 */
void
PrintString::operator() (const char*, const char*) const {
    std::cerr << str_;
}

/////////////////////////////////////////////////////////////////////////////
/// NewCodeSectionActor
/////////////////////////////////////////////////////////////////////////////
/**
 * Starts new code section starting from defined address.
 *
 * @param creator Creator that is used for code section generating.
 * @param startAddress Start address of code.
 */
NewCodeSectionActor::NewCodeSectionActor(
    CodeSectionCreator& creator,
    UValue& startAddress) :
    creator_(creator), startAddress_(startAddress) {
}

void
NewCodeSectionActor::operator()(const char*, const char*) const {
    creator_.newSection(startAddress_);
}

/////////////////////////////////////////////////////////////////////////////
/// AddMoveActor
/////////////////////////////////////////////////////////////////////////////

/**
 * Adds parsed move to code section creator.
 *
 * @param creator Creator where to add move.
 * @param move Move to add.
 */
AddMoveActor::AddMoveActor(
    CodeSectionCreator& creator,
    const ParserMove& move) :
    creator_(creator), move_(move) {
}

void
AddMoveActor::operator() (const char*, const char*) const {
    creator_.addMove(move_);
}

/////////////////////////////////////////////////////////////////////////////
/// SetStartAddressActor
/////////////////////////////////////////////////////////////////////////////

/**
 * Sets start address for next data area definition.
 *
 * @param creator Creator that is used for data section generation.
 * @param startAddress Address for next data area definition.
 */
SetStartAddressActor::SetStartAddressActor(DataSectionCreator& creator,
                                           UValue& startAddress) :
    creator_(creator), startAddress_(startAddress) {
}

void
SetStartAddressActor::operator() (const char*, const char*) const {
    creator_.setAreaStartAddress(startAddress_);
}

/////////////////////////////////////////////////////////////////////////////
/// AddDataLineActor
/////////////////////////////////////////////////////////////////////////////

/**
 * Adds new data area definition to data section creator.
 *
 * @param creator Creator that is used for data section generation.
 * @param dataLine Data area definition to add.
 */
AddDataLineActor::AddDataLineActor(DataSectionCreator& creator,
                                   const DataLine& dataLine) :
    creator_(creator), dataLine_(dataLine) {
}

void
AddDataLineActor::operator() (const char*, const char*) const {
    creator_.addDataLine(dataLine_);
}

/////////////////////////////////////////////////////////////////////////////
/// AddLabelActor
/////////////////////////////////////////////////////////////////////////////

/**
 * Adds new label to label manager.
 *
 * @param manager Label manager that is used to label bookkeeping.
 * @param aSpace Address space for label.
 * @param name Label name.
 * @param value Value of the label.
 */
AddLabelActor::AddLabelActor(LabelManager& manager,
                             TPEF::ASpaceElement& aSpace,
                             std::string& name, UValue& value) :
    manager_(manager), aSpace_(aSpace),
    name_(name), value_(value) {
}

void
AddLabelActor::operator() (const char*, const char*) const {
    manager_.addLabel(aSpace_, name_, value_);
}

/////////////////////////////////////////////////////////////////////////////
/// AddProcedureActor
/////////////////////////////////////////////////////////////////////////////

/**
 * Adds procedure symbol to label manager.
 *
 * @param manager Label manager that is used to label bookkeeping.
 * @param name Name of the procedure.
 * @param value Instruction address to procedure start.
 */
AddProcedureActor::AddProcedureActor(
    LabelManager& manager,
    std::string& name, UValue& value) :
    manager_(manager),
    name_(name), value_(value) {
}

void
AddProcedureActor::operator() (const char*, const char*) const {
    manager_.addProcedure(name_, value_);
}

/////////////////////////////////////////////////////////////////////////////
/// SetGlobalActor
/////////////////////////////////////////////////////////////////////////////
/**
 * Sets label to be globally visible.
 *
 * @param manager Manager wehere to set global.
 */
SetGlobalActor::SetGlobalActor(LabelManager &manager) :
    manager_(manager) {
}

void
SetGlobalActor::operator() (const char* start, const char* end) const {
    std::string str(start, end);
    manager_.setGlobal(str);
}

/////////////////////////////////////////////////////////////////////////////
/// AssemblerParser
/////////////////////////////////////////////////////////////////////////////

/**
 * Constructor.
 *
 * @param aBin TPEF object where to create program.
 * @param aMach Machine which for program is written.
 * @param parent Assembler root class for warning handling.
 */
AssemblerParser::AssemblerParser(
    TPEF::Binary &aBin, TTAMachine::Machine &aMach,
    Assembler* parent) :
    bin_(aBin), mach_(aMach),
    resourceManager_(aBin, aMach, parent),
    dataSectionCreator_(resourceManager_, parent),
    codeSectionCreator_(resourceManager_, parent),
    labelManager_(aBin, resourceManager_, parent),
    parent_(parent) {
}

/**
 * Frees all resources allocated by parser.
 */
void
AssemblerParser::cleanup() {
    dataSectionCreator_.cleanup();
    codeSectionCreator_.cleanup();
    labelManager_.cleanup();
}

bool
AssemblerParser::compile(std::string& asmCode) const {
    return boost::spirit::parse(asmCode.c_str(), *this).full;
}

/**
 * Returns line number where parse error happened in assembler file.
 *
 * @return Line number where parse error happened in assembler file.
 */
UValue
AssemblerParser::errorLine() {
    return parserTemp_.lineNumber;
}

/**
 * Finalizes parsed TPEF.
 *
 * After calling this successfully parser should contain valid tpef.
 */
void
AssemblerParser::finalize() const
    throw (CompileError) {

    bin_.setArch(Binary::FA_TTA_TUT);
    bin_.setType(Binary::FT_PARALLEL);

    // these must be called in this order to make sure that all label values
    // are resolved before they are used
    dataSectionCreator_.finalize(bin_, labelManager_);
    codeSectionCreator_.finalize(bin_, labelManager_);
    labelManager_.finalize();
}
