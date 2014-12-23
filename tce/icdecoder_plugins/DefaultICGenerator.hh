/*
    Copyright (c) 2002-2011 Tampere University of Technology.

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
 * @file DefaultICGenerator.hh
 *
 * Declaration of DefaultICGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @author Pekka J‰‰skel‰inen 2011
 * @author Vinogradov Viacheslav(added Verilog generating) 2012
 * @note rating: red
 */

#ifndef TTA_DEFAULT_IC_GENERATOR_HH
#define TTA_DEFAULT_IC_GENERATOR_HH

#include <map>

#include "CentralizedControlICGenerator.hh"
#include "ProGeTypes.hh"
#include "HDBTypes.hh"
#include "Socket.hh"
#include "Machine.hh"

namespace TTAMachine {
    class Machine;
}

namespace ProGe {
    class Netlist;
    class NetlistBlock;
    class NetlistGenerator;
}

/**
 * This class generates interconnection network in VHDL/Verilog.
 * The network uses and-or buses.
 */
class DefaultICGenerator : public CentralizedControlICGenerator {
public:
    DefaultICGenerator(const TTAMachine::Machine& machine);
    virtual ~DefaultICGenerator();
    
    void SetHDL(ProGe::HDL language);
    
    void addICToNetlist(
        const ProGe::NetlistGenerator& generator,
        ProGe::Netlist& netlist);
    void generateInterconnectionNetwork(const std::string& dstDirectory)
        throw (IOException);
    void verifyCompatibility() const
        throw (InvalidData);

    void setGenerateBusTrace(bool generate);
    void setBusTraceStartingCycle(unsigned int cycle);

    virtual int outputSocketCntrlPinForSegment(
        const TTAMachine::Socket& socket,
        const TTAMachine::Segment& segment) const
        throw (NotAvailable);

    virtual int outputSocketDataControlValue(
        const TTAMachine::Socket& socket,
        const TTAMachine::Port& port) const
        throw (NotAvailable);

    virtual int inputSocketControlValue(
        const TTAMachine::Socket& socket,
        const TTAMachine::Segment& segment) const
        throw (NotAvailable);

private:
    typedef std::map<const TTAMachine::Socket*, int> SocketSignalMap;
    typedef std::map<const TTAMachine::Bus*, SocketSignalMap*> 
    BusAltSignalMap;

    void writeInterconnectionNetwork(std::ostream& stream);

    void generateSockets(const std::string& dstDirectory) const
        throw (IOException);

    bool isGcuPort(const TTAMachine::Port* port) const;

    void generateSocket(
        TTAMachine::Socket::Direction direction,
        int portConns,
        int segmentConns,
        const std::string& dstDirectory) const
        throw (IOException);
    void generateInputSocket(
        int segmentConns,
        std::ofstream& stream) const;
    void generateInputSocketRuleForBus(
        int bus,
        int ind,
        std::ofstream& stream) const;
    void generateOutputSocket(
        int portConns,
        int segmentConns,
        std::ofstream& stream) const;
    void createSignalsForIC(std::ostream& stream);
    void declareSocketEntities(std::ostream& stream) const;
    static void writeOutputSocketComponentDeclaration(
        const ProGe::HDL language,
        int portConns,
        int segmentConns,
        int ind,
        std::ostream& stream);
    static void writeInputSocketComponentDeclaration(
        const ProGe::HDL language,
        int segmentConns,
        int ind,
        std::ostream& stream);
    void writeBusDumpCode(std::ostream& stream) const;

    static std::set<TTAMachine::Socket*> outputSockets(
        const TTAMachine::Bus& bus);

    static bool containsSimilarSocket(
        const std::set<TTAMachine::Socket*> set, 
        const TTAMachine::Socket& socket);
        
    static int inputSocketDataPortWidth(const TTAMachine::Socket& socket);
    static int outputSocketDataPortWidth(
        const TTAMachine::Socket& socket, 
        int port);
    static int maxOutputSocketDataPortWidth(
        const TTAMachine::Socket& socket);
    static int busControlWidth(
        TTAMachine::Socket::Direction direction,
        int busConns);
    static int dataControlWidth(
        TTAMachine::Socket::Direction direction,
        int portConns);
    static int simmPortWidth(const TTAMachine::Bus& bus);

    static std::string inputSocketDataPort(const std::string& socket);
    static std::string outputSocketDataPort(
        const std::string& socket, 
        int port);
    static std::string socketBusControlPort(const std::string& name);
    static std::string socketDataControlPort(const std::string& name);
    static std::string simmDataPort(const std::string& busName);
    static std::string simmControlPort(const std::string& busName);
    static std::string inputSocketBusPort(int bus);
    static std::string outputSocketBusPort(int bus);
    static std::string outputSocketDataPort(int port);

    static std::string busWidthGeneric(int bus);
    static std::string dataWidthGeneric(int port);

    static std::string simmSocket(const TTAMachine::Bus& bus);
    static std::string simmSignal(const TTAMachine::Bus& bus);

    static std::string busSignal(const TTAMachine::Bus& bus);
    std::string busAltSignal(
        const TTAMachine::Bus& bus, 
        const TTAMachine::Socket& socket);

    static HDB::Direction convertDirection(
        TTAMachine::Socket::Direction direction);

    static std::string socketFileName(
        const ProGe::HDL language,
        TTAMachine::Socket::Direction direction,
        int portConns,
        int segmentConns);
    std::string socketEntityName(
        TTAMachine::Socket::Direction direction,
        int portConns,
        int segmentConns) const;
    std::string inputSocketEntityName(int conns) const;
    std::string outputSocketEntityName(int busConns, int portConns) const;

    static std::string indentation(unsigned int level);

    /// The machine.
    const TTAMachine::Machine& machine_;
    /// The netlist block of IC.
    ProGe::NetlistBlock* icBlock_;
    /// Signal numbers for controlling sockets.
    BusAltSignalMap altSignalMap_;
    /// Tells whether to generate bus tracing code.
    bool generateBusTrace_;
    /// The starting cycle for bus tracing.
    unsigned int busTraceStartingCycle_;
    TCEString entityNameStr_;
    ProGe::HDL  language_;
};

#endif
