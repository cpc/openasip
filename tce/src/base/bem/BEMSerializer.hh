/*
    Copyright 2002-2008 Tampere University of Technology.  All Rights
    Reserved.

    This file is part of TTA-Based Codesign Environment (TCE).

    TCE is free software; you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 2 as published by the Free
    Software Foundation.

    TCE is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details.

    You should have received a copy of the GNU General Public License along
    with TCE; if not, write to the Free Software Foundation, Inc., 51 Franklin
    St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
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
