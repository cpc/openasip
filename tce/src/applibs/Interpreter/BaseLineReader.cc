/**
 * @file BaseLineReader.cc
 *
 * Definition of BaseLineReader class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
BaseLineReader::readLine(std::string prompt) 
    throw (ObjectNotInitialized, EndOfFile) {
    
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
    std::string question,
    std::string allowedChars,
    bool caseSensitive,
    char defaultAnswer) throw (ObjectNotInitialized) {

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
