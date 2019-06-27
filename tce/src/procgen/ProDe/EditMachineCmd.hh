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
/*
 * @file EditMachineCmd.hh
 *
 * Declaration of EditMachine class.
 *
 * Created on: 6.2.2015
 * @author: Henry Linjam�ki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef EDITMACHINE_HH
#define EDITMACHINE_HH

#include "EditorCommand.hh"

class EditMachineCmd: public EditorCommand {
public:
    EditMachineCmd();
    virtual ~EditMachineCmd();
    virtual bool Do();
    virtual int id() const;
    virtual EditMachineCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif /* EDITMACHINE_HH */
