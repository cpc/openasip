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
 * @file DefaultICGenerator.cc
 *
 * Implementation of DefaultICGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2008 (otto.esko-no.spam-tut.fi)
 * @author Pekka J��skel�inen 2011
 * @author Vinogradov Viacheslav(added Verilog generating) 2012
 * @author Henry Linjam�ki (henry.linjamaki-no.spam-tut.fi) 2014
 * @note rating: red
 */

#include <string>
#include <fstream>
#include <iostream>

#include "DefaultICGenerator.hh"
#include "HDBTypes.hh"

#include "NetlistBlock.hh"
#include "Netlist.hh"
#include "NetlistPort.hh"
#include "NetlistGenerator.hh"
#include "VHDLNetlistWriter.hh"
#include "VerilogNetlistWriter.hh"

#include "Machine.hh"
#include "Socket.hh"
#include "Segment.hh"
#include "ControlUnit.hh"
#include "SpecialRegisterPort.hh"

#include "Conversion.hh"
#include "FileSystem.hh"
#include "MathTools.hh"
#include "MapTools.hh"
#include "AssocTools.hh"

using namespace ProGe;
using namespace TTAMachine;
using std::string;
using std::endl;

const string INPUT_SOCKET_DATAW_GENERIC = "DATAW";
const string INPUT_SOCKET_DATA_PORT = "data";
const string SOCKET_BUS_CONTROL_PORT = "databus_cntrl";
const string SOCKET_DATA_CONTROL_PORT = "data_cntrl";

/**
 * The constructor.
 *
 * Generates the inteconnection network of the given machine.
 *
 * @param machine The machine.
 */
DefaultICGenerator::DefaultICGenerator(const TTAMachine::Machine& machine) :
    machine_(machine), icBlock_(NULL), generateBusTrace_(false),
    generateDebugger_(false), busTraceStartingCycle_(0) {
}


/**
 * The destructor.
 */
DefaultICGenerator::~DefaultICGenerator() {
    for (BusAltSignalMap::iterator iter = altSignalMap_.begin(); 
         iter != altSignalMap_.end(); iter++) {
        delete (*iter).second;
    }
}

/**
 *  Set HDL
 *  @param language
 */
void
DefaultICGenerator::SetHDL(ProGe::HDL language){
    language_= language;
} 
/**
 * Adds the interconnection network block to the given netlist and connects
 * it to the FUs, RFs and IUs.
 *
 * @param generator The netlist generator which generated the netlist.
 * @param netlist The netlist.
 */
void
DefaultICGenerator::addICToNetlist(
    const ProGe::NetlistGenerator& generator,
    ProGe::Netlist& netlist) {

    entityNameStr_ = netlist.topLevelBlock().moduleName();
    NetlistBlock* icBlock = 
        new NetlistBlock(
            entityNameStr_ + "_" + "interconn", "ic", netlist);
    icBlock_ = icBlock;
    netlist.topLevelBlock().addSubBlock(icBlock);
    ControlUnit* gcu = machine_.controlUnit();

    // Add clock, reset port and glock port
    NetlistPort* tlClkPort =
        netlist.topLevelBlock()
        .portByName(NetlistGenerator::DECODER_CLOCK_PORT);
    NetlistPort* icClkPort = new NetlistPort(
        NetlistGenerator::DECODER_CLOCK_PORT, "1", BIT, HDB::IN, *icBlock);
    netlist.connectPorts(*icClkPort, *tlClkPort);

    NetlistPort* tlResetPort =
        netlist.topLevelBlock()
        .portByName(NetlistGenerator::DECODER_RESET_PORT);
    NetlistPort* icResetPort = new NetlistPort(
        NetlistGenerator::DECODER_RESET_PORT, "1", BIT, HDB::IN, *icBlock);
    netlist.connectPorts(*icResetPort, *tlResetPort);

    NetlistPort* icGlockPort = new NetlistPort(
        "glock", "1", BIT, HDB::IN, *icBlock);
    setGlockPort(*icGlockPort);

    // add data ports and control ports for sockets
    Machine::SocketNavigator socketNav = machine_.socketNavigator();
    for (int i = 0; i < socketNav.count(); i++) {
        Socket* socket = socketNav.item(i);
        if (socket->segmentCount() == 0 || socket->portCount() == 0) {
            continue;
        }
           
        HDB::Direction socketDirection = convertDirection(
            socket->direction());

        // add the data port(s)
        if (socket->direction() == Socket::INPUT) {
            int width = inputSocketDataPortWidth(*socket);
            assert(width > 0);
            NetlistPort* socketDataPort = NULL;
            
            // connect the data port to the units
            for (int i = 0; i < socket->portCount(); i++) {
                Port* port = socket->port(i);
                NetlistPort* unitPort;
                
                // gcu ports must use IMEMADDRWIDTH as width
                if (isGcuPort(port)) {
                    socket->setDataPortWidth("IMEMADDRWIDTH");
                    socketDataPort = new NetlistPort(
                        this->inputSocketDataPort(socket->name()),
                        "IMEMADDRWIDTH", ProGe::BIT_VECTOR,
                        socketDirection, *icBlock);
                } else {
                    socketDataPort = new NetlistPort(
                        this->inputSocketDataPort(socket->name()), 
                        Conversion::toString(width), width, ProGe::BIT_VECTOR,
                        socketDirection, *icBlock);
                }

                // RA port of GCU is special case
                if (gcu->hasReturnAddressPort() && 
                    port == gcu->returnAddressPort()) {
                    unitPort = &generator.gcuReturnAddressInPort();
                } else {
                    unitPort = &generator.netlistPort(*port);
                }
                netlist.connectPorts(*socketDataPort, *unitPort);
            }
        } else {
            for (int i = 0; i < socket->portCount(); i++) {
                int width = outputSocketDataPortWidth(*socket, i);
                assert(width > 0);
                NetlistPort* socketDataPort = NULL;
                
                // connect the data port to the unit
                Port* port = socket->port(i);
                NetlistPort* unitPort;
                // gcu ports are treated differently
                if (isGcuPort(port)) {
                    socket->setDataPortWidth("IMEMADDRWIDTH");
                    socketDataPort = new NetlistPort(
                        outputSocketDataPort(socket->name(), i),
                        "IMEMADDRWIDTH", ProGe::BIT_VECTOR,
                        socketDirection, *icBlock);
                } else {
                    socketDataPort = new NetlistPort(
                    outputSocketDataPort(socket->name(), i), 
                    Conversion::toString(width), width, ProGe::BIT_VECTOR,
                    socketDirection, *icBlock);
                }

                // RA port of GCU is special case
                if (gcu->hasReturnAddressPort() &&
                    port == gcu->returnAddressPort()) {
                    unitPort = &generator.gcuReturnAddressOutPort();
                } else {
                    unitPort = &generator.netlistPort(*port);
                }
                netlist.connectPorts(*socketDataPort, *unitPort);
            }
        }
        
        // add control ports
        if (busControlWidth(
                socket->direction(), socket->segmentCount()) > 0) {
            int cntrlWidth = busControlWidth(
                socket->direction(), socket->segmentCount());
            assert(cntrlWidth > 0);
            NetlistPort* icSocketCntrlPort = new NetlistPort(
                socketBusControlPort(socket->name()), 
                Conversion::toString(cntrlWidth), cntrlWidth,
                ProGe::BIT_VECTOR, HDB::IN, *icBlock);
            mapBusCntrlPortOfSocket(socket->name(), *icSocketCntrlPort);
        }
        if (dataControlWidth(socket->direction(), socket->portCount()) > 0) {
            int cntrlWidth = dataControlWidth(
                socket->direction(), socket->portCount());
            assert(cntrlWidth > 0);
            NetlistPort* icSocketCntrlPort = new NetlistPort(
                socketDataControlPort(socket->name()), 
                Conversion::toString(cntrlWidth), cntrlWidth,
                ProGe::BIT_VECTOR, HDB::IN, *icBlock);
            mapDataCntrlPortOfSocket(socket->name(), *icSocketCntrlPort);
        }
    }   

    // add ports for short immediates to IC
    Machine::BusNavigator busNav = machine_.busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
        Bus* bus = busNav.item(i);
        if (bus->immediateWidth() > 0) {
            NetlistPort* icSimmPort = new NetlistPort(
                simmDataPort(bus->name()), 
                Conversion::toString(simmPortWidth(*bus)),
                simmPortWidth(*bus), ProGe::BIT_VECTOR, HDB::IN, *icBlock);
            mapSImmDataPort(bus->name(), *icSimmPort);
            NetlistPort* icSimmCntrlPort = new NetlistPort(
                simmControlPort(bus->name()), "1", 1, ProGe::BIT_VECTOR,
                HDB::IN, *icBlock);
            mapSImmCntrlPort(bus->name(), *icSimmCntrlPort);
        }
    }
}


/**
 * Generates the interconnection network to the given destination directory.
 *
 * @param dstDirectory The destination directory.
 * @exception IOException If an IO error occurs.
 */
void
DefaultICGenerator::generateInterconnectionNetwork(
    const std::string& dstDirectory)
    throw (IOException) {

    generateSockets(dstDirectory);
    
    // generate interconnection network
    string icFile = dstDirectory + FileSystem::DIRECTORY_SEPARATOR + 
        ((language_==Verilog)?"ic.v":"ic.vhdl");
    bool icCreated = FileSystem::createFile(icFile);
    if (!icCreated) {
        string errorMsg = "Unable to create file " + icFile;
        throw IOException(__FILE__, __LINE__, __func__, errorMsg);
    }
    std::ofstream icStream(icFile.c_str(), std::ofstream::out);
    writeInterconnectionNetwork(icStream);
    icStream.close();
}


/**
 * Verifies that the IC generator is compatible with the machine.
 *
 * @exception InvalidData If the generator is incompatible.
 */
