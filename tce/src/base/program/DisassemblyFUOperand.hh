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
 * @file DisassemblyFUOperand.hh
 *
 * Declaration of DisassemblyFUOperand class.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_DISASSEMBLY_FU_OPERAND_HH
#define TTA_DISASSEMBLY_FU_OPERAND_HH

#include "DisassemblyElement.hh"
#include "BaseType.hh"

/**
 * Represents an fu.operand.index referencein the disassembler.
 */
class DisassemblyFUOperand : public DisassemblyElement {
public:
    DisassemblyFUOperand(
        std::string fuName, std::string operationName, Word operand);

    virtual ~DisassemblyFUOperand();

    virtual std::string toString() const;

private:
    /// Name of the function unit.
    std::string fuName_;
    /// Name of the operation.
    std::string operationName_;
    /// Operand index.
    Word operand_;
};
#endif
