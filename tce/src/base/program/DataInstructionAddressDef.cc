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
 * @file DataInstructionAddressDef.cc
 *
 * Implementation of DataInstructionAddressDef class.
 *
 * @author Mikael Lepistö 2006 (mikael.lepisto-no.spam-tut.fi)
 * @note rating: red
 */

#include "DataInstructionAddressDef.hh"
#include "InstructionReference.hh"
#include "Instruction.hh"
#include "Address.hh"

namespace TTAProgram {

/**
 * Creates data area definition containing reference to an instruction.
 *
 * @param start Starting address of definition.
 * @param size Number of MAUs that area represents.
 * @param dest Instruction which is referred by the data definition.
 */
DataInstructionAddressDef::DataInstructionAddressDef(
    Address start, int size, InstructionReference& dest, bool littleEndian) :
    DataDefinition(start, size, littleEndian), dest_(dest) {
}

/**
 * Nothing to free.
 */
DataInstructionAddressDef::~DataInstructionAddressDef() {
}

/**
 * Returns true, class defines always initialized data area.
 * 
 * @return Always true.
 */
bool 
DataInstructionAddressDef::isInitialized() const {
    return true;
}

/**
 * Returns true, class contains always address reference.
  * 
 * @return Always true.
 */
bool 
DataInstructionAddressDef::isAddress() const {
    return true;
}

/**
 * Returns true if init data of definition contains an instruction address.
 *
 * @return True if init data of definition contains an instruction address.
 */
bool 
DataInstructionAddressDef::isInstructionAddress() const {
    return true;
}

/**
 * Returns destination address, where to intialization value refers.
  * 
 * @return Destination address of initialized data area.
 */
Address
DataInstructionAddressDef::destinationAddress() const {
    return dest_.instruction().address();
}

/**
 * Copying of this class is *not* allowed.

 * This class can't be copied, because InstructionReferece cannot
 * be copied correctly.
 *
 * @return Throws always an exception.
 */
DataDefinition* 
DataInstructionAddressDef::copy() const {
    throw WrongSubclass(
        __FILE__, __LINE__, __func__, 
        "DataInstructionAddress definition cannot be copied.");
}

}