void
DefaultICGenerator::verifyCompatibility() const
    throw (InvalidData) {

    // check that the machine does not use segments or bridges
    Machine::BusNavigator busNav = machine_.busNavigator();
    for (int i = 0; i < busNav.count(); i++) {
        Bus* bus = busNav.item(i);
        if (bus->segmentCount() > 1) {
            throw InvalidData(
                __FILE__, __LINE__, __func__,
                "IC generator does not support segmented buses.");
        }
    }

    Machine::BridgeNavigator bridgeNav = machine_.bridgeNavigator();
    if (bridgeNav.count() > 0) {
        throw InvalidData(
            __FILE__, __LINE__, __func__,
            "IC generator does not support bridges.");
    }
}

/**
 * Enables or disables generating debugger code.
 *
 * @param generate Tells whether to generate the debugger code.
 */
void
DefaultICGenerator::setGenerateDebugger(bool generate) {
    generateDebugger_ = generate;
}

/**
 * Enables or disables generating bus trace code.
 *
 * @param generate Tells whether to generate the bus tracing code.
 */
void
DefaultICGenerator::setGenerateBusTrace(bool generate) {
    generateBusTrace_ = generate;
}


/**
 * Sets the starting cycle to be generated to the bus trace.
 *
 * @param cycle The cycle.
 */
void
DefaultICGenerator::setBusTraceStartingCycle(unsigned int cycle) {
    busTraceStartingCycle_ = cycle;
}


/**
 * Returns the pin of the socket control port that control the given segment
 * connection.
 *
 * @param socket The socket.
 * @param segment The segment.
 * @return The pin of the control port.
 * @exception NotAvailable If the socket is not output socket or if it is not
 *                         connected to the given segment.
 */
int
DefaultICGenerator::outputSocketCntrlPinForSegment(
    const TTAMachine::Socket& socket,
    const TTAMachine::Segment& segment) const
    throw (NotAvailable) {

    if (socket.direction() != Socket::OUTPUT) {
        throw NotAvailable(__FILE__, __LINE__, __func__);
    }

    for (int i = 0; i < socket.segmentCount(); i++) {
        Segment* seg = socket.segment(i);
        if (seg == &segment) {
            return i;
        }
    }

    throw NotAvailable(__FILE__, __LINE__, __func__);
}


/**
 * Generates sockets needed in the machine to the given directory.
 *
 * @param dstDirectory The destination directory.
 * @exception IOException If an IO error occurs.
 */
void
DefaultICGenerator::generateSockets(const std::string& dstDirectory) const 
    throw (IOException) {

    bool needSimmSocket = true;
    std::set<Socket*> generatedSockets;
    Machine::SocketNavigator socketNav = machine_.socketNavigator();
    for (int i = 0; i < socketNav.count(); i++) {
        Socket* socket = socketNav.item(i);
        if (socket->portCount() > 0 && socket->segmentCount() > 0 &&
            !containsSimilarSocket(generatedSockets, *socket)) {
            generateSocket(
                socket->direction(), socket->portCount(),
                socket->segmentCount(), dstDirectory);
            generatedSockets.insert(socket);

            if (socket->portCount() == 1 && socket->segmentCount() == 1
                && socket->direction() == Socket::OUTPUT) {
                needSimmSocket = false;
            }
        }
    }

    // generate short immediate sockets if needed
    if (needSimmSocket) {
        Machine::BusNavigator busNav = machine_.busNavigator();
        for (int i = 0; i < busNav.count(); i++) {
            Bus* bus = busNav.item(i);
            if (bus->immediateWidth() > 0) {
                generateSocket(Socket::OUTPUT, 1, 1, dstDirectory);
            }
        }
    }   
}

/**
 * Tests if the given port belongs to GCU
 *
 * @param port The port to be tested
 */
bool 
DefaultICGenerator::isGcuPort(const Port* port) const {
    ControlUnit* gcu = machine_.controlUnit();
    for (int i = 0; i < gcu->portCount(); i++) {
        TTAMachine::Port* gcuPort = gcu->port(i);
        if (gcuPort == port) {
            return true;
        }
    }
    return false;
}

/**
 * Generates the given socket to a VHDL file in the given directory.
 *
 * @param direction Direction of the socket.
 * @param portConns The number of port connections.
 * @param segmentConns The number of segment connections.
 * @param dstDirectory The destination directory.
 * @exception IOException If the file cannot be created.
 */
void
DefaultICGenerator::generateSocket(
    TTAMachine::Socket::Direction direction,
    int portConns,
    int segmentConns,
    const std::string& dstDirectory) const
    throw (IOException) {
    
    string fileName = socketFileName(language_,
            direction, portConns, segmentConns);
    string pathToFile = dstDirectory + FileSystem::DIRECTORY_SEPARATOR +
        fileName;
    bool created = FileSystem::createFile(pathToFile);
    if (!created) {
        string errorMsg = "Unable to create file " + pathToFile;
        throw IOException(__FILE__, __LINE__, __func__, errorMsg);
    }
    
    std::ofstream stream(pathToFile.c_str(), std::ofstream::out);
    if (language_ == VHDL) {
        stream << "library IEEE;" << endl;
        stream << "use IEEE.std_logic_1164.all;" << endl;
        stream << "use IEEE.std_logic_arith.all;" << endl;
        if (direction == Socket::OUTPUT) {
            stream << "use work.tce_util.all;" << endl;
        }
    } else {
        //nothing for verilog
    }
    stream << endl;
    if (direction == Socket::INPUT) {
        generateInputSocket(segmentConns, stream);
    } else if (direction == Socket::OUTPUT) {
        generateOutputSocket(portConns, segmentConns, stream);
    } else {
        assert(false);
    }
    stream.close();
}

/**
 * Generates the given input socket to the given stream.
 *
 * @param segmentConns The number of segment connections.
 * @param stream The stream.
 */
void
DefaultICGenerator::generateInputSocket(
    int segmentConns,
    std::ofstream& stream) const {
    
    assert(segmentConns > 0);
    if (language_ == VHDL) {
        string entityName = socketEntityName(Socket::INPUT, 1, segmentConns);
        stream << "entity " << entityName << " is" << endl << endl;
        writeInputSocketComponentDeclaration(VHDL,segmentConns, 1, stream);
        stream << endl << "end " << entityName << ";" << endl << endl;
        
        stream << "architecture input_socket of " << entityName << " is"
               << endl;
        stream << "begin" << endl << endl;
        stream << indentation(2) 
               << "-- If width of input bus is greater than width of output,"
               << endl;
        stream << indentation(2) << "-- using the LSB bits." << endl;
        stream << indentation(2) 
               << "-- If width of input bus is smaller than width of output,"
               << endl;
        stream << indentation(2) 
               << "-- using zero extension to generate extra bits." << endl;
        stream << endl;
        
        if (segmentConns > 1) {
            stream << indentation(1) << "sel : process (" 
                   << SOCKET_BUS_CONTROL_PORT << ", ";
            
            for (int i = 0; i < segmentConns; i++) {
                stream << inputSocketBusPort(i);
                if (i + 1 < segmentConns) {
                    stream << ", ";
                } else {
                    stream << ")" << endl;
                }
            }
            
            stream << indentation(1) << "begin" << endl;
            stream << indentation(2) << "case " << SOCKET_BUS_CONTROL_PORT 
                   << " is" << endl;
            
            for (int i = 0; i < segmentConns; i++) {
                if (i+1 < segmentConns) {
                    stream << indentation(3) << "when \""
                           << Conversion::toBinary(
                               i, busControlWidth(Socket::INPUT, segmentConns))
                           << "\" =>" << endl;
                } else {
                    stream << indentation(3) << "when others =>" << endl;
                }
                generateInputSocketRuleForBus(i, 4, stream);
            }
            
            stream << indentation(2) << "end case;" << endl;
            stream << indentation(1) << "end process sel;" << endl;
            
        } else {
            stream << indentation(1) << "process (" << inputSocketBusPort(0)
                   << ")" << endl;
            stream << indentation(1) << "begin" << endl;
            generateInputSocketRuleForBus(0, 2, stream);
            stream << indentation(1) << "end process;" << endl;
        }
        
        stream << "end input_socket;" << endl;
    } else {
        string entityName = socketEntityName(Socket::INPUT, 1, segmentConns);
        stream << "`timescale 10ns/1ns" << endl
               << "module " << entityName << "" << endl << endl;
        writeInputSocketComponentDeclaration(Verilog,segmentConns, 1, stream);
        stream << indentation(2) 
               << "// If width of input bus is greater than width of output,"
               << endl
               << indentation(2) << "// using the LSB bits." << endl
               << indentation(2) 
               << "// If width of input bus is smaller than width of output,"
               << endl
               << indentation(2) 
               << "// using zero extension to generate extra bits." << endl
               << endl;
        if (segmentConns > 1) {
            stream << indentation(1) << "always@(" 
                   << SOCKET_BUS_CONTROL_PORT << ", ";
            
            for (int i = 0; i < segmentConns; i++) {
                stream << inputSocketBusPort(i);
                if (i + 1 < segmentConns) {
                    stream << ", ";
                } else {
                    stream << ")" << endl;
                }
            }
            stream << indentation(1) << "begin" << endl
                   << indentation(2) << "case(" << SOCKET_BUS_CONTROL_PORT 
                   << ")" << endl;
            for (int i = 0; i < segmentConns; i++) {
                if (i+1 < segmentConns) {
                    stream << indentation(3)
                           << i << " :" << endl;
                } else {
                    stream << indentation(3) << "default:" << endl;
                }
                generateInputSocketRuleForBus(i, 4, stream);
            }
            stream << indentation(2) << "endcase" << endl
                   << indentation(1) << "end" << endl;
        } else {
            stream << indentation(1) << "always@(" << inputSocketBusPort(0)
                   << ")" << endl
                   << indentation(1) << "begin" << endl;
            generateInputSocketRuleForBus(0, 2, stream);
            stream << indentation(1) << "end" << endl;
        }
        stream << "endmodule" << endl;    
    }
}

    
/**
 * Generates rule in VHDL for an input socket for the given bus number.
 *
 * @param bus The bus.
 * @param ind Indentation level.
 * @param stream The stream to write.
 */
