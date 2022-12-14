/*
    Copyright (c) 2002-2015 Tampere University.

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
 * @file NetlistBlock.hh
 *
 * Declaration of NetlistBlock class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_NETLIST_BLOCK_HH
#define TTA_NETLIST_BLOCK_HH

#include <string>
#include <vector>
#include <map>

#include "BaseNetlistBlock.hh"
#include "SignalTypes.hh"

#include "Exception.hh"
#include "Netlist.hh"

namespace HDB {
    class HWBlockImplementationParameter;
}

namespace ProGe {

class Netlist;
class NetlistPort;
class Parameter;

/**
 * Represents a hardware block in the netlist.
 */
class NetlistBlock : public BaseNetlistBlock {
public:
    NetlistBlock(
        const std::string& moduleName,
        const std::string& instanceName,
        BaseNetlistBlock* parent = NULL);
    virtual ~NetlistBlock();

    void setParameter(
        const std::string& name,
        const std::string& type,
        const std::string& value);
    using BaseNetlistBlock::addParameter;
    using BaseNetlistBlock::setParameter;
    using BaseNetlistBlock::parameter;

    using BaseNetlistBlock::addPort;
    using BaseNetlistBlock::removePort;
    using BaseNetlistBlock::portCount;
    using BaseNetlistBlock::port;
    using BaseNetlistBlock::addPortGroup;
    void removePort(NetlistPort& port);

    virtual NetlistPort* port(
        const std::string& portName,
        bool partialMatch = true);

    using BaseNetlistBlock::addSubBlock;
    using BaseNetlistBlock::subBlockCount;
    NetlistBlock& subBlock(size_t index) override;

    virtual const NetlistBlock& parentBlock() const override;
    virtual NetlistBlock& parentBlock() override;

    using BaseNetlistBlock::netlist;

    NetlistBlock* shallowCopy(
        const std::string& instanceName) const;

    using BaseNetlistBlock::addPackage;
    using BaseNetlistBlock::packageCount;
    using BaseNetlistBlock::package;

    virtual void write(
        const Path& targetBaseDir, HDL targetLang = VHDL) const override;
};

}

#endif
