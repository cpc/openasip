/**
 * @file Serializable.hh
 *
 * Declaration of Serializable interface.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen@tut.fi)
 */

#ifndef TTA_SERIALIZABLE_HH
#define TTA_SERIALIZABLE_HH

class ObjectState;


/**
 * Objects which are going to be serialized using XMLSerializer or any
 * other serializer must implement this interface.
 */
class Serializable {
public:
    /**
     * Loads the state of the object from the given ObjectState object.
     *
     * @param state ObjectState object from which (and the children of
     *              which) the state is loaded.
     */
    virtual void loadState(const ObjectState* state) = 0;

    /**
     * Saves the objects state into an ObjectState object and maybe its
     * child objects.
     *
     * @return The root of the ObjectState tree created.
     */
    virtual ObjectState* saveState() const = 0;
    virtual ~Serializable() {}
};

#endif
