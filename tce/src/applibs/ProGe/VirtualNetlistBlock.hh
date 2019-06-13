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
 * @file VirtualNetlistBlock.hh
 *
 * Declaration of VirtualNetlistBlock class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_VIRTUAL_NETLIST_BLOCK_HH
#define TTA_VIRTUAL_NETLIST_BLOCK_HH

#include <string>

#include "Exception.hh"
#include "NetlistBlock.hh"

namespace ProGe {

class Netlist;

/**
 * Represents a virtual block in the netlist.
 *
 * Virtual blocks are not written to HDL files so they can include special
 * ports such as all zero or all one connections.
 * Ports belonging to a virtual block do not generate into signals in HDL.
 */
class VirtualNetlistBlock : public NetlistBlock {
public:
    VirtualNetlistBlock(
        const std::string& moduleName,
        const std::string& instanceName,
        Netlist& netlist);
    virtual ~VirtualNetlistBlock();

    virtual bool isVirtual() const;
};
}

#endif
