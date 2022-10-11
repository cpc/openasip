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
 * @file CmdLineParser.hh
 *
 * Declaration of class CmdLineParser.
 *
 * @author Jussi Nyk�nen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Jari M�ntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
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

class Options;

/**
 * Abstract base class for command line parsers.
 *
 * Is capable of storing and parsing commmand line options.
 */
class CmdLineParser {
public:
    CmdLineParser(std::string description);
    virtual ~CmdLineParser();

    virtual void parse(char* argv[], int argc);
    virtual void parse(std::vector<std::string> options);
    virtual void storeOptions(Options& options);
    
    virtual int numberOfArguments() const;
    virtual std::string argument(int index) const;

protected:
    void addOption(CmdLineOptionParser* opt);
    CmdLineOptionParser* findOption(std::string name) const;

    bool parseOption(
        std::string option, std::string& name, std::string& arguments,
        std::string& prefix, bool& hasArgument) const;
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

    void parseAll();

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
