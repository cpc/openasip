/**
 * @file EditLineReader.cc
 *
 * Definition of EditLineReader.cc.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
 * @note reviewed 2 June 2004 by jm, pj, tr, jn
 * @note rating: yellow
 */

#include <string>
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
    if (in != stdin) {
        // when reading from file, editing is set off
        el_set(editLine_, EL_EDITMODE, 0);
    }
    // define prompt printing function
    el_set(editLine_, EL_PROMPT, wrapperToCallPrompt);
    // bind all keys to the standard GNU emacs-like bindings.
    el_set(editLine_, EL_BIND, "-e");

    lineReaders_.insert(ValType(editLine_, this));
    
    // initialize History
    history_ = history_init();
    el_set(editLine_, EL_HIST, history, history_);
    HistEvent ev;
    // size of the history is set to 100
    history(history_, &ev, H_SETSIZE, 100);
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
EditLineReader::readLine(std::string prompt) 
    throw (ObjectNotInitialized, EndOfFile) {
    
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
    std::string question, 
    std::string allowedChars,
    bool caseSensitive,
    char defaultAnswer) throw (ObjectNotInitialized) {
    
    if (!initialized()) {
        string method = "EditLineReader::charQuestion()";
        string message = "LineReader not initialized";
        throw ObjectNotInitialized(__FILE__, __LINE__, method, message);
    }

    char* oldPrompt = prompt_;
    prompt_ = StringTools::stringToCharPtr(question);

    if (defaultAnswer == '\0') {
        const char* answer;
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
        const char* answer;
        int count;
        answer = el_gets(editLine_, &count);
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
