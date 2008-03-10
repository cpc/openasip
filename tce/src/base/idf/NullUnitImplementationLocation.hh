/**
 * @file NullUnitImplementationLocation.hh
 *
 * Declaration of NullUnitImplementationLocation class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_NULL_UNIT_IMPLEMENTATION_LOCATION_HH
#define TTA_NULL_UNIT_IMPLEMENTATION_LOCATION_HH

#include "Serializable.hh"
#include "UnitImplementationLocation.hh"

namespace IDF {

/**
 * Indicates what implementation of a unit is used. 
 *
 * A null class that indicates of unset location of implementation or
 * no implementation available.
 */
class NullUnitImplementationLocation : public UnitImplementationLocation  {
public:
    static NullUnitImplementationLocation& instance();

    virtual std::string hdbFile() const
        throw (FileNotFound);
    virtual int id() const;
    virtual std::string unitName() const;
    virtual void setParent(MachineImplementation& parent)
        throw (InvalidData);

protected:
    NullUnitImplementationLocation();
    virtual ~NullUnitImplementationLocation();

private:
    // the singleton instance of this class
    static NullUnitImplementationLocation instance_;
};
}

#endif
