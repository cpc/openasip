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
 * @file BEMValidator.hh
 *
 * Declaration of BEMValidator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_BEM_VALIDATOR_HH
#define TTA_BEM_VALIDATOR_HH

#include <vector>
#include <string>

#include "Exception.hh"

namespace TTAMachine {
    class Machine;
    class Bus;
    class ImmediateSlot;
    class ControlUnit;
}

class BinaryEncoding;
class SocketEncoding;
class MoveSlot;

/**
 * Validates binary encoding maps against machines. Checks that the
 * BEM contains all the required information.
 */
class BEMValidator {
public:
    BEMValidator(
        const BinaryEncoding& bem,
        const TTAMachine::Machine& machine);
    ~BEMValidator();

    bool validate();
    int errorCount() const;
    std::string errorMessage(int index) const;
    int warningCount() const;
    std::string warningMessage(int index) const;

private:
    /// Vector type for string.
    typedef std::vector<std::string> StringVector;

    void checkMoveSlot(const TTAMachine::Bus& bus);
    void checkDestinationField(const TTAMachine::Bus& bus);
    void checkSourceField(const TTAMachine::Bus& bus);
    void checkGuardField(const TTAMachine::Bus& bus);
    void checkSocketCodeTable(const SocketEncoding& socketEnc);
    void checkImmediateSlot(const TTAMachine::ImmediateSlot& immSlot);
    void checkImmediateControlField();
    void checkLImmDstRegisterFields();
    void checkImemMauWidth(TTAMachine::ControlUnit& gcu);

    bool needsSourceField(const MoveSlot& slot) const;
    bool needsSocketCodeTable(const SocketEncoding& socketEnc) const;

    /// The binary encoding map.
    const BinaryEncoding& bem_;
    /// The machine.
    const TTAMachine::Machine& machine_;

    /// Contains the error messages.
    StringVector errorMessages_;
    /// Contains the warning messages.
    StringVector warningMessages_;
};

#endif
