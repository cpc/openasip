/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file ProcessorConfigurationFile.cc
 *
 * Definition of ProcessorConfigurationFile class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include "ProcessorConfigurationFile.hh"
#include "Conversion.hh"
#include "FileSystem.hh"

using std::string;

const string ProcessorConfigurationFile::ARCHITECTURE = "Architecture";
const string ProcessorConfigurationFile::ARCHITECTURE_SIZE = "ArchitectureSize";
const string ProcessorConfigurationFile::ARCHITECTURE_MODIFIED =
"ArchitectureModified";
const string ProcessorConfigurationFile::ENCODING_MAP = "EncodingMap";
const string ProcessorConfigurationFile::ENCODING_MAP_SIZE = "EncodingSize";
const string ProcessorConfigurationFile::ENCODING_MAP_MODIFIED =
"EncodingModified";
const string ProcessorConfigurationFile::IMPLEMENTATION = "Implementation";
const string ProcessorConfigurationFile::IMPLEMENTATION_SIZE =
"ImplementationSize";
const string ProcessorConfigurationFile::IMPLEMENTATION_MODIFIED =
"ImplementationModified";

/**
 * Constructor.
 *
 * @param inputStream Stream where configuration file is read from.
 */
ProcessorConfigurationFile::ProcessorConfigurationFile(
    std::istream& inputStream) : ConfigurationFile(true), pcfDir_(".") {

    addSupportedKey(ARCHITECTURE, ConfigurationFile::VT_STRING, true);
    addSupportedKey(ARCHITECTURE_SIZE, ConfigurationFile::VT_INTEGER, true);
    addSupportedKey(ARCHITECTURE_MODIFIED, ConfigurationFile::VT_READABLE_TIME,
                    true);

    addSupportedKey(ENCODING_MAP, ConfigurationFile::VT_STRING, true);
    addSupportedKey(ENCODING_MAP_SIZE, ConfigurationFile::VT_INTEGER, true);
    addSupportedKey(ENCODING_MAP_MODIFIED, ConfigurationFile::VT_READABLE_TIME,
                    true);

    addSupportedKey(IMPLEMENTATION, ConfigurationFile::VT_STRING, true);
    addSupportedKey(IMPLEMENTATION_SIZE, ConfigurationFile::VT_INTEGER, true);
    addSupportedKey(IMPLEMENTATION_MODIFIED,
                    ConfigurationFile::VT_READABLE_TIME, true);

    load(inputStream);
}

/**
 * Destructor.
 */
ProcessorConfigurationFile::~ProcessorConfigurationFile() {
}


/**
 * Sets the directory of PCF file itself.
 *
 * Path of the ADF, IDF and BEM files are constructed relative to the
 * PCF directory if a relative path is given in PCF.
 *
 * @param directory The directory of the PCF file.
 */
void
ProcessorConfigurationFile::setPCFDirectory(const std::string& directory) {
    pcfDir_ = directory;
}


/**
 * Returns the name of the architecture.
 *
 * @return The name of the architecture.
 * @exception KeyNotFound If architecture is not found.
 */
string
ProcessorConfigurationFile::architectureName()
    throw (KeyNotFound) {

    return realPath(value(ARCHITECTURE));
}


/**
 * Returns the size of the architecture.
 *
 * @return The size of the architecture.
 * @exception KeyNotFound If architecture size is not found.
 */
unsigned int
ProcessorConfigurationFile::architectureSize()
    throw (KeyNotFound) {

    return intValue(ARCHITECTURE_SIZE);
}

/**
 * Returns the timestamp of the architecture modified time.
 *
 * @return Architecture modified timestamp.
 * @exception KeyNotFound If architecture modified time is not found.
 */
unsigned int
ProcessorConfigurationFile::architectureModified()
    throw (KeyNotFound) {

    return timeStampValue(ARCHITECTURE_MODIFIED);
}

/**
 * Returns the name of the implementation.
 *
 * @return The name of the implementation.
 * @exception KeyNotFound If implementation is not found.
 */
