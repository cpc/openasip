/**
 * @file BlockSourceCopier.hh
 *
 * Declaration of BlockSourceCopier class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BLOCK_SOURCE_COPIER_HH
#define TTA_BLOCK_SOURCE_COPIER_HH

#include <set>
#include "RFImplementationLocation.hh"

namespace IDF {
    class MachineImplementation;
}

namespace HDB {
    class HWBlockImplementation;
}

namespace ProGe {

/**
 * Copies the block definition files used in the processor implementation to
 * the output directory of ProGe.
 */
class BlockSourceCopier {
public:
    BlockSourceCopier(const IDF::MachineImplementation& implementation);
    virtual ~BlockSourceCopier();

    void copy(const std::string& dstDirectory)
        throw (IOException);

private:
    void copyBaseRFFiles(
        const IDF::RFImplementationLocation& implementation,
        const std::string& dstDirectory)
        throw (IOException);
    void copyFiles(
        const HDB::HWBlockImplementation& implementation,
        const std::string& hdbFile,
        const std::string& dstDirectory)
        throw (UnreachableStream, FileNotFound);
    void setCopied(const std::string& file);
    bool isCopied(const std::string& file) const;

    /// The IDF object model.
    const IDF::MachineImplementation& implementation_;
    /// Copied files.
    std::set<std::string> copiedFiles_;
};
}

#endif
