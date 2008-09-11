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
 * @file CommandLineTest.hh
 *
 * Test Suite for command line parser.
 *
 * @author Jussi Nykänen 2003 (nykanen-no.spam-cs.tut.fi)
 * @author Jari Mäntyneva 2006 (jari.mantyneva-no.spam-tut.fi)
 * @note reviewed 3 December 2003 by jn, kl, ao
 */

#ifndef TTA_COMMAND_LINE_TEST_HH
#define TTA_COMMAND_LINE_TEST_HH

#include <TestSuite.h>
#include <string>

#include "Application.hh"
#include "CmdLineOptions.hh"
#include "CmdLineOptionParser.hh"
#include "Exception.hh"
#include "Conversion.hh"

#include <iostream>
using std::cout;
using std::endl;
using std::cerr;

using std::string;

//////////////////////////////////////////////////////////////////////////////
// JussiOptions
//////////////////////////////////////////////////////////////////////////////

/**
 * Concrete derived CmdLineOptions class to test command line parsing.
 */
class JussiOptions : public CmdLineOptions {
public:
    JussiOptions(string desc);
    virtual ~JussiOptions();
    
    virtual void printVersion() const;
    virtual void printHelp() const;
    
    int integer(string name);
    string String(string name);
    bool boolean(string name);
    double real(string name);
    int listSize(string list);
    int listValue(int index, string list);
};

/**
 * Constructor.
 *
 * All options used by JussiOptions are initialized in costructor and added
 * to Options.
 *
 * @param desc The description of the program which uses JussiOptions.
 */
JussiOptions::JussiOptions(string desc) : CmdLineOptions(desc) {
    IntegerCmdLineOptionParser* iOpt = 
        new IntegerCmdLineOptionParser("agre", "", "a");
    addOption(iOpt);

    iOpt = new IntegerCmdLineOptionParser("bus", "", "b");
    addOption(iOpt);

    iOpt = new IntegerCmdLineOptionParser("caesar", "", "c");
    addOption(iOpt);

    StringCmdLineOptionParser* sOpt =
        new StringCmdLineOptionParser("dilemma", "", "d");
    addOption(sOpt);

    sOpt = new StringCmdLineOptionParser("eritrea", "", "e");
    addOption(sOpt);

    sOpt = new StringCmdLineOptionParser("Filthy", "", "f");
    addOption(sOpt);

    BoolCmdLineOptionParser* bOpt =
        new BoolCmdLineOptionParser("gigolo", "", "g");
    addOption(bOpt);

    bOpt = new BoolCmdLineOptionParser("hiawatha", "", "h");
    addOption(bOpt);

    bOpt = new BoolCmdLineOptionParser("immortal", "", "i");
    addOption(bOpt);

    RealCmdLineOptionParser* rOpt =
        new RealCmdLineOptionParser("jiggy", "", "j");
    addOption(rOpt);
    
    rOpt = new RealCmdLineOptionParser("king", "", "k");
    addOption(rOpt);

    rOpt = new RealCmdLineOptionParser("lame", "", "l");
    addOption(rOpt);

    IntegerListCmdLineOptionParser* isOpt = 
        new IntegerListCmdLineOptionParser("musashi", "", "m");
    addOption(isOpt);

    isOpt = new IntegerListCmdLineOptionParser("niagara", "", "n");
    addOption(isOpt);
}

/**
 * Destructor.
 */
JussiOptions::~JussiOptions() {
}

/**
 * Prints the version of program which uses JussiOptions.
 */
void
JussiOptions::printVersion() const {
}

/**
 * Prints the help menu.
 */
void
JussiOptions::printHelp() const {
}

/**
 * Returns the value of integer option.
 *
 * @param name The name of the option.
 * @return The value of integer option.
 */
int
JussiOptions::integer(string name) {
    CmdLineOptionParser* opt = findOption(name);
    return opt->integer();
}

