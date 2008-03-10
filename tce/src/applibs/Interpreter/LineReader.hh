/**
 * @file LineReader.hh
 *
 * Declaration of LineReader class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
