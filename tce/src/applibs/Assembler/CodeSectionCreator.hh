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
 * @file CodeSectionCreator.hh
 *
 * Declaration of CodeSectionCreator class.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TCEASM_CODE_SECTION_CREATOR_HH
#define TCEASM_CODE_SECTION_CREATOR_HH

#include "ParserStructs.hh"
#include "MoveElement.hh"
#include "Exception.hh"

namespace TPEF {
    class CodeSection;
    class Binary;
    class InstructionElement;
}

class MachineResourceManager;
class LabelManager;
class Assembler;

/**
 * Read moves and creates TPEF code section out of them.
 */
class CodeSectionCreator {
public:

    CodeSectionCreator(
        MachineResourceManager &resourceManager,
        Assembler* parent);

    void newSection(UValue startAddress)
        throw (OutOfRange);

    void addMove(const ParserMove &move)
        throw (CompileError);

    void finalize(TPEF::Binary &tpef, LabelManager &labels)
        throw (CompileError);

    void cleanup();


private:

    /**
     * Type of move element.
     */
    enum ElementType {
        EMPTY,    ///< Empty move.
        MOVE,     ///< Data transport.
        IMMEDIATE ///< Long immediate assignment.
    };

    /**
     * Internal representation of one move.
     *
     * Element can define normal move, empty move or long immediate 
     * assignment. This format is written as TPEF in finalize pahse.
     */
    struct InternalElement {
        /// Type of move.
        ElementType type;
        /// Bus slot number.
        UValue slot;

        /// Is first move of an instruction.
        bool isBegin;

        /// Is guarded move.
        bool isGuarded;
        /// Is guard inverted.
        bool isInverted;
        /// Guard type.
        TPEF::MoveElement::FieldType guardType;
        /// TPEF Resource section unit id of guard register or port.
        UValue guardUnit;
        /// TPEF Resource section operand id or register number.
        UValue guardIndex;

        /// Source type.
        TPEF::MoveElement::FieldType srcType;
        /// TPEF Resource section unit id of register or port.
        UValue srcUnit;
        /// TPEF Resource section operand id or register number.
        UValue srcIndex;

        /// Destination type.
        TPEF::MoveElement::FieldType dstType;
        /// TPEF Resource section unit id of register or port.
        UValue dstUnit;
        /// TPEF Resource section operand id or register number.
        UValue dstIndex;
        
        /// If source is immediate, the value.
        LiteralOrExpression immValue;
        
        /// Line number where in source code this move is located.
        UValue asmLineNumber;

        /// Annotations for the element.
        std::vector<Annotation> annotationes;
    };

    /**
     * Internal representation of code section.
     */
    struct InternalSection {
        /// Start address of the section.
        UValue startAddress;
        /// Elements of the section.
        std::vector<InternalElement> elements;
    };

    void startNewInstruction();

    UValue slotNumber();

    UValue immediateIndex();

    bool isDestinationAlreadyWritten(const InternalElement& elem) const;

    void addAnnotationes(TPEF::InstructionElement& instrElem, 
                         InternalElement& elem, LabelManager& lables) const;

    /// Internal representation of code section.
    InternalSection internalSection_;

    /// TPEF Resources and strings.
    MachineResourceManager& resources_;

    /// Place to add warnings during compilation.
    Assembler* parent_;

    /// Next element is starting element of instruction.
    bool isNextBegin_;

    /// Slot number of current move.
    UValue slotNumber_;

    /// Immediate index.
    UValue immediateIndex_;

    /// Bitwidth of immediate value containing address to relocate.
    static const UValue CODE_RELOC_SIZE;
};

#endif
