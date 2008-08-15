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
 * @file InterPassDatum.hh
 *
 * Declaration of InterPassDatum.
 *
 * @author Pekka J‰‰skel‰inen 2007 (pjaaskel@cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_INTER_PASS_DATUM_HH
#define TTA_INTER_PASS_DATUM_HH

#include <map>
#include <string>

class InterPassDatum;

/** 
 * A base class for classes storing inter-pass data of any type.
 */
class InterPassDatum {
public:
    virtual ~InterPassDatum() {};

protected:
    // no direct instantiation assumed
    InterPassDatum() {}
};

/** 
 * A templated class for classes storing inter-pass data of any type.
 *
 * Inherits the interface of the given class and the InterPassDatum, thus
 * acts like the given class, but can be stored as inter-pass data. Should
 * make creating new InterPassDatum types easier.
 */
template <typename T>
class SimpleInterPassDatum : public InterPassDatum, public T {
public:
    SimpleInterPassDatum() : InterPassDatum(), T() {}
    virtual ~SimpleInterPassDatum() {};
};

#endif
