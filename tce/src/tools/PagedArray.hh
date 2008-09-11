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
 * @file PagedArray.hh
 *
 * Declaration of PagedArray class.
 *
 * @author Pekka J‰‰skel‰inen 2006 (pjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#ifndef TTA_PAGED_ARRAY_HH
#define TTA_PAGED_ARRAY_HH

#include <vector>
#include <stdint.h>

#include "BaseType.hh"
#include "Exception.hh"

#define DEFAULT_PAGE_SIZE 1024
#define UNDEFINED_VALUE (0)

/// Type used for indexing the array. Currently, limited to 32 bits.
typedef uint32_t IndexType;

/**
 * Models the data contained in array.
 *
 * The array space is divided into pages which are allocated on-demand,
 * that is, the first time an index in that page is accessed for a write 
 * operation. Pages are stored in a table from which they are
 * retrieved using (address / page_size) as index. The offset within the
 * selected page is given by (address % chunk_size). This should result in
 * O(1) access.
 *
 * This allows modeling large arrays without reserving the memory for the
 * array if it's not accessed. The idea behind this implementation is borrowed 
 * from common (paged) virtual memory implementations of operating systems.
 *
 * Please note that this container does not perform any checking for the
 * validity of the indices due to efficiency reasons.
 */
template <
    typename ValueType, 
    int PageSize=DEFAULT_PAGE_SIZE, 
    ValueType DefaultValue=static_cast<ValueType>(UNDEFINED_VALUE)>
class PagedArray {
public:

    /// Type for a set of values in a basic array type.
    typedef ValueType* ValueTable;

    /// Type for a set of values in a std::vector container.
    typedef std::vector<ValueType> ValueVector;

    PagedArray(std::size_t size);
    virtual ~PagedArray();

    void write(IndexType index, const ValueTable data, std::size_t size);
    void writeData(IndexType index, const ValueType& data);
    ValueType readData(IndexType index);
    void read(IndexType index, ValueVector& data, size_t size);
    void read(IndexType index, ValueVector& data);
    void read(IndexType index, ValueTable data, size_t size);
    size_t allocatedMemory() const;
    void clear();

private:
    void deletePages();

    /// Copying not allowed.
    PagedArray(const PagedArray&);
    /// Assignment not allowed.
    PagedArray& operator=(const PagedArray&);

    /// Storage for the data pages.
    /// Created pages are stored in table from which they are found
    /// with address / size_of_page.
    ValueType** pageTable_;
    /// Size of the page table.
    std::size_t pageTableSize_;
};

#include "PagedArray.icc"

#endif
