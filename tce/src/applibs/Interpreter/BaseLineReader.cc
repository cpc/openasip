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
 * @file BaseLineReader.cc
 *
 * Definition of BaseLineReader class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <iostream>

#include "BaseLineReader.hh"
#include "StringTools.hh"
#include "Application.hh"

const int BaseLineReader::MAX_LINE_LENGTH = 256;

/**
 * Constructor.
 */
BaseLineReader::BaseLineReader(std::istream& iStream, std::ostream& oStream) :
    LineReader(iStream, oStream), iStream_(iStream), oStream_(oStream),
    promptPrinting_(true) {
}

/**
 * Destructor.
 */
BaseLineReader::~BaseLineReader() {
}

/**
 * Initializes the reader.
 *
 * @param prompt Prompt for the reader.
 * @param in Input stream. Not used, input is always cin.
 * @param out Output stream. Not used, output is always cout.
 * @param err Error stream. Not used, error output is always cerr.
 */
void
BaseLineReader::initialize(
    std::string prompt, 
    FILE*, 
    FILE*, 
    FILE*) {
    
    prompt_ = prompt;

    setInitialized();
}

/**
 * Disables/enables printing of prompt altogether.
 *
 * @param flag 
 */
void
BaseLineReader::setPromptPrinting(bool flag) {
    promptPrinting_ = flag;
}

/**
 * Reads a line from the input stream.
 *
 * @param prompt The prompt to be used.
 * @return The read string.
 * @exception ObjectNotInitialized If LineReader is not initialized.
 * @exception EndOfFile When end of file mark is received.
 * @todo Implement detection of end-of-file.
 */
std::string
BaseLineReader::readLine(std::string prompt) {
    if (!initialized()) {
        std::string msg = "LineReader not initialized.";
        throw ObjectNotInitialized(__FILE__, __LINE__, __func__, msg);
    }
    
    std::string origPrompt = prompt_;
    prompt_ = prompt;
 
    if (promptPrinting_)
        printPrompt();
    
    std::string result;
    getline(iStream_, result);

    prompt_ = origPrompt;

    if (iStream_.eof() || iStream_.fail()) {
        std::string msg = 
            "End of file from input stream or input stream in bad state.";
        throw EndOfFile(__FILE__, __LINE__, __func__, msg);
    }
    return result;
}

/**
 * User is asked a question, and answer is excepted to be one character.
 *
 * @param question Question to be asked.
 * @param allowedChars Chars allowed in answer.
 * @param caseSensitive If true answer is treated case sensitive.
 * @param defaultAnswer Default answer for the question.
 * @return The answered character.
 * @exception ObjectNotInitialized If LineReader is not initialized.
 */
char
BaseLineReader::charQuestion(
    std::string question, std::string allowedChars, bool caseSensitive,
    char defaultAnswer) {
    if (!initialized()) {
        std::string method = "BaseLineReader::charQuestion()";
        std::string msg = "LineReader not initialized.";
        throw ObjectNotInitialized(__FILE__, __LINE__, method, msg);
    }

    std::string origPrompt = prompt_;
    prompt_ = question;
    printPrompt();

    char answer;

    if (defaultAnswer == '\0') {
        // default answer not given
        do {
            iStream_ >> answer;
        } while (!StringTools::containsChar(
                     allowedChars, answer, caseSensitive) && 
                 !iStream_.eof() && !iStream_.fail());
        prompt_ = origPrompt;
        return answer;
    } else {
        // default answer given
        iStream_ >> answer;
        
        if (!StringTools::containsChar(allowedChars, answer, caseSensitive)) {
            prompt_ = origPrompt;
            return defaultAnswer;
        } else {
            prompt_ = origPrompt;
            return answer;
        }
    }
    
    assert(false);
    return '!';
}

/**
 * Prints the prompt.
 */
void
BaseLineReader::printPrompt() const {
    oStream_ << prompt_;
}
