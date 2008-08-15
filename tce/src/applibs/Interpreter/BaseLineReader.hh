/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/
/**
 * @file BaseLineReader.hh
 *
 * Declaration of BaseLineReader class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
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

    virtual std::string readLine(std::string prompt = "")
        throw (ObjectNotInitialized, EndOfFile);

    virtual char charQuestion(
        std::string question, 
        std::string allowedChars,
        bool caseSensitive = false,
        char defaultAnswer = '\0') throw (ObjectNotInitialized);

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
