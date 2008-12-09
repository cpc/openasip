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
 * @file NullBus.hh
 *
 * Declaration of NullBus class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: yellow
 */

#ifndef TTA_NULL_BUS_HH
#define TTA_NULL_BUS_HH

#include "Bus.hh"

namespace TTAMachine {

/**
 * A singleton class which represents a null Bus.
 *
 * All the methods abort the program.
 */
class NullBus : public Bus {
public:
    static NullBus& instance();

    virtual std::string name() const;
    virtual void setName(const std::string& name)
        throw (ComponentAlreadyExists, InvalidName);

    virtual int position() const
        throw (NotAvailable);

    virtual int width() const;
    virtual int immediateWidth() const;
    virtual bool signExtends() const;
    virtual bool zeroExtends() const;

    virtual void setWidth(int width)
        throw (OutOfRange);
    virtual void setImmediateWidth(int width)
        throw (OutOfRange);
    virtual void setZeroExtends();
    virtual void setSignExtends();

    virtual void addSegment(Segment& segment)
        throw (ComponentAlreadyExists);
    virtual void removeSegment(Segment& segment);
    virtual bool hasSegment(const std::string& name) const;
    virtual Segment* segment(int index) const
        throw (OutOfRange);
    virtual Segment* segment(const std::string& name) const
        throw (InstanceNotFound);
    virtual int segmentCount() const;

    virtual bool isConnectedTo(const Socket& socket) const;

    virtual void addGuard(Guard& guard)
        throw (ComponentAlreadyExists);
    virtual void removeGuard(Guard& guard);
    virtual int guardCount() const;
    virtual Guard* guard(int index) const
        throw (OutOfRange);

    virtual bool hasNextBus() const;
    virtual bool hasPreviousBus() const;
    virtual Bus* nextBus() const
        throw (InstanceNotFound);
    virtual Bus* previousBus() const
        throw (InstanceNotFound);

    virtual bool canRead(const Bus& bus) const;
    virtual bool canWrite(const Bus& bus) const;
    virtual bool canReadWrite(const Bus& bus) const;

    virtual void setSourceBridge(Bridge& bridge);
    virtual void setDestinationBridge(Bridge& bridge);
    virtual void clearSourceBridge(Bridge& bridge);
    virtual void clearDestinationBridge(Bridge& bridge);

    virtual void setMachine(Machine& mach)
        throw (ComponentAlreadyExists);
    virtual void unsetMachine();
    virtual Machine* machine() const;

    virtual void ensureRegistration(const Component& component) const
        throw (IllegalRegistration);
    virtual bool isRegistered() const;

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);

private:
    NullBus();
    virtual ~NullBus();

    static NullBus instance_;
};
}

#endif
