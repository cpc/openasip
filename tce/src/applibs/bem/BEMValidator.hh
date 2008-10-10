/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file BEMValidator.hh
 *
 * Declaration of BEMValidator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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
    std::string errorMessage(int index) const
        throw (OutOfRange);

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

    bool needsSourceField(const MoveSlot& slot) const;
    bool needsSocketCodeTable(const SocketEncoding& socketEnc) const;

    /// The binary encoding map.
    const BinaryEncoding& bem_;
    /// The machine.
    const TTAMachine::Machine& machine_;

    /// Contains the error messages.
    StringVector errorMessages_;
};

#endif
