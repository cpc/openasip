/**
 * @file CmdLineParser.hh
 *
 * Declaration of class CmdLineParser.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Jari Mäntyneva 2006 (jari.mantyneva@tut.fi)
 * @note reviewed 3 December 2003 by jn, kl, ao
 * @note rating: red
 */

#ifndef TTA_CMD_LINE_PARSER_HH
#define TTA_CMD_LINE_PARSER_HH

#include <map>
#include <vector>
#include <string>

#include "CmdLineOptionParser.hh"
#include "Exception.hh"
#include "Application.hh"
#include "Options.hh"
#include "OptionValue.hh"

/**
 * Abstract base class for command line parsers.
 *
 * Is capable of storing and parsing commmand line options.
 */
class CmdLineParser {
public:
    CmdLineParser(std::string description);
    virtual ~CmdLineParser();

    virtual void parse(char* argv[], int argc)
	throw (IllegalCommandLine, ParserStopRequest);
    virtual void parse(std::vector<std::string> options)
	throw (IllegalCommandLine, ParserStopRequest);
    virtual void storeOptions(Options& options);

    virtual int numberOfArguments() const;
    virtual std::string argument(int index) const
        throw (OutOfRange);

protected:
    void addOption(CmdLineOptionParser* opt);
    CmdLineOptionParser* findOption(std::string name) const
        throw (IllegalCommandLine);

    bool parseOption(
        std::string option,
        std::string& name,
        std::string& arguments,
        std::string& prefix,
        bool& hasArgument) const throw (IllegalCommandLine);
    bool readPrefix(
        std::string& option,
        std::string& prefix,
        bool& longOption) const;
    bool isPrefix(std::string name) const;

    /// Database for holding options with their long names as a key.
    std::map<std::string, CmdLineOptionParser*> optionLongNames_;
    /// Database for holding options with their short names as a key.
    std::map<std::string, CmdLineOptionParser*> optionShortNames_;

    /// Command line is stored here.
    std::vector<std::string> commandLine_;
    /// Command line arguments are stored here.
    std::vector<std::string> arguments_;
    /// Legal prefixes are stored here.
    std::vector<std::string> prefixes_;

private:
    /// For adding new values to maps.
    typedef std::map<std::string, CmdLineOptionParser*>::value_type valType;
    /// For traversing non-const maps.
    typedef std::map<std::string, CmdLineOptionParser*>::iterator mapIter;
    /// For traversing const maps.
    typedef
    std::map<std::string, CmdLineOptionParser*>::const_iterator constMapIter;

    /// Copying not allowed.
    CmdLineParser(const CmdLineParser&);
    /// Assignment not allowed.
    CmdLineParser& operator=(const CmdLineParser&);

    void parseAll() throw (IllegalCommandLine);

    /// The name of the program.
    std::string progName_;
    /// The description of usage of program.
    std::string description_;

    /// Number of characters reserved for printing short version
    /// of commandline flag.
    static const int SHORT_FLAG;

    /// Number of characters reserved for printing long version
    /// of commandline flag.
    static const int LONG_FLAG;
};

#include "CmdLineParser.icc"

#endif
