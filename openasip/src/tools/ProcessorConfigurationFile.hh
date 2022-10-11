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
 * @file ProcessorConfigurationFile.hh
 *
 * Declaration of ProcessorConfigurationFile class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PROCESSOR_CONFIGURATION_FILE_HH
#define TTA_PROCESSOR_CONFIGURATION_FILE_HH

#include <vector>

#include "ConfigurationFile.hh"

/**
 * Has knowledge of the format of the processor configuration file.
 *
 * It knows its supported keys and provides shortcut methods for querying
 * the values of the keys.
 */
class ProcessorConfigurationFile : public ConfigurationFile {
public:
    explicit ProcessorConfigurationFile(std::istream& inputStream);
    virtual ~ProcessorConfigurationFile();

    void setPCFDirectory(const std::string& path);

    std::string architectureName();
    unsigned int architectureSize();
    unsigned int architectureModified();

    std::string implementationName();
    unsigned int implementationSize();
    unsigned int implementationModified();

    std::string encodingMapName();
    unsigned int encodingMapSize();
    unsigned int encodingMapModified();

    int errorCount();
    std::string errorString(int index);

    bool errors();

protected:

    virtual bool handleError(
        int lineNumber,
        ConfigurationFile::ConfigurationFileError error,
        const std::string& line);

private:
    std::string realPath(const std::string& pathInPCF) const;

    static const std::string ARCHITECTURE;
    static const std::string ARCHITECTURE_SIZE;
    static const std::string ARCHITECTURE_MODIFIED;

    static const std::string ENCODING_MAP;
    static const std::string ENCODING_MAP_SIZE;
    static const std::string ENCODING_MAP_MODIFIED;

    static const std::string IMPLEMENTATION;
    static const std::string IMPLEMENTATION_SIZE;
    static const std::string IMPLEMENTATION_MODIFIED;

    std::vector<std::string> errors_;

    /**
     * Directory of the PCF file, affects the path returned when ADF, IDF or
     * BEM is requested.
     */
    std::string pcfDir_;
};

#endif
