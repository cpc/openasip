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
 * @file ValueReplacer.hh
 *
 * Declaration of ValueReplacer class.
 *
 * @author Mikael Lepistö 2003 (tmlepist-no.spam-cs.tut.fi)
 *
 * @note rating: yellow
 */

#ifndef TTA_REFERENCE_REPLACER_HH
#define TTA_REFERENCE_REPLACER_HH

#include <set>

#include "Exception.hh"

namespace TPEF {
    class BinaryStream;
    class SafePointable;
/**
 * Abstract base class of value replacer classes.
 *
 * A value replacer class handles writing of object references into an
 * output binary stream.  An object reference is represented with a value in
 * the stream.  The order in which reference values are computed may depend
 * on the data already written into the stream, which may include other
 * references whose value is not yet available.  When an object reference
 * must be written out and its corresponding value is not yet available,
 * value replacers are used.
 *
 * E.g., If file offset to the body of the section should is written 
 *       while writing section header, FileOffsetReplacer is used
 *       to write FileOffset of the first element of the section.
 *       When file offset of the first element is known, the 
 *       FileOffsetReplacer object knows where to replace write that offset.
 *
 * Value replacers defer the writing of the actual reference values.  A
 * dummy reference value is written out in stream sequential order and later
 * replaced witht he actual value whent his becomes available.
 *
 * The ValueReplacer base class is not a pure interface: it handles all
 * bookkeeping of deferred replacements.
 *
 * Each concrete value replacer handles the writing of reference to a
 * specific object type in a specific data format.
 */
class ValueReplacer {
public:
    static void finalize()
        throw (MissingKeys, UnreachableStream, WritePastEOF);

    static void initialize(BinaryStream& stream);

    void resolve()
        throw (UnreachableStream, WritePastEOF);

protected:
    ValueReplacer(const SafePointable* obj);

    ValueReplacer(const ValueReplacer& replacer);

    virtual ~ValueReplacer();

    /// Does replacement if can. If can't returns false.
    virtual bool tryToReplace()
        throw (UnreachableStream, WritePastEOF) = 0;

    /// Creates dynamically allocated clone of object.
    virtual ValueReplacer* clone() = 0;

    static BinaryStream& stream();
    const SafePointable* reference() const;
    unsigned int streamPosition() const;

private:
    static void addReplacement(ValueReplacer* obj);

    /// File offset where replacement is done.
    unsigned int streamPosition_;
    /// Reference which to be written.
    const SafePointable* reference_;
    /// Stream for writing replacements.
    static BinaryStream* stream_;

    /// Replacements which should be done afterwards.
    static std::set<ValueReplacer*> replacements_;
};
}

#endif
