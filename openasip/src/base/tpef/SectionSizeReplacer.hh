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
 * @file SectionSizeReplacer.hh
 *
 * Declaration of SectionSizeReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_SECTION_SIZE_REPLACER_HH
#define TTA_SECTION_SIZE_REPLACER_HH

#include <map>

#include "ValueReplacer.hh"

#include "Exception.hh"
#include "TPEFBaseType.hh"

namespace TPEF {
    class SafePointable;
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
    virtual bool tryToReplace();

    virtual ValueReplacer* clone();

private:
    SectionSizeReplacer(const SectionSizeReplacer& replacer);
    SectionSizeReplacer operator=(const SectionSizeReplacer&);

    /// Contains sections sizes.
    static std::map<const SafePointable*, Word> sizeMap;
};
}

#endif
