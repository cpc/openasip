/*
 Copyright (C) 2023 Tampere University.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA

*/
/**
 * @file LicenseGenerator.cc
 *
 * Helper class for generating licenses
 *
 * @author Kari Hepola 2023 (kari.hepola@tuni.fi)
 * @note rating: red
 */

#include "LicenseGenerator.hh"

/**
 * Destructor
*/

LicenseGenerator::~LicenseGenerator() {}

/**
 * Generates a MIT license header for a module.
 * @param year The copyright year
 * @param comment String used for comment
 * @return The generated MIT license
*/

std::string
LicenseGenerator::generateMITLicense(const std::string& year,
const std::string& comment) {
    std::string license;
    license = 
      comment + " Copyright (c) " + year + " Tampere University.\n"
    + comment + "\n"
    + comment + " Module generated by OpenASIP.\n"
    + comment + "\n"
    + comment + " Permission is hereby granted, free of charge, to any"
    " person obtaining a\n"
    + comment + " copy of this software and associated documentation files"
    " (the \"Software\"),\n"
    + comment + " to deal in the Software without restriction, including"
    " without limitation\n"
    + comment + " the rights to use, copy, modify, merge, publish, "
    "distribute, sublicense,\n"
    + comment + " and/or sell copies of the Software, and to permit "
    "persons to whom the\n"
    + comment + " Software is furnished to do so, subject to the following"
    " conditions:\n"
    + comment + "\n"
    + comment + " The above copyright notice and this permission notice"
    " shall be included in\n"
    + comment + " all copies or substantial portions of the Software.\n"
    + comment + "\n"
    + comment + " THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY" 
    " OF ANY KIND, EXPRESS OR\n"
    + comment + " IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES"
    " OF MERCHANTABILITY,\n"
    + comment + " FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT."
    " IN NO EVENT SHALL THE\n"
    + comment + " AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,"
    " DAMAGES OR OTHER\n"
    + comment + " LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR"
    " OTHERWISE, ARISING\n"
    + comment + " FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR"
    " THE USE OR OTHER\n"
    + comment + " DEALINGS IN THE SOFTWARE.\n\n";
    
    return license;
}