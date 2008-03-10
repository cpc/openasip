/**
 * @file IDFSerializer.hh
 *
 * Declaration of IDFSerializer class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_IDF_SERIALIZER_HH
#define TTA_IDF_SERIALIZER_HH

#include "XMLSerializer.hh"

namespace IDF {

class MachineImplementation;

/**
 * Writer and reader for IDF files.
 */
class IDFSerializer : public XMLSerializer {
public:
    IDFSerializer();
    virtual ~IDFSerializer();

    virtual ObjectState* readState()
        throw (SerializerException);
    virtual void writeState(const ObjectState* state)
        throw (SerializerException);

    MachineImplementation* readMachineImplementation()
        throw (SerializerException, ObjectStateLoadingException);
    void writeMachineImplementation(
        const MachineImplementation& implementation)
        throw (SerializerException);

private:
    static ObjectState* convertToOMFormat(const ObjectState* fileState);
    static ObjectState* convertToFileFormat(const ObjectState* omState);
};
}

#endif
