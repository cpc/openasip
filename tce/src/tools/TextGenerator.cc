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
 * @file TextGenerator.cc
 *
 * Implementation of TextGenerator class.
 *
 * @author Jussi Nyk�nen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka J��skel�inen 2005 (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 19 May 2004 by ml, jn, ao, am
 * @note rating: green
 */

#include <string>
#include <map>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include "MapTools.hh"
#include "Conversion.hh"
#include "Application.hh"
#include "TextGenerator.hh"

using Texts::TextGenerator;
using std::string;
using std::map;
using boost::format;


/**
 * Constructor.
 */
Texts::TextGenerator::TextGenerator() {
    addText(Texts::TXT_HELLO_WORLD, "Hello %s world!");
    addText(Texts::TXT_FILE_NOT_FOUND, "File not found.");
    addText(Texts::TXT_FILE_X_NOT_FOUND, "File %s not found.");
    addText(Texts::TXT_FILE_NOT_READABLE, "File not readable.");
    addText(Texts::TXT_ILLEGAL_INPUT_FILE, "Illegal input file.");
    addText(Texts::TXT_NO_FILENAME_DEFINED, "No filename defined.");
    addText(
        Texts::TXT_ONLY_ONE_FILENAME_EXPECTED,
        "Only one filename expected.");
    addText(Texts::TXT_NO_SUCH_SETTING, "No such setting.");
    addText(Texts::TXT_UNKNOWN_COMMAND, "Unknown command.");
    addText(Texts::TXT_UNKNOWN_SUBCOMMAND, "Unknown subcommand.");
    addText(Texts::TXT_ILLEGAL_ARGUMENT, "Illegal argument.");
    addText(Texts::TXT_ILLEGAL_ARGUMENTS, "Illegal arguments.");

}


/**
 * Destructor.
 *
 * Frees the allocated memory.
 */
Texts::TextGenerator::~TextGenerator() {
    for (MapIter mi = dataBase_.begin(); mi != dataBase_.end(); mi++) {
        delete (*mi).second;
    }
}


/**
 * Returns a format object that contains the template string.
 *
 * @param textId Numeric code that identifies the template string.
 * @return format object that contains the template string.
 * @exception KeyNotFound If textId doesn't identify any template string.
 */
format
Texts::TextGenerator::text(int textId) {
    if (!MapTools::containsKey(dataBase_, textId)) {
        string method = "TextGenerator::text()";
        string message("Requested message (code ");
        message += Conversion::toString(textId) + ") does not exist.";
        throw KeyNotFound(__FILE__, __LINE__, method, message);
    }

    MapIter mt = dataBase_.find(textId);
    const string& templateString = *((*mt).second);

    return format(templateString);
}

/**
 * Records a new template string that can be used to generate text messages.
 *
 * The template string is first copied to the dynamically allocated string,
 * just to ensure that it will be not destroyed before TextGenerator.
 *
 * @param textId Numeric code that identifies the template string.
 * @param templateString Template string to be recorded.
 */
void
Texts::TextGenerator::addText(int textId, const std::string& templateString) {
    string* toBeAdded = new string(templateString);
    dataBase_.insert(ValType(textId, toBeAdded));
}


/**
 * Replaces existing text with a new one.
 * 
 * @param textID ID of the text to be replaced
 * @param newString new string to be set
 * @exception KeyNotFound Thrown if a text of given ID couldn't be found.
 */
void 
Texts::TextGenerator::replaceText(int textId, const std::string& newString) {
    if (!MapTools::containsKey(dataBase_, textId)) {
        string message("Requested message (code ");
        message += Conversion::toString(textId) + ") does not exist.";
        throw KeyNotFound(__FILE__, __LINE__, __FUNCTION__, message);
    }

    delete dataBase_[textId];
    string* toBeAdded = new string(newString);
    dataBase_[textId] = toBeAdded;
}

