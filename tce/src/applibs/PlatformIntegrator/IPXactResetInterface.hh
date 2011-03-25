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
 * @file IPXactResetInterface.hh
 *
 * Declaration of IPXactResetInterface class.
 *
 * @author Otto Esko 2011 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#ifndef TTA_IP_XACT_RESET_INTERFACE_HH
#define TTA_IP_XACT_RESET_INTERFACE_HH

#include "IPXactInterface.hh"

class IPXactResetInterface : public IPXactInterface {
public:

    IPXactResetInterface();

    virtual ~IPXactResetInterface();

    virtual bool mapPortsToInterface(const ProGe::NetlistBlock& toplevel);

private:

    static const std::string TTA_RESET_PORT;
    static const std::string SPIRIT_RESET_PORT;

    static const std::string DEFAULT_INSTANCE_NAME;
    static const std::string VENDOR;
    static const std::string LIBRARY;
    static const std::string NAME;
    static const std::string BUS_VERSION;
    static const std::string ABS_VENDOR;
    static const std::string ABS_LIBRARY;
    static const std::string ABS_NAME;
    static const std::string ABS_VERSION;

    static const IPXactModel::BusMode DEFAULT_BUS_MODE;
};

#endif
