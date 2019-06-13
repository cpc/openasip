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
 * @file NullUnitImplementationLocation.hh
 *
 * Declaration of NullUnitImplementationLocation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_UNIT_IMPLEMENTATION_LOCATION_HH
#define TTA_NULL_UNIT_IMPLEMENTATION_LOCATION_HH

#include "Serializable.hh"
#include "UnitImplementationLocation.hh"

namespace IDF {

/**
 * Indicates what implementation of a unit is used. 
 *
 * A null class that indicates of unset location of implementation or
 * no implementation available.
 */
class NullUnitImplementationLocation : public UnitImplementationLocation  {
public:
    static NullUnitImplementationLocation& instance();

    virtual std::string hdbFile() const;
    virtual int id() const;
    virtual std::string unitName() const;
    virtual void setParent(MachineImplementation& parent);

protected:
    NullUnitImplementationLocation();
    virtual ~NullUnitImplementationLocation();

private:
    // the singleton instance of this class
    static NullUnitImplementationLocation instance_;
};
}

#endif
