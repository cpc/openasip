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
 * @file OSEdOptionsSerializer.hh
 *
 * Declaration of OSEdOptionsSerializer class.
 *
 * @author Jussi Nykänen 2004 (nykanen-no.spam-cs.tut.fi)
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

    virtual void writeState(const ObjectState* state);
    virtual ObjectState* readState();

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
