/**
 * @file BEMGenerator.hh
 *
 * Declaration of BEMGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BEM_GENERATOR_HH
#define TTA_BEM_GENERATOR_HH

#include <map>
#include <vector>

#include "Socket.hh"

class BinaryEncoding;
class MoveSlot;
class ImmediateControlField;
class DestinationField;
class SourceField;
class GuardField;
class SocketCodeTable;

namespace TTAMachine {
    class Machine;
    class Socket;
    class Bus;
    class Bridge;
    class BaseRegisterFile;
}

/**
 * Generates a binary encoding map for the given machine.
 */
class BEMGenerator {
public:
    BEMGenerator(const TTAMachine::Machine& machine);
    virtual ~BEMGenerator();

    BinaryEncoding* generate();

private:
    /// Map type for storing Socket - SocketCodeTable pairs.
    typedef std::map<const TTAMachine::Socket*, SocketCodeTable*> SCTableMap;
    /// Typedef for encoding (first = encoding, second = extra bits).
    typedef std::pair<unsigned int, unsigned int> Encoding;

    void addLongImmDstRegisterFields(BinaryEncoding& bem) const;
    void addSocketCodeTables(BinaryEncoding& bem);
    void addTopLevelFields(BinaryEncoding& bem) const;
    void addSubfields(MoveSlot& slot) const;
    void addEncodings(ImmediateControlField& field) const;
    void addEncodings(DestinationField& field) const;
    void addEncodings(SourceField& field) const;
    void addEncodings(GuardField& field) const;
    void addPortCodes(
        SocketCodeTable& table, 
        const TTAMachine::Socket& socket) const;
    SocketCodeTable* socketCodeTable(const TTAMachine::Socket& socket) const;
    SocketCodeTable* suitableSocketCodeTable(
        const TTAMachine::Socket& socket) const;
    void assignSocketCodeTable(
        const TTAMachine::Socket* socket, 
        SocketCodeTable* table);
    std::multiset<int> socketCodeWidthsForBus(
        const TTAMachine::Bus& bus,
        TTAMachine::Socket::Direction socketDir) const;
    unsigned int maxLongImmSlotWidth(const MoveSlot& slot) const;

    static int socketCount(
        const TTAMachine::Bus& bus, 
        TTAMachine::Socket::Direction direction);
    static TTAMachine::Socket& socket(
        int index,
        const TTAMachine::Bus& bus,
        TTAMachine::Socket::Direction direction);
    static bool needsSocketCodeTable(const TTAMachine::Socket& socket);
    static bool haveEqualConnections(
        const TTAMachine::Socket& socket1,
        const TTAMachine::Socket& socket2);
    static int sourceBridgeCount(const TTAMachine::Bus& bus);
    static TTAMachine::Bridge& sourceBridge(
        int index, 
        const TTAMachine::Bus& bus);
    static bool hasUnconditionalGuard(const TTAMachine::Bus& bus);
    static int requiredIndexWidth(
        const TTAMachine::BaseRegisterFile& regFile);
    static void calculateEncodings(
        const std::multiset<int>& oppositeFieldWidths,
        bool leftAlignment,
        std::multiset<Encoding>& encodings);
    static void addExtraBits(
        std::multiset<Encoding>& encodings,
        int bitCount);

    /// The machine for which the BEM is generated.
    const TTAMachine::Machine* machine_;
    /// A map which tells which socket code table belongs to a socket.
    SCTableMap scTableMap_;
};

#endif
