/*
    Copyright (c) 2002-2021 Tampere University.

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
 * @file BlocksModel.hh
 *
 * Declaration of the BlocksModel class which contains a Blocks model.
 *
 * @author Maarten Molendijk 2020 (m.j.molendijk@tue.nl)
 * @author Kanishkan Vadivel 2021 (k.vadivel@tue.nl)
 */

#ifndef BLOCKS_MODEL_HH
#define BLOCKS_MODEL_HH

#include <list>
#include <map>
#include <string>

#include "ObjectState.hh"
#include "XMLSerializer.hh"

namespace BlocksTranslator {
enum class FU_TYPE {
    ID,
    IU,  // Immediate unit
    ALU,
    RF,
    MUL,
    LSU,  // Load store unit
    ABU
};
}

class BlocksModel {
private:
    void LoadModelFromXml();
    bool VerifyXmlStructure();

    const std::map<std::string, BlocksTranslator::FU_TYPE> stringToEnum{
        {"ID", BlocksTranslator::FU_TYPE::ID},
        {"ABU", BlocksTranslator::FU_TYPE::ABU},
        {"IU", BlocksTranslator::FU_TYPE::IU},
        {"LSU", BlocksTranslator::FU_TYPE::LSU},
        {"ALU", BlocksTranslator::FU_TYPE::ALU},
        {"RF", BlocksTranslator::FU_TYPE::RF},
        {"MUL", BlocksTranslator::FU_TYPE::MUL}};

    struct FunctionalUnit {
        BlocksTranslator::FU_TYPE type;
        std::string name;
        std::list<std::string> src;
        bool usesOut0;
        bool usesOut1;
        // TODO(mm): add config bit
    };

    ObjectState* mdfState_;

public:
    // Parse Blocks architecture into program memory
    BlocksModel(std::string filename) {
        XMLSerializer* serializer = new XMLSerializer();
        serializer->setSourceFile(filename);
        mdfState_ = serializer->readState();
        LoadModelFromXml();
    };
    std::list<FunctionalUnit> mFunctionalUnitList;
};
#endif
