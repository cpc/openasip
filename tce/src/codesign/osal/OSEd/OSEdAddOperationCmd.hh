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
 * @file OSEdAddOperationCmd.hh
 *
 * Declaration of OSEdAddOperationCmd class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_ADD_OPERATION_CMD_HH
#define TTA_ADD_OPERATION_CMD_HH

#include <string>

#include "GUICommand.hh"

/**
 * Command for adding new operation to operation set.
 */
class OSEdAddOperationCmd : public GUICommand {
public:
    OSEdAddOperationCmd();
    virtual ~OSEdAddOperationCmd();

    virtual int id() const;
    virtual GUICommand* create() const;
    virtual bool Do();
    virtual bool isEnabled();
    virtual std::string icon() const;

private:
    /// Copying not allowed.
    OSEdAddOperationCmd(const OSEdAddOperationCmd&);
    /// Assignment not allowed.
    OSEdAddOperationCmd operator=(OSEdAddOperationCmd&);
};

#endif
