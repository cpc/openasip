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
