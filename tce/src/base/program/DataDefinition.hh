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
 * @file DataDefinition.hh
 *
 * Declaration of DataDefinition class.
 *
 * @author Mikael Lepistö 2006 (mikael.lepisto-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2006 (pekka.jaaskelainen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_DATA_DEFINITION_HH
#define TTA_DATA_DEFINITION_HH

#include <vector>
#include "BaseType.hh"
#include "Exception.hh"
#include "Address.hh"

namespace TTAMachine {
    class AddressSpace;
}

namespace TTAProgram {
    
/**
 * Describes characteristics of an initialized or uninitialized 
 * memory area.
 */
class DataDefinition {
public:
    DataDefinition(
        Address start, int size, bool littleEndian,
        MinimumAddressableUnit* initData = NULL, bool allZeros = false);

    DataDefinition(
        Address start, const std::vector<MinimumAddressableUnit>& initData,
        bool littleEndian);

    virtual ~DataDefinition();

    virtual Address startAddress() const;
    virtual void setStartAddress(Address start);

    virtual bool isInitialized() const;
    virtual MinimumAddressableUnit MAU(int index) const;
    virtual int size() const;

    virtual bool isAddress() const;
    virtual bool isInstructionAddress() const;
    virtual Address destinationAddress() const;
    virtual void setDestinationAddress(Address dest);

    virtual DataDefinition* copy() const;

    bool isLittleEndian() const { return littleEndian_; }

private:
    /// Start address of definition.
    Address start_;

    /// Size of uninitialized data definition.
    int size_;
    
    /// Init data of definition.
    std::vector<MinimumAddressableUnit>* data_;

    /// Is all the data zeros? (In this case data_ is null)
    bool allZeros_;

    bool littleEndian_;

};

}

#endif
