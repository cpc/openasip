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
