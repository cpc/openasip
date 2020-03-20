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
 * @file BaseLineReader.hh
 *
 * Declaration of BaseLineReader class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_BASE_LINE_READER_HH
#define TTA_BASE_LINE_READER_HH

#include <string>

#include "LineReader.hh"

/**
 * Handles the basic line reading without a use of any fancy libraries.
 *
 * This LineReader is used only when nothing else is available. This
 * LineReader supports only reading from std::cin and outputing to
 * std::cout.
 */
class BaseLineReader : public LineReader {
public:
    BaseLineReader(
        std::istream& iStream = std::cin, std::ostream& oStream = std::cout);
    virtual ~BaseLineReader();

    virtual void initialize(
        std::string defPrompt = "",
        FILE* in = stdin, 
        FILE* out = stdout, 
        FILE* err = stderr);

    virtual void setPromptPrinting(bool flag);

    virtual std::string readLine(std::string prompt = "");

    virtual char charQuestion(
        std::string question, std::string allowedChars,
        bool caseSensitive = false, char defaultAnswer = '\0');

private:
    /// Copying not allowed.
    BaseLineReader(const BaseLineReader&);
    /// Assignment not allowed.
    BaseLineReader& operator=(const BaseLineReader&);

    void printPrompt() const;

    static const int MAX_LINE_LENGTH;

    /// Prompt of the reader.
    std::string prompt_;
    /// Input stream.
    FILE* in_;
    /// Output stream.
    FILE* out_;
    /// Error stream.
    FILE* error_;
    /// Input stream.
    std::istream& iStream_;
    /// Output stream.
    std::ostream& oStream_;
    /// Prompt printing flag.
    bool promptPrinting_;
};

#endif
