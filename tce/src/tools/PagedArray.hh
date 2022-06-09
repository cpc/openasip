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
 * @file PagedArray.hh
 *
 * Declaration of PagedArray class.
 *
 * @author Pekka Jääskeläinen 2006 (pjaaskel-no.spam-cs.tut.fi)
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
