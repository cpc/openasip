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
 * @file Serializer.hh
 *
 * Declaration of Serializer interface.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 */

#ifndef TTA_SERIALIZER_HH
#define TTA_SERIALIZER_HH

#include "Serializable.hh"
#include "Exception.hh"

class ObjectState;

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
