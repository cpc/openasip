/*
 Copyright (c) 2002-2016 Tampere University.

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
/*
 * @file BitTrie.hh
 *
 * Implementation/Declaration of BitTrie class.
 *
 * Created on: 28.1.2016
 * @author Henry Linjamäki 2016 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef BITTRIE_HH
#define BITTRIE_HH

#include <tuple>
#include <iostream>

#include "Exception.hh"

/*
 * Binary trie class.
 *
 * Bit patterns are inserted into the trie using BitVector, which is tuple
 * of (binary value, bit width). The value is right aligned in the bit vector.
 * For example (5, 6) is as bit vector 0b000101.
 *
 * When default constructed BitTrie or with BitTrie(false), the inserted bit
 * pattern are stored LSB first. That is, when bit patterns 0b110101 and
 * 0b111101 is added to the trie, the common prefix of the patterns is 0b101.
 *
 * When constructed with BitTrie(true), the inserted bit pattern are stored
 * MSB first (or in reserve order in contrast to default constructed BitTrie).
 * That is, when bit patterns 0b110101 and 0b111101 is added to the trie, the
 * common prefix of the patterns is 0b11.
 */
template<class DataType, class ValueType = long long unsigned int>
class BitTrie {
public:
    BitTrie();
    BitTrie(bool bitReadLeftToRight);
    virtual ~BitTrie();

    /// The type for width of bit vector.
    using WidthType = unsigned;
    /// The bit vector type used to store patterns to tries.
    /// Binary value is queried by value() method.
    /// Width value is queried by width() method.
    using BitVector = std::tuple<ValueType, WidthType>;
    /// The type for frequencies of the patterns.
    using Frequency = unsigned int;

    unsigned frequency(const BitVector& prefixBits) const;
    bool check(const BitVector& exactbits) const;
    bool insert(BitVector bits);
    bool insert(BitVector bits, const DataType& data);
    bool erase(BitVector bits);
    void clear();
    unsigned size() const;
    BitVector uniqueUnusedPrefix() const;
    DataType& at(BitVector bits);

    BitVector longestCompletePattern(const BitVector& bits) const;

    void dump(std::ostream& out) const;

    static WidthType& width(BitVector& bitVector);
    static const WidthType& width(const BitVector& bitVector);
    static ValueType& value(BitVector& bitVector);
    static const ValueType& value(const BitVector& bitVector);
private:

    BitTrie(bool bitReadLeftToRight, BitTrie& parent);
    bool nextBit(const BitVector& bitVector) const;
    BitVector stripBit(BitVector bitVector) const;
    BitTrie* findNode(const BitVector& bits);
    const BitTrie* findNode(const BitVector& bits) const;
    static BitVector uniqueUnusedPrefixImpl(const BitTrie& bitTrie);
    static BitVector uniquePrefixImpl(const BitTrie& bitTrie);
    static bool nodeTerminatesPattern(const BitTrie& node);
    static BitVector patternAtNode(const BitTrie& node);
    static bool nodeBitValue(const BitTrie& node);
    static bool isRootNode(const BitTrie& node);
    static unsigned depth(const BitTrie& node);

    /// The configuration how bits are read and stored into the trie.
    /// If true, the rightmost (LSB) bit is placed in the leaf node.
    /// If false, the leftmost (MSB) bit is placed in the leaf node.
    bool bitReadLeftToRight_ = false;
    /// The bits are encoded as indexes where 0: zero node, 1: one node.
    /// If node at index is not nullptr, a bit has been stored in the trie.
    BitTrie* node_[2] = {nullptr, nullptr};
    /// Parent of the (sub)trie. If the pointer is null, the node is root of
    /// the trie.
    BitTrie* parent_ = nullptr;
    /// The frequency. The current occurrence of the bit pattern so far.
    Frequency frequency_ = 0;
    /// The data at this node.
    DataType* data_ = nullptr;

};

#include "BitTrie.icc"

#endif /* BITTRIE_HH */
