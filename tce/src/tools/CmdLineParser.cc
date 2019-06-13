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
 * @file CmdLineParser.cc
 *
 * Definition of CmdLineParser class.
 *
 * @author Jari Mäntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @note rating: red
 */


#include <cstdlib>
#include <vector>
#include <map>
#include <string>
#include <iomanip>

#include "CmdLineParser.hh"
#include "Exception.hh"
#include "Options.hh"

const int CmdLineParser::SHORT_FLAG = 2;
const int CmdLineParser::LONG_FLAG = 22;

using std::vector;
using std::map;
using std::string;
using std::setw;
using std::left;

/**
 * Constructor.
 *
 * @param description Brief description of the program and how to use it.
 * Only prefix is currently "no-".
 */
CmdLineParser::CmdLineParser(std::string description) :
    progName_(""), description_(description) {

    prefixes_.push_back(string("no-"));
}

/**
 * Destructor.
 */
CmdLineParser::~CmdLineParser() {
    mapIter i;
    for (i = optionLongNames_.begin(); i != optionLongNames_.end(); i++) {
        delete (*i).second;
    }
    optionShortNames_.clear();
    commandLine_.clear();
}

/**
 * Stores options to Options class.
 *
 * @param options Options-object where the options are stored.
 */
void
CmdLineParser::storeOptions(Options& options) {

    mapIter i = optionLongNames_.begin();
    for (; i != optionLongNames_.end(); i++) {
        string name = (*i).first;
        CmdLineOptionParser* current = (*i).second;
        options.addOptionValue(name, current->copy());
    }

    i = optionShortNames_.begin();
    for (; i != optionShortNames_.end(); i++) {
        string name = (*i).first;
        CmdLineOptionParser* current = (*i).second;
        options.addOptionValue(name, current->copy());
    }
}

/**
 * Loads all command line arguments and parses them.
 *
 * @param argv Command line options.
 * @param argc The number of command line options.
 * @exception IllegalCommandLine If parsing is not succesfull.
 * @exception ParserStopRequest If help or version option is found.
 */
void
CmdLineParser::parse(char* argv[], int argc) {
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
 * @param options Command line options pre-parsed in vector.
 * @param argc The number of command line options.
 * @exception IllegalCommandLine If parsing is not succesfull.
 * @exception ParserStopRequest If help or version option is found.
 */
void
CmdLineParser::parse(std::vector<std::string> options) {
    // command line is emptied
    commandLine_.clear();

    for (unsigned int i = 0; i < options.size(); i++) {
	bool spaces = true;
        string::size_type curPos = 0;
	while (spaces) {
 	    string::size_type pos = options[i].find(" ", curPos);
	    if (pos != string::npos) {
		commandLine_.push_back(string(options[i].substr(pos, curPos)));
		curPos = pos;
	    } else {
		if (options[i].length() - curPos > 0) {
		    commandLine_.push_back(string(options[i].substr(curPos)));
		}
		spaces = false;
	    }
	}
    }
    parseAll();
}

/**
 * Try to find a particular option.
 *
 * @param name The name of the option.
 * @return The option if it is found.
 * @exception IllegalCommandLine If option is not found.
 */
CmdLineOptionParser*
CmdLineParser::findOption(std::string name) const {
    constMapIter i = optionLongNames_.find(name);
    if (i != optionLongNames_.end()) {
        return (*i).second;
    }

    i = optionShortNames_.find(name);
    if (i != optionShortNames_.end()) {
        return (*i).second;
    }

    string msg = "Unknown option: " + name;
    string method = "CmdLineParser::findOption()";
    throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
}

/**
 * Parses all command line options.
 *
 * @exception IllegalCommandLine If parsing fails.
 */
void
CmdLineParser::parseAll() {
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

            if (!parseOption(optString, name, arguments, prefix, hasArgument)) {
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
                string method = "CmdLineParser::parse()";
                throw IllegalCommandLine(__FILE__, __LINE__, method, msg);
            }
            arguments_.push_back(optString);
        }
        i++;
    }
}

/**
 * Parses one option.
 *
 * Each option should have name and prefix (-, --, -no, or --no). Arguments are
 * mandatory for all except Boolean options.
 *
 * @param option The whole option.
 * @param name The name of the option.
 * @param arguments The arguments for option.
 * @param prefix The prefix of option.
 * @param hasArgument False if argument is part of option body (eg. -abc).
 * @return True if option is command line option, false if option is
 *         command line argument.
 * @exception IllegalCommandLine If option is illegal.
 */
bool
CmdLineParser::parseOption(
    std::string option, std::string& name, std::string& arguments,
    std::string& prefix, bool& hasArgument) const {
    // first there is either '-' or '--'
    bool longOption = false;
    if (!readPrefix(option, prefix, longOption)) {
        return false;
    }

    if (longOption) {
        // then there is the name of the option
        unsigned int pos = 0;
        while (pos < option.length() && option[pos] != '=') {
            ++pos;
        }
        name = option.substr(0, pos);
        option.erase(0, pos);
    } else {
        // option name is only one character
        name = option.substr(0, 1);
        option.erase(0, 1);
    }

    // then there might be value
    if (option.length() > 0 && option[0] == '=') {
        if (!longOption) {
            string method = "CmdLineParser::parseOption()";
            string message = "Illegal short option: = not allowed.";
            throw IllegalCommandLine(__FILE__, __LINE__, method, message);
        }
        option.erase(0, 1);
        arguments = option;
    } else {
        arguments = option;
        if (arguments == "" || !longOption) {
            hasArgument = false;
        }
    }
    return true;
}

/**
 * Reads prefix of option.
 *
 * @param option The whole option as a string.
 * @param prefix The prefix of option.
 * @param longOption True if option starts with "--".
 * @return True, if prefix is found, false otherwise.
 */
bool
CmdLineParser::readPrefix(
    std::string& option,
    std::string& prefix,
    bool& longOption) const {

    if (option == "--") {
        return false;
    } else if (option.substr(0, 1) != "-") {
        return false;
    } else {
        option.erase(0, 1);
        if (option.substr(0, 1) == "-") {
            longOption = true;
            option.erase(0, 1);
        }

        // then there might be also something else in the prefix
        // (eg. --no-print, prefix is --no)
        for (unsigned int i = 0; i < prefixes_.size(); i++) {

            if (option.length() > prefixes_[i].length() &&
                option.substr(0, prefixes_[i].length()) == prefixes_[i]) {

                prefix = prefixes_[i];
                option.erase(0, prefix.length());
                break;
            }
        }
    }
    return true;
}
