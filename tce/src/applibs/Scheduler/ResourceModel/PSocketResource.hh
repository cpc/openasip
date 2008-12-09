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
 * @file PSocketResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the abstract PSocketResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PSOCKETRESOURCE_HH
#define TTA_PSOCKETRESOURCE_HH

#include <string>
#include <map>

#include "SchedulingResource.hh"
/**
 * An interface for scheduling resources of Resource Model.
 *
 * The derived class PSocketResource represents a combined resource of
 * a FU/RF port and a socket that connects it to a bus.
 */

class PSocketResource : public SchedulingResource {
public:
    virtual ~PSocketResource();
    PSocketResource(const std::string& name);

    virtual bool isInUse(const int cycle) const;
    virtual bool isAvailable(const int cycle) const;
    virtual bool canAssign(const int cycle, const MoveNode& node) const;
    virtual void assign(const int cycle, MoveNode& node)
        throw (Exception);
    virtual void unassign(const int cycle, MoveNode& node)
        throw (Exception);

protected:

private:
    // Recording PSocket usage - cycle = value map
    typedef std::map<int, int> ResourceRecordType;
    // Copying forbidden
    PSocketResource(const PSocketResource&);
    // Assignment forbidden
    PSocketResource& operator=(const PSocketResource&);

    ResourceRecordType resourceRecord_;
};

#endif
