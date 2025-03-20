/*
    Copyright (C) 2025 Tampere University.

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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301 USA
 */
/**
 * @file GenerateCoprocessor.hh
 *
 * Declaration of GenerateCoprocessor class from GenerateProcessor.
 *
 * @author Tharaka Sampath
 */

#ifndef GENERATE_COPROCESSOR_HH
#define GENERATE_COPROCESSOR_HH

#include "Exception.hh"
#include "ProGeUI.hh"

class CoprocessorCmdLineOptions;

/**
 * Implements the command line user interface 'generateCoprocessor'.
 */
class GenerateCoprocessor : public ProGe::ProGeUI {
public:
    GenerateCoprocessor();
    virtual ~GenerateCoprocessor();

    bool generateCoprocessor(int argc, char* argv[]);

private:
    void getOutputDir(
        const CoprocessorCmdLineOptions& options, std::string& outputDir);
};

#endif
