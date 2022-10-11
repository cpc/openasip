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
 * @file CustomCommand.cc
 *
 * Definition of CustomCommand class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 27 May 2004 by pj, jn, vpj, ll
 * @note rating: green
 */

#include <string>
#include <cctype>
#include "CustomCommand.hh"
#include "Conversion.hh"

using std::string;


/**
 * Constructor.
 *
 * @param name The name of the command.
 */
CustomCommand::CustomCommand(std::string name) : 
    name_(name), context_(NULL), interpreter_(NULL) {
}

/**
 * Copy Constructor.
 *
 * @param cmd CustomCommand to be copied.
 */
CustomCommand::CustomCommand(const CustomCommand& cmd) :
    name_(cmd.name()), context_(cmd.context()), 
    interpreter_(cmd.interpreter()) {
}


/**
 * Destructor.
 */
CustomCommand::~CustomCommand() {
}

/**
 * Checks if the count of arguments fits between the given minimum and
 * maximum arguments.
 *
 * If the count of arguments is wrong, sets the error message and
 * returns false.
 *
 * @param argumentCount The count of the arguments.
 * @param minimum Minimum amount of arguments.
 * @param maximum Maximum amount of arguments.
 * @return True if the count of arguments is valid.
 */
bool
CustomCommand::checkArgumentCount(
    int argumentCount, 
    int minimum, 
    int maximum) {

    if (argumentCount < minimum) {
        DataObject* result = new DataObject();
        result->setString("Not enough arguments.");
        interpreter()->setResult(result);
        return false;
    } else if (argumentCount > maximum) {
        DataObject* result = new DataObject();
        result->setString("Too many arguments.");
        interpreter()->setResult(result);
        return false;
    }    
    return true;
}

/**
 * Checks if the given argument is an integer.
 *
 * If argument is not integer, sets the error message and returns false.
 *
 * @param argument The argument to check.
 * @return True if argument is integer.
 */
bool
CustomCommand::checkIntegerArgument(const DataObject& argument) {
    try {
        argument.integerValue();
    } catch (const NumberFormatException&) {
        DataObject* result = new DataObject();
        result->setString("Argument not integer as expected.");
        interpreter()->setResult(result);
        return false;
    }
    return true;
}

/**
 * Checks if the given argument is a positive integer (zero is included).
 *
 * If argument is not positive integer, sets the error message and 
 * returns false. Note that this function assumes that the value range
 * is that of an 'int', i.e., the maximum positive value is less than
 * that of an 'unsigned int'.
 *
 * @param argument The argument to check.
 * @return True if argument is integer.
 */
bool
CustomCommand::checkPositiveIntegerArgument(const DataObject& argument) {
    bool argumentOk = false;
    try {
        argumentOk = (argument.integerValue() >= 0);
        
    } catch (const NumberFormatException&) {
        argumentOk = false;
    }

    if (!argumentOk) {
        DataObject* result = new DataObject();
        result->setString("Argument not positive integer as expected.");
        interpreter()->setResult(result);
        return false;
    }

    return true;
}

/**
 * Checks if the given argument is an unsigned integer.
 *
 * If argument is not unsigned integer, sets the error message and 
 * returns false. This function assumes that the value range of the argument
 * is that of an 'unsigned int'.
 *
 * @param argument The argument to check.
 * @return True if argument is integer.
 */
bool
CustomCommand::checkUnsignedIntegerArgument(const DataObject& argument) {

    try {
        if (argument.stringValue().size() > 0) {
            // ensure the first char is a digit, not a '-'
            if (isdigit(argument.stringValue().at(0))) {
                // this should throw NumberFormatException if the argument
                // cannot be converted to an int
                argument.integerValue();
                return true;
            }
        } 
    } catch (const NumberFormatException&) {
    }

    // some of the checks failed, it's not an unsigned int
    DataObject* result = new DataObject();
    result->setString("Argument not positive integer as expected.");
    interpreter()->setResult(result);
    return false;
}

/**
 * Checks if the given argument is a valid double
 *
 * If the argument is not double, sets the error message and returns false.
 *
 * @param argument The argument to check.
 * @return True if argument is double.
 */
bool
CustomCommand::checkDoubleArgument(const DataObject& argument) {
    try {
        argument.doubleValue();
    } catch (const NumberFormatException&) {
        DataObject* result = new DataObject();
        result->setString("Argument not double as expected.");
        interpreter()->setResult(result);
        return false;
    }
    return true;
}
