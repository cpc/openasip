/*
 Copyright (c) 2002-2015 Tampere University.

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
/*
 * @file CompileTools.cc
 *
 * Implementation of CompileTools class.
 *
 * Created on: 13.3.2015
 * @author: Henry Linjamäki (henry.linjamaki-no.spam-tut.fi)
 * @note rating: red
 */

#include "CompileTools.hh"

#include <vector>
#include <fstream>

#include "Machine.hh"
#include "Program.hh"
#include "ADFSerializer.hh"
#include "Application.hh"
#include "FileSystem.hh"
#include "Exception.hh"

CompileTools::CompileTools() {
}

CompileTools::~CompileTools() {
}

TTAProgram::Program*
CompileTools::compileAsC(
    const TTAMachine::Machine& target,
    const std::string& code,
    const std::string& cflags) {

    std::stringstream codeStream(code);
    return compileAsC(target, codeStream, cflags);
}

/**
 * Interprets code as C code and tries to convert it to Program.
 *
 * @return Pointer to new program if compilation is successful, otherwise NULL.
 */
TTAProgram::Program*
CompileTools::compileAsC(
    const TTAMachine::Machine& target,
    std::stringstream& code,
    const std::string& cflags) {
    using namespace TTAProgram;

    Program* resultProgram = NULL;

    std::string tempDir = FileSystem::createTempDirectory(
        FileSystem::currentWorkingDir());
    if (tempDir.empty()) {
        return NULL;
    }
    std::string tempAdfFile =
        tempDir + FileSystem::DIRECTORY_SEPARATOR + "temp.adf";
    std::string sourceCFile =
        tempDir + FileSystem::DIRECTORY_SEPARATOR + "temp.c";
    std::string resultFile =
        tempDir + FileSystem::DIRECTORY_SEPARATOR + "temp.tpef";

    ADFSerializer serializer;
    serializer.setDestinationFile(tempAdfFile);
    try {
        serializer.writeMachine(target);
    } catch (const SerializerException& exception) {
        FileSystem::removeFileOrDirectory(tempDir);
        return NULL;
    }

    std::ofstream cStream(sourceCFile);
    if (cStream.fail()) {
        FileSystem::removeFileOrDirectory(tempDir);
        return NULL;
    }
    cStream << code.rdbuf();
    cStream.close();

    std::string command = Environment::tceCompiler() + " "
        "-a " + tempAdfFile + " " +
        "-o " + resultFile + " " +
        cflags + " " +
        sourceCFile;
    std::vector<std::string> output;

    if (Application::runShellCommandAndGetOutput(command, output) == 0 &&
        FileSystem::fileExists(resultFile)) {
        try {
            resultProgram = Program::loadFromTPEF(resultFile, target);
        } catch (Exception& e) {
            FileSystem::removeFileOrDirectory(tempDir);
            return NULL;
        }
    }

    FileSystem::removeFileOrDirectory(tempDir);
    return resultProgram;
}

/**
 * Interprets string stream as LLVM (.ll) assembly code and tries to convert
 * it to Program.
 *
 * @return Pointer to new program if compilation is successful, otherwise NULL.
 */
TTAProgram::Program*
CompileTools::compileAsLLVM(
    const TTAMachine::Machine& target,
    const std::stringstream& code,
    const std::string compileOptions) {
    using namespace TTAProgram;

    Program* resultProgram = NULL;

    std::string tempDir = FileSystem::createTempDirectory(
        FileSystem::currentWorkingDir());
    if (tempDir.empty()) {
        return NULL;
    }
    std::string tempAdfFile =
        tempDir + FileSystem::DIRECTORY_SEPARATOR + "temp.adf";
    std::string sourceLLFile =
        tempDir + FileSystem::DIRECTORY_SEPARATOR + "temp.ll";
    std::string resultFile =
        tempDir + FileSystem::DIRECTORY_SEPARATOR + "temp.tpef";

    ADFSerializer serializer;
    serializer.setDestinationFile(tempAdfFile);
    try {
        serializer.writeMachine(target);
    } catch (const SerializerException& exception) {
        FileSystem::removeFileOrDirectory(tempDir);
        return NULL;
    }

    std::ofstream llStream(sourceLLFile);
    if (llStream.fail()) {
        FileSystem::removeFileOrDirectory(tempDir);
        return NULL;
    }
    llStream << code.rdbuf();
    llStream.close();

    std::string compileLLFilecmd = Environment::tceCompiler() + " "
        +  "-a " + tempAdfFile + " "
        + "-o " + resultFile + " " + " "
        + compileOptions + " "
        +  sourceLLFile;

    if (Application::runShellCommandSilently(compileLLFilecmd) == 0) {
        try {
            resultProgram = Program::loadFromTPEF(resultFile, target);
        } catch (Exception& e) {
            FileSystem::removeFileOrDirectory(tempDir);
            return NULL;
        }
    }

    FileSystem::removeFileOrDirectory(tempDir);
    return resultProgram;
}

