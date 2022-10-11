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
 * @file Vlnv.cc
 *
 * Implementation of Vlnv struct.
 *
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @note rating: red
 */
#include "Vlnv.hh"

IPXact::Vlnv::Vlnv(): vendor(""), library(""), name(""), version("") {
}

IPXact::Vlnv::Vlnv(
    TCEString vendor,
    TCEString library,
    TCEString name,
    TCEString version):
    vendor(vendor), library(library), name(name), version(version) {
}

IPXact::Vlnv::Vlnv(const Vlnv& old) {
    
    vendor = old.vendor;
    library = old.library;
    name = old.name;
    version = old.version;
}

bool
IPXact::Vlnv::operator==(const Vlnv& other) {
    
    return (vendor == other.vendor &&
            library == other.library &&
            name == other.name &&
            version == other.version);
}

bool
IPXact::Vlnv::operator!=(const Vlnv& other) {

    return !(vendor == other.vendor &&
             library == other.library &&
             name == other.name &&
             version == other.version);
}

