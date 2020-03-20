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
 * @file CentralizedControlICGenerator.hh
 *
 * Declaration of CentralizedControlICGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_CENTRALIZED_CONTROL_IC_GENERATOR_HH
#define TTA_CENTRALIZED_CONTROL_IC_GENERATOR_HH

#include <string>
#include <map>

#include "Exception.hh"

namespace ProGe {
    class NetlistPort;
}

namespace TTAMachine {
    class Socket;
    class Bus;
    class Port;
    class Segment;
}

class CentralizedControlICGenerator {
public:
    CentralizedControlICGenerator();
    virtual ~CentralizedControlICGenerator();

    ProGe::NetlistPort& simmDataPort(const std::string& busName) const;
    ProGe::NetlistPort& simmCntrlPort(const std::string& busName) const;
    ProGe::NetlistPort& busCntrlPortOfSocket(
        const std::string& socketName) const;
    ProGe::NetlistPort& dataCntrlPortOfSocket(
        const std::string& socketName) const;
    bool hasGlockPort() const;
    ProGe::NetlistPort& glockPort() const;

    virtual int outputSocketCntrlPinForSegment(
        const TTAMachine::Socket& socket,
        const TTAMachine::Segment& segment) const = 0;

    virtual int outputSocketDataControlValue(
        const TTAMachine::Socket& socket,
        const TTAMachine::Port& port) const = 0;

    virtual int inputSocketControlValue(
        const TTAMachine::Socket& socket,
        const TTAMachine::Segment& segment) const = 0;

protected:
    void mapSImmDataPort(const std::string& busName, ProGe::NetlistPort& port);
    void mapSImmCntrlPort(const std::string& busName, ProGe::NetlistPort& port);
    void mapBusCntrlPortOfSocket(
        const std::string& socketName,
        ProGe::NetlistPort& port);
    void mapDataCntrlPortOfSocket(
        const std::string& socketName,
        ProGe::NetlistPort& port);
    void setGlockPort(ProGe::NetlistPort& glockPort);

private:
    typedef std::map<std::string, ProGe::NetlistPort*> NetlistPortMap;

    /// Maps the short immediate data ports for buses.
    NetlistPortMap simmDataPortMap_;
    /// Maps the short immediate control ports for buses.
    NetlistPortMap simmCntrlPortMap_;
    /// Maps the data ports of sockets.
    NetlistPortMap socketDataPortMap_;
    /// Maps the bus control ports of sockets.
    NetlistPortMap busCntrlPortMap_;
    /// Maps the data control ports of sockets.
    NetlistPortMap dataCntrlPortMap_;
    /// (optional) Glock port
    ProGe::NetlistPort* glockPort_;
};

#endif
