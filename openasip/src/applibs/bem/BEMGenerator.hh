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
 * @file BEMGenerator.hh
 *
 * Declaration of BEMGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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
class InstructionFormat;

namespace TTAMachine {
    class Machine;
    class Socket;
    class Bus;
    class Bridge;
    class BaseRegisterFile;
    class OperationTriggeredFormat;
}

/**
 * Generates a binary encoding map for the given machine.
 */
class BEMGenerator {
public:
    BEMGenerator(const TTAMachine::Machine& machine);
    BEMGenerator(const TTAMachine::Machine& machine, bool coproInterF);
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
    void funcencodeRiscv(
        TTAMachine::OperationTriggeredFormat* format,
        InstructionFormat* instrFormat, const unsigned& custom_op,
        unsigned& amountOfRCustomOps, unsigned& rocc_f3) const;

    void addRiscvFormat(TTAMachine::OperationTriggeredFormat* format,
    BinaryEncoding& bem, unsigned& amountOfRCustomOps,
    unsigned& amountOfR3RCustomOps) const;

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
    // ROCC interface selector for the Coprocessor Generation
    bool rocc_;
};

#endif