/**
 * Returns the value of string option.
 *
 * @param name The name of the option.
 * @return The value of string option. 
 */
string
JussiOptions::String(string name) {
    CmdLineOptionParser* opt = findOption(name);
    return opt->String();
}

/**
 * Returns the value of boolean option.
 * 
 * @param name The name of the option.
 * @return The value of boolean option.
 */
bool
JussiOptions::boolean(string name) {
    CmdLineOptionParser* opt = findOption(name);
    return opt->isFlagOn();
}

/**
 * Returns the value of real option.
 *
 * @param name The name of the option.
 * @return The value of real option.
 */
double
JussiOptions::real(string name) {
    CmdLineOptionParser* opt = findOption(name);
    return opt->real();
}

int
JussiOptions::listSize(string list) {
    CmdLineOptionParser* opt = findOption(list);
    return opt->listSize();
}

/**
 * Returns the particular value of integer list option.
 *
 * @param index The index of element wanted.
 * @param list The name of the integer list option.
 * @return The value of a particular integer list element.
 */
int
JussiOptions::listValue(int index, string list) {
    CmdLineOptionParser* opt = findOption(list);
    return opt->integer(index);
}

//////////////////////////////////////////////////////////////////////////////
// CommandLineTest
//////////////////////////////////////////////////////////////////////////////

/**
 * The test class for Command Line Parser.
 *
 * All types of options are test, as well as erronous options.
 */
class CommandLineTest : public CxxTest::TestSuite {
public:

    /**
     * Sets test environment.
     *
     * Not used.
     */
    void setUp() {};

    /**
     * Destroys test environment.
     * 
     * Not used.
     */
    void tearDown() {};

    void testIntegerOptions();
    void testStringOptions();
    void testRealOptions();
    void testBoolOptions();
    void testIntegerListOptions();
    void testAllOptions();
    void testErronousCommandLine();
    void testHelpAndVersion();
    
private:
    /// The number of integer options.
    static const int INT_OPTIONS = 7;
    /// The number of string options.
    static const int STRING_OPTIONS = 7;
    /// The number of real options.
    static const int REAL_OPTIONS = 5;
    /// The number of boolean options.
    static const int BOOL_OPTIONS = 4;
    /// The number of options in integer list.
    static const int INTLIST_OPTIONS = 4;
    /// The number of options.
    static const int ALL_OPTIONS = 11;
    /// The number of erronous options.
    static const int OPT_ERROR = 2;

    /// Command line containing only integer options.
    static string integerCommandLine[INT_OPTIONS];
    /// Command line containing only string options.
    static string stringCommandLine[STRING_OPTIONS];
    /// Command line containing only real options.
    static string realCommandLine[REAL_OPTIONS];
    /// Command line containing only boolean options.
    static string boolCommandLine[BOOL_OPTIONS];
    /// Command line containing only integer list options.
    static string integerListCommandLine[INTLIST_OPTIONS];
    /// Command line containing all types of options.
    static string commandLine[ALL_OPTIONS];
    
    /// First erronous command line.
    static string erronousCommandLine1[OPT_ERROR];
    /// Second erronous command line.
    static string erronousCommandLine2[OPT_ERROR];
    /// Third erronous command line.
    static string erronousCommandLine3[OPT_ERROR];
    /// Fourth erronous command line.
    static string erronousCommandLine4[OPT_ERROR + 1];
    /// Fifth erronous command line.
    static string erronousCommandLine5[OPT_ERROR];
    /// Sixth erronous command line.
    static string erronousCommandLine6[OPT_ERROR];
    /// Seventh erronous command line.
    static string erronousCommandLine7[OPT_ERROR];
    /// Eighth erronous command line.
    static string erronousCommandLine8[OPT_ERROR];
    /// Ninth erronous command line.
    static string erronousCommandLine9[OPT_ERROR];
    /// Tenth erronous command line.
    static string erronousCommandLine10[OPT_ERROR];
    /// Eleventh erronous command line.
    static string erronousCommandLine11[OPT_ERROR + 2];
    /// Twelth erronous command line.
    static string erronousCommandLine12[OPT_ERROR];
    /// Thirteenth erronous command line.
    static string erronousCommandLine13[OPT_ERROR];
};

