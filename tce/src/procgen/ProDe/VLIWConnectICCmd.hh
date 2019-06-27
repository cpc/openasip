/*
    Copyright (c) 2002-2017 Tampere University.

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
 * @file VLIWConnectICCmd.hh
 *
 * Declaration of VLIWConnectICCmd class.
 *
 * @author Alex Hirvonen 2017 (alex.hirvonen-no.spam-gmail.com)
 * @note rating: red
 */

#ifndef TTA_VLIW_CONNECT_IC_CMD_HH
#define TTA_VLIW_CONNECT_IC_CMD_HH

#include "EditorCommand.hh"


/**
 * Command for calling VLIWConnectIC explorer plugin.
 */
class VLIWConnectICCmd : public EditorCommand {
public:
    VLIWConnectICCmd();
    virtual ~VLIWConnectICCmd();
    virtual bool Do();
    virtual int id() const;
    virtual VLIWConnectICCmd* create() const;
    virtual bool isEnabled();
    virtual std::string shortName() const;
};

#endif
