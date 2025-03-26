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
 * @file CacheImplementation.cc
 *
 * Implementation of CacheImplementation.
 *
 * @author Henry Linjamäki 2015 (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "CacheImplementation.hh"

#include <cassert>
#include <map>

#include "ObjectState.hh"

namespace IDF {

const std::string CacheImplementation::OSNAME_CACHE_IMPLEMENTATION
        = "cache_implementation";
const std::string CacheImplementation::OSKEY_BLOCK_SIZE
        = "cache_block_size";
const std::string CacheImplementation::OSKEY_SET_SIZE
        = "cache_set_size";
const std::string CacheImplementation::OSKEY_CACHE_SIZE
        = "cache_size";
const std::string CacheImplementation::OSKEY_REPLACEMENT_POLICY
        = "cache_replacement_policy";

namespace /* anonymous */ {

using RepPolicy = CacheImplementation::ReplacementPolicy;
const std::map<RepPolicy, std::string> replacementPolicyToString{
    { RepPolicy::RANDOM, "random" },
    { RepPolicy::LRU, "LRU" }
};

const std::map<std::string, RepPolicy> stringToReplacementPolicy{
    { "random", RepPolicy::RANDOM },
    { "LRU", RepPolicy::LRU }
};

} // anonymous namespace   

/**
 * Default constructor.
 */
CacheImplementation::CacheImplementation()
: CacheImplementation(1, 1, 1, ReplacementPolicy::INFERRED) {
}

/**
 *
 * @param blockSize The size of a block/entry/line in words. Must be non-zero.
 * @param setSize The size of a set/number of ways. Must be non-zero.
 * @param cacheSize The size of the cache in number of sets. Must be non-zero.
 * @param replacementPolicy The replacement policy.
 */
CacheImplementation::CacheImplementation(
    unsigned blockSize,
    unsigned setSize,
    unsigned cacheSize,
    ReplacementPolicy replacementPolicy)
    : blockSize_(blockSize),
      setSize_(setSize),
      cacheSize_(cacheSize),
      replacementPolicy_(replacementPolicy) {

    assert(blockSize_ > 0 && "Block size must be > 0.");
    assert(setSize_ > 0 && "Set size must be > 0.");
    assert(cacheSize_ > 0 && "Cache size must be > 0.");

}

/**
 * Construct the object from ObjectState.
 */
CacheImplementation::CacheImplementation(const ObjectState* state)

    : CacheImplementation() {
    loadState(state);
}

CacheImplementation::~CacheImplementation() {
}
/**
 * Return size of the blocks that is measured in number of "words".
 */
unsigned
CacheImplementation::blockSize() const {
    return blockSize_;
}

/**
 * Set size of the blocks that is measured in number of "words".
 *
 * The block is synonumous with line or entry. Argument must be > 0;
 */
void
CacheImplementation::setBlockSize(unsigned size) {
    blockSize_ = size;
    assert(blockSize_ > 0 && "Block size must be > 0.");
}

/**
 * Returns size of the cache's set in number of blocks.
 *
 * The set is synonymous with way associativity. Argument must be > 0;
 */
unsigned
CacheImplementation::setSize() const {
    return setSize_;
}

/**
 * Sets size of the cache's set in number of blocks.
 *
 * The set is equivalent to way associativity. Argument must be > 0;
 */
void
CacheImplementation::setSetSize(unsigned size) {
    setSize_ = size;
    assert(setSize_ > 0 && "Set size must be > 0.");
}

/**
 * Returns size of the cache in number of sets (or rows).
 *
 */
unsigned
CacheImplementation::cacheSize() const {
    return cacheSize_;
}

/**
 * Sets size of the cache in number of sets (or rows).
 *
 * Argument must be > 0;
 */
void
CacheImplementation::setCacheSize(unsigned size) {
    cacheSize_ = size;
    assert(cacheSize_ > 0 && "Cache size must be > 0.");
}

/**
 * Returns replacement policy.
 */
CacheImplementation::ReplacementPolicy
CacheImplementation::replacementPolicy() const {
    return replacementPolicy_;
}

/**
 * Sets replacement policy.
 */
void
CacheImplementation::setReplacementPolicy(
        CacheImplementation::ReplacementPolicy policy) {
    replacementPolicy_ = policy;
}

void
CacheImplementation::loadState(const ObjectState* state) {
    if (state->name() != OSNAME_CACHE_IMPLEMENTATION) {
        THROW_EXCEPTION(ObjectStateLoadingException, "Invalid object state.");
    }

    try {
        blockSize_ = state->unsignedIntAttribute(OSKEY_BLOCK_SIZE);
        setSize_ = state->unsignedIntAttribute(OSKEY_SET_SIZE);
        cacheSize_ = state->unsignedIntAttribute(OSKEY_CACHE_SIZE);
        if (state->hasAttribute(OSKEY_REPLACEMENT_POLICY)) {
            replacementPolicy_ = unserializeReplacementPolicy(
                state->stringAttribute(OSKEY_REPLACEMENT_POLICY));
        } else {
            replacementPolicy_ = ReplacementPolicy::INFERRED;
        }

    } catch (const Exception& exception) {
        THROW_EXCEPTION(ObjectStateLoadingException, exception.errorMessage());
    }
}

ObjectState*
CacheImplementation::saveState() const {
    ObjectState* state = new ObjectState(OSNAME_CACHE_IMPLEMENTATION);
    state->setAttribute(OSKEY_BLOCK_SIZE, blockSize());
    state->setAttribute(OSKEY_SET_SIZE, setSize());
    state->setAttribute(OSKEY_CACHE_SIZE, cacheSize());
    if (replacementPolicy_ != ReplacementPolicy::INFERRED) {
        state->setAttribute(
            OSKEY_REPLACEMENT_POLICY, serialize(replacementPolicy()));
    }
    return state;
}

/**
 * Converts replacement policy enumeration into corresponding string.
 */
std::string
CacheImplementation::serialize(
    ReplacementPolicy policy) {

    return replacementPolicyToString.at(policy);
}

/**
 * Converts serialized replacement policy into replacement policy enumeration.
 */
CacheImplementation::ReplacementPolicy
CacheImplementation::unserializeReplacementPolicy(
    const std::string& serializedPolicy) {
    if (stringToReplacementPolicy.count(serializedPolicy) == 0) {
        std::string msg = std::string("Unknown replacement policy \"")
            + serializedPolicy + "\".";
        THROW_EXCEPTION(NoKnownConversion, msg);
    } else {
        return stringToReplacementPolicy.at(serializedPolicy);
    }
}

} // namespace IDF
