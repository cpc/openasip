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
 * @file UnitImplementationLocation.hh
 *
 * Declaration of UnitImplementationLocation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_UNIT_IMPLEMENTATION_HH
#define TTA_UNIT_IMPLEMENTATION_HH

#include "Serializable.hh"
#include "Exception.hh"

namespace IDF {

class MachineImplementation;

/**
 * Indicates what implementation of a unit is used. 
 *
 * Refers to an entry in an HDB.
 */
class UnitImplementationLocation : public Serializable {
public:
    UnitImplementationLocation(
        const std::string& hdbFile,
        int id,
        const std::string& unitName);
    UnitImplementationLocation(const ObjectState* state)
        throw (ObjectStateLoadingException);
    virtual ~UnitImplementationLocation();

    virtual std::string hdbFile() const
        throw (FileNotFound);
    std::string hdbFileOriginal() const;
    virtual int id() const;
    virtual std::string unitName() const;
    virtual void setParent(MachineImplementation& parent)
        throw (InvalidData);

    virtual void setID(int id);
    virtual void setHDBFile(std::string file);

    // methods from Serializable interface
    void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
    ObjectState* saveState() const;

    /// ObjectState name for unit implementation.
    static const std::string OSNAME_UNIT_IMPLEMENTATION;
    /// ObjectState attribute key for the name of the HDB file.
    static const std::string OSKEY_HDB_FILE;
    /// ObjectState attribute key for the entry ID.
    static const std::string OSKEY_ID;
    /// Objectstate attribute key for the name of the unit.
    static const std::string OSKEY_UNIT_NAME;

private:
    /// Name of the HDB file.
    std::string hdbFile_;
    /// Entry ID in the HDB.
    int id_;
    /// Name of the unit in ADF.
    std::string unitName_;
    /// The parent MachineImplementation instance.
    MachineImplementation* parent_;
};
}

#endif
