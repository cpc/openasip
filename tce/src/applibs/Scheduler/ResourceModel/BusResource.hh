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
 * @file BusResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the BusResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_BUSRESOURCE_HH
#define TTA_BUSRESOURCE_HH

#include<string>
#include <map>
#include "SchedulingResource.hh"

/**
 * An interface for scheduling resources of Resource Model
 * The derived class BusResource
 */

class BusResource : public SchedulingResource {
public:
    BusResource(
        const std::string& name, int width, int limmSlotCount, int guardCount,
        int immSize, int socketCount, unsigned int initiationInterval = 0);
    virtual ~BusResource();

    virtual bool isInUse(const int cycle) const;
    virtual bool isAvailable(const int cycle) const;
    virtual bool isAvailable(
        const int cycle,
        const SchedulingResource& inputPSocket,
        const SchedulingResource& outputPSocket) const;
    virtual bool canAssign(const int cycle, const MoveNode& node) const;
    virtual void assign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void unassign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual bool canAssign(
        const int cycle,
        const MoveNode& node,
        const SchedulingResource& inputPSocket,
        const SchedulingResource& outputPSocket) const;
    virtual bool isBusResource() const;

    virtual bool operator < (const SchedulingResource& other) const;

    void clear();
protected:
    virtual bool validateDependentGroups();
    virtual bool validateRelatedGroups();

private:
    // Copying forbidden
    BusResource(const BusResource&);
    // assignment forbidden
    BusResource& operator=(const BusResource&);
    // Bus width in bits
    int busWidth_;
    // count of limm slots associated into this bus resource.
    int limmSlotCount_;
    // coutn of guard associated to this bus.
    int guardCount_;
    // bitwidth of imm.
    int immSize_;
    // number of connected sockets
    int socketCount_;

    // map contains <Cycle : testCounter>
    typedef std::map<int, int> ResourceRecordType;
    ResourceRecordType resourceRecord_;

};

#endif
