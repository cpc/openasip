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
 * @file DataInstructionAddressDef.hh
 *
 * Declaration of DataInstructionAddressDef class.
 *
 * @author Mikael Lepistö 2006 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_DATA_INSTRUCTION_ADDRESS_DEF_HH
#define TTA_DATA_INSTRUCTION_ADDRESS_DEF_HH

#include "DataDefinition.hh"
#include "InstructionReference.hh"

namespace TTAProgram {
    
    class Address;
    class InstructionReference;

/**
 * Describes characteristics of a memory area containing 
 * an address to an instruction.
 */
class DataInstructionAddressDef : public DataDefinition {
public:
    DataInstructionAddressDef(
        Address start, int size, InstructionReference& dest,
        bool littleEndian);

    virtual ~DataInstructionAddressDef();
    
    virtual bool isInitialized() const;
    virtual bool isAddress() const;
    virtual bool isInstructionAddress() const;

    virtual Address destinationAddress() const;

    DataDefinition* copy() const;

private:
    /// Destination instruction of data area.
    InstructionReference dest_;
};

}

#endif