void
DefaultICGenerator::generateInputSocketRuleForBus(
    int bus, int ind, std::ofstream& stream) const {
    if (language_ == VHDL) {
        stream << indentation(ind) << INPUT_SOCKET_DATA_PORT
               << " <= ext(" << inputSocketBusPort(bus) << ", "
               << INPUT_SOCKET_DATA_PORT << "'length);" << endl;
    } else {
        stream << indentation(ind) << "if (" << busWidthGeneric(bus) << " < " 
               << INPUT_SOCKET_DATAW_GENERIC << ")" << endl
               << indentation(ind+1) << INPUT_SOCKET_DATA_PORT << " <= $unsigned(" 
               << inputSocketBusPort(bus) << ");"<< endl
               << indentation(ind) << "else if (" << busWidthGeneric(bus) 
               << " > " << INPUT_SOCKET_DATAW_GENERIC << ")" << endl
               << indentation(ind+1) << INPUT_SOCKET_DATA_PORT << " <= " 
               << inputSocketBusPort(bus) << "[" << INPUT_SOCKET_DATAW_GENERIC
               << "-1 : 0];" << endl
               << indentation(ind) << "else" << endl
               << indentation(ind+1) << INPUT_SOCKET_DATA_PORT << " <= " 
               << inputSocketBusPort(bus) << "[" << busWidthGeneric(bus) 
               << "-1 : 0];" << endl;
    }
} 


/**
 * Generates the output socket to the given stream.
 *
 * @param socket The socket.
 * @param stream The stream.
 */
void
DefaultICGenerator::generateOutputSocket(
    int portConns,
    int segmentConns,
    std::ofstream& stream) const {
    
    string entityName = socketEntityName(
        Socket::OUTPUT, portConns, segmentConns);
    if (language_ == VHDL) {
        stream << "entity " << entityName << " is" << endl;
        writeOutputSocketComponentDeclaration(VHDL,
            portConns, segmentConns, 1, stream);
        stream << "end " << entityName << ";" << endl << endl << endl;
        
        stream << "architecture output_socket_andor of " << entityName 
               << " is" << endl << endl;

        if (portConns > 1) {
            stream << indentation(1) << "constant data_widths : integer_array(" 
                   << portConns - 1 << " downto 0) := (";
            for (int i = 0; i < portConns; i++) {
                stream << dataWidthGeneric(i);
                if (i+1 < portConns) {
                    stream << ", ";
                }
            }
            stream << ");" << endl;
           
            for (int i = 0; i < segmentConns; i++) {
                stream << indentation(1) << "signal databus_" << i 
                       << "_temp : std_logic_vector(return_highest(data_widths, "
                       << portConns << ")-1 downto 0);" << endl;
            }
            
            stream << indentation(1) 
                   << "signal data : std_logic_vector(return_highest(data_widths, "
                   << portConns << ")-1 downto 0);" << endl;
        } else {
            for (int i = 0; i < segmentConns; i++) {
                stream << indentation(1) << "signal databus_" << i
                       << "_temp : std_logic_vector(" << dataWidthGeneric(0)
                       << "-1 downto 0);" << endl;
            }
            stream << indentation(1) << "signal data : std_logic_vector("
                   << dataWidthGeneric(0) << "-1 downto 0);" << endl;
        }
            
        stream << endl;
        stream << "begin -- output_socket_andor" << endl << endl;
            
        if (portConns > 1) {
            stream << indentation(1) << "data_sel : process(";
            for (int i = 0; i < portConns; i++) {
                stream << outputSocketDataPort(i) << ", ";
            }
            stream << SOCKET_DATA_CONTROL_PORT << ")" << endl;
            stream << indentation(1) << "begin  -- process data_sel" << endl;
            for (int i = 0; i < portConns; i++) {
                stream << indentation(2);
                if (i == 0) {
                    stream << "if conv_integer(unsigned(" 
                           << SOCKET_DATA_CONTROL_PORT << ")) = 0 then" 
                           << endl;
                } else if (i < portConns-1) {
                    stream << "elsif conv_integer(unsignd("
                           << SOCKET_DATA_CONTROL_PORT << ")) = " << i 
                           << endl;
                } else {
                    stream << "else" << endl;
                }
                stream << indentation(3) << "data <= sxt(" 
                       << outputSocketDataPort(i) << ", data'length);" 
                       << endl;
            }
            stream << indentation(2) << "end if;" << endl;
            stream << indentation(1) << "end process data_sel;" << endl 
                   << endl;
        } else {
            stream << indentation(1) << "data <= " << outputSocketDataPort(0)
                   << ";" << endl << endl;
        }

        stream << indentation(1) << "internal_signal : process(data, " 
               << SOCKET_BUS_CONTROL_PORT << ")" << endl; 
        stream << indentation(1) << "begin -- process internal_signal" 
               << endl;

        for (int i = 0; i < segmentConns; i++) {
            stream << indentation(2) << "databus_" << i 
                   << "_temp <= data and sxt(" << SOCKET_BUS_CONTROL_PORT 
                   << "(" << i << " downto " << i << "), data'length);" 
                   << endl;
        }

        stream << indentation(1) << "end process internal_signal;" << endl
               << endl;

        stream << indentation(1) << "output : process (";

        for (int i = 0; i < segmentConns; i++) {
            stream << "databus_" << i << "_temp";
            if (i+1 < segmentConns) {
                stream << ",";
            } else {
                stream << ")" << endl;
            }
        }
            
        stream << indentation(1) << "begin -- process output" << endl;
            
        for (int i = 0; i < segmentConns; i++) {
            stream << indentation(2) << outputSocketBusPort(i)
                   << " <= ext(databus_" << i << "_temp, "
                   << outputSocketBusPort(i) << "'length);" << endl;
        }
        stream << indentation(1) << "end process output;" << endl << endl;
        stream << "end output_socket_andor;" << endl;
    } else { // language_ == Verilog
        stream << "`timescale 10ns/1ns" << endl
               << "module " << entityName << endl;
        writeOutputSocketComponentDeclaration(Verilog,
            portConns, segmentConns, 1, stream);
        
        //for avoiding absence of length attribute in verilog 2001        
        std::string data_length = dataWidthGeneric(0);
        
        stream << "//architecture output_socket_andor of " << entityName 
               << endl << endl;
        if (portConns > 1) {
#if 1
            assert(false &&
                   "Case portConns > 1 not supported by Verilog backend!");
#else
//!!!!!!!!!!!!!!!!!!!!!!!!!not finished here!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            // stream << indentation(1) << "constant data_widths : integer_array("
                   // << portConns - 1 << " : 0] := (";
            // for (int i = 0; i < portConns; i++) {
                // stream << dataWidthGeneric(i);
                // if (i+1 < portConns) {
                    // stream << ", ";
                // }
            // }
            // stream << ");" << endl;
           
            // for (int i = 0; i < segmentConns; i++) {
                // stream << indentation(1) << "signal databus_" << i 
                       // << "_temp : std_logic_vector(return_highest(data_widths, "
                       // << portConns << ")-1 downto 0);" << endl;
            // }
            
            // stream << indentation(1) 
                   // << "signal data : std_logic_vector(return_highest(data_widths, "
                   // << portConns << ")-1 downto 0);" << endl;
#endif
        } else {
            for (int i = 0; i < segmentConns; i++) {
                stream << indentation(1)
                       << "reg["<< dataWidthGeneric(0)
                       << "-1 : 0] "
                       <<" databus_" << i << "_temp;" << endl;
            }
            stream << indentation(1)
                   << "reg["<< dataWidthGeneric(0)
                   << "-1 : 0] "
                   << "data;" << endl;
        }
            
        stream << endl;

        if (portConns > 1) {
            stream << indentation(1) << "always@(";
            // for (int i = 0; i < portConns; i++) {
                // stream << outputSocketDataPort(i) << ", ";
            // }
            // stream << SOCKET_DATA_CONTROL_PORT << ")" << endl;
            stream << "*" << ")" << endl;
            stream << indentation(1) << "begin  // process data_sel" << endl;
            for (int i = 0; i < portConns; i++) {
                stream << indentation(2);
                if (i == 0) {
                    stream << "if(" 
                           << SOCKET_DATA_CONTROL_PORT << "== 0)" 
                           << endl;
                } else if (i < portConns-1) {
                    stream << "else if("
                           << SOCKET_DATA_CONTROL_PORT << "== " << i
                           << ")"
                           << endl;
                } else {
                    stream << "else" << endl;
                }
                stream << indentation(3) << "data = $signed(" 
                       << outputSocketDataPort(i) << ");" 
                       << endl;
            }
            stream << indentation(1) << "end //process data_sel;" << endl 
                   << endl;
        } else {
            stream << indentation(1)
                   << "always@(*)" << "data = " << outputSocketDataPort(0)
                   << ";" << endl << endl;
        }

        // stream << indentation(1) << "always@(data, "
               // << SOCKET_BUS_CONTROL_PORT << ")" << endl; 
        stream << indentation(1) << "always@(*)" << endl; 
        stream << indentation(1) << "begin //process internal_signal" 
               << endl;

        for (int i = 0; i < segmentConns; i++) {
            stream << indentation(2)
                   << "databus_"   << i << "_temp"
                   << "= data & {" << dataWidthGeneric(0) << "{"
                   << SOCKET_BUS_CONTROL_PORT 
                   << "[" << i << " : " << i << "]}};"
                   << endl;
        }

        stream << indentation(1) << "end //process internal_signal;" << endl
               << endl;

        stream << indentation(1) << "always@(*)" << endl;
        // stream << indentation(1) << "always@(";
        // for (int i = 0; i < segmentConns; i++) {
            // stream << "databus_" << i << "_temp";
            // if (i+1 < segmentConns) {
                // stream << ",";
            // } else {
                // stream << ")" << endl;
            // }
        // }
            
        stream << indentation(1) << "begin // process output" << endl;
            
        for (int i = 0; i < segmentConns; i++) {
            stream << indentation(2) << "if(" << busWidthGeneric(i) 
                   << " < " << data_length << ")" << endl;
            stream << indentation(3) << outputSocketBusPort(i) 
                   << " = databus_" << i << "_temp[" << busWidthGeneric(i)
                   << "-1 : 0];" << endl;
            stream << indentation(2) << "else" << endl;
            stream << indentation(3) << outputSocketBusPort(i) 
                   << " = $unsigned(databus_" << i << "_temp);" 
                   << endl;
        }
        stream << indentation(1) << "end //process output;" << endl << endl;
        stream << "endmodule //output_socket_andor;" << endl;    
    }
}

