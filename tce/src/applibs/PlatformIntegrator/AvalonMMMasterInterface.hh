/*
    Copyright (c) 2002-2010 Tampere University of Technology.

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
 * @file AvalonMMMasterInterface.hh
 *
 * Declaration of AvalonMMMasterInterface class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#ifndef TTA_AVALON_MM_MASTER_INTERFACE_HH
#define TTA_AVALON_MM_MASTER_INTERFACE_HH

#include "SOPCInterface.hh"
#include "NetlistPort.hh"

class AvalonMMMasterInterface : public SOPCInterface {
public:
    
    AvalonMMMasterInterface(
        const std::string& name,
        const std::string& declaration,
        const std::string& avalonPrefix,
        const SOPCInterface& clock);
    
    virtual ~AvalonMMMasterInterface();

    bool isValidPort(const ProGe::NetlistPort& port) const;

    void addPort(const ProGe::NetlistPort& port);

    virtual void writeInterface(std::ostream& stream) const;

    static const std::string AVALON_MM_ADDRESS;
    static const std::string AVALON_MM_WDATA;
    static const std::string AVALON_MM_RDATA;
    static const std::string AVALON_MM_READ;
    static const std::string AVALON_MM_WRITE;
    static const std::string AVALON_MM_BYTE_ENABLE;
    static const std::string AVALON_MM_WAIT_REQ;
    static const std::string AVALON_MM_IRQ_RECV;

private:

    const SOPCPort* findInterfacePort(
        std::string hdlName,
        HDB::Direction direction) const;

    void addIrqInterface(
        const ProGe::NetlistPort& port,
        const SOPCPort& avalonPort);

    std::string avalonPrefix_;

    PortList masterPorts_;
    SOPCPort irqPort_;

    const SOPCInterface* clock_;
    std::vector<SOPCInterface*> childInterfaces_;

    static const int ADDRESS_DEF_WIDTH;
    static const int WDATA_DEF_WIDTH;
    static const int RDATA_DEF_WIDTH;
    static const int READ_DEF_WIDTH;
    static const int WRITE_DEF_WIDTH;
    static const int BYTE_ENABLE_DEF_WIDTH;
    static const int WAIT_REQ_DEF_WIDTH;
    static const int IRQ_RECV_DEF_WIDTH;
};

#endif
