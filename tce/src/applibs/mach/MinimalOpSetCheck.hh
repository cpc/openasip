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
 * @file MinimalOpSetCheck.hh
 *
 * Declaration of MinimalOpSetCheck class that checks and fixes machines
 * operation set.
 *
 * @author Esa Määttä 2008 (esa.maatta-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef MINIMAL_OPSET_CHECK_HH
#define MINIMAL_OPSET_CHECK_HH

#include <set>
#include <vector>
#include <string>

#include "MachineCheck.hh"
#include "MachineCheckResults.hh"
#include "Machine.hh"

/**
 * MachineCheck to check is the machine fully connected.
 */
class MinimalOpSetCheck : public MachineCheck {

public:
    MinimalOpSetCheck();
    virtual ~MinimalOpSetCheck();

    virtual bool check(
        const TTAMachine::Machine& machine, MachineCheckResults& results) const;
    virtual bool check(
        const TTAMachine::Machine& machine) const;

    bool checkWithIgnore(
        const TTAMachine::Machine& machine,
        const std::set<std::string>& ignoreFUName) const;

    void missingOperations(
        const TTAMachine::Machine& machine,
        std::vector<std::string>& missingOps) const;

    void buildMinimalOpSet(const TTAMachine::Machine* machine = NULL);
    std::set<std::string> minimalOpSet() const;

    virtual bool canFix(const TTAMachine::Machine& mach) const;
    virtual std::string fix(TTAMachine::Machine& machine) const
        throw (InvalidData);

private:
    // minimal opset
    std::set<std::string> minimalOpSet_;
};

#endif

