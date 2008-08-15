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
 * @file SectionSizeReplacer.hh
 *
 * Declaration of SectionSizeReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist@cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_SECTION_SIZE_REPLACER_HH
#define TTA_SECTION_SIZE_REPLACER_HH

#include <map>

#include "ValueReplacer.hh"

#include "SafePointable.hh"
#include "BinaryStream.hh"
#include "Exception.hh"
#include "TPEFBaseType.hh"

namespace TPEF {

/**
 * Replaces object reference with size of object. Size for object must be
 * stored with static setSize method of this class.
 */
class SectionSizeReplacer : public ValueReplacer {
public:
    SectionSizeReplacer(const SafePointable* obj);
    virtual ~SectionSizeReplacer();

    static void setSize(const SafePointable* obj, Word size);
    static void clear();

protected:
    virtual bool tryToReplace()
        throw (UnreachableStream, WritePastEOF);

    virtual ValueReplacer* clone();

private:
    SectionSizeReplacer(const SectionSizeReplacer& replacer);
    SectionSizeReplacer operator=(const SectionSizeReplacer&);

    /// Contains sections sizes.
    static std::map<const SafePointable*, Word> sizeMap;
};
}

#endif
