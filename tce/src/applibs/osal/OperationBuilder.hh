/**
 * @file OperationBuilder.cc
 *
 * Declaration of OperationBuilder class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OPERATION_BUILDER_HH
#define TTA_OPERATION_BUILDER_HH

#include <string>
#include <vector>

/**
 * Class that handles building the operation behavior.
 */
class OperationBuilder {
public:
    static OperationBuilder& instance();

    std::string xmlFilePath(const std::string& xmlFile);
    std::string behaviorFile(
        const std::string& baseName,
        std::string& path);

    bool buildObject(const std::string& baseName,
                     const std::string& behaviorFile,
                     const std::string& path,
                     std::vector<std::string>& output);

    bool installDataFile(
        const std::string& path,
        const std::string& xmlFile,
        const std::string& destination);

    bool verifyXML(const std::string file);

private:
    OperationBuilder();
    virtual ~OperationBuilder();

    std::string makeIncludeString(const std::vector<std::string>& paths);

    /// Static unique instance.
    static OperationBuilder* instance_;
};

#endif
