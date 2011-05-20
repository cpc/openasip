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
 * @file IPXactClkInterface.hh
 *
 * Declaration of IPXactClkInterface class.
 *
 * @author Otto Esko 2011 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#ifndef TTA_IP_XACT_CLK_INTERFACE_HH
#define TTA_IP_XACT_CLK_INTERFACE_HH

#include "IPXactInterface.hh"

class IPXactClkInterface : public IPXactInterface {
public:

    IPXactClkInterface();

    virtual ~IPXactClkInterface();

    virtual bool mapPortsToInterface(const ProGe::NetlistBlock& toplevel);

private:

    static const TCEString TTA_CLK_PORT;
    static const TCEString SPIRIT_CLK_PORT;

    static const TCEString DEFAULT_INSTANCE_NAME;
    static const TCEString VENDOR;
    static const TCEString LIBRARY;
    static const TCEString NAME;
    static const TCEString BUS_VERSION;
    static const TCEString ABS_VENDOR;
    static const TCEString ABS_LIBRARY;
    static const TCEString ABS_NAME;
    static const TCEString ABS_VERSION;

    static const IPXactModel::BusMode DEFAULT_BUS_MODE;
};

#endif
