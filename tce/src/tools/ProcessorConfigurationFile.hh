/**
 * @file ProcessorConfigurationFile.hh
 *
 * Declaration of ProcessorConfigurationFile class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
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

    std::string architectureName() throw (KeyNotFound);
    unsigned int architectureSize() throw (KeyNotFound);
    unsigned int architectureModified() throw (KeyNotFound);

    std::string implementationName() throw (KeyNotFound);
    unsigned int implementationSize() throw (KeyNotFound);
    unsigned int implementationModified() throw (KeyNotFound);

    std::string encodingMapName() throw (KeyNotFound);
    unsigned int encodingMapSize() throw (KeyNotFound);
    unsigned int encodingMapModified() throw (KeyNotFound);

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
