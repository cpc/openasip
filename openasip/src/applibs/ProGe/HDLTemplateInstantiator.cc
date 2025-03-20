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
 * @file HDLTemplateInstantiator.cc
 * @author Pekka Jääskeläinen 2011
 * @note rating: red
 */
#include "HDLTemplateInstantiator.hh"
#include <fstream>
#include <cassert>
#include "Exception.hh"
#include "StringTools.hh"
#include "FileSystem.hh"

// Format for placeholder marker in the template files.
const std::string HDLTemplateInstantiator::PLACEHOLDERSEPARATOR = ",";
const std::string HDLTemplateInstantiator::PLACEHOLDERBEGIN =
    "<<placeholder" + PLACEHOLDERSEPARATOR;
const std::string HDLTemplateInstantiator::PLACEHOLDEREND = ">>";

HDLTemplateInstantiator::HDLTemplateInstantiator()
    : HDLTemplateInstantiator(TCEString()) {}

HDLTemplateInstantiator::HDLTemplateInstantiator(const TCEString& entityStr)
    : entityStr_(entityStr), replacers_() {}

/**
 * Adds replace string for the template placeholder by key.
 *
 * During HDL template instantiation all placeholders by the key will
 * be replaced with given replace string.
 *
 * @param key The key to identify placeholder.
 * @param replacer The string to replace the placeholder.
 * @param append Appends the given string to the existing placeholder filler
 *               at the key.
 * @exception KeyAlreadyExists If a placeholder is already filled by the key
 *                             and append flag is false.
 */
void
HDLTemplateInstantiator::replacePlaceholder(
    const std::string& key, const std::string& replacer, bool append) {
    // todo: add rule for key (all uppercase)?
    if (append) {
        replacers_[key] += replacer;
    } else {
        if (replacers_.count(key)) {
            THROW_EXCEPTION(
                KeyAlreadyExists,
                "The placeholder \"" + key + "\" is already filled. ");
        }
        replacers_.insert(std::pair<PlaceholderKey, Replacer>(key, replacer));
    }
}

/**
 * Adds replace string from file for the template placeholder by key.
 *
 * During HDL template instantiation all placeholders by the key will
 * be replaced with given replace string.
 *
 * @param key The key to identify placeholder.
 * @param replacer The string to replace the placeholder.
 * @param append Appends the given string to the existing placeholder filler
 *               at the key.
 */
void
HDLTemplateInstantiator::replacePlaceholderFromFile(
    const std::string& key, const Path& filePath, bool append) {
    std::string snippetFileName = filePath;
    std::ifstream snippetfile(snippetFileName.c_str());
    if (!snippetfile.is_open()) {
        THROW_EXCEPTION(
            UnreachableStream, "Could not open snippet file: \"" +
                                   snippetFileName + "\" for reading.");
    }
    std::string str(
        (std::istreambuf_iterator<char>(snippetfile)),
        std::istreambuf_iterator<char>());
    replacePlaceholder(key, str, append);
}

/**
 * Creates a target HDL file from a HDL template, replacing certain magic
 * strings with certain variable contents.
 *
 * Currently supported magic strings:
 * ENTITY_STR The entity name string used to make entities unique.
 *            Same as the toplevel entity name.
 */
void 
HDLTemplateInstantiator::instantiateTemplateFile(
    const std::string& templateFile,
    const std::string& dstFile) {
    std::ifstream input(templateFile.c_str());

    if (!input.is_open())
        throw UnreachableStream(
            __FILE__, __LINE__, __func__,
            TCEString("Could not open ") + templateFile + " for reading.");

    std::ofstream output(dstFile.c_str(), std::ios::trunc);

    if (!output.is_open())
        throw UnreachableStream(
            __FILE__, __LINE__, __func__,
            TCEString("Could not open ") + dstFile + " for writing.");

    while (!input.eof()) {
        char line_buf[1024]; 
        input.getline(line_buf, 1024);
        TCEString line(line_buf);
        line.replaceString("ENTITY_STR", entityStr_);
        fillPlaceholders(line);
        output << line << std::endl;
    }
    input.close();
    output.close();
}

/**
 * Creates a target HDL file from a HDL template replacing Nelements number of keywords 
 */
void 
HDLTemplateInstantiator::instantiateCoprocessorTemplateFile(
    const std::string& templateFile,
    const std::string& dstFile,
    const std::string (&coproreplcement_keys)[8],
    const std::string (&coproreplcement_words)[8],
    int Nelements) {
    std::ifstream input(templateFile.c_str());

    if (!input.is_open())
        throw UnreachableStream(
            __FILE__, __LINE__, __func__,
            TCEString("Could not open ") + templateFile + " for reading.");

    std::ofstream output(dstFile.c_str(), std::ios::trunc);

    if (!output.is_open())
        throw UnreachableStream(
            __FILE__, __LINE__, __func__,
            TCEString("Could not open ") + dstFile + " for writing. CVXIF");

        while (!input.eof()) {
            char line_buf[1024]; 
            input.getline(line_buf, 1024);
            TCEString line(line_buf);
            for (int i = 0; i < Nelements ; i++){
                line.replaceString(coproreplcement_keys[i], coproreplcement_words[i]);
            }
            fillPlaceholders(line);
            output << line << std::endl;
        
    }
    input.close();
    output.close();
    
}

