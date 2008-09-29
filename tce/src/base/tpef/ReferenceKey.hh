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

