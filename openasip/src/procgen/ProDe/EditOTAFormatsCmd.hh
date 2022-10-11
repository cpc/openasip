/*
    Copyright (c) 2022 Tampere University.

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
 * @file EditOTAFormatsCmd.hh
 *
 * Declaration of EditOTAFormatsCmd class.
 *
 * @author Kari Hepola 2022
 * @note rating: red
 */

#ifndef TTA_EDIT_OTA_FORMATS_CMD_HH
#define TTA_EDIT_OTA_FORMATS_CMD_HH

#include "EditorCommand.hh"

/**
 * Command for editing OTAFormats.
 *
 * Opens a OTAFormatListDialog which lists OTAFormats in the machine.
 */
class EditOTAFormatsCmd : public EditorCommand {
public:
    EditOTAFormatsCmd();
    virtual ~EditOTAFormatsCmd();
    virtual bool Do();
    virtual int id() const;
    virtual EditOTAFormatsCmd* create() const;
    virtual std::string icon() const;
    virtual std::string shortName() const;
    virtual bool isEnabled();
};

#endif
