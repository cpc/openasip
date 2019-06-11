/*
    Copyright (c) 2002-2010 Tampere University.

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
 * @file IPXactFileSerializer.hh
 *
 * Declaration of IPXactFileSerializer class.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#ifndef TTA_IP_XACT_SERIALIZER_HH
#define TTA_IP_XACT_SERIALIZER_HH

#include "XMLSerializer.hh"
#include "TCEString.hh"

class IPXactModel;

class IPXactSerializer : public XMLSerializer {
public:
    
    IPXactSerializer();
    virtual ~IPXactSerializer();

    
    virtual void writeState(const ObjectState* ipXactState)
        throw (SerializerException);

    void writeIPXactModel(const IPXactModel& model);

private:
    /// Copying forbidden.
    IPXactSerializer(const IPXactSerializer&);
    /// Assingment forbidden.
    IPXactSerializer& operator=(const IPXactSerializer&);

    /// Reading is not (yet) implemented
    ObjectState* readState()
        throw (SerializerException);

    static const TCEString SPIRIT_NS_URI;

};
#endif
