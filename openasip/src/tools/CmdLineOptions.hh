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
 * @file CmdLineOptions.hh
 *
 * Declaration of class CmdLineOptions.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Jari Mäntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @note reviewed 3 December 2003 by jn, kl, ao
 * @note rating: red
 */

#ifndef TTA_CMD_LINE_OPTIONS_HH
#define TTA_CMD_LINE_OPTIONS_HH

#include <map>
#include <vector>
#include <string>

#include "CmdLineParser.hh"
//#include "CmdLineOptionParser.hh"
#include "Exception.hh"
#include "Application.hh"

class CmdLineOptionParser;

/**
 * Abstract base class for command line parsers.
 *
 * Is capable of storing and parsing commmand line options.
 */
class CmdLineOptions : public CmdLineParser {
public:
    CmdLineOptions(std::string description, std::string version = "");
    virtual ~CmdLineOptions();

    void parse(char* argv[], int argc);
    void parse(std::string argv[], int argc);
    void parse(std::vector<std::string> argv);
    virtual void printHelp() const;
    virtual void printVersion() const = 0;

    virtual bool isVerboseSwitchDefined() const;
    virtual bool isVerboseSpamSwitchDefined() const;

    CmdLineOptions(const CmdLineOptions&) = delete;
    CmdLineOptions& operator=(const CmdLineOptions&) = delete;

protected:
    bool optionGiven(std::string key) const;
private:
    /// For adding new values to maps.
    typedef  std::map<std::string, CmdLineOptionParser*>::value_type valType;
    /// For traversing non-const maps.
    typedef std::map<std::string, CmdLineOptionParser*>::iterator mapIter;
    /// For traversing const maps.
    typedef
    std::map<std::string, CmdLineOptionParser*>::const_iterator constMapIter;


    void parseAll();

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

    /// Switch for verbose output listing scheduler modules
    static const std::string VERBOSE_SWITCH;

    /// Switch for verbose output listing spam from scheduler internals
    static const std::string VERBOSE_SPAM_SWITCH;

};

#endif
