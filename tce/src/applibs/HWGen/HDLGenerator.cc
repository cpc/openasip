/*
    Copyright (c) 2002-2017 Tampere University.

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
 * @file HDLGenerator.cc
 *
 * @author Lasse Lehtonen 2017 (lasse.lehtonen-no.spam-tut.fi)
 */
#include "HDLGenerator.hh"

namespace HDLGenerator {
void
Synchronous::build() {
    Generatable::build();
    for (auto&& v : variables_) {
        parentType<Module>()->registerVariable(v);
    }
    for (auto&& r : registers_) {
        if (!isRegister(r)) {
            throw std::runtime_error(
                r + " written in synchronous but isn't a register.");
        }
    }
}

void
Asynchronous::build() {
    Generatable::build();
    for (auto&& v : variables_) {
        parentType<Module>()->registerVariable(v);
    }
}
}  // namespace HDLGenerator