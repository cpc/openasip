/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file PIGCLITextGenerator.hh
 *
 * Declaration of PIGCLITextGenerator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_PIG_CLI_TEXT_GENERATOR_HH
#define TTA_PIG_CLI_TEXT_GENERATOR_HH

#include "TextGenerator.hh"

/**
 * Text generator used for the texts of PIG command line user interface.
 */
class PIGCLITextGenerator : public Texts::TextGenerator {
public:
    PIGCLITextGenerator();
    virtual ~PIGCLITextGenerator();

    /// Ids for the stored texts.
    enum {
        TXT_CLI_TITLE = Texts::LAST__, 
        TXT_CLI_VERSION,
        TXT_CLI_USAGE,
        TXT_GENERATING_BEM,
        TXT_ADF_REQUIRED,
        TXT_ILLEGAL_ARGS
    };
};

#endif
