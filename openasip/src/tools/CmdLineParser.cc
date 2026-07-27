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

namespace po = boost::program_options;

const int CmdLineParser::SHORT_FLAG = 2;
const int CmdLineParser::LONG_FLAG = 22;

using std::vector;
using std::map;
using std::string;

/**
 * Constructor.
 *
 * @param description Brief description of the program and how to use it.
 * Only prefix is currently "no-".
 */
CmdLineParser::CmdLineParser(std::string description) :
    prefixes_(),
    visibleOptions_("Options"),
    hiddenOptions_("Hidden options"),
    progName_(""),
    description_(description) {

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
 * Add a new option to option data base and to the boost option descriptions.
 *
 * Option name must differ from any prefix.
 *
 * @param opt The option to be added.
 */
void
CmdLineParser::addOption(CmdLineOptionParser* opt) {
    assert(!isPrefix(opt->longName()));

    // Historical behavior: first registration for a long name wins.
    // Ignore duplicates so boost::program_options does not see the same
    // option twice (which causes ambiguous-option errors at parse time).
    std::pair<mapIter, bool> inserted =
        optionLongNames_.insert(valType(opt->longName(), opt));
    if (!inserted.second) {
        delete opt;
        return;
    }

    // if option has shorter alias, also it is added
    if (opt->shortName() != "") {
        optionShortNames_.insert(valType(opt->shortName(), opt));
    }

    registerBoostOption(opt);
}

/**
 * Register the option with boost::program_options.
 *
 * Value-bearing options are registered as strings so existing parseValue()
 * logic can validate and convert them. Boolean options also get a hidden
 * --no-<name> counterpart to preserve the historical "no-" prefix.
 *
 * @param opt The option to register.
 */
void
CmdLineParser::registerBoostOption(CmdLineOptionParser* opt) {
    if (registeredBoostNames_.count(opt->longName()) != 0) {
        return;
    }
    registeredBoostNames_.insert(opt->longName());

    string boostName = opt->longName();
    // Only single-character aliases are short options for boost. Some
    // callers incorrectly pass the long name as the "alias"; ignore those.
    if (opt->shortName().size() == 1 &&
        opt->shortName() != opt->longName()) {
        boostName += "," + opt->shortName();
    }

    po::options_description& target =
        opt->isHidden() ? hiddenOptions_ : visibleOptions_;

    if (dynamic_cast<BoolCmdLineOptionParser*>(opt) != NULL) {
        target.add_options()(
            boostName.c_str(), po::bool_switch(),
            opt->description().c_str());
        // Preserve historical --no-<longname> support for boolean flags.
        string noName = "no-" + opt->longName();
        if (registeredBoostNames_.count(noName) == 0) {
            registeredBoostNames_.insert(noName);
            hiddenOptions_.add_options()(
                noName.c_str(), po::bool_switch(), "");
        }
    } else if (dynamic_cast<OptionalStringCmdLineOptionParser*>(opt) !=
               NULL) {
        target.add_options()(
            boostName.c_str(),
            po::value<string>()->implicit_value(""),
            opt->description().c_str());
    } else {
        // Typed parsing and validation remain in CmdLineOptionParser.
        target.add_options()(
            boostName.c_str(), po::value<string>(),
            opt->description().c_str());
    }
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
 * Parses all command line options using boost::program_options.
 *
 * @exception IllegalCommandLine If parsing fails.
 */
void
CmdLineParser::parseAll() {
    arguments_.clear();

    po::options_description allOptions;
    allOptions.add(visibleOptions_);
    allOptions.add(hiddenOptions_);
    allOptions.add_options()(
        "__positional", po::value<vector<string> >(), "positional");

    po::positional_options_description positional;
    positional.add("__positional", -1);

    try {
        po::parsed_options parsed =
            po::command_line_parser(commandLine_)
                .options(allOptions)
                .positional(positional)
                .style(po::command_line_style::unix_style)
                .run();

        // Apply in command-line order so later flags override earlier ones
        // (e.g. -g followed by --no-gigolo).
        for (vector<po::option>::const_iterator i = parsed.options.begin();
             i != parsed.options.end(); ++i) {
            const po::option& opt = *i;

            if (opt.string_key.empty() || opt.string_key == "__positional") {
                for (vector<string>::const_iterator v = opt.value.begin();
                     v != opt.value.end(); ++v) {
                    arguments_.push_back(*v);
                }
                continue;
            }

            if (opt.string_key.compare(0, 3, "no-") == 0) {
                string realName = opt.string_key.substr(3);
                findOption(realName)->parseValue("", "no-");
                continue;
            }

            CmdLineOptionParser* parser = findOption(opt.string_key);
            if (dynamic_cast<BoolCmdLineOptionParser*>(parser) != NULL) {
                parser->parseValue("", "");
            } else if (opt.value.empty()) {
                parser->parseValue("", "");
            } else {
                parser->parseValue(opt.value.front(), "");
            }
        }
    } catch (const IllegalCommandLine&) {
        throw;
    } catch (const po::error& e) {
        string method = "CmdLineParser::parseAll()";
        throw IllegalCommandLine(__FILE__, __LINE__, method, e.what());
    }
}
