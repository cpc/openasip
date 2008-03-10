/**
 * @file BEMValidator.hh
 *
 * Declaration of BEMValidator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
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
