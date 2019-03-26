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
 * @file BridgeEncoding.hh
 *
 * Declaration of BridgeEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_BRIDGE_ENCODING_HH
#define TTA_BRIDGE_ENCODING_HH

#include <string>

#include "Exception.hh"
#include "Encoding.hh"

class SourceField;
class ObjectState;

/**
 * Class BridgeEncoding maps a bridge to a binary encoding.
 */
class BridgeEncoding : public Encoding {
public:
    BridgeEncoding(
	const std::string& name,
	unsigned int encoding,
	unsigned int extraBits,
	SourceField& parent)
	throw (ObjectAlreadyExists);
    BridgeEncoding(const ObjectState* state, SourceField& parent)
	throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~BridgeEncoding();

    SourceField* parent() const;
    std::string bridgeName() const;
    virtual int bitPosition() const;

    virtual ObjectState* saveState() const;

    /// ObjectState name for bridge encoding.
    static const std::string OSNAME_BRIDGE_ENCODING;
    /// ObjectState attribute key for the name of the bridge.
    static const std::string OSKEY_BRIDGE_NAME;

private:
    /// Name of the bridge.
    std::string bridge_;
};

#endif
