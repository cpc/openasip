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
 * @file Serializable.hh
 *
 * Declaration of Serializable interface.
 *
 * @author Lasse Laasonen 2003 (lasse.laasonen-no.spam-tut.fi)
 * @note This file is used in compiled simulation. Keep dependencies *clean*
 * @note rating: red
 */

#ifndef TTA_SERIALIZABLE_HH
#define TTA_SERIALIZABLE_HH

class ObjectState;


/**
 * Objects which are going to be serialized using XMLSerializer or any
 * other serializer must implement this interface.
 */
class Serializable {
public:
    /**
     * Loads the state of the object from the given ObjectState object.
     *
     * @param state ObjectState object from which (and the children of
     *              which) the state is loaded.
     */
    virtual void loadState(const ObjectState* state) = 0;

    /**
     * Saves the objects state into an ObjectState object and maybe its
     * child objects.
     *
     * @return The root of the ObjectState tree created.
     */
    virtual ObjectState* saveState() const = 0;
    virtual ~Serializable() {}
};

#endif