/**
 * Fills all placeholder templates with added replace strings by key.
 *
 * The placeholder markers are in form "<<placeholder,key>>" or
 * "<<placeholder,key,default-value>>". If no replacer is not found for the
 * key the placeholder template is replaced with empty string or default-value
 * if defined.
 *
 * @param str The string to search and fill placeholders.
 */
void
HDLTemplateInstantiator::fillPlaceholders(TCEString& str) {
    TCEString target(str);
    TCEString placeholder;
    while (!(placeholder = findPlaceholder(target)).empty()) {
        TCEString replacer("");
        TCEString key = getPlaceholderKey(placeholder);
        if (replacers_.count(key)) {
            replacer = replacers_[key];
        } else {
            replacer = getPlaceholderDefault(placeholder);
        }
        target.replaceString(placeholder, replacer);
    }

    // Correct indentation
    if (!target.empty()) {
        TCEString identation("");
        unsigned int i = 0;
        while (i < target.size() && target.at(i) == ' ') {
            identation.append(" ");
            i++;
        }
        // Note: only LF newline convention is considered.
        target.replaceString("\n", "\n" + identation);
    }

    str = target;
}

/**
 * Searches placeholder template over given string.
 *
 * @param str The string to search over for placeholder template.
 * @return The placeholder string or empty string if not found.
 */
TCEString
HDLTemplateInstantiator::findPlaceholder(const TCEString& str) {
    size_t beginpos = str.find(PLACEHOLDERBEGIN);
    if (beginpos == std::string::npos) {
        return "";
    }
    size_t endpos =
        str.find(PLACEHOLDEREND, beginpos + PLACEHOLDERBEGIN.size());
    if (endpos == std::string::npos) {
        return "";
    }
    TCEString placeholder;
    placeholder =
        str.substr(beginpos, endpos - beginpos + PLACEHOLDEREND.size());
    return placeholder;
}

/**
 * Searches placeholder template over given string and returns its key.
 *
 * The placeholder template is in form of <placeholder, keystring, default>
 * and this function return keystring part.
 *
 * @param str The string to search over for placeholder template
 * @return Key for the placeholder or empty string if not found.
 */
TCEString
HDLTemplateInstantiator::getPlaceholderKey(const TCEString& str) {
    TCEString placeholder = findPlaceholder(str);
    if (placeholder.empty()) {
        return "";
    }
    // todo Better field separation
    size_t beginpos = placeholder.find(PLACEHOLDERBEGIN);
    size_t midpos = placeholder.rfind(PLACEHOLDERSEPARATOR);
    size_t endpos = placeholder.find(PLACEHOLDEREND);
    if (beginpos == std::string::npos || endpos == std::string::npos) {
        return "";
    }
    // Check for optional default value field
    if ((beginpos + PLACEHOLDERBEGIN.size()) < midpos && midpos < endpos) {
        endpos = midpos;
    }
    assert(endpos >= beginpos);
    TCEString key = placeholder.substr(
        beginpos + PLACEHOLDERBEGIN.size(),
        endpos - beginpos - PLACEHOLDERBEGIN.size());
    return StringTools::trim(key);
}

/**
 * Searches placeholder template over given string and returns its default
 * part.
 *
 * The placeholder template is in form of <placeholder,keystring,default>
 * and this function return default part.
 *
 */
TCEString
HDLTemplateInstantiator::getPlaceholderDefault(const TCEString& str) {
    TCEString placeholder = findPlaceholder(str);
    if (placeholder.empty()) {
        return "";
    }
    int count = 0;
    for (size_t offset = placeholder.find(PLACEHOLDERSEPARATOR);
         offset != std::string::npos;
         offset = placeholder.find(
             PLACEHOLDERSEPARATOR, offset + PLACEHOLDERSEPARATOR.size())) {
        count++;
    }
    if (count == 2) {
        size_t beginpos = placeholder.rfind(PLACEHOLDERSEPARATOR);
        size_t endpos = placeholder.rfind(PLACEHOLDEREND);
        if (beginpos == std::string::npos || endpos == std::string::npos) {
            return "";
        }
        assert(endpos >= beginpos);
        TCEString defaultStr;
        defaultStr = placeholder.substr(
            beginpos + PLACEHOLDERSEPARATOR.size(),
            endpos - beginpos - PLACEHOLDERSEPARATOR.size());
        return StringTools::trim(defaultStr);
    } else {
        return "";
    }
}
