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
 * @file CoProGe.hh
 *
 * Declaration of Co Processor Generator class.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen-no.spam-tut.fi)
 * @author Otto Esko 2010 (otto.esko-no.spam-tut.fi)
 * @author Pekka Jääskeläinen 2011
 * @author Vinogradov Viacheslav(added SV generating) 2012 
 * @note rating: red
 */

#ifndef COPROCESSOR_GENERATOR_HH
#define COPROCESSOR_GENERATOR_HH

#include "Exception.hh"
#include "ProGeContext.hh"
#include "ProGeTypes.hh"
#include "TCEString.hh"
#include "ProGeOptions.hh"
#include "ProcessorGenerator.hh"
#include "HDLTemplateInstantiator.hh"

namespace TTAMachine {
    class Machine;
    class FunctionUnit;
}

namespace IDF {
    class MachineImplementation;
}

class BinaryEncoding;
class FUPortCode;

namespace ProGe{

    class ICDecoderGeneratorPlugin;
    class Netlist;
    class NetlistBlock;
    class ProGeContext;
    class NetlistGenerator;

    /**
     * Controller class of ProGe.
     */
    class CoproGen : public ProcessorGenerator {
    public:
        CoproGen();
        virtual ~CoproGen();

        void coproGenerate(
            const ProGeOptions& options, const TTAMachine::Machine& machine,
            const IDF::MachineImplementation& implementation,
            ICDecoderGeneratorPlugin& plugin, int imemWidthInMAUs,
            std::ostream& errorStream, std::ostream& warningStream,
            std::ostream& verboseStream);

    private:
        void validateMachine(
            const TTAMachine::Machine& machine, std::ostream& errorStream,
            std::ostream& warningStream);
        void generateSupPackage(const std::string& dstDirectory);
        void generateinsdecoder(const ProGeOptions& options); //Instruciton decoder maker
        void makecoprocessor(const ProGeOptions& options, 
            IDF::FUGenerated& Fu,const TTAMachine::Machine& machine);
        void makeROCCcoprocessor(const ProGeOptions& options, IDF::FUGenerated& Fu,
    const TTAMachine::Machine& machine);
        

        NetlistBlock* coreTopBlock_;
        TCEString entityStr_;
        ProGeContext* generatorContext_;

        static const TCEString DEFAULT_ENTITY_STR;
        /// Object that instantiates templates.
        HDLTemplateInstantiator instantiate_;
    };
}

#endif
