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
 * @file InputPSocketResource.hh
 *
 * Declaration of prototype of Resource Model:
 * declaration of the InputPSocketResource class.
 *
 * @author Vladimir Guzma 2006 (vladimir.guzma-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_INPUTPSOCKETRESOURCE_HH
#define TTA_INPUTPSOCKETRESOURCE_HH

#include<string>

#include "SchedulingResource.hh"
#include "PSocketResource.hh"

/**
 * An interface for scheduling resources of Resource Model
 * The derived class InputPSocketResource
 */

class InputPSocketResource : public PSocketResource {
public:
    virtual ~InputPSocketResource();
    InputPSocketResource(const std::string& name, unsigned int initiationInterval = 0);

    virtual bool isInputPSocketResource() const override;
    virtual bool canAssign(const int cycle, const MoveNode& node) const override;

    virtual bool operator < (const SchedulingResource& other) const override;
protected:
    virtual bool validateDependentGroups() override;
    virtual bool validateRelatedGroups() override;

private:
    // Copying forbidden
    InputPSocketResource(const InputPSocketResource&);
    // Assignment forbidden
    InputPSocketResource& operator=(const InputPSocketResource&);
};

#endif