/**
 * Writes the bus dump lines required by the HW debugger.
 *
 * @param stream The stream.
 */
void
DefaultICGenerator::writeDebuggerCode(std::ostream& stream) const {
    stream << indentation(1) << "db_bustraces <= " << endl;

    Machine::BusNavigator busNav = machine_.busNavigator();

    for (int i = 0; i < busNav.count(); i++) {
        if (i % 4 == 0) {
            stream << indentation(2);
        }

        // Reverse order
        int idx = busNav.count() - 1 - i;
        int busWidth = busNav.item(idx)->width();

        if (busWidth < 32) { // Pad short busses with zeroes
            stream << "\"" << string(32 - busWidth, '0') << "\"&";
        }

        stream << busSignal(*busNav.item(idx));

        if (busWidth > 32) { // Truncate large busses
            stream << "(31 downto 0)";
        }

        if (i != busNav.count() - 1) {
            stream << " & ";

            if (i % 4 == 3) {
                stream << endl;
            }
        } else {
            stream << ";";
        }

        
    }
    stream << endl;
}

/**
 * Writes the interconnection network to the given stream.
 *
 * @param stream The stream.
 */
void
DefaultICGenerator::writeInterconnectionNetwork(std::ostream& stream) {

    if (language_ == VHDL) {
        stream << "library IEEE;" << endl;
        stream << "use IEEE.std_logic_1164.all;" << endl;
        stream << "use IEEE.std_logic_arith.all;" << endl;
        stream << "use STD.textio.all;" << endl;
        stream << "use work." << entityNameStr_ << "_globals.all;" << endl
               << endl;
            
        string entityName = entityNameStr_ + "_interconn";
        stream << "entity " << entityName << " is" << endl << endl;

        if (generateDebugger_
            && (icBlock_->portByName("db_bustraces") == NULL)) {
            //Assume that all buses are 32-bit
            Machine::BusNavigator busNav = machine_.busNavigator();
            int bustrace_width = 32*busNav.count();

            new NetlistPort(
                "db_bustraces", "32*BUSCOUNT",
                bustrace_width, ProGe::BIT_VECTOR, HDB::OUT, *icBlock_);
        }

        VHDLNetlistWriter::writePortDeclaration(
            *icBlock_, 1, indentation(1), stream);

        stream << endl << "end " << entityName << ";" << endl << endl;

        // create architecture
        stream << "architecture comb_andor of " << entityName << " is" << endl
               << endl;
        createSignalsForIC(stream);
        stream << endl;
        declareSocketEntities(stream);
        stream << endl;
        stream << "begin -- comb_andor" << endl << endl;

        if (generateBusTrace_) {
            writeBusDumpCode(stream);
            stream << endl;
        }

        if (generateDebugger_) {
            writeDebuggerCode(stream);
            stream << endl;
        }
            
        // Sort sockets by name to get deterministic order in HDL.
        Machine::SocketNavigator socketNav = machine_.socketNavigator();
        std::set<Socket*, Component::ComponentNameComparator> socketsToWrite;
        for (int i = 0; i < socketNav.count(); i++) {
            socketsToWrite.insert(socketNav.item(i));
        }
        for (std::set<Socket*,
                Component::ComponentNameComparator>::const_iterator iter =
                socketsToWrite.begin();
                iter != socketsToWrite.end(); iter++) {
            Socket* socket = *iter;
            int segmentCount = socket->segmentCount();
            if (segmentCount == 0 || socket->portCount() == 0) {
                continue;
            }
            stream << indentation(1) << socket->name() << " : " 
                   << socketEntityName(
                       socket->direction(), socket->portCount(),
                       socket->segmentCount())
                   << endl;
            stream << indentation(2) << "generic map (" << endl;
            for (int i = 0; i < segmentCount; i++) {
                int actualGenericWidth = 0;
                if (socket->direction() == Socket::OUTPUT) {
                    actualGenericWidth = maxOutputSocketDataPortWidth(*socket);
                } else if (socket->direction() == Socket::INPUT) {
                    actualGenericWidth =
                        socket->segment(i)->parentBus()->width();
                }
                stream << indentation(3) << busWidthGeneric(i) << " => " 
                       << actualGenericWidth << "," << endl;
            }
            if (socket->direction() == Socket::OUTPUT) {
                for (int i = 0; i < socket->portCount(); i++) {
                    string socketWidth = "";
                    if (socket->hasDataPortWidth()) {
                        socketWidth = socket->dataPortWidth();
                    } else {
                        socketWidth =
                            Conversion::toString(socket->port(i)->width());
                    }
                    stream << indentation(3) << dataWidthGeneric(i) 
                           << " => " << socketWidth;
                    if (i+1 == socket->portCount()) {
                        stream << ")" << endl;
                    } else {
                        stream << "," << endl;
                    }
                }
            } else { // socket->direction() == Socket::INPUT
                string socketWidth;
                if (socket->hasDataPortWidth()) {
                    socketWidth = socket->dataPortWidth();
                } else {
                    socketWidth = Conversion::toString(
                        inputSocketDataPortWidth(*socket));
                }
                stream << indentation(3) << INPUT_SOCKET_DATAW_GENERIC 
                       << " => " << socketWidth << ")"
                       << endl;
            }
            stream << indentation(2) << "port map (" << endl;
            for (int i = 0; i < segmentCount; i++) {
                Bus* bus = socket->segment(i)->parentBus();
                stream << indentation(3);
                if (socket->direction() == Socket::INPUT) {
                    stream << inputSocketBusPort(i) << " => " 
                           << busSignal(*bus) << "," << endl;
                } else {
                    stream << outputSocketBusPort(i) << " => "
                           << busAltSignal(*bus, *socket) << "," << endl;
                }
            }

            if (socket->direction() == Socket::OUTPUT) {
                for (int i = 0; i < socket->portCount(); i++) {
                    stream << indentation(3) << outputSocketDataPort(i) 
                           << " => " 
                           << outputSocketDataPort(socket->name(), i);
                    if (i+1 < socket->portCount()) {
                        stream << "," << endl;
                    }
                }
            } else {                
                stream << indentation(3) << INPUT_SOCKET_DATA_PORT << " => " 
                       << inputSocketDataPort(socket->name());
            }
            if (busControlWidth(
                    socket->direction(), socket->segmentCount()) > 0) {
                stream << "," << endl;
                stream << indentation(3) << SOCKET_BUS_CONTROL_PORT << " => "
                       << socketBusControlPort(socket->name());
            }
            if (dataControlWidth(socket->direction(), socket->portCount()) > 0) {
                stream << "," << endl;
                stream << indentation(3) << SOCKET_DATA_CONTROL_PORT << " => "
                       << socketDataControlPort(socket->name());
            }
            stream << ");" << endl << endl;
        }

        // add the sockets for short immediates
        Machine::BusNavigator busNav = machine_.busNavigator();
        for (int i = 0; i < busNav.count(); i++) {
            Bus* bus = busNav.item(i);
            if (bus->immediateWidth() > 0) {
                stream << indentation(1) << simmSocket(*bus) << " : "
                       << outputSocketEntityName(1, 1) << endl;
                stream << indentation(2) << "generic map (" << endl;
                stream << indentation(3) << busWidthGeneric(0) << " => " 
                       << simmPortWidth(*bus) << "," << endl;
                stream << indentation(3) << dataWidthGeneric(0) << " => "
                       << simmPortWidth(*bus) << ")" << endl;
                stream << indentation(2) << "port map (" << endl;
                stream << indentation(3) << outputSocketBusPort(0) << " => "
                       << simmSignal(*bus) << "," << endl;
                stream << indentation(3) << outputSocketDataPort(0) << " => " 
                       << simmDataPort(bus->name()) << "," << endl;
                stream << indentation(3) << SOCKET_BUS_CONTROL_PORT << " => "
                       << simmControlPort(bus->name()) << ");" << endl << endl;  
            }
        }

        // add assignments to data buses
        for (int i = 0; i < busNav.count(); i++) {
            Bus* bus = busNav.item(i);
            std::set<Socket*> outputSockets = this->outputSockets(*bus);
            // Skip bus assignment if no one can write into it.
            if (outputSockets.size() == 0 && bus->immediateWidth() == 0) {
                continue;
            }
            // Sort sockets by name to get deterministic HDL output.
            std::set<Socket*, Component::ComponentNameComparator> socketsToWrite;
            for (std::set<Socket*>::iterator iter = outputSockets.begin();
                    iter != outputSockets.end(); iter++) {
                socketsToWrite.insert(*iter);
            }

            stream << indentation(1) << busSignal(*bus) << " <= ";
            for (std::set<Socket*,
                    Component::ComponentNameComparator>::const_iterator iter =
                     socketsToWrite.begin(); iter != socketsToWrite.end();) {
                Socket* socket = *iter;
                stream << "ext(" << busAltSignal(*bus, *socket)
                       << ", " << busSignal(*bus) << "'length)";
                iter++;
                if (iter != socketsToWrite.end()) {
                    stream << " or ";
                }
            }
            if (bus->immediateWidth() > 0) {
                if (socketsToWrite.begin() != socketsToWrite.end()) {
                    stream << " or ";
                }
                if (bus->signExtends()) {
                    stream << "sxt(";
                } else if (bus->zeroExtends()) {
                    stream << "ext(";
                } else {
                    assert(false && "Unknown extension policy.");
                }
                stream << simmSignal(*bus)
                       << ", " << busSignal(*bus) << "'length)";
            }
            stream << ";" << endl;
        }
            
        stream << endl;
        stream << "end comb_andor;" << endl;
    } else { // language_ == Verilog
        const std::string DS = FileSystem::DIRECTORY_SEPARATOR;
        string entityName = entityNameStr_ + "_interconn";
        stream << "`timescale 1ns/1ns" << endl
               << "module " << entityName << endl
        //include parameters here
               << "#(" << endl
               << "`include \""
               << entityNameStr_ << "_globals_pkg.vh\"" << endl
               << ")" << endl;

        VerilogNetlistWriter::writePortDeclaration(
            *icBlock_, 1, indentation(1), stream);
        // create architecture

        createSignalsForIC(stream);
        stream << endl;

        if (generateBusTrace_) {
            writeBusDumpCode(stream);
            stream << endl;
        }
        
        // Sort sockets by name to get deterministic HDL output.    
        Machine::SocketNavigator socketNav = machine_.socketNavigator();
        std::set<Socket*, Component::ComponentNameComparator> socketsToWrite;
        for (int i = 0; i < socketNav.count(); i++) {
            socketsToWrite.insert(socketNav.item(i));
        }
        for (std::set<Socket*,
                Component::ComponentNameComparator>::const_iterator iter =
                socketsToWrite.begin(); iter != socketsToWrite.end(); iter++) {
            Socket* socket = *iter;
            int segmentCount = socket->segmentCount();
            if (segmentCount == 0 || socket->portCount() == 0) {
                continue;
            }
            stream << indentation(1)
                   << socketEntityName(
                       socket->direction(), socket->portCount(),
                       socket->segmentCount())
                   << endl;
            
            //parameters(generics)
            stream << indentation(2) << "#(" << endl;
            for (int i = 0; i < segmentCount; i++) {
                stream << indentation(3) << "." << busWidthGeneric(i) << "(" 
                       << socket->segment(i)->parentBus()->width() << ")," 
                       << endl;
            }
            
            if (socket->direction() == Socket::OUTPUT) {
                for (int i = 0; i < socket->portCount(); i++) {
                    string socketWidth = "";
                    if (socket->hasDataPortWidth()) {
                        socketWidth = socket->dataPortWidth();
                    } else {
                        socketWidth =
                            Conversion::toString(socket->port(i)->width());
                    }
                    stream << indentation(3) << "." << dataWidthGeneric(i) 
                           << "(" << socketWidth;
                    if (i+1 == socket->portCount()) {
                        stream << ")" << endl;
                    } else {
                        stream << ")," << endl;
                    }
                }
            } else {
                string socketWidth;
                if (socket->hasDataPortWidth()) {
                    socketWidth = socket->dataPortWidth();
                } else {
                    socketWidth =
                        Conversion::toString(inputSocketDataPortWidth(*socket));
                }
                stream << indentation(3) << "." << INPUT_SOCKET_DATAW_GENERIC 
                       << "(" << socketWidth << ")"
                       << endl;
            }
            stream << indentation(2) << ")" << endl
                   << indentation(2) << socket->name() << endl
                   << indentation(2) << "(" << endl;
            for (int i = 0; i < segmentCount; i++) {
                Bus* bus = socket->segment(i)->parentBus();
                stream << indentation(3);
                if (socket->direction() == Socket::INPUT) {
                    stream << "." << inputSocketBusPort(i) << "(" 
                           << busSignal(*bus) << ")," << endl;
                } else {
                    stream << "." << outputSocketBusPort(i) << "("
                           << busAltSignal(*bus, *socket) << ")," << endl;
                }
            }

            if (socket->direction() == Socket::OUTPUT) {
                for (int i = 0; i < socket->portCount(); i++) {
                    stream << indentation(3) << "." << outputSocketDataPort(i) 
                           << "(" 
                           << outputSocketDataPort(socket->name(), i);
                    if (i+1 < socket->portCount()) {
                        stream << ")," << endl;
                    }
                }
            } else {                
                stream << indentation(3) << "." << INPUT_SOCKET_DATA_PORT << "(" 
                       << inputSocketDataPort(socket->name());
            }
            if (busControlWidth(
                    socket->direction(), socket->segmentCount()) > 0) {
                stream << ")," << endl;
                stream << indentation(3) << "." << SOCKET_BUS_CONTROL_PORT << "("
                       << socketBusControlPort(socket->name());
            }
            if (dataControlWidth(socket->direction(), socket->portCount()) > 0) {
                stream << ")," << endl;
                stream << indentation(3) << "." << SOCKET_DATA_CONTROL_PORT << "("
                       << socketDataControlPort(socket->name());
            }
            stream << "));" << endl << endl;
        }

        // add the sockets for short immediates
        Machine::BusNavigator busNav = machine_.busNavigator();
        for (int i = 0; i < busNav.count(); i++) {
            Bus* bus = busNav.item(i);
            if (bus->immediateWidth() > 0) {
                stream << indentation(1)
                       << outputSocketEntityName(1, 1)
                       << " "
                       << endl
                       
                       << indentation(2) << "#(" << endl
                       << indentation(3) << "." << busWidthGeneric(0) << "("
                       << bus->width() << ")," << endl
                       << indentation(3) << "." << dataWidthGeneric(0) << "("
                       << simmPortWidth(*bus) << ")" << endl
                       << indentation(2) << ")" << endl
                       << indentation(2) << simmSocket(*bus) << endl
                       << indentation(2) << "(" << endl
                       << indentation(3) << "." << outputSocketBusPort(0) << "("
                       << simmSignal(*bus) << ")," << endl
                       << indentation(3) << "." << outputSocketDataPort(0) << "("
                       << simmDataPort(bus->name()) << ")," << endl
                       << indentation(3) << "." << SOCKET_BUS_CONTROL_PORT << "("
                       << simmControlPort(bus->name()) << "));" << endl << endl;
            }
        }

        // add assignments to data buses
        for (int i = 0; i < busNav.count(); i++) {
            Bus* bus = busNav.item(i);
            std::set<Socket*> outputSockets = this->outputSockets(*bus);
            stream << indentation(1) << "assign " << busSignal(*bus) << " = ";
            // Sort sockets by name to get deterministic HDL output.
            std::set<Socket*, Component::ComponentNameComparator> socketsToWrite;
            for (std::set<Socket*>::iterator iter = outputSockets.begin();
                    iter != outputSockets.end(); iter++) {
                socketsToWrite.insert(*iter);
            }

            for (std::set<Socket*,
                    Component::ComponentNameComparator>::const_iterator iter =
                     socketsToWrite.begin(); iter != socketsToWrite.end();) {
                Socket* socket = *iter;
                stream << busAltSignal(*bus, *socket);
                iter++;
                if (iter != socketsToWrite.end()) {
                    stream << " | ";
                }
            }
            if (bus->immediateWidth() > 0) {
                if (socketsToWrite.begin() != socketsToWrite.end()) {
                    stream << " | ";
                }
                stream << simmSignal(*bus);
            }
            stream << ";" << endl;
        }
        stream << endl;
        stream << "endmodule"<< endl;    
    }
}


