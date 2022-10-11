/*
    Copyright (c) 2012 Tampere University.

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
 * @file PRegionMarkerAnalyzer.hh
 *
 * Helper class for analyzing the PREGION_START/PREGION_END markers.
 *
 * @author Pekka Jääskeläinen 2012
 */

#ifndef TCE_PREGION_MARKER_ANALYZER_HH
#define TCE_PREGION_MARKER_ANALYZER_HH

#include <map>

namespace llvm {
    class MachineFunction;
    class MachineInstr;
}

class PRegionMarkerAnalyzer {
public:
    PRegionMarkerAnalyzer(const llvm::MachineFunction& MF_);
    virtual ~PRegionMarkerAnalyzer() {}

    bool markersFound() const;
    unsigned pregion(const llvm::MachineInstr &I) const;

private:
    bool isPregionEndMarker(const llvm::MachineInstr &I) const;
    bool isPregionStartMarker(const llvm::MachineInstr &I) const;
    unsigned parsePregionID(const llvm::MachineInstr &I) const;

    void findPregions();
    void propagatePregionID(
        const llvm::MachineInstr& start, unsigned id);

    // The machine function we are analyzing with this instance.
    const llvm::MachineFunction& MF;
    bool markersFound_;
    std::map<const llvm::MachineInstr*, unsigned> pregionIDs_;
};

#endif
