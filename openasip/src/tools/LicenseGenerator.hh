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
 * @file LicenseGenerator.hh
 *
 * Helper class for generating licenses
 *
 * @author Kari Hepola 2023 (kari.hepola@tuni.fi)
 * @note rating: red
 */

#ifndef LICENSE_GENERATOR_HH
#define LICENSE_GENERATOR_HH

#include <string>

class LicenseGenerator {
public:
    ~LicenseGenerator();
    static std::string generateMITLicense(const std::string& year,
    const std::string& comment);
private:
    LicenseGenerator() = delete;
};

#endif