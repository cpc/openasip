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

#include <map>
#include <string>

#include "TCEString.hh"
#include "Exception.hh"

class Path;

/**
 * Helper class for instantiating HDL template files.
 */ 
class HDLTemplateInstantiator {
public:
    friend class HDLTemplateInstantiatorTest;

    HDLTemplateInstantiator();
    HDLTemplateInstantiator(const TCEString& entityStr);
    void setEntityString(const TCEString& entityStr) {
        entityStr_ = entityStr; }
    void replacePlaceholder(
        const std::string& key, const std::string& replacer,
        bool append = false);
    void replacePlaceholderFromFile(
        const std::string& key,
        const Path& filePath,
        bool append = false);
    void instantiateTemplateFile(
        const std::string& templateFile,
        const std::string& dstFile);
    //For implementing CVXIF coprocessor specific templates
    void instantiateCoprocessorTemplateFile(
        const std::string& templateFile,
        const std::string& dstFile,
        const std::string (&coproreplcement_keys)[8],
        const std::string (&coproreplcement_words)[8],
        int Nelements);

private:
    typedef TCEString PlaceholderKey;
    typedef TCEString Replacer;
    typedef std::map<PlaceholderKey, Replacer> ReplacerMap;

    void fillPlaceholders(TCEString& str);
    TCEString findPlaceholder(const TCEString& str);
    TCEString getPlaceholderKey(const TCEString& str);
    TCEString getPlaceholderDefault(const TCEString& str);

    static const std::string PLACEHOLDERSEPARATOR;
    static const std::string PLACEHOLDERBEGIN;
    static const std::string PLACEHOLDEREND;

    TCEString entityStr_;
    ReplacerMap replacers_;

};
#endif
