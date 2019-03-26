/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file OperationSerializer.hh
 *
 * Declaration of OperationSerializer class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
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
