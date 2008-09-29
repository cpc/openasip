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
