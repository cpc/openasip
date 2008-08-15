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
 * @file CustomCommand.cc
 *
 * Definition of CustomCommand class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2005 (pjaaskel@cs.tut.fi)
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