const int CommandLineTest::INT_OPTIONS;
const int CommandLineTest::STRING_OPTIONS;
const int CommandLineTest::REAL_OPTIONS;
const int CommandLineTest::BOOL_OPTIONS;
const int CommandLineTest::INTLIST_OPTIONS;
const int CommandLineTest::OPT_ERROR;

string CommandLineTest::integerCommandLine[] = {
    "./test", "-a3200", "--bus", "789", "-c5000", "foo", "bar" 
};

string CommandLineTest::stringCommandLine[] = {
    "./test", "-djabajaa", "--eritrea", "jarjar", "--Filthy=/home/d_vader",
    "--", "-foobar"
};

string CommandLineTest::realCommandLine[] = {
    "./test", "-j45.89", "--king", "45789.5", "--lame=1231.7"
};

string CommandLineTest::boolCommandLine[] = {
    "./test", "-gi", "--no-hiawatha", "foo.dat" 
};

string CommandLineTest::integerListCommandLine[] = {
    "./test", "--musashi=4,5,6,7,9", "-n", "111"
};

string CommandLineTest::commandLine[] = {
    "./test", "-ghi", "--no-gigolo", "--king=2.45", "-n3,4",
    "--niagara", "5,6", "-eFooBar", "--", "/home/nykanen/foo",
    "--foo--"
};

string CommandLineTest::erronousCommandLine1[] = {
    "./test", "-int3200"
};

string CommandLineTest::erronousCommandLine2[] = {
    "./test", "--int=tampere"
};

string CommandLineTest::erronousCommandLine3[] = {
    "./test", "--secondboolean=3200"
};

string CommandLineTest::erronousCommandLine4[] = {
    "./test", "--int", "5400tre"
};

string CommandLineTest::erronousCommandLine5[] = {
    "./test", "-a=1"
};

string CommandLineTest::erronousCommandLine6[] = {
    "./test", "-m,2,3,4"
};

string CommandLineTest::erronousCommandLine7[] = {
    "./test", "--musashi=2,3,4,5foo"
};

string CommandLineTest::erronousCommandLine8[] = {
    "./test", "-g hi"
};

string CommandLineTest::erronousCommandLine9[] = {
    "./test", "--hiawatha i=3"
};

string CommandLineTest::erronousCommandLine10[] = {
    "./test", "--jiggy=32.45.3"
};

string CommandLineTest::erronousCommandLine11[] = {
    "./test", "-j 3.2", "foo.dat", "--dumdum"
};

string CommandLineTest::erronousCommandLine12[] = {
    "./test", "--no-bus=8"
};

string CommandLineTest::erronousCommandLine13[] = {
    "./test", "--agre"
};

/**
 * Tests that integer options are read correctly.
 */
void
CommandLineTest::testIntegerOptions() {
    
    JussiOptions options("");
    TS_ASSERT_THROWS_NOTHING(
        options.parse(integerCommandLine, INT_OPTIONS));

    TS_ASSERT_EQUALS(options.integer("a"), 3200);
    TS_ASSERT_EQUALS(options.integer("agre"), 3200);
    
    TS_ASSERT_EQUALS(options.integer("b"), 789);
    TS_ASSERT_EQUALS(options.integer("bus"), 789);
    
    TS_ASSERT_EQUALS(options.integer("c"), 5000);
    TS_ASSERT_EQUALS(options.integer("caesar"), 5000);
    
    TS_ASSERT_EQUALS(options.numberOfArguments(), 2);
    TS_ASSERT_EQUALS(options.argument(1), "foo");
    TS_ASSERT_EQUALS(options.argument(2), "bar");
}

