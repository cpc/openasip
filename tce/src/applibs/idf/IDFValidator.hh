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
 * @file IDFValidator.hh
 *
 * Declaration of IDFValidator class.
 *
 * @author Lasse Laasonen 2006 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_IDF_VALIDATOR_HH
#define TTA_IDF_VALIDATOR_HH

#include <string>
#include <vector>

#include "Exception.hh"

namespace IDF {
    class MachineImplementation;
}

namespace TTAMachine {
    class Machine;
}

/**
 * Validates IDF files.
 */
class IDFValidator {
public:
    IDFValidator(
        const IDF::MachineImplementation& idf,
        const TTAMachine::Machine& machine);
    virtual ~IDFValidator();

    bool validate();
    int errorCount() const;
    std::string errorMessage(int index) const
        throw (OutOfRange);

    static void removeUnknownImplementations(
        IDF::MachineImplementation& idf,
        const TTAMachine::Machine& machine);

private:
    /// Vector type for string.
    typedef std::vector<std::string> StringVector;

    void checkFUImplementations();
    void checkRFImplementations();
    void checkIUImplementations();

    /// The machine.
    const TTAMachine::Machine& machine_;
    /// The implementation definition,
    const IDF::MachineImplementation& idf_;
    /// Vector of error messages.
    StringVector errorMessages_;
};

#endif
