/**
 * @file BEMSerializer.hh
 *
 * Declaration of BEMSerializer class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#ifndef TTA_BEM_SERIALIZER_HH
#define TTA_BEM_SERIALIZER_HH

#include "XMLSerializer.hh"

class BinaryEncoding;

/**
 * BEMSerializer reads and writes BEM (Binary Encoding Map) files.
 */
class BEMSerializer : public XMLSerializer {
public:
    BEMSerializer();
    virtual ~BEMSerializer();

    virtual ObjectState* readState()
	throw (SerializerException);
    virtual void writeState(const ObjectState* state)
	throw (SerializerException);
    BinaryEncoding* readBinaryEncoding()
	throw (SerializerException, ObjectStateLoadingException);
    void writeBinaryEncoding(const BinaryEncoding& bem)
	throw (SerializerException);

private:
    static ObjectState* convertToOMFormat(const ObjectState* fileState);
    static ObjectState* convertToFileFormat(const ObjectState* omState);

    static ObjectState* socketCodeTableToFile(const ObjectState* scTable);
    static ObjectState* longImmTagToFile(const ObjectState* immTag);
    static ObjectState* moveSlotToFile(const ObjectState* slotState);
    static ObjectState* guardFieldToFile(const ObjectState* gFieldState);
    static ObjectState* sourceFieldToFile(const ObjectState* sFieldState);
    static ObjectState* destinationFieldToFile(
        const ObjectState* dFieldState);
    static ObjectState* slotFieldToFile(const ObjectState* sFieldState);
    static ObjectState* immediateSlotToFile(const ObjectState* slotState);
    static ObjectState* longImmDstRegisterFieldToFile(
        const ObjectState* omState);

    static ObjectState* socketCodeTableToOM(const ObjectState* scTable);
    static ObjectState* longImmTagToOM(const ObjectState* fileTag);
    static ObjectState* moveSlotToOM(const ObjectState* fileSlot);
    static ObjectState* guardFieldToOM(const ObjectState* fileGuard);
    static ObjectState* sourceFieldToOM(const ObjectState* fileSource);
    static ObjectState* destinationFieldToOM(
        const ObjectState* fileDestination);
    static ObjectState* slotFieldToOM(const ObjectState* fileSlot);
    static ObjectState* immediateSlotToOM(const ObjectState* fileSlot);
    static ObjectState* longImmDstRegFieldToOM(const ObjectState* fileField);

    static void readRegisterFilePortCode(
        const ObjectState* filePortCode,
        ObjectState* omPortCode);
};

#endif