/**
 * Tests that string options are read correctly.
 */
void
CommandLineTest::testStringOptions() {
    
    JussiOptions options("");
    TS_ASSERT_THROWS_NOTHING(
        options.parse(stringCommandLine, STRING_OPTIONS));
    
    TS_ASSERT_EQUALS(options.String("d"), "jabajaa");
    TS_ASSERT_EQUALS(options.String("dilemma"), "jabajaa");
    
    TS_ASSERT_EQUALS(options.String("e"), "jarjar");
    TS_ASSERT_EQUALS(options.String("eritrea"), "jarjar");
    
    TS_ASSERT_EQUALS(options.String("f"), "/home/d_vader");
    TS_ASSERT_EQUALS(options.String("Filthy"), "/home/d_vader");
    
    TS_ASSERT_EQUALS(options.numberOfArguments(), 1);
    TS_ASSERT_EQUALS(options.argument(1), "-foobar");
}

/**
 * Tests that real options are read correctly.
 */
void
CommandLineTest::testRealOptions() {
    
    JussiOptions options("");
    TS_ASSERT_THROWS_NOTHING(
        options.parse(realCommandLine, REAL_OPTIONS));
    
    TS_ASSERT_EQUALS(options.real("j"), 45.89);
    TS_ASSERT_EQUALS(options.real("jiggy"), 45.89);
    
    TS_ASSERT_EQUALS(options.real("k"), 45789.5);
    TS_ASSERT_EQUALS(options.real("king"), 45789.5);
    
    TS_ASSERT_EQUALS(options.real("l"), 1231.7);
    TS_ASSERT_EQUALS(options.real("lame"), 1231.7);
}

/**
 * Tests that boolean options are read correctly.
 */
void
CommandLineTest::testBoolOptions() {
    
    JussiOptions options("");
    TS_ASSERT_THROWS_NOTHING(
        options.parse(boolCommandLine, BOOL_OPTIONS));
    
    TS_ASSERT_EQUALS(options.boolean("g"), true);
    TS_ASSERT_EQUALS(options.boolean("gigolo"), true);
    
    TS_ASSERT_EQUALS(options.boolean("h"), false);
    TS_ASSERT_EQUALS(options.boolean("hiawatha"), false);
    
    TS_ASSERT_EQUALS(options.boolean("i"), true);
    TS_ASSERT_EQUALS(options.boolean("immortal"), true);
    
    TS_ASSERT_EQUALS(options.numberOfArguments(), 1);
    TS_ASSERT_EQUALS(options.argument(1), "foo.dat");
}

/**
 * Tests that integer list options are read correctly.
 */
void
CommandLineTest::testIntegerListOptions() {     
   
    JussiOptions options("");
    TS_ASSERT_THROWS_NOTHING(
        options.parse(integerListCommandLine, INTLIST_OPTIONS));
    
    TS_ASSERT_EQUALS(options.listSize("m"), 5);
    TS_ASSERT_EQUALS(options.listSize("musashi"), 5);
    
    TS_ASSERT_EQUALS(options.listValue(1, "m"), 4);
    TS_ASSERT_EQUALS(options.listValue(1, "musashi"), 4);
    
    TS_ASSERT_EQUALS(options.listValue(5, "m"), 9);
    TS_ASSERT_EQUALS(options.listValue(5, "musashi"), 9);
    
    TS_ASSERT_EQUALS(options.listSize("n"), 1);
    TS_ASSERT_EQUALS(options.listSize("niagara"), 1);
        
    TS_ASSERT_EQUALS(options.listValue(1, "n"), 111);
    TS_ASSERT_EQUALS(options.listValue(1, "niagara"), 111);
}

/**
 * Tests all kind of options.
 */
