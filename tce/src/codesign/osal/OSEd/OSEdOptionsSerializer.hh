/**
 * @file OSEdOptionsSerializer.hh
 *
 * Declaration of OSEdOptionsSerializer class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_OSED_OPTIONS_SERIALIZER_HH
#define TTA_OSED_OPTIONS_SERIALIZER_HH

#include <string>

#include "Serializer.hh"
#include "XMLSerializer.hh"

/**
 * Constructs the ObjectState tree that represents the options of OSEd.
 */
class OSEdOptionsSerializer : public Serializer {
public:
    OSEdOptionsSerializer();
    virtual ~OSEdOptionsSerializer();

    virtual void writeState(const ObjectState* state)
        throw (SerializerException);
    virtual ObjectState* readState()
        throw (SerializerException);

    void setSourceFile(const std::string& fileName);
    void setDestinationFile(const std::string& fileName);
    void setSchemaFile(const std::string& fileName);
    void setUseSchema(bool useSchema);

private:
    /// Copying not allowed.
    OSEdOptionsSerializer(const OSEdOptionsSerializer&);
    /// Assignment not allowed.
    OSEdOptionsSerializer& operator=(const OSEdOptionsSerializer&);

    /// Instance of XMLSerializer.
    XMLSerializer serializer_;
};

#endif
