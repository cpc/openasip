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
#include "Exception.hh"

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
            TCEString("Could not open ") + dstFile + " for reading.");

    while (!input.eof()) {
        char line_buf[1024]; 
        input.getline(line_buf, 1024);
        TCEString line(line_buf);
        line.replaceString("ENTITY_STR", entityStr_);
        output << line << std::endl;
    }
    input.close();
    output.close();
}