string
ProcessorConfigurationFile::implementationName()
    throw (KeyNotFound) {

    return realPath(value(IMPLEMENTATION));
}

/**
 * Returns the size of the implementation.
 *
 * @return The size of the implementation.
 * @exception KeyNotFound If implementation size is not found.
 */
unsigned int
ProcessorConfigurationFile::implementationSize()
    throw (KeyNotFound) {

    return intValue(IMPLEMENTATION_SIZE);
}

/**
 * Returns the implementation modified timestamp.
 *
 * @return Timestamp of implementation modified time.
 * @exception KeyNotFound If implementation modified timestamp is not found.
 */
unsigned int
ProcessorConfigurationFile::implementationModified()
    throw (KeyNotFound) {

    return timeStampValue(IMPLEMENTATION_MODIFIED);
}

/**
 * Returns the name of the encoding map.
 *
 * @return The name of the encoding map.
 * @exception KeyNotFound If encoding map is not found.
 */
string
ProcessorConfigurationFile::encodingMapName()
    throw (KeyNotFound) {

    return realPath(value(ENCODING_MAP));
}

/**
 * Returns the size of the encoding map.
 *
 * @return The size of the encoding map.
 * @exception KeyNotFound If encoding map size is not found.
 */
unsigned int
ProcessorConfigurationFile::encodingMapSize()
    throw (KeyNotFound) {

    return intValue(ENCODING_MAP_SIZE);
}

/**
 * Returns the timestamp of encoding map modified time.
 *
 * @return The timestamp of encoding map modified time.
 * @exception KeyNotFound If timestamp is not found.
 */
unsigned int
ProcessorConfigurationFile::encodingMapModified()
    throw (KeyNotFound) {

    return timeStampValue(ENCODING_MAP_MODIFIED);
}

/**
 * Returns the number of errors.
 *
 * @return The number of errors.
 */
int
ProcessorConfigurationFile::errorCount() {
    return errors_.size();
}

/**
 * Returns the error string with a given index.
 *
 * If such error string is not found, returns empty string.
 *
 * @return Error string with a given index.
 */
string
ProcessorConfigurationFile::errorString(int index) {

    if (index < 0 || index > static_cast<int>(errors_.size()) - 1) {
        return "";
    }
    return errors_[index];
}

/**
 * Returns true if errors were found.
 *
 * @return True if errors were found.
 */
bool
ProcessorConfigurationFile::errors() {
    return !errors_.empty();
}

/**
 * Handles the errors.
 *
 * Error string are added in internal data structure.
 *
 * @param lineNumber The line number.
 * @param error The type of the error.
 * @param line The line in which error occurred.
 * @return True.
 */
bool
ProcessorConfigurationFile::handleError(
    int lineNumber,
    ConfigurationFile::ConfigurationFileError error,
    const std::string& line) {

    string errorString;
    switch (error) {
    case ConfigurationFile::FE_SYNTAX:
        errorString += "Syntax error ";
        break;
    case ConfigurationFile::FE_ILLEGAL_TYPE:
        errorString += "Illegal type error ";
        break;
    case ConfigurationFile::FE_UNKNOWN_KEY:
        errorString += "Unknown key error ";
        break;
    case ConfigurationFile::FE_MISSING_VALUE:
        errorString += "Missing value error ";
        break;
    }
    errorString += "in line " + Conversion::toString(lineNumber) +
        ": " + line;

    errors_.push_back(errorString);
    return true;
}


/**
 * Creates real path to the file referred to in PCF.
 *
 * The path is constructed assuming the PCF is in the directory set with
 * setPCFDirectory method.
 *
 * @param pathInPCF The path that reads in the PCF.
 * @return Real path to the file.
 */
std::string
ProcessorConfigurationFile::realPath(const std::string& pathInPCF) const {
    if (FileSystem::isAbsolutePath(pathInPCF) || pcfDir_ == "") {
        return pathInPCF;
    } else {
        return pcfDir_ + FileSystem::DIRECTORY_SEPARATOR + pathInPCF;
    }
}
