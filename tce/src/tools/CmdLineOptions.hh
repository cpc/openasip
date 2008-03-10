/**
 * @file CmdLineOptions.hh
 *
 * Declaration of class CmdLineOptions.
 *
 * @author Jussi Nykänen 2003 (nykanen@cs.tut.fi)
 * @author Jari Mäntyneva 2006 (jari.mantyneva@tut.fi)
 * @note reviewed 3 December 2003 by jn, kl, ao
 * @note rating: red
 */

#ifndef TTA_CMD_LINE_OPTIONS_HH
#define TTA_CMD_LINE_OPTIONS_HH

#include <map>
#include <vector>
#include <string>

#include "CmdLineParser.hh"
#include "CmdLineOptionParser.hh"
#include "Exception.hh"
#include "Application.hh"

/**
 * Abstract base class for command line parsers.
 *
 * Is capable of storing and parsing commmand line options.
 */
class CmdLineOptions : public CmdLineParser {
public:
    CmdLineOptions(std::string description, std::string version = "");
    virtual ~CmdLineOptions();

    void parse(char* argv[], int argc)
	throw (IllegalCommandLine, ParserStopRequest);
    void parse(string argv[], int argc)
	throw (IllegalCommandLine, ParserStopRequest);
    virtual void printHelp() const;
    virtual void printVersion() const = 0;

private:
    /// For adding new values to maps.
    typedef  std::map<std::string, CmdLineOptionParser*>::value_type valType;
    /// For traversing non-const maps.
    typedef std::map<std::string, CmdLineOptionParser*>::iterator mapIter;
    /// For traversing const maps.
    typedef
    std::map<std::string, CmdLineOptionParser*>::const_iterator constMapIter;

    /// Copying not allowed.
    CmdLineOptions(const CmdLineOptions&);
    /// Assignment not allowed.
    CmdLineOptions& operator=(const CmdLineOptions&);

    void parseAll() throw (IllegalCommandLine, ParserStopRequest);

    /// The name of the program.
    std::string progName_;
    /// The description of usage of program.
    std::string description_;
    /// The version of the program.
    std::string version_;

    /// Number of characters reserved for printing short version
    /// of commandline flag.
    static const int SHORT_FLAG;

    /// Number of characters reserved for printing long version
    /// of commandline flag.
    static const int LONG_FLAG;
};

#endif
