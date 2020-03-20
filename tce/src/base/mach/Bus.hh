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
 * @file Bus.hh
 *
 * Declaration of Bus class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note reviewed 11 Jun 2004 by am, pj, jn, ll
 * @note rating: red
 */

#ifndef TTA_BUS_HH
#define TTA_BUS_HH

#include <string>
#include <vector>

#include "MachinePart.hh"
#include "Machine.hh"
#include "Exception.hh"

namespace TTAMachine {

class Guard;
class Machine;
class Segment;

/**
 * Represents a bus in TTA-processor.
 */
class Bus : public Component {
public:
    Bus(const std::string& name, int width, int immWidth,
        Machine::Extension extensionMode);
    Bus(const ObjectState* state);
    virtual ~Bus();
    virtual Bus* copy() const;
    virtual void copyGuardsTo(Bus& other) const;

    virtual int position() const;

    int width() const;
    int immediateWidth() const;
    bool signExtends() const;
    bool zeroExtends() const;

    virtual void setName(const std::string& name);
    virtual void setWidth(int width);
    virtual void setImmediateWidth(int width);
    virtual void setZeroExtends();
    virtual void setSignExtends();
    virtual void setExtensionMode(const Machine::Extension extension);

    virtual void addSegment(Segment& segment);
    virtual void removeSegment(Segment& segment);
    virtual bool hasSegment(const std::string& name) const;
    virtual Segment* segment(int index) const;
    virtual Segment* segment(const std::string& name) const;
    virtual int segmentCount() const;

    virtual bool isConnectedTo(const Socket& socket) const;

    bool hasGuard(const Guard& guard) const;
    void addGuard(Guard& guard);
    virtual void removeGuard(Guard& guard);
    int guardCount() const;
    Guard* guard(int index) const;

    virtual bool hasNextBus() const;
    virtual bool hasPreviousBus() const;
    virtual Bus* nextBus() const;
    virtual Bus* previousBus() const;

    virtual bool canRead(const Bus& bus) const;
    virtual bool canWrite(const Bus& bus) const;
    virtual bool canReadWrite(const Bus& bus) const;

    virtual void setSourceBridge(Bridge& bridge);
    virtual void setDestinationBridge(Bridge& bridge);
    virtual void clearSourceBridge(Bridge& bridge);
    virtual void clearDestinationBridge(Bridge& bridge);

    virtual void setMachine(Machine& mach);
    virtual void unsetMachine();

    virtual ObjectState* saveState() const;
    virtual void loadState(const ObjectState* state);

    virtual bool isArchitectureEqual(const Bus& bus) const;

    /// ObjectState name for Bus ObjectState.
    static const std::string OSNAME_BUS;
    /// ObjectState attribute key for bus width.
    static const std::string OSKEY_WIDTH;
    /// ObjectState attribute key for immediate width.
    static const std::string OSKEY_IMMWIDTH;
    /// ObjectState attribute key for extension mode.
    static const std::string OSKEY_EXTENSION;
    /// ObjectState attribute value for sign extension.
    static const std::string OSVALUE_SIGN;
    /// ObjectState attribute key for zero extension.
    static const std::string OSVALUE_ZERO;

private:
    /// Vector of Guard pointers.
    typedef std::vector<Guard*> GuardTable;
    /// Vector of Segment pointers.
    typedef std::vector<Segment*> SegmentTable;
    // Vector of Bridge pointers.
    typedef std::vector<Bridge*> BridgeTable;

    /// Assingment forbidden.
    Bus& operator=(const Bus&);
    void loadStateWithoutReferences(const ObjectState* state);
    void deleteAllSegments();
    void deleteAllGuards();
    Bridge* previousBridge() const;
    Bridge* nextBridge() const;
    Bridge* previousBridge(const BridgeTable& bridges) const;
    Bridge* nextBridge(const BridgeTable& bridges) const;
    void adjustSegmentChain(const ObjectState* busState);

    /// Bit width of the bus.
    int width_;
    /// Bit width of the inline immediate word.
    int immWidth_;
    /// Extension mode applied to the inline immediate word.
    Machine::Extension extensionMode_;

    /// Contains all guards of the bus.
    GuardTable guards_;
    /// Contains all the segments of the bus.
    SegmentTable segments_;

    /// Contains the source bridges (max 2).
    BridgeTable sourceBridges_;
    /// Contains the destination bridges (max 2).
    BridgeTable destinationBridges_;
};
}

#endif
