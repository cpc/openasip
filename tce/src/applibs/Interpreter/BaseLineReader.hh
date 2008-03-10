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
