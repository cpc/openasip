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
 * @file OutputPSocketResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the OutputPSocketResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_OUTPUTPSOCKETRESOURCE_HH
#define TTA_OUTPUTPSOCKETRESOURCE_HH

#include <map>
#include <string>

#include "SchedulingResource.hh"
#include "PSocketResource.hh"

namespace TTAMachine {
    class Port;
}

/**
 * An interface for scheduling resources of Resource Model
 * The derived class OutputPSocketResource
 */

class OutputPSocketResource : public PSocketResource {
public:
    virtual ~OutputPSocketResource();
    OutputPSocketResource(const std::string& name, unsigned int initiationInterval = 0);

    virtual bool isAvailable(const int cycle) const override;
    virtual bool isOutputPSocketResource() const override;
    virtual void assign(const int cycle, MoveNode& node) override;
    virtual void unassign(const int cycle, MoveNode& node) override;
    virtual bool canAssign(const int cycle, const MoveNode& node) const override;
    virtual bool operator < (const SchedulingResource& other) const override;
    void clear() override;
protected:
    virtual bool validateDependentGroups() override;
    virtual bool validateRelatedGroups() override;

private:
    mutable int activeCycle_;
    // Copying forbidden
    OutputPSocketResource(const OutputPSocketResource&);
    // Assignment forbidden
    OutputPSocketResource& operator=(const OutputPSocketResource&);
    // For each cycle stores which Port connected to Socket is read
    std::map<int, std::pair<const TTAMachine::Port*, int> > storedPorts_;
};

#endif
