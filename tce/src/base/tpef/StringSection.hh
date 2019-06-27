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
 * @file StringSection.hh
 *
 * Declaration of StringSection class.
 *
 * @author Ari Metsähalme 2003 (ari.metsahalme-no.spam-tut.fi)
 * @author Mikael Lepistö 2004 (tmlepist-no.spam-cs.tut.fi)
 * @note reviewed 17 October 2003 by am, pj, rm, kl
 *
 * @note rating: yellow
 */

#ifndef TTA_STRINGSECTION_HH
#define TTA_STRINGSECTION_HH

#include "Section.hh"
#include "DataSection.hh"

namespace TPEF {
    class Chunk;

/**
 * Represents a string table section.
 */
class StringSection : public DataSection {
public:
    virtual ~StringSection();

    virtual SectionType type() const;

    std::string chunk2String(const Chunk* chunk) const;

    Chunk* string2Chunk(const std::string &str);

protected:
    StringSection(bool init);
    virtual Section* clone() const;

private:
    /// Prototype of the section.
    static StringSection proto_;
};
}

#endif
