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
 * @file ProDeOptionsSerializer.hh
 *
 * Declaration of ProDeOptionsSerializer class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef PRODE_OPTIONS_SERIALIZER_HH
#define PRODE_OPTIONS_SERIALIZER_HH

#include <string>

#include "GUIOptionsSerializer.hh"

/**
 * Reads/writes ProDe options from/to an XML file.
 */
class ProDeOptionsSerializer : public GUIOptionsSerializer {
public:
    ProDeOptionsSerializer();
    virtual ~ProDeOptionsSerializer();
private:
    ObjectState* convertToConfigFileFormat(const ObjectState* options) const;
    ObjectState* convertToOptionsObjectFormat(const ObjectState* root) const;

    /// Copying not allowed.
    ProDeOptionsSerializer(const ProDeOptionsSerializer&);
    /// Assignment not allowed.
    ProDeOptionsSerializer& operator=(const ProDeOptionsSerializer&);
};

#endif
