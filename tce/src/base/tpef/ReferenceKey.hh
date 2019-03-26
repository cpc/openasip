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
 * @file ReferenceKey.hh
 *
 * Declarations of different key types used in reference managing.
 *
 * @author Pekka J‰‰skel‰inen 2003 (pjaaskel-no.spam-cs.tut.fi)
 * @note reviewed 17 Oct 2003 by am, rm, kl
 *
 * @note rating: yellow
 */
#ifndef TTA_REFERENCEKEY_HH
#define TTA_REFERENCEKEY_HH

#include "TPEFBaseType.hh"

namespace TPEF {
namespace ReferenceManager {

//////////////////////////////////////////////////////////////////////////////
// ReferenceKey
//////////////////////////////////////////////////////////////////////////////

/**
 * Base (interface) class for all keys that identify objects in
 * reference manager.
 */
class ReferenceKey {
public:
    virtual ~ReferenceKey();
protected:
    ReferenceKey();
};

//////////////////////////////////////////////////////////////////////////////
// SectionIndexKey
//////////////////////////////////////////////////////////////////////////////

/**
 * With this type of key one can identify an object by means of a section
 * identification code and the index of the element in the section.
 */
class SectionIndexKey : public ReferenceKey {
public:
    SectionIndexKey(SectionId sectionId, SectionIndex index);
    virtual ~SectionIndexKey();

    bool operator==(const SectionIndexKey& other) const;
    bool operator!=(const SectionIndexKey& other) const;
    bool operator<(const SectionIndexKey& other) const;

    SectionId sectionId() const;
    SectionIndex index() const;

private:
    /// Section Identification code of the section that contains the element
    /// this key points to.
    SectionId sectionId_;
    /// Index of the element this key points to.
    SectionIndex index_;
};

//////////////////////////////////////////////////////////////////////////////
// SectionOffsetKey
//////////////////////////////////////////////////////////////////////////////

/**
 * With this type of keys one can identify objects by means of a section
 * identification code and a byte offset in the section.
 */
class SectionOffsetKey : public ReferenceKey {
public:
    SectionOffsetKey(SectionId sectionId, SectionOffset offset);
    virtual ~SectionOffsetKey();

    SectionId sectionId() const;
    SectionOffset offset() const;

    bool operator==(const SectionOffsetKey& other) const;
    bool operator!=(const SectionOffsetKey& other) const;
    bool operator<(const SectionOffsetKey& other) const;

private:
    /// Identification code of the section that contains the element this
    /// key points to.
    SectionId sectionId_;
    /// Byte offset relative to section start to the element this key points
    /// to.
    SectionOffset offset_;
};

//////////////////////////////////////////////////////////////////////////////
// FileOffsetKey
//////////////////////////////////////////////////////////////////////////////

/**
 * With this type of key one can identify objects with its file offset.
 */
class FileOffsetKey : public ReferenceKey {
public:
    FileOffsetKey(FileOffset fileOffset);
    virtual ~FileOffsetKey();

    bool operator==(const FileOffsetKey& other) const;
    bool operator!=(const FileOffsetKey& other) const;
    bool operator<(const FileOffsetKey& other) const;

    FileOffset fileOffset() const;

private:
    /// File offset of the element this key points to.
    FileOffset fileOffset_;
};

///////////////////////////////////////////////////////////////////////////////
// SectionKey
///////////////////////////////////////////////////////////////////////////////

/**
 * With this type of key one can identify (section) objects with an
 * arbitrary numeric code that is unique among all objects.
 */
class SectionKey : public ReferenceKey {
public:

    SectionKey(SectionId sectionId);
    virtual ~SectionKey();

    bool operator==(const SectionKey& other) const;
    bool operator!=(const SectionKey& other) const;
    bool operator<(const SectionKey& other) const;

    SectionId sectionId() const;

private:
    /// Identification code of the section that contains the element this
    /// key points to.
    SectionId sectionId_;
};

} // namespace ReferenceManager
}

#include "ReferenceKey.icc"

#endif

