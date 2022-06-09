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
 * @file ConfigurationFile.hh
 *
 * Declaration of ConfigurationFile class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @author Pekka Jääskeläinen 2007 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_CONFIGURATION_FILE_HH
#define TTA_CONFIGURATION_FILE_HH

#include <map>
#include <string>
#include <vector>

#include "Exception.hh"
#include "TCEString.hh"

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
    std::string value(const std::string& key, int index = 0);

    int itemCount(const std::string& key);

    int intValue(const std::string& key, int index = 0);
    float floatValue(const std::string& key, int index = 0);
    std::string stringValue(const std::string& key, int index = 0);
    bool booleanValue(const std::string& key, int index = 0);

    unsigned int timeStampValue(const std::string& key);

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
    typedef std::map<TCEString, std::vector<TCEString> > ValueContainer;

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
    std::string valueOfKey(const std::string& key, int index);

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
