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
 * @file BusState.hh
 *
 * Declaration of BusState class.
 *
 * @author Jussi Nyk�nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J��skel�inen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BUS_STATE_HH
#define TTA_BUS_STATE_HH

#include <string>

#include "RegisterState.hh"

//////////////////////////////////////////////////////////////////////////////
// BusState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models the state of the bus.
 */
class BusState : public RegisterState {
public:
    BusState(int width);
    virtual ~BusState();

    virtual void setValue(const SimValue& value);
    void setValueInlined(const SimValue& value);

    void clear();

    void setSquashed(bool isSquashed);
    bool isSquashed() const;

    int width() const;

    BusState(const BusState&) = delete;
    BusState& operator=(const BusState&) = delete;

private:
    /// Name of the bus.
    std::string name_;
    /// Width of the bus.
    int width_;
    /// True in case this bus was squashed the last time a move was executed
    /// in this bus.
    bool squashed_;
};

//////////////////////////////////////////////////////////////////////////////
// NullBusState
//////////////////////////////////////////////////////////////////////////////

/**
 * Models null BusState object.
 */
class NullBusState : public BusState {
public:
    static NullBusState& instance();

    virtual ~NullBusState();

    virtual void setValue(const SimValue& value);
    virtual const SimValue& value() const;

    NullBusState(const NullBusState&) = delete;
    NullBusState& operator=(const NullBusState&) = delete;

protected:
    NullBusState();

private:
    /// Unique instance of NullBusState.
    static NullBusState* instance_;
};

#include "BusState.icc"

#endif
