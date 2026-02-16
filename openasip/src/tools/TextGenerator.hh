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
 * @file TextGenerator.hh
 *
 * Declaration of TextGenerator class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note reviewed 19 May 2004 by ml, jn, ao, am
 * @note rating: green
 */

#ifndef TTA_TEXT_GENERATOR_HH
#define TTA_TEXT_GENERATOR_HH

#include <string>
#include <map>
#include <boost/format.hpp>

#include "Exception.hh"

namespace Texts {

    /**
     * Enumeration containing all text ids.
     *
     * Text ids are used to achieve a right template string.
     * In your own TextGenerator class you should define your enum like this:
     * enum {
     *    TXT_SOMETHING = LAST__
     *    ...
     * };
     */
    enum {
        TXT_HELLO_WORLD = 0, ///< For testing. Do not remove.
        TXT_FILE_NOT_FOUND,
        TXT_FILE_X_NOT_FOUND,
        TXT_FILE_NOT_READABLE,
        TXT_ILLEGAL_INPUT_FILE,
        TXT_NO_FILENAME_DEFINED,
        TXT_ONLY_ONE_FILENAME_EXPECTED,
        TXT_NO_SUCH_SETTING,
        TXT_UNKNOWN_COMMAND,
        TXT_UNKNOWN_SUBCOMMAND,
        TXT_ILLEGAL_ARGUMENT,
        TXT_ILLEGAL_ARGUMENTS,
        LAST__
    };

    /**
     * The class that holds template strings and formats them.
     *
     * Template strings are added with addText function by giving the text id
     * and the template string. Formatted string are achieved by calling text()
     * with text id and parameters. For example text(TXT_HELLO_WORLD, "all")
     * returns "Hello all world".
     */
    class TextGenerator {

    public:
        TextGenerator();
        virtual ~TextGenerator();

        virtual boost::format text(int textId);
        virtual void addText(int textId, const std::string& templateString);
        virtual void replaceText(int textId, const std::string& newString);

        /// value_type for map.
        typedef std::map<int, const std::string*>::value_type ValType;
        /// Iterator for map.
        typedef std::map<int, const std::string*>::iterator MapIter;

        TextGenerator(const TextGenerator&) = delete;
        TextGenerator& operator=(const TextGenerator&) = delete;

    private:
        /// Database that contains all template strings.
        std::map<int, const std::string*> dataBase_;
    };
}

#endif
