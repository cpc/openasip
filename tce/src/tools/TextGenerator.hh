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
 * @file TextGenerator.hh
 *
 * Declaration of TextGenerator class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
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

        virtual boost::format text(int textId)
            throw (KeyNotFound);
        virtual void addText(int textId, const std::string& templateString);
        virtual void replaceText(int textId, const std::string& newString);

    private:
        /// value_type for map.
        typedef std::map<int, const std::string*>::value_type ValType;
        /// Iterator for map.
        typedef std::map<int, const std::string*>::iterator MapIter;

        /// Copying not allowed.
        TextGenerator(const TextGenerator&);
        /// Assignment not allowed.
        TextGenerator& operator=(const TextGenerator&);

        /// Database that contains all template strings.
        std::map<int, const std::string*> dataBase_;
    };
}

#endif