/**
 * Writes the signal declarations of interconnection network to the
 * given stream.
 *
 * @param stream The stream.
 */
void
DefaultICGenerator::createSignalsForIC(std::ostream& stream) {
    Machine::BusNavigator busNav = machine_.busNavigator();
    if (language_ == VHDL){
        for (int i = 0; i < busNav.count(); i++) {
            Bus* bus = busNav.item(i);
            // create signal for the bus
            stream << indentation(1) << "signal " << busSignal(*bus) 
                   << " : std_logic_vector(" << bus->width() - 1 
                   << " downto 0);" << endl;
                
            // create a signal for all the output sockets connected to the bus
            // Sort alphabetically to get deterministic HDL output.
            std::set<Socket*> outputSockets = this->outputSockets(*bus);
            std::set<Socket*, Component::ComponentNameComparator> socketsToWrite;
            for (std::set<Socket*>::iterator iter = outputSockets.begin();
                    iter != outputSockets.end(); iter++) {
                socketsToWrite.insert(*iter);
            }

            for (std::set<Socket*, Component::ComponentNameComparator>::iterator
                    iter = socketsToWrite.begin(); iter != socketsToWrite.end();
                    iter++) {
                stream << indentation(1) << "signal " 
                       << busAltSignal(*bus, **iter) << " : std_logic_vector("
                       << maxOutputSocketDataPortWidth(**iter) - 1
                       << " downto 0);" << endl;
            }

            // create additional signal for short immediate
            if (bus->immediateWidth() > 0) {
                stream << indentation(1) << "signal " << simmSignal(*bus) 
                       << " : std_logic_vector(" << simmPortWidth(*bus) - 1
                       << " downto 0);" << endl;
            }
        }
    } else { // language_ == Verilog
        for (int i = 0; i < busNav.count(); i++) {
            Bus* bus = busNav.item(i);
            // create wire for the bus
            stream << indentation(1) << "wire[" << bus->width() - 1 << ":0] "
                   << busSignal(*bus) << ";"<< endl;
                
            // create a wires for all the output sockets connected to the bus
            // Sort alphabetically to get deterministic HDL output.
            std::set<Socket*> outputSockets = this->outputSockets(*bus);
            std::set<Socket*, Component::ComponentNameComparator> socketsToWrite;
            for (std::set<Socket*>::iterator iter = outputSockets.begin();
                    iter != outputSockets.end(); iter++) {
                socketsToWrite.insert(*iter);
            }

            for (std::set<Socket*, Component::ComponentNameComparator>::iterator
                    iter = socketsToWrite.begin(); iter != socketsToWrite.end();
                    iter++) {
                stream << indentation(1) << "wire[" << bus->width() - 1 <<":0] "
                       << busAltSignal(*bus, **iter)<<";"<< endl;
            }

            // create additional wire for short immediate
            if (bus->immediateWidth() > 0) {
                stream << indentation(1) << "wire[" << bus->width() - 1 << ":0] "
                       << simmSignal(*bus) << ";" << endl;
            }
        }    
    }
}


/**
 * Declares the socket entities used in the IC.
 *
 * @param stream The stream to write.
 */
