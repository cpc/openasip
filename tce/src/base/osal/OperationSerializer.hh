/**
 * @file OperationSerializer.hh
 *
 * Declaration of OperationSerializer class.
 *
 * @author Jussi Nykänen 2004 (nykanen@cs.tut.fi)
 * @note rating: yellow
 * @note reviewed 7 September 2004 by pj, jn, jm, ao
 */

#ifndef TTA_OPERATION_SERIALIZER_HH
#define TTA_OPERATION_SERIALIZER_HH

#include <string>

#include "Serializer.hh"
#include "XMLSerializer.hh"
#include "Exception.hh"

class ObjectState;

/**
 * Class that constructs an ObjectState tree that represents the information
 * contents of an operation module from a given external file.
 */
class OperationSerializer : public Serializer {
public:
    OperationSerializer();
    virtual ~OperationSerializer();

    virtual void writeState(const ObjectState* state)
        throw (SerializerException);
    virtual ObjectState* readState()
        throw (SerializerException);
   
    void setSourceFile(const std::string& filename);
    void setDestinationFile(const std::string& filename);
    void setSchemaFile(const std::string& filename);
    void setUseSchema(bool useSchema);

private:
    /// Copying not allowed.
    OperationSerializer(const OperationSerializer&);
    /// Assignment not allowed.
    OperationSerializer& operator=(const OperationSerializer&);

    ObjectState* convertToXMLFormat(const ObjectState* state)
        throw (ObjectStateLoadingException);
    ObjectState* convertToOperationFormat(const ObjectState* state)
        throw (ObjectStateLoadingException);

    ObjectState* toOperation(const ObjectState* state)
        throw (ObjectStateLoadingException);
    ObjectState* toXMLFormat(const ObjectState* state)
        throw (ObjectStateLoadingException);

    void setOperandProperties(ObjectState* operand, ObjectState* source);

    /// Constructs the ObjectState tree from the XML file.
    XMLSerializer serializer_;
};

#endif
