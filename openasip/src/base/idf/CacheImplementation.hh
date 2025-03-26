/*
    Copyright (c) 2002-2015 Tampere University.

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
 * @file CacheImplementation.hh
 *
 * Declaration of CacheImplementation.
 *
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */


#ifndef CACHEIMPLEMENTATION_HH
#define	CACHEIMPLEMENTATION_HH

#include <string>

#include "Serializable.hh"
#include "Exception.hh"

namespace IDF {

/**
 * Represents implementation parameters for caches.
 */
class CacheImplementation : public Serializable {
public:

    enum class ReplacementPolicy {
        /// Policy is not defined and is instead selected by application
        INFERRED,
        /// Block is evicted randomly.
        RANDOM,
        /// Least recently used block is evicted.
        LRU
    };

    CacheImplementation();
    CacheImplementation(
        unsigned blockSize,
        unsigned setSize,
        unsigned cacheSize,
        ReplacementPolicy replacementPolicy);
    CacheImplementation(const ObjectState* state);
    virtual ~CacheImplementation();

    unsigned blockSize() const;
    void setBlockSize(unsigned size);
    unsigned setSize() const;
    void setSetSize(unsigned size);
    unsigned cacheSize() const;
    void setCacheSize(unsigned size);
    ReplacementPolicy replacementPolicy() const;
    void setReplacementPolicy(ReplacementPolicy policy);

    void loadState(const ObjectState* state) override;

    ObjectState* saveState() const override;

    /// ObjectState name for cache implementation.
    static const std::string OSNAME_CACHE_IMPLEMENTATION;
    /// ObjectState for cache block size.
    static const std::string OSKEY_BLOCK_SIZE;
    /// ObjectState for cache set size.
    static const std::string OSKEY_SET_SIZE;
    /// ObjectState for cache size.
    static const std::string OSKEY_CACHE_SIZE;
    /// ObjectState for replacement policy.
    static const std::string OSKEY_REPLACEMENT_POLICY;

private:

    static std::string serialize(ReplacementPolicy policy);
    static ReplacementPolicy unserializeReplacementPolicy(
        const std::string& serializedPolicy);

    /// The Size of a block/entry/line in words.
    unsigned blockSize_;
    /// The Size of a set of in blocks.
    unsigned setSize_;
    /// The number of sets in the cache.
    unsigned cacheSize_;
    /// The replacement policy.
    ReplacementPolicy replacementPolicy_;

};

} // namespace IDF

#endif	/* CACHEIMPLEMENTATION_HH */

