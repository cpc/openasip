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
 * @file SafePointable.hh
 *
 * Declaration of SafePointable interface class.
 *
 * @author Pekka Jääskeläinen 2003 (pjaaskel@cs.tut.fi)
 * @author Mikael Lepistö 2005 (tmlepist@cs.tut.fi)
 * @note Reviewed 29 Aug 2003 by Risto Mäkinen, Mikael Lepistö, Andrea Cilio,
 * Tommi Rantanen
 *
 * @note rating: yellow
 */

#ifndef TTA_SAFEPOINTABLE_HH
#define TTA_SAFEPOINTABLE_HH

#include "Application.hh" // debug

namespace TPEF {

/**
 * All classes that implement the SafePointable interface can be used
 * in reference manager.
 */
class SafePointable {
public:
    virtual ~SafePointable();
protected:
    // this is an 'interface' class so we don't want direct instances of it
    SafePointable();

};
}
#endif