void
DefaultICGenerator::declareSocketEntities(std::ostream& stream) const {

    std::set<string> declaredSockets;
    // Sort sockets by name to get deterministic HDL output.
    std::set<Socket*, Component::ComponentNameComparator> socketsToWrite;
    Machine::SocketNavigator socketNav = machine_.socketNavigator();
    for (int i = 0; i < socketNav.count(); i++) {
        socketsToWrite.insert(socketNav.item(i));
    }

    for (std::set<Socket*,
            Component::ComponentNameComparator>::const_iterator iter =
            socketsToWrite.begin(); iter != socketsToWrite.end(); iter++) {
        Socket* socket = *iter;
        if (socket->segmentCount() > 0 && socket->portCount() > 0 &&
            !AssocTools::containsKey(
                declaredSockets, socketEntityName(
                    socket->direction(), socket->portCount(),
                    socket->segmentCount()))) {
            string entityName = socketEntityName(
                socket->direction(), socket->portCount(),
                socket->segmentCount());
            declaredSockets.insert(entityName);
            stream << indentation(1) << "component " << entityName << endl;
            if (socket->direction() == Socket::INPUT) {
                writeInputSocketComponentDeclaration(VHDL,
                    socket->segmentCount(), 2, stream);
            } else {
                writeOutputSocketComponentDeclaration(VHDL,
                    socket->portCount(), socket->segmentCount(), 2, stream);
            }
            stream << indentation(1) << "end component;" << endl << endl;
        }
    }
    
    // adding declaration for a output_socket_1_1 needed by short immediates
    // if it didn't already exist
    string entityName = socketEntityName(Socket::OUTPUT, 1, 1);
    if (!AssocTools::containsKey(declaredSockets, entityName)) {
        stream << indentation(1) << "component " << entityName << endl;
        writeOutputSocketComponentDeclaration(VHDL,1, 1, 2, stream);
        stream << indentation(1) << "end component;" << endl << endl;
    }
}


/**
 * Writes the component interface declaration of the given output socket to
 * the given stream.
 *
 * @param portConns The number of port connections.
 * @param segmentConns The number of segment connections.
 * @param ind The indentation level.
 * @param stream The stream to write.
 */
void
DefaultICGenerator::writeOutputSocketComponentDeclaration(
    const ProGe::HDL language,
    int portConns,
    int segmentConns,
    int ind,
    std::ostream& stream) {

    if (language == VHDL) {
        stream << indentation(ind) << "generic (" << endl;
        
        for (int i = 0; i < segmentConns; i++) {
            stream << indentation(ind+1) << busWidthGeneric(i) 
                   << " : integer := 32;" << endl;
        }
        
        for (int i = 0; i < portConns; i++) {
            stream << indentation(ind+1) << dataWidthGeneric(i) 
                   << " : integer := 32";
            if (i+1 == portConns) {
                stream << ");" << endl;
            } else {
                stream << ";" << endl;
            }
        }
        
        stream << indentation(ind) << "port (" << endl;
        for (int i = 0; i < segmentConns; i++) {
            stream << indentation(ind+1) << outputSocketBusPort(i) 
                   << " : out std_logic_vector(" << busWidthGeneric(i) 
                   << "-1 downto 0);" << endl;
        }
        for (int i = 0; i < portConns; i++) {
            stream << indentation(ind+1) << outputSocketDataPort(i) 
                   << " : in std_logic_vector(" << dataWidthGeneric(i) 
                   << "-1 downto 0);" << endl;   
        }
        
        const int busControlWidth = segmentConns;
        stream << indentation(ind+1) << SOCKET_BUS_CONTROL_PORT 
               << " : in std_logic_vector(" << busControlWidth - 1 
               << " downto 0)";
        
        if (portConns > 1) {
            stream << ";" << endl;
            const int dataControlWidth = DefaultICGenerator::dataControlWidth(
                Socket::OUTPUT, portConns);
            stream << indentation(ind+1) << SOCKET_DATA_CONTROL_PORT 
                   << " : in std_logic_vector(" << dataControlWidth - 1 
                   << " downto 0));" << endl;
        } else {
            stream << ");" << endl;
        }
    } else {
        stream << indentation(ind) << "#( parameter " << endl;
        
        for (int i = 0; i < segmentConns; i++) {
            stream << indentation(ind+1) << busWidthGeneric(i) 
                   << " = 32," << endl;
        }
        
        for (int i = 0; i < portConns; i++) {
            stream << indentation(ind+1) << dataWidthGeneric(i) 
                   << " = 32";
            if (i+1 == portConns) {
                stream << ")" << endl;
            } else {
                stream << "," << endl;
            }
        }
        
        stream << indentation(ind) << "(" << endl;
        for (int i = 0; i < segmentConns; i++) {
            stream << indentation(ind+1)
                   << "output reg[" << busWidthGeneric(i) 
                   << "-1 : 0] "
                   << outputSocketBusPort(i)
                   << "," << endl;
        }
        for (int i = 0; i < portConns; i++) {
            stream << indentation(ind+1)
                   << "input[" << dataWidthGeneric(i) 
                   << "-1 : 0] " 
                   << outputSocketDataPort(i) 
                   << "," << endl;   
        }
        
        const int busControlWidth = segmentConns;
        stream << indentation(ind+1)
               << "input [" << busControlWidth - 1 
               << " : 0]"
               << SOCKET_BUS_CONTROL_PORT;
        
        if (portConns > 1) {
            stream << "," << endl;
            const int dataControlWidth = DefaultICGenerator::dataControlWidth(
                Socket::OUTPUT, portConns);
            stream << indentation(ind+1) << SOCKET_DATA_CONTROL_PORT 
                   << "input [" << dataControlWidth - 1 
                   << " : 0]);" << endl;
        } else {
            stream << ");" << endl;
        }    
    }
}


/**
 * Writes the component interface declaration of the given input socket to
 * the given stream.
 *
 * @param segmentConns The number of segment connections.
 * @param ind The indentation level.
 * @param stream The stream to write.
 */
void
DefaultICGenerator::writeInputSocketComponentDeclaration(
    const ProGe::HDL language,
    int segmentConns,
    int ind,
    std::ostream& stream) {
    
    if (language == VHDL) {
        stream << indentation(ind) << "generic (" << endl;
        
        for (int i = 0; i < segmentConns; i++) {
            stream << indentation(ind+1) << busWidthGeneric(i) << 
                " : integer := 32;" << endl;
        }
        
        stream << indentation(ind+1) << INPUT_SOCKET_DATAW_GENERIC 
               << " : integer := 32);" << endl;
        stream << indentation(ind) << "port (" << endl;
        
        for (int i = 0; i < segmentConns; i++) {
            stream << indentation(ind+1) << inputSocketBusPort(i) 
                   << " : in std_logic_vector(" << busWidthGeneric(i) 
                   << "-1 downto 0);" << endl;
        }
        
        stream << indentation(ind+1) << INPUT_SOCKET_DATA_PORT 
               << " : out std_logic_vector(" << INPUT_SOCKET_DATAW_GENERIC 
               << "-1 downto 0)";
        int controlWidth = MathTools::bitLength(segmentConns - 1);
        if (segmentConns > 1) {
            stream << ";" << endl;
            stream << indentation(ind+1) << SOCKET_BUS_CONTROL_PORT 
                   << " : in std_logic_vector(" << controlWidth - 1 
                   << " downto 0));" << endl;
        } else {
            stream << ");" << endl;
        }
    } else {
        stream << indentation(ind) << "#( parameter " << endl;
        
        for (int i = 0; i < segmentConns; i++) {
            stream << indentation(ind+1) << busWidthGeneric(i) << 
                " = 32," << endl;
        }
        
        stream << indentation(ind+1) << INPUT_SOCKET_DATAW_GENERIC 
               << " = 32)" << endl;
        stream << indentation(ind) << "(" << endl;
        
        for (int i = 0; i < segmentConns; i++) {
            stream << indentation(ind+1)
                   << "input[" << busWidthGeneric(i) 
                   << "-1 : 0] " << inputSocketBusPort(i) << ","
                   << endl;
        }
        
        stream << indentation(ind+1)
               << "output reg[" << INPUT_SOCKET_DATAW_GENERIC 
               << "-1 : 0] "
               << INPUT_SOCKET_DATA_PORT;
        int controlWidth = MathTools::bitLength(segmentConns - 1);
        if (segmentConns > 1) {
            stream << "," << endl;
            stream << indentation(ind+1)
                   << "input[" << controlWidth - 1 
                   << " : 0] " 
                   << SOCKET_BUS_CONTROL_PORT;
        }
        stream << ");" << endl;
    }
}


/**
 * Writes the code that dumps the bus contents to an output file.
 *
 * @param stream The stream to write.
 */
