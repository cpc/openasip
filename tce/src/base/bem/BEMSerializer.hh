/*
    Copyright (c) 2002-2009 Tampere University of Technology.

    This file is part of TTA-Based Codesign Environment (TCE).

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
 */
/**
 * @file BEMSerializer.hh
 *
 * Declaration of BEMSerializer class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
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
