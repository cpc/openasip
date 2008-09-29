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
 * @file SectionIndexReplacer.hh
 *
 * Declaration of SectionIndexReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_SECTION_INDEX_REPLACER_HH
#define TTA_SECTION_INDEX_REPLACER_HH

#include "ValueReplacer.hh"

#include "SafePointable.hh"
#include "BinaryStream.hh"
#include "Exception.hh"

namespace TPEF {

/**
 * Replaces object reference with section index. Section index key
 * must be stored in reference manager for that object whose section
 * index is written.
 */
class SectionIndexReplacer : public ValueReplacer {
public:
    SectionIndexReplacer(const SafePointable* obj, Byte fieldSize);
    virtual ~SectionIndexReplacer();

protected:
    virtual bool tryToReplace()
        throw (UnreachableStream, WritePastEOF);

    virtual ValueReplacer* clone();

private:
    SectionIndexReplacer(const SectionIndexReplacer& replacer);
    SectionIndexReplacer operator=(const SectionIndexReplacer&);

    void writeReplacement(Word value);

    /// Field how many bytes of stream are replaced with section index.
    const Byte fieldSize_;
};
}

#endif
