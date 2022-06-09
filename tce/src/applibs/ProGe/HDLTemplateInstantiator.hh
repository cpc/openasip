/*
    Copyright (c) 2002-2011 Tampere University.

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
 * @file HDLTemplateInstantiator.hh
 *
 * @author Pekka Jääskeläinen 2011
 * @note rating: red
 */

#ifndef TTA_HDL_TEMPLATE_INSTANTIATOR_HH
#define TTA_HDL_TEMPLATE_INSTANTIATOR_HH

#include "TCEString.hh"
/**
 * Helper class for instantiating HDL template files.
 */ 
class HDLTemplateInstantiator {
public:
    HDLTemplateInstantiator() {}
    void setEntityString(TCEString entityStr) { entityStr_ = entityStr; }

    void instantiateTemplateFile(
        const std::string& templateFile,
        const std::string& dstFile);

private:
    TCEString entityStr_;
};
#endif
