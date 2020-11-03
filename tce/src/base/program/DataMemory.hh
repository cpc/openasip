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
 * @file DataMemory.hh
 *
 * Declaration of DataMemory class.
 *
 * @author Mikael Lepistö 2006 (mikael.lepisto-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_DATA_MEMORY_HH
#define TTA_DATA_MEMORY_HH

#include <map>
#include <vector>

#include "Exception.hh"
#include "Address.hh"

namespace TTAMachine {
    class AddressSpace;
}

namespace TTAProgram {    
    class DataDefinition;
    
/**
 * Represents an address space containing data.
 *
 * Data definitions are ordered in the DataMemory class so that largest index
 * contains the last data definition.
 */
class DataMemory {
public:
    DataMemory(const TTAMachine::AddressSpace& aSpace);
    virtual ~DataMemory();

    void addDataDefinition(DataDefinition* dataDef);
    DataDefinition& dataDefinition(Address address) const;
    DataDefinition& dataDefinition(int index) const;
    int dataDefinitionCount() const;
    int dataDefinitionsEnd() const;

    void deleteDataDefinition(int index);

    const TTAMachine::AddressSpace& addressSpace() const;
    void setAddressSpace(const TTAMachine::AddressSpace& space);

    DataMemory* copy() const;

private:
    /// Map for data definitions.
    typedef std::map<AddressImage, DataDefinition*> DataDefMap;

    /// Address space of the memory.
    const TTAMachine::AddressSpace* ramSpace_;

    /// Data definitions for the address space.
    DataDefMap dataDefs_;

    /// Cache for indexing dataDefinitions
    mutable std::vector<DataDefinition*> indexCache_;
};

}

#endif
