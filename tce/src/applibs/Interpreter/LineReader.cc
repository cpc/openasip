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
 * @file LineReader.cc
 *
 * Declaration of LineReader class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 2 June 2004 by jm, pj, tr, jn
 * @note rating: red
 */

#include <string>
#include <fstream>
#include <istream>
#include <ostream>

#include "LineReader.hh"
#include "Application.hh"

using std::string;

/**
 * Constructor.
 */
LineReader::LineReader(std::istream& iStream, std::ostream& oStream) : 
    initialized_(false), inputHistorySize_(DEFAULT_INPUT_HISTORY_SIZE), 
    saveHistoryToFile_(false), historyFile_(NULL), 
    historyFilename_("commandhistory.txt"), iStream_(iStream),
    oStream_(oStream) {
}

/**
 * Destructor.
 */
LineReader::~LineReader() {
    if (historyFile_ != NULL) {
        historyFile_->close();
        delete historyFile_;
        historyFile_ = NULL;
    }
}

/**
 * Asks confirmation from the user.
 *
 * Question is asked from user, who should answer with a single character.
 *
 * @param question The question that is asked from the user.
 * @param defaultAnswer The default answer.
 * @param yesChar Character meaning "yes" answer.
 * @param noChar Character meaning "no" answer.
 * @return True, if answer is yesChar, false otherwise.
 * @exception ObjectNotInitialized If LineReader is not initialized.
 */
bool
LineReader::confirmation(
    std::string question, 
    char defaultAnswer, 
    char yesChar, 
    char noChar) throw (ObjectNotInitialized) {

    if (!initialized_) {
        string method = "LineReader::confirmation()";
        string message = "LineReader not initialized.";
        throw ObjectNotInitialized(__FILE__, __LINE__, method, message);
    }

    string allowed = string(1, yesChar) + string(1, noChar);
    char answer = charQuestion(question, allowed, false, defaultAnswer);

    return (answer == yesChar);
}

/**
 * Returns the output stream which can be used to print information to
 * the user.
 *
 * This stream should be used to ask confirmation, etc. from the user.
 *
 * @return The output stream (std::cout by default).
 */
std::ostream&
LineReader::outputStream() {
    return oStream_;
}

/**
 * Sets whether command history should be also saved to a stream.
 *
 * The file name should be set with setInputHistoryLog().
 *
 * @param flag Status of this option.
 */
void
LineReader::setSaveInputHistoryToFile(bool flag) {
    saveHistoryToFile_ = flag;
}

/**
 * Sets the output file name to save the command history to.
 *
 * @param historyFilename The command log stream.
 */
void 
LineReader::setInputHistoryLog(const std::string& historyFilename) {

    if (historyFile_ != NULL) {
        historyFile_->close();
        delete historyFile_;
        historyFile_ = NULL;
    }
    historyFilename_ = historyFilename;
}

/**
 * Sets the maximum size of the command history log.
 *
 * @param length New length for the command history log.
 */
void 
LineReader::setInputHistoryLength(std::size_t length) {
    inputHistorySize_ = length;
}

/**
 * Returns the count of entries in the input history log.
 *
 * @return The count of entries in the log.
 */
std::size_t
LineReader::inputsInHistory() const {
    return inputHistory_.size();
}


/**
 * Returns the string at input history log with the given age.
 *
 * In case the given age is 0, the newest entry is returned, if it's 1, 
 * the 2nd newest, and so on.
 *
 * @param age The age of the requested history entry. An empty string is
 *            is returned in case there's no entry with the given age.
 */
std::string 
LineReader::inputHistoryEntry(std::size_t age) const {
    if (inputHistory_.size() == 0 || 
        age > static_cast<std::size_t>(inputHistory_.size() - 1))
        return "";
    return inputHistory_.at(inputHistory_.size() - age - 1);
}

/**
 * Puts an input to the input history log.
 *
 * Also writes the input to the input history log stream, if there's one, and
 * input history logging to a file is enabled.
 *
 * Expects that the terminating '\n' is in place in case it's wanted to be
 * stored in the log.
 *
 * @param inputLine The line to store.
 */
void 
LineReader::putInInputHistory(const std::string& inputLine) {

    // make room for the entry in case the input history is full
    if (inputHistory_.size() >= inputHistorySize_)
        inputHistory_.pop_front();
    
    inputHistory_.push_back(inputLine);
    
    if (saveHistoryToFile_) {
        // initialize the log file in case it hasn't been initialized before
        if (historyFile_ == NULL) {
            historyFile_ = 
                new std::ofstream(historyFilename_.c_str(), std::ios::app);
            if (!historyFile_->is_open()) {
                debugLog("Could not open history file for writing.");
                return;
            }
        }
        *historyFile_ << inputLine;
    }
}
