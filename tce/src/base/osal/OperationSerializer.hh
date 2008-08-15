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
