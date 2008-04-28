/**
 * @file UnitImplementationLocation.hh
 *
 * Declaration of UnitImplementationLocation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_UNIT_IMPLEMENTATION_HH
#define TTA_UNIT_IMPLEMENTATION_HH

#include "Serializable.hh"
#include "Exception.hh"

namespace IDF {

class MachineImplementation;

/**
 * Indicates what implementation of a unit is used. 
 *
 * Refers to an entry in an HDB.
 */
class UnitImplementationLocation : public Serializable {
public:
    UnitImplementationLocation(
        const std::string& hdbFile,
        int id,
        const std::string& unitName);
    UnitImplementationLocation(const ObjectState* state)
        throw (ObjectStateLoadingException);
    virtual ~UnitImplementationLocation();

    virtual std::string hdbFile() const
        throw (FileNotFound);
    virtual int id() const;
    virtual std::string unitName() const;
    virtual void setParent(MachineImplementation& parent)
        throw (InvalidData);

    virtual void setID(int id);
    virtual void setHDBFile(std::string file);

    // methods from Serializable interface
    void loadState(const ObjectState* state)
        throw (ObjectStateLoadingException);
    ObjectState* saveState() const;

    /// ObjectState name for unit implementation.
    static const std::string OSNAME_UNIT_IMPLEMENTATION;
    /// ObjectState attribute key for the name of the HDB file.
    static const std::string OSKEY_HDB_FILE;
    /// ObjectState attribute key for the entry ID.
    static const std::string OSKEY_ID;
    /// Objectstate attribute key for the name of the unit.
    static const std::string OSKEY_UNIT_NAME;

private:
    /// Name of the HDB file.
    std::string hdbFile_;
    /// Entry ID in the HDB.
    int id_;
    /// Name of the unit in ADF.
    std::string unitName_;
    /// The parent MachineImplementation instance.
    MachineImplementation* parent_;
};
}

#endif
