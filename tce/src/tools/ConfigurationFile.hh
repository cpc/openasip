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
 * @file ConfigurationFile.hh
 *
 * Declaration of ConfigurationFile class.
 *
 * @author Jussi Nyk‰nen 2004 (nykanen@cs.tut.fi)
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONFIGURATION_FILE_HH
#define TTA_CONFIGURATION_FILE_HH

#include <map>
#include <string>
#include <vector>

#include "Exception.hh"

/**
 * Class that is able to read files with key-value-pairs, such as the processor
 * configuration file.
 *
 * Can do simple sanity and type checking of the file while loading it.
 */
class ConfigurationFile {
public:
    ConfigurationFile(bool doChecking = false);
    virtual ~ConfigurationFile();

    void load(std::istream& inputStream);

    bool hasKey(const std::string& key);
    std::string value(const std::string& key, int index = 0)
        throw (KeyNotFound, OutOfRange);

    int itemCount(const std::string& key)
        throw (KeyNotFound);

    int intValue(const std::string& key, int index = 0)
        throw (KeyNotFound, OutOfRange, InvalidData);
    float floatValue(const std::string& key, int index = 0)
        throw (KeyNotFound, OutOfRange, InvalidData);
    std::string stringValue(const std::string& key, int index = 0)
        throw (KeyNotFound, OutOfRange);
    bool booleanValue(const std::string& key, int index = 0)
        throw (KeyNotFound, OutOfRange, InvalidData);

    unsigned int timeStampValue(const std::string& key)
        throw (KeyNotFound);

protected:

    /**
     * Different value types.
     */
    enum ConfigurationValueType {
        VT_INTEGER,       ///< Integer value.
        VT_FLOAT,         ///< Float value.
        VT_STRING,        ///< String value.
        VT_BOOLEAN,       ///< Boolean value.
        VT_READABLE_TIME, ///< Time in readable format.
        VT_UNIX_TIMESTAMP ///< Time as seconds since starting of 1970.
    };

    /**
     * Error types.
     */
    enum ConfigurationFileError {
        FE_SYNTAX,        ///< Syntax error.
        FE_ILLEGAL_TYPE,  ///< Illegal type error.
        FE_UNKNOWN_KEY,   ///< Unknown key error.
        FE_MISSING_VALUE  ///< Missing value error.
    };

    void addSupportedKey(
        const std::string& key,
        ConfigurationValueType type,
        bool caseSensitive = false);

    virtual bool handleError(
        int lineNumber,
        ConfigurationFileError error,
        const std::string& line);

private:
    struct Key;
    typedef std::map<std::string, std::vector<std::string> > ValueContainer;

    // potential indeterminism?
    typedef std::map<Key*, ConfigurationValueType> KeyContainer;

    void parse(std::istream& inputStream);
    bool checkSemantics(
        const std::string& key,
        const std::string& value,
        int lineNumber,
        const std::string& line);
    bool isComment(const std::string& line);
    bool legalTime(const std::string& line);
    std::string valueOfKey(const std::string& key, int index)
        throw (KeyNotFound, OutOfRange);

    /**
     * Key.
     */
    struct Key {
        /**
         * Constructor.
         */
        Key() : name_(""), caseSensitive_(false) {}
        /// Name of the key.
        std::string name_;
        /// True if name is case sensitive.
        bool caseSensitive_;
    };

    /// True if semantics of the configuration file is checked.
    bool check_;
    /// Contains all the values of configuration file.
    ValueContainer values_;
    /// Contains all the legal keys of the configuration file.
    KeyContainer keys_;
};

#endif
