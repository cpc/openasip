/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file GPRGuardEncoding.hh
 *
 * Declaration of GPRGuardEncoding class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_GPR_GUARD_ENCODING_HH
#define TTA_GPR_GUARD_ENCODING_HH

#include <string>

#include "GuardEncoding.hh"
#include "Exception.hh"

/**
 * The GPRGuardEncoding class maps a guard expression with general purpose
 * register term to a control code.
 *
 * Instances of this class are always registered to a guard field.
 */
class GPRGuardEncoding : public GuardEncoding {
public:
    GPRGuardEncoding(
	const std::string& regFile,
	int index,
	bool inverted,
	unsigned int encoding,
	GuardField& parent)
	throw (ObjectAlreadyExists);
    GPRGuardEncoding(const ObjectState* state, GuardField& parent)
	throw (ObjectStateLoadingException, ObjectAlreadyExists);
    virtual ~GPRGuardEncoding();

    std::string registerFile() const;
    int registerIndex() const;

    virtual ObjectState* saveState() const;

    /// ObjectState name for GPR guard encoding.
    static const std::string OSNAME_GPR_GUARD_ENCODING;
    /// ObjectState attribute key for the name of the register file.
    static const std::string OSKEY_RF_NAME;
    /// ObjectState attribute key for register index.
    static const std::string OSKEY_REGISTER_INDEX;

private:
    /// Name of the register file.
    std::string regFile_;
    /// Register index.
    int index_;
};

#endif
