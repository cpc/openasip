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
 * @file LineReader.hh
 *
 * Declaration of LineReader class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 2 June 2004 by jm, pj, tr, jn
 * @note rating: red
 */

#ifndef TTA_LINE_READER_HH
#define TTA_LINE_READER_HH

#include <cstdio>
#include <string>
#include <iostream>
#include <deque>
#include <fstream>

#include "Exception.hh"

/**
 * Abstract base class for line readers.
 *
 * LineReader's purpose is to handle reading from command line and to make it
 * possible for the user to edit the input given to program.
 */
class LineReader {
public:
    LineReader(
        std::istream& iStream = std::cin, std::ostream& oStream = std::cout);
    virtual ~LineReader();

    virtual void initialize(
        std::string defPrompt = "",
        FILE* in = stdin, 
        FILE* out = stdout, 
        FILE* err = stderr) = 0;

    virtual std::string readLine(std::string prompt = "")
        throw (ObjectNotInitialized, EndOfFile) = 0;

    virtual std::ostream& outputStream();

    virtual char charQuestion(
        std::string question, 
        std::string allowedChars,
        bool caseSensitive = false,
        char defaultAnswer = '\0') throw (ObjectNotInitialized) = 0;

    bool confirmation(
        std::string question, 
        char defaultAnswer = 'n', 
        char yesChar = 'y',
        char noChar = 'n') throw (ObjectNotInitialized);

    virtual void setSaveInputHistoryToFile(bool flag);
    virtual bool saveInputHistoryToFile() const;
    virtual void setInputHistoryLog(const std::string& historyFilename);
    virtual void setInputHistoryLength(std::size_t length);
    virtual size_t inputHistoryMaxLength() const;
    virtual std::size_t inputsInHistory() const;
    virtual std::string inputHistoryEntry(std::size_t age) const;

protected:
    void setInitialized();
    bool initialized() const;
    void putInInputHistory(const std::string& inputLine);

private:
    /// Copying not allowed.
    LineReader(const LineReader&);
    /// Assignment not allowed.
    LineReader& operator=(const LineReader&);
    /// Flag indicating whether LineReader is initialized.
    bool initialized_;
    /// The input history.
    std::deque<std::string> inputHistory_;
    /// The maximum size for input history.
    std::size_t inputHistorySize_;
    /// Should the history be appended in a file?
    bool saveHistoryToFile_;
    /// The output stream to write the command history to.
    std::ofstream* historyFile_; 
    /// The filename to write the command history to.
    std::string historyFilename_;
    /// The input stream.
    std::istream& iStream_;
    /// The output stream.
    std::ostream& oStream_;
};

#define DEFAULT_INPUT_HISTORY_SIZE 50

#include "LineReader.icc"

#endif
