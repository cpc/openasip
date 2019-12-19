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
 * @file EditLineReader.cc
 *
 * Definition of EditLineReader.cc.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 2 June 2004 by jm, pj, tr, jn
 * @note rating: yellow
 */

#include <string>
#include <cstring>
#include <cctype>
#include <cstdio>
#include <map>

#include "EditLineReader.hh"
#include "StringTools.hh"
#include "MapTools.hh"
#include "Application.hh"

using std::string;
using std::map;

map<EditLine*, EditLineReader*> EditLineReader::lineReaders_;

/**
 * Constructor.
 *
 * @param program The name of the invocating program.
 */
EditLineReader::EditLineReader(std::string program) : 
    LineReader(), prompt_(NULL), program_(program), editLine_(NULL), 
    history_(NULL) {
}

/**
 * Destructor.
 */
EditLineReader::~EditLineReader() {
    
    lineReaders_.erase(editLine_);
    
    if (history_ != NULL) {
        history_end(history_);
    }
    
    if (editLine_ != NULL) {
        el_end(editLine_);
    }
    
    if (prompt_ != NULL) {
        delete[] prompt_;
    }
}

/**
 * Initializes LineReader.
 *
 * Sets the prompt printing function and initializes history.
 *
 * @param defPrompt The prompt for the program.
 * @param in Input file stream.
 * @param out Output file stream.
 * @param err Error file stream.
 */
void
EditLineReader::initialize(
    std::string defPrompt, 
    FILE* in, 
    FILE* out, 
    FILE* err) {

    prompt_ = StringTools::stringToCharPtr(defPrompt);
    // initialize EditLine
    editLine_ = el_init(program_.c_str(), in, out, err);
    assert(editLine_!=NULL);
    // define prompt printing function
    el_set(editLine_, EL_PROMPT, wrapperToCallPrompt);
    // bind all keys to the standard GNU emacs-like bindings.
    //el_set(editLine_, EL_BIND, "-e");
    el_set(editLine_, EL_EDITOR, "emacs");
    if (in != stdin) {
        // when reading from file, editing is set off
        el_set(editLine_, EL_EDITMODE, 0);
    }
    // initialize History
    HistEvent ev;    
    history_ = history_init();
    if (history_!=NULL) {
        // size of the history is set to 100
        history(history_, &ev, H_SETSIZE, 100);
        el_set(editLine_, EL_HIST, history, history_);
    }
    lineReaders_.insert(ValType(editLine_, this));
    in_ = in;
    setInitialized();
}

/**
 * Reads line from input stream.
 *
 * If prompt is given, it is used as a prompt only for reading this one line.
 *
 * @param prompt The prompt.
 * @return The string that is read.
 * @exception ObjectNotInitialized If LineReader is not initialized.
 * @exception EndOfFile When end of file mark (ctrl-d) is received.
 */
string
EditLineReader::readLine(std::string prompt) {
    if (!initialized()) {
        string message = "LineReader not initialized";
        throw ObjectNotInitialized(__FILE__, __LINE__, __func__, message);
    }
    
    char* oldPrompt = NULL;
    if (prompt != "") {
        oldPrompt = prompt_;
        prompt_ = StringTools::stringToCharPtr(prompt);
    }

    int count;
    const char* c = el_gets(editLine_, &count);

    bool endOfFile = (c == NULL || strlen(c) == 0 || feof(in_));
    if (endOfFile) {
        string message = "End of file.";
        throw EndOfFile(__FILE__, __LINE__, __func__, message);
    }
  
    // add command to history
    updateHistory(c);
    if (oldPrompt != NULL) {
        delete[] prompt_;
        prompt_ = oldPrompt;
    }
    return c;
}

/**
 * Asks user a question and expects that answer is given as char.
 *
 * Prompt is temporarily changed to 'question' during the execution of this
 * function.
 * 
 * @param question The asked question.
 * @param allowedChars The chars that are legal answers.
 * @param caseSensitive Flag indicating whether answer is read case sensitive
 *        or not.
 * @param defaultAnswer The default answer.
 * @return User given answer.
 * @exception ObjectNotInitialized If LineReader is not initialized.
 */
char
EditLineReader::charQuestion(
    std::string question, std::string allowedChars, bool caseSensitive,
    char defaultAnswer) {
    if (!initialized()) {
        string method = "EditLineReader::charQuestion()";
        string message = "LineReader not initialized";
        throw ObjectNotInitialized(__FILE__, __LINE__, method, message);
    }

    char* oldPrompt = prompt_;
    prompt_ = StringTools::stringToCharPtr(question);

    if (defaultAnswer == '\0') {
        const char* answer = nullptr;
        do {
            int count;
            answer = el_gets(editLine_, &count);
        } while (strlen(answer) != 1 && 
                 !StringTools::
                 containsChar(allowedChars, answer[0], caseSensitive));
        
        updateHistory(answer);
        delete[] prompt_;
        prompt_ = oldPrompt;
        return answer[0];
    } else {
        const char* answer = nullptr;
        int count;
        assert(editLine_ != nullptr);
        answer = el_gets(editLine_, &count);
        if (answer == nullptr) {
            updateHistory("");
            delete[] prompt_;
            prompt_ = oldPrompt;
            return defaultAnswer;
        }
        if (strlen(answer) != 1 && 
            !StringTools::
            containsChar(allowedChars, answer[0], caseSensitive)) {
            
            updateHistory(&defaultAnswer);
            delete[] prompt_;
            prompt_ = oldPrompt;
            return defaultAnswer;
        } else {
            updateHistory(answer);
            delete[] prompt_;
            prompt_ = oldPrompt;
            return answer[0];
        }
    }
}

/**
 * Static wrapper function that calls function that returns the prompt.
 *
 * If correct EditLine* instance is not found in the map, an empty string
 * (null char*) is returned.
 *
 * @param edit EditLine which prompt is needed.
 * @return The prompt.
 */
char*
EditLineReader::wrapperToCallPrompt(EditLine* edit) {
    
    MapIt mi = lineReaders_.find(edit);
    if (mi == lineReaders_.end()) {
        return NULL;
    }
    EditLineReader* reader = (*mi).second;
    return reader->prompt();
}

/**
 * Returns the used prompt.
 *
 * @return The prompt.
 */
char*
EditLineReader::prompt() {
    return prompt_;
}

/**
 * Inserts a new entry to history.
 *
 * Only non-empty strings are saved to history.
 *
 * @param newEntry A new entry to be added.
 */
void
EditLineReader::updateHistory(const char* newEntry) {
    string entry(newEntry);
    entry = StringTools::trim(entry);
    if (entry != "") {
        HistEvent ev;
        history(history_, &ev, H_ENTER, newEntry);
        putInInputHistory(newEntry);
    }
}
