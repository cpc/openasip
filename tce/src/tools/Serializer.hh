/**
 * @file Serializer.hh
 *
 * Declaration of Serializer interface.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen@tut.fi)
 */

#ifndef TTA_SERIALIZER_HH
#define TTA_SERIALIZER_HH

#include "ObjectState.hh"
#include "Serializable.hh"
#include "Exception.hh"

/**
 * All serializers must implement this interface.
 */
class Serializer {
public:
    /**
     * Serializes the given ObjectState tree.
     *
     * @param state Root of the ObjectState tree.
     * @exception SerializerException If an error occurs while serializing.
     */
    virtual void writeState(const ObjectState* state)
        throw (SerializerException) = 0;

    /**
     * Creates an ObjectState tree according to the input.
     *
     * @return The created ObjectState tree.
     * @exception SerializerException If an error occurs while reading input.
     */
    virtual ObjectState* readState()
        throw (SerializerException) = 0;

    virtual ~Serializer() {}
};


#endif
