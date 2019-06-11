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
 * @file ModifyComponentCmd.hh
 *
 * Declaration of ModifyComponentCmd class.
 *
 * @author Veli-Pekka J‰‰skel‰inen (vjaaskel-no.spam-cs.tut.fi)
 */

#ifndef TTA_MODIFY_COMPONENT_CMD_HH
#define TTA_MODIFY_COMPONENT_CMD_HH

#include <wx/wx.h>
#include <wx/cmdproc.h>
#include "EditorCommand.hh"


/**
 * wxCommand for modifying components in the Machine.
 */
class ModifyComponentCmd: public EditorCommand {
public:
    ModifyComponentCmd();
    virtual bool Do();
    virtual int id() const;
    virtual ModifyComponentCmd* create() const;
    virtual std::string shortName() const;
    virtual std::string icon() const;
    virtual bool isEnabled();
};

#endif