void
CommandLineTest::testAllOptions() {     
    
    JussiOptions options("");
    TS_ASSERT_THROWS_NOTHING(
        options.parse(commandLine, ALL_OPTIONS));
    
    TS_ASSERT_EQUALS(options.boolean("g"), false);
    TS_ASSERT_EQUALS(options.boolean("gigolo"), false);
    
    TS_ASSERT_EQUALS(options.boolean("h"), true);
    TS_ASSERT_EQUALS(options.boolean("hiawatha"), true);
    
    TS_ASSERT_EQUALS(options.boolean("i"), true);
    TS_ASSERT_EQUALS(options.boolean("immortal"), true);
    
    TS_ASSERT_EQUALS(options.real("k"), 2.45);
    TS_ASSERT_EQUALS(options.real("king"), 2.45);
    
    TS_ASSERT_EQUALS(options.listSize("n"), 4);
    TS_ASSERT_EQUALS(options.listSize("niagara"), 4);
    
    TS_ASSERT_EQUALS(options.listValue(1, "n"), 3);
    TS_ASSERT_EQUALS(options.listValue(4, "niagara"), 6);
    
    TS_ASSERT_EQUALS(options.String("e"), "FooBar");
    TS_ASSERT_EQUALS(options.String("eritrea"), "FooBar");
    
    TS_ASSERT_EQUALS(options.numberOfArguments(), 2);
    
    TS_ASSERT_EQUALS(options.argument(1), "/home/nykanen/foo");
    TS_ASSERT_EQUALS(options.argument(2), "--foo--");
}

/**
 * Tests that errors in command line are found.
 */
void 
CommandLineTest::testErronousCommandLine() {
    
    JussiOptions options("");
    TS_ASSERT_THROWS(
        options.parse(erronousCommandLine1, OPT_ERROR),
        IllegalCommandLine);
    
    TS_ASSERT_THROWS(
        options.parse(erronousCommandLine2, OPT_ERROR),
        IllegalCommandLine);
    
    TS_ASSERT_THROWS(
        options.parse(erronousCommandLine3, OPT_ERROR),
        IllegalCommandLine);
    
    TS_ASSERT_THROWS(
        options.parse(erronousCommandLine4, OPT_ERROR + 1),
        IllegalCommandLine);
    
    TS_ASSERT_THROWS(
        options.parse(erronousCommandLine5, OPT_ERROR),
        IllegalCommandLine);
    
    TS_ASSERT_THROWS(
        options.parse(erronousCommandLine6, OPT_ERROR),
        IllegalCommandLine);
    
    TS_ASSERT_THROWS(
        options.parse(erronousCommandLine7, OPT_ERROR),
        IllegalCommandLine);
    
    TS_ASSERT_THROWS(
        options.parse(erronousCommandLine8, OPT_ERROR),
        IllegalCommandLine);
    
    TS_ASSERT_THROWS(
        options.parse(erronousCommandLine9, OPT_ERROR),
        IllegalCommandLine);
    
    TS_ASSERT_THROWS(
        options.parse(erronousCommandLine10, OPT_ERROR),
        IllegalCommandLine);
    
    TS_ASSERT_THROWS(
        options.parse(erronousCommandLine11, OPT_ERROR + 2),
        IllegalCommandLine);

    TS_ASSERT_THROWS(
        options.parse(erronousCommandLine12, OPT_ERROR),
        IllegalCommandLine);
    
    TS_ASSERT_THROWS(
        options.parse(erronousCommandLine13, OPT_ERROR),
        IllegalCommandLine);
}

/**
 * Just to add test coverage.
 */
void
CommandLineTest::testHelpAndVersion() {
    JussiOptions options("");
    string cmdLine[] = {"./foo", "-h"};
    string cmdLine2[] = {"./foo", "--version"};

    TS_ASSERT_THROWS(options.parse(cmdLine, 2), ParserStopRequest);
    TS_ASSERT_THROWS(options.parse(cmdLine2, 2), ParserStopRequest);
}

#endif
