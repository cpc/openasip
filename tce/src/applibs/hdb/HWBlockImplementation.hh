/**
 * @file HWBlockImplementation.hh
 *
 * Declaration of HWBlockImplementation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_HW_BLOCK_IMPLEMENTATION_HH
#define TTA_HW_BLOCK_IMPLEMENTATION_HH

#include <string>
#include <vector>

#include "Exception.hh"
#include "DBTypes.hh"

namespace HDB {

class BlockImplementationFile;

/**
 * Base class for FUImplementation and RFImplementation.
 */
class HWBlockImplementation {
public:
    HWBlockImplementation(
        const std::string& moduleName,
        const std::string& clkPort,
        const std::string& rstPort,
        const std::string& glockPort);
    
    HWBlockImplementation(const HWBlockImplementation& b);

    virtual ~HWBlockImplementation();

    bool hasID() const;
    void setID(RowID id);
    RowID id() const
        throw (NotAvailable);

    void setModuleName(const std::string& name);
    std::string moduleName() const;
    void setClkPort(const std::string& name);
    std::string clkPort() const;
    void setRstPort(const std::string& name);
    std::string rstPort() const;
    void setGlockPort(const std::string& name);
    std::string glockPort() const;

    void addImplementationFile(BlockImplementationFile* file);

    void removeImplementationFile(const BlockImplementationFile& file);
    int implementationFileCount() const;
    BlockImplementationFile& file(int index) const
        throw (OutOfRange);

private:
    /// Vector type for BlockImplementationFile.
    typedef std::vector<BlockImplementationFile*> FileTable;

    /// Name of the module.
    std::string moduleName_;
    /// Name of the clock port.
    std::string clkPort_;
    /// Name of the reset port.
    std::string rstPort_;
    /// Name of the global lock port.
    std::string glockPort_;
    /// Contains the block implementation files.
    FileTable files_;
    /// Tells whether the implementation has an ID.
    bool hasID_;

    /// ID of the implementation.
    RowID id_;
};
}

#endif
