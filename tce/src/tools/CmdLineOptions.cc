/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file CmdLineOptions.cc
 *
 * Definition of CmdLineOptions class.
 *
 * @author Jussi Nyk‰nen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Jari M‰ntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @author Esa M‰‰tt‰ 2007 (esa.maatta-no.spam-tut.fi)
 * @note reviewed 3 December 2003 by jn, kl, ao
 */


#include <cstdlib>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <iomanip>

#include "CmdLineOptions.hh"
#include "Exception.hh"

#include "CmdLineOptionParser.hh"

const int CmdLineOptions::SHORT_FLAG = 2;
const int CmdLineOptions::LONG_FLAG = 22;

const std::string CmdLineOptions::VERBOSE_SWITCH = "verbose";

using std::vector;
using std::map;
using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::setw;
using std::left;

/**
 * Constructor.
 *
 * @param description Brief description of the program and how to use it.
 * Only prefix is currently "no-".
 */
CmdLineOptions::CmdLineOptions(
    std::string description,
    std::string version) :
    CmdLineParser(description),
    progName_(""),
    description_(description),
    version_(version) {

    BoolCmdLineOptionParser* verboseSwitch = 
        new BoolCmdLineOptionParser(
            VERBOSE_SWITCH, "The verbose switch", "v");
    addOption(verboseSwitch);
}

/**
 * Destructor.
 */
CmdLineOptions::~CmdLineOptions() {
}

/**
 * Loads all command line arguments and parses them.
 *
 * @param options Command line options pre-parsed in char* array.
 * @param argc The number of command line options.
 * @exception IllegalCommandLine If parsing is not succesfull.
 * @exception ParserStopRequest If help or version option is found.
 */
void
CmdLineOptions::parse(char* argv[], int argc)
    throw (IllegalCommandLine, ParserStopRequest) {

    // command line is emptied
    commandLine_.clear();
    progName_ = string(argv[0]);

    for (int i = 1; i < argc; i++) {
        commandLine_.push_back(string(argv[i]));
    }

    parseAll();
}

/**
 * Loads all command line arguments and parses them.
 *
 * @param options Command line options pre-parsed in string array.
 * @param argc The number of command line options.
 * @exception IllegalCommandLine If parsing is not succesfull.
 * @exception ParserStopRequest If help or version option is found.
 */
void
CmdLineOptions::parse(string argv[], int argc)
    throw (IllegalCommandLine, ParserStopRequest) {

    // command line is emptied
    commandLine_.clear();
    progName_ = argv[0];

    for (int i = 1; i < argc; i++) {
        commandLine_.push_back(argv[i]);
    }

    parseAll();
}

/**
 * Parses all command line options.
 *
 * @exception IllegalCommandLine If parsing fails.
 * @exception ParserStopRequest The client should not proceed.
 */
void
CmdLineOptions::parseAll()
    throw (IllegalCommandLine, ParserStopRequest) {

    // finished is set to true when options are parsed and the rest are
    // command line arguments
    bool finished = false;

    // checkArguments is set to false when command line arguments can start
    // with "-" or "--"
    bool checkArguments = true;
    unsigned int i = 0;

    while (i < commandLine_.size()) {
        string optString = commandLine_[i];

        if (!finished) {
            string prefix = "";
            string name = "";
            string arguments = "";

            // hasArgument is true when option has argument
            bool hasArgument = true;

            if (!parseOption(
		    optString, name, arguments, prefix, hasArgument)) {
                finished = true;
                if (optString == "--") {
                    checkArguments = false;
                    i++;
                    continue;
                } else {
                    arguments_.push_back(optString);
                    i++;
                    continue;
                }
            }

	    //
            if (name == "help" || name == "h") {
                printHelp();
		string msg = "The client should not proceed";
		string method = "CmdLineParser::parseAll()";
		throw ParserStopRequest(__FILE__, __LINE__, method, msg);
            } else if (name == "version") {
                printVersion();
                string msg = "The client should not proceed";
		string method = "CmdLineParser::parseAll()";
		throw ParserStopRequest(__FILE__, __LINE__, method, msg);
            }

            CmdLineOptionParser* opt = findOption(name);

            if (arguments == "" &&
                dynamic_cast<BoolCmdLineOptionParser*>(opt) == NULL) {

                // argument for an option may be separated with space
                if (i < commandLine_.size() - 1 &&
                    commandLine_[i+1].substr(0, 1) != "-") {
                    hasArgument = true;
                    arguments = commandLine_[i+1];
                    i++;
                }
            }

            bool doneWithParsing = opt->parseValue(arguments, prefix);

            if (!doneWithParsing) {
                if (hasArgument) {
                    // all the rest in command line are "extra" strings
                    arguments_.push_back(arguments);
                    finished = true;
                    i++;
                } else {
                    // this is the situation when we have something like
                    // -abcd (multible flags put together)
                    for (unsigned int i = 0; i < arguments.length(); i++) {
                        opt = findOption(arguments.substr(i, 1));
                        opt->parseValue("", prefix);
                    }
                }
            }
        } else {

            // finished reading options, all rest are command line arguments
            if (checkArguments && optString[0] == '-') {
                string msg = "Illegal command line argument: " + optString;
                string method = "CmdLineOptions::parse()";
                throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
            }
            arguments_.push_back(optString);
        }
        i++;
    }
}

/**
 * Prints the help menu of the program.
 */
void
CmdLineOptions::printHelp() const {
    cout << description_ << endl;
    cout << "Options:" << endl;
    constMapIter i;
    for (i = optionLongNames_.begin(); i != optionLongNames_.end(); i++) {
        CmdLineOptionParser* opt = findOption((*i).first);
        cout << left << setw(SHORT_FLAG) << "-" + opt->shortName() + ", "
             << left << setw(LONG_FLAG) << "--" + opt->longName()
             << opt->description() << endl;
    }
}

/**
 * Return true if the verbose switch was defined in the command line.
 *
 * @return True if the verbose switch was defined in the command line.
 */
bool
CmdLineOptions::isVerboseSwitchDefined() const {
    return findOption(VERBOSE_SWITCH)->isDefined();
}

/**
 * Returns true if there is a value available for given option.
 *
 * @return True if the option is defined.
 */
bool
CmdLineOptions::optionGiven(std::string key) const {
    try {
        /// @todo: This returns always true if trying to find added
        /// CmdLineOption... 
        return findOption(key)->isDefined();
    } catch (const IllegalCommandLine&) {
        return false;
    }
    return true;
}