void
DefaultICGenerator::writeBusDumpCode(std::ostream& stream) const {
    if (language_ == VHDL) {
        stream << indentation(1)
               << "-- Dump the value on the buses into a file once in clock cycle"
               << endl;
        stream << indentation(1)
               << "-- setting DUMP false will disable dumping" << endl << endl;
        stream << indentation(1) << "-- Do not synthesize this process!" << endl;
        stream << indentation(1) << "-- pragma synthesis_off" << endl;

        stream << indentation(1) << "file_output : process" << endl << endl;
        stream << indentation(2)
               << "file regularfileout : text;" << endl;
        stream << indentation(2)
               << "file executionfileout : text;" << endl << endl;
        stream << indentation(2) << "variable lineout : line;" << endl;
        stream << indentation(2) << "variable start : boolean := true;" << endl;
        stream << indentation(2) << "variable cyclecount : integer := 0;"
               << endl;
        stream << indentation(2) << "variable executioncount : integer := 0;"
               << endl << endl;
        stream << indentation(2) << "constant SEPARATOR : string := \" | \";"
               << endl;
        stream << indentation(2) << "constant DUMP : boolean := true;" << endl;
        stream << indentation(2)
               << "constant REGULARDUMPFILE : string := \"bus.dump\";"
               << endl;
        stream << indentation(2)
               << "constant EXECUTIONDUMPFILE : string := \"execbus.dump\";"
               << endl << endl;

        stream << indentation(1) << "begin" << endl;
        stream << indentation(2) << "if DUMP = true then" << endl;
        stream << indentation(3) << "if start = true then" << endl;
        stream << indentation(4)
               << "file_open(regularfileout, REGULARDUMPFILE, write_mode);"
               << endl;
        stream << indentation(4)
               << "file_open(executionfileout, EXECUTIONDUMPFILE, write_mode);"
               << endl;
        stream << indentation(4) << "start := false;" << endl;
        stream << indentation(3) << "end if;" << endl << endl;
        
        stream << indentation(3) << "wait for PERIOD;" << endl;
        int ind = 3;
        if (busTraceStartingCycle_ > 0) {
            stream << indentation(3) << "if (cyclecount > "
                   << busTraceStartingCycle_ - 1
                   << ") then" << endl;
            ind++;
        }
        stream << indentation(ind) << "write(lineout, cyclecount-"
               << busTraceStartingCycle_ << ", right, 12);"
               << endl;
        stream << indentation(ind) << "write(lineout, SEPARATOR);" << endl;

        Machine::BusNavigator busNav = machine_.busNavigator();
        for (int i = 0; i < busNav.count(); i++) {
            stream << indentation(ind) << "write(lineout, conv_integer(signed(" 
                   << busSignal(*busNav.item(i)) << ")), right, 12);" << endl;
            stream << indentation(ind) << "write(lineout, SEPARATOR);" << endl;
        }
        
        stream << endl << indentation(ind)
               << "writeline(regularfileout, lineout);" << endl;

        stream << indentation(ind) << "if glock = '0' then" << endl;
        stream << indentation(ind+1) << "write(lineout, executioncount"
               << ", right, 12);"
               << endl;
        stream << indentation(ind+1) << "write(lineout, SEPARATOR);" << endl;

        for (int i = 0; i < busNav.count(); i++) {
            stream << indentation(ind+1)
                   << "write(lineout, conv_integer(signed("
                   << busSignal(*busNav.item(i)) << ")), right, 12);" << endl;
            stream << indentation(ind+1) << "write(lineout, SEPARATOR);"
                   << endl;
        }
        stream << endl << indentation(ind+1)
               << "writeline(executionfileout, lineout);" << endl;
        stream << indentation(ind+1) << "executioncount := executioncount + 1;"
               << endl;
        stream << indentation(ind) << "end if;" << endl;

        if (busTraceStartingCycle_ > 0) {
            stream << indentation(3) << "end if;" << endl;
        }
        stream << indentation(3) << "cyclecount := cyclecount + 1;" << endl;
        stream << indentation(2) << "end if;" << endl;
        stream << indentation(1) << "end process file_output;" << endl;
        stream << indentation(1) << "-- pragma synthesis_on" << endl;
    } else { // language_ == Verilog
        stream << indentation(1)
               << "// Dump the value on the buses into a file once in clock cycle"
               << endl
               << indentation(1)
               << "// setting DUMP false will disable dumping" << endl << endl
               << indentation(1) << "// Do not synthesize!" << endl
               << indentation(1) << "//synthesis translate_off" << endl
               << indentation(1) << "integer regularfileout;" << endl << endl
               << indentation(1) << "integer executionfileout;" << endl << endl
               << indentation(1) << "integer count=0;" << endl
               << indentation(1) << "integer executioncount=0;" << endl << endl
               << indentation(1) << "`define REGULARDUMPFILE \"bus.dump\""
               << indentation(1)
               << "`define EXECUTIONDUMPFILE \"execbus.dump\""
               << endl << endl

               << indentation(1) << "initial" << endl
               << indentation(1) << "begin" << endl
               << indentation(2)
               << "regularfileout = $fopen(`REGULARDUMPFILE,\"w\");" << endl
               << indentation(2) << "$fclose(regularfileout);" << endl
               << indentation(2)
               << "executionfileout = $fopen(`EXECUTIONDUMPFILE,\"w\");"
               << endl
               << indentation(2) << "$fclose(executionfileout);" << endl
               << indentation(2) << "forever" << endl
               << indentation(2) << "begin" << endl
               << indentation(3) << "#PERIOD;" << endl;
        if (busTraceStartingCycle_ > 0) {
            stream << indentation(3) << "if(count > "
                   << busTraceStartingCycle_ - 1
                   << ")" << endl;
        }
        std::string format_string = " %11d";
        std::string count_string = "count - " +
            Conversion::toString(busTraceStartingCycle_);
        std::string variable_list = "";


        Machine::BusNavigator busNav = machine_.busNavigator();
        for (int i = 0; i < busNav.count(); i++) {
            format_string += " |  %11d";
            variable_list += ", $signed(" +
                Conversion::toString(busSignal(*busNav.item(i)))+")";
        }
        
        stream << indentation(3) << "begin" << endl
               << indentation(4) << "regularfileout = "
                   "$fopen(`REGULARDUMPFILE,\"a\");" << endl
               << indentation(4) << "$fwrite(regularfileout,"
               << "\"" << format_string << " | \\n\"" << ", "
               << count_string << variable_list << ");" << endl
               << indentation(4) << "$fclose(regularfileout);" << endl
               << indentation(4) << "if(glock == 0)" << endl
               << indentation(4) << "begin" << endl
               << indentation(5) << "executionfileout = "
               "$fopen(`EXECUTIONDUMPFILE,\"a\");" << endl
               << indentation(5) << "$fwrite(executionfileout,"
               << "\"" << format_string << " | \\n\"" << ", executioncount"
               << variable_list << ");" << endl
               << indentation(5) << "$fclose(executionfileout);" << endl
               << indentation(5) << "executioncount = executioncount + 1;"
               << endl
               << indentation(4) << "end" << endl
               << indentation(3) << "end" << endl
               << indentation(3) << "count = count + 1;" << endl
               << indentation(2) << "end" << endl
               << indentation(1) << "end" << endl
               << indentation(1) << "//synthesis translate_on" << endl;    
    }
}   


/**
 * Returns a set of all the output sockets that are connected to the 
 * given bus.
 *
 * @param bus The bus.
 * @return The socket set.
 */
std::set<Socket*>
DefaultICGenerator::outputSockets(const TTAMachine::Bus& bus) {
    std::set<Socket*> outputSockets;
    for (int i = 0; i < bus.segmentCount(); i++) {
        Segment* segment = bus.segment(i);
        for (int i = 0; i < segment->connectionCount(); i++) {
            Socket* socket = segment->connection(i);
            if (socket->direction() == Socket::OUTPUT) {
                outputSockets.insert(socket);
            }
        }
    }
    return outputSockets;
}


/**
 * Tells whether the given socket set contains similar socket to the
 * given one.
 *
 * @param set The socket set.
 * @param socket The socket.
 * @return True if the set contains similar socket, otherwise false.
 */
bool
DefaultICGenerator::containsSimilarSocket(
    const std::set<TTAMachine::Socket*> set, 
    const TTAMachine::Socket& socket) {

    int segmentConns = socket.segmentCount();
    int portConns = socket.portCount();
    Socket::Direction direction = socket.direction();
    for (std::set<Socket*>::const_iterator iter = set.begin(); 
         iter != set.end(); iter++) {
        Socket* contained = *iter;
        if (direction == Socket::INPUT) {
            if (contained->segmentCount() == segmentConns && 
                contained->direction() == direction) {
                return true;
            }
        } else if (direction == Socket::OUTPUT) {
            if (contained->segmentCount() == segmentConns &&
                contained->portCount() == portConns &&
                contained->direction() == direction) {
                return true;
            }
        } else {
            assert(false);
        }
    }
    
    return false;
}


/**
 * Returns the control value that selects the given port if the socket
 * is connected to several ports.
 *
 * @param socket The socket.
 * @param port The port.
 * @return The control value.
 * @exception NotAvailable If the given socket does not need data control or if the
 *                         given port is not attached to the socket.
 */
int
DefaultICGenerator::outputSocketDataControlValue(
    const TTAMachine::Socket& socket,
    const TTAMachine::Port& port) const 
    throw (NotAvailable) {
    
    if (socket.direction() != Socket::OUTPUT) {
        throw NotAvailable(__FILE__, __LINE__, __func__);
    }

    for (int i = 0; i < socket.portCount(); i++) {
        if (socket.port(i) == &port) {
            return i;
        }
    }
    
    throw NotAvailable(__FILE__, __LINE__, __func__);
}


/**
 * Returns the control value that selects the given segment if the given input
 * socket is connected to several segments.
 *
 * @param socket The socket.
 * @param segment The segment.
 * @return The control value.
 * @exception NotAvailable If the socket does not need control or if it is not
 *                         connected to the given segment.
 */
int
DefaultICGenerator::inputSocketControlValue(
    const TTAMachine::Socket& socket,
    const TTAMachine::Segment& segment) const
    throw (NotAvailable) {

    if (busControlWidth(socket.direction(), socket.segmentCount()) < 1) {
        throw NotAvailable(__FILE__, __LINE__, __func__);
    }
    
    for (int i = 0; i < socket.segmentCount(); i++) {
        Segment* seg = socket.segment(i);
        if (seg == &segment) {
            return i;
        }
    }

    throw NotAvailable(__FILE__, __LINE__, __func__);
}


/**
 * Calculates the width of the data port of the given input socket.
 *
 * @param socket The socket.
 * @return The width of the data port.
 */
int
DefaultICGenerator::inputSocketDataPortWidth(const TTAMachine::Socket& socket) {
    assert(socket.direction() == Socket::INPUT);
    int width = 0;
    for (int i = 0; i < socket.portCount(); i++) {
        Port* port = socket.port(i);
        if (width < port->width()) {
            width = port->width();
        }
    }
    return width;
}


/**
 * Returns the width of the data port of the given output socket.
 *
 * @param socket The socket.
 * @param port The port.
 */
int
DefaultICGenerator::outputSocketDataPortWidth(
    const TTAMachine::Socket& socket, 
    int port) {
    
    assert(socket.direction() == Socket::OUTPUT);
    assert(port < socket.portCount());
    return socket.port(port)->width();
}


