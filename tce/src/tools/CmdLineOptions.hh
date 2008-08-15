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
