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
    Address start, int size, InstructionReference& dest) :
    DataDefinition(start, size), dest_(dest) {
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