/**
 * Returns the maximum width of the data ports of the given output socket.
 *
 * @param socket The socket.
 */
int
DefaultICGenerator::maxOutputSocketDataPortWidth(
    const TTAMachine::Socket& socket) {
    int maxPortWidth = 0;
    assert(socket.direction() == Socket::OUTPUT);
    for (int i = 0; i < socket.portCount(); i++) {
        Port* port = socket.port(i);
        if (maxPortWidth < port->width()) {
            maxPortWidth = port->width();
        }
    }
    return maxPortWidth;
}


/**
 * Returns the number of bits required to control the bus connections of
 * the given socket.
 *
 * @param direction Direction of the socket.
 * @param busConns The number of bus connections.
 * @return The control width.
 */
int
DefaultICGenerator::busControlWidth(
    TTAMachine::Socket::Direction direction,
    int busConns) {

    assert(busConns >= 1);

    if (direction == Socket::INPUT) {
        return MathTools::bitLength(busConns - 1);
    } else {
        return busConns;
    }
}


/**
 * Returns the number of bits required to control from which port the
 * data is written to the bus.
 *
 * @param socket Direction of the socket,
 * @param portConns The number of port connections.
 * @return The control width.
 */
int
DefaultICGenerator::dataControlWidth(
    TTAMachine::Socket::Direction direction,
    int portConns) {

    if (direction == Socket::OUTPUT) { // && portConns > 1) {
        return MathTools::bitLength(portConns - 1);
    } else {
        return 0;
    }
}


/**
 * Returns the required width of the short immediate port of the given bus.
 *
 * @param bus The bus.
 * @return The width of the port.
 */
int
DefaultICGenerator::simmPortWidth(const TTAMachine::Bus& bus) {
    if (bus.signExtends()) {
        return bus.width();
    } else if (bus.zeroExtends()) {
        return bus.immediateWidth();
    } else {
        assert(false && "Unknown extension policy.");
        return -1;
    }
}


/**
 * Returns the name of the data port of the given input socket in the
 * interconnection network.
 *
 * @param socket The socket.
 * @return The name of the data port.
 */
std::string
DefaultICGenerator::inputSocketDataPort(const std::string& socket) {
    return "socket_" + socket + "_data";
}


/**
 * Returns the name of the data port of the given output socket in the
 * interconnection network.
 *
 * @param socket The socket name.
 * @param port The data port number.
 * @return The name of the data port.
 */
std::string
DefaultICGenerator::outputSocketDataPort(
    const std::string& socket, 
    int port) {
    
    return "socket_" + socket + "_data" + Conversion::toString(port);
}


/**
 * Returns the name of the bus connection control port of the given 
 * socket.
 *
 * @param name Name of the socket.
 * @return The name of the control port.
 */
std::string
DefaultICGenerator::socketBusControlPort(const std::string& name) {
    return "socket_" + name + "_bus_cntrl";
}


/**
 * Returns the name of the data control port of the given socket.
 *
 * @param name Name of the socket.
 * @return The name of the control port.
 */
std::string
DefaultICGenerator::socketDataControlPort(const std::string& name) {
    return "socket_" + name + "_data_cntrl";
}


/**
 * Returns the name of the data port for short immediate of the given bus.
 *
 * @param busName Name of the bus.
 * @return The name of the port.
 */
std::string
DefaultICGenerator::simmDataPort(const std::string& busName) {
    return "simm_" + busName;
}


/**
 * Returns the name of the control port for short immediate of the given
 * bus.
 *
 * @param busName Name of the bus.
 * @return The name of the port.
 */
std::string
DefaultICGenerator::simmControlPort(const std::string& busName) {
    return "simm_cntrl_" + busName;
}


/**
 * Returns the name of the data bus port in input socket.
 *
 * @param bus The bus number.
 * @return The name of the port.
 */
std::string
DefaultICGenerator::inputSocketBusPort(int bus) {
    return "databus" + Conversion::toString(bus);
}


/**
 * Returns the name of the data bus port in output socket.
 *
 * @param bus The bus number.
 * @return The name of the port.
 */
std::string
DefaultICGenerator::outputSocketBusPort(int bus) {
    return "databus" + Conversion::toString(bus) + "_alt";
}


/**
 * Returns the name of the data port in output socket.
 *
 * @param port The port number.
 * @return The name of the port.
 */
std::string
DefaultICGenerator::outputSocketDataPort(int port) {
    return "data" + Conversion::toString(port);
}


/**
 * Returns the name of the signal of the given bus.
 *
 * @param bus The bus.
 * @return The name of the signal.
 */
std::string
DefaultICGenerator::busSignal(const TTAMachine::Bus& bus) {
    return "databus_" + bus.name();
}


/**
 * Returns the signal name of the given bus for the given output socket.
 *
 * @param bus The bus.
 * @param socket The socket.
 * @return The signal name.
 */
std::string
DefaultICGenerator::busAltSignal(
    const TTAMachine::Bus& bus, 
    const TTAMachine::Socket& socket) {
    
    assert(socket.direction() == Socket::OUTPUT);
    if (!MapTools::containsKey(altSignalMap_, &bus)) {
        SocketSignalMap* newMap = new SocketSignalMap;
        altSignalMap_.insert(
            std::pair<const Bus*, SocketSignalMap*>(&bus, newMap));
    }
    
    SocketSignalMap* signalMap = MapTools::valueForKey<SocketSignalMap*>(
        altSignalMap_, &bus);
    if (!MapTools::containsKey(*signalMap, &socket)) {
        int maxValue = -1;
        for (SocketSignalMap::const_iterator iter = signalMap->begin(); 
             iter != signalMap->end(); iter++) {
            if ((*iter).second > maxValue) {
                maxValue = (*iter).second;
            }
        }
        maxValue++;
        signalMap->insert(
            std::pair<const Socket*, int>(&socket, maxValue));
    }
    
    int value = MapTools::valueForKey<int>(*signalMap, &socket);
    return busSignal(bus) + "_alt" + Conversion::toString(value);
}


/**
 * Returns the generic name for width of the given bus.
 *
 * @param bus The bus number.
 * @return The generic name.
 */
std::string
DefaultICGenerator::busWidthGeneric(int bus) {
    return "BUSW_" + Conversion::toString(bus);
}


/**
 * Returns the generic name for width of the given port.
 *
 * @param port The port number.
 * @return The generic name.
 */
std::string
DefaultICGenerator::dataWidthGeneric(int port) {
    return "DATAW_" + Conversion::toString(port);
}


/**
 * Returns the name of the socket for short immediate of the given bus.
 *
 * @param bus The bus.
 * @return The name of the socket.
 */
std::string
DefaultICGenerator::simmSocket(const TTAMachine::Bus& bus) {
    return "simm_socket_" + bus.name();
}


/**
 * Returns the name of the signal of short immediate of the given bus.
 *
 * @param bus The bus.
 * @return The name of the signal.
 */
std::string
DefaultICGenerator::simmSignal(const TTAMachine::Bus& bus) {
    return "databus_" + bus.name() + "_simm";
}


/**
 * Converts the given socket direction to direction of the corresponding
 * data port in the IC block.
 *
 * @param direction The socket direction.
 * @return The direction of the corresponding socket data port in IC 
 *         block.
 */
HDB::Direction
DefaultICGenerator::convertDirection(
    TTAMachine::Socket::Direction direction) {
    
    switch (direction) {
    case Socket::INPUT: return HDB::OUT;
    case Socket::OUTPUT: return HDB::IN;
    case Socket::UNKNOWN: assert(false);
    }
    
    assert(false);
}


/**
 * Generates file where to write VHDL definition of the given socket.
 *
 * @param direction Direction of the socket.
 * @param portConns Number of port connections.
 * @param segmentConns Number of segment connections.
 * @return The file name.
 */
std::string
DefaultICGenerator::socketFileName(
    const ProGe::HDL language,
    TTAMachine::Socket::Direction direction,
    int portConns,
    int segmentConns) {

    if (direction == Socket::INPUT) {
        return "input_socket_" + 
            Conversion::toString(segmentConns) +
            ((language==VHDL)?".vhdl":".v");
    } else if (direction == Socket::OUTPUT) {
        return "output_socket_" +
            Conversion::toString(segmentConns) + "_" + 
            Conversion::toString(portConns) + ((language==VHDL)?".vhdl":".v");
    } else {
        assert(false);
    }
}


/**
 * Generates VHDL entity name for the given socket.
 *
 * @param direction Direction of the socket.
 * @param portConns The number of port connections.
 * @param segmentConns The number of segment connections.
 * @return The entity name.
 */
std::string
DefaultICGenerator::socketEntityName(
    TTAMachine::Socket::Direction direction,
    int portConns,
    int segmentConns) const {

    if (direction == Socket::INPUT) {
        return inputSocketEntityName(segmentConns);
    } else if (direction == Socket::OUTPUT) {
        return outputSocketEntityName(segmentConns, portConns);
    } else {
        assert(false);
    }
}


/**
 * Returns the entity name of the input socket which has the given number
 * of connections.
 *
 * @param conns The number of connections.
 * @return The name of the entity.
 */
std::string
DefaultICGenerator::inputSocketEntityName(int conns) const {
    return 
        entityNameStr_ + "_input_socket_cons_" + 
        Conversion::toString(conns);
}


/**
 * Returns the entity name of the output socket which has the given number
 * of connections.
 *
 * @param busConns The number of bus connections.
 * @param portConns The number of port connections.
 * @return The name of the entity.
 */
std::string
DefaultICGenerator::outputSocketEntityName(int busConns, int portConns) const {
    return 
        entityNameStr_ + "_output_socket_cons_" + 
        Conversion::toString(busConns) + "_" +
        Conversion::toString(portConns);
}


/**
 * Generates an indentation of the given level.
 *
 * @param level The level.
 */
std::string
DefaultICGenerator::indentation(unsigned int level) {
    string indentation("");
    for (unsigned int i = 0; i < level; i++) {
        indentation += "  ";
    }
    return indentation;
}

