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
 * @file ADFSerializerTextGenerator.hh
 *
 * Declaration of ADFSerializerTextGenerator class.
 *
 * @author Lasse Laasonen 2004 (lasse.laasonen-no.spam-tut.fi)
 * @note rating: red
 */

#ifndef TTA_MDF_SERIALIZER_TEXT_GENERATOR_HH
#define TTA_MDF_SERIALIZER_TEXT_GENERATOR_HH

#include "TextGenerator.hh"

/**
 * Stores the error messages given in exceptions thrown from ADFSerializer.
 */
class ADFSerializerTextGenerator : public Texts::TextGenerator {
public:
    ADFSerializerTextGenerator();
    virtual ~ADFSerializerTextGenerator();

    /// Ids for the stored texts.
    enum {
        TXT_MULTIPLE_DESTINATIONS_IN_TEMPLATE_SLOT = Texts::LAST__,
        TXT_SAME_TEMPLATE_SLOT,
        TXT_IT_EMPTY_AND_NON_EMPTY,
        TXT_EMPTY_IT_NOT_DECLARED
    };
};

#endif
