/**
 * @file GenerateBits.cc
 *
 * Implementation of the main function of generatebits application.
 *
 * @author Lasse Laasonen 2005 (lasse.laasonen@tut.fi)
 * @note rating: red
 */

#include <iostream>

#include "PIGCmdLineOptions.hh"
#include "ProgramImageGenerator.hh"
#include "PIGCLITextGenerator.hh"

#include "ADFSerializer.hh"
#include "Machine.hh"
#include "ControlUnit.hh"

#include "BinaryEncoding.hh"
#include "BEMSerializer.hh"
#include "BEMGenerator.hh"

#include "Binary.hh"
#include "BinaryStream.hh"
#include "BinaryReader.hh"

#include "FileSystem.hh"

using std::cerr;
using std::endl;
using std::string;
using std::ofstream;

using namespace TPEF;
using namespace TTAMachine;

/**
 * Loads the given TPEF file and creates a Binary instance from it.
 *
 * @param tpefFile The TPEF file.
 * @return The newly created Binary instance.
 * @exception InstanceNotFound If instance for reading wasn't found.
 * @exception UnreachableStream If given file can't be read.
 * @exception KeyAlreadyExists Key was in use when trying to register object.
 * @exception EndOfFile If end of file were reached while it shouldn't.
 * @exception OutOfRange Some read value was out of range.
 * @exception WrongSubclass Some class couldn't do what it was asked for.
 * @exception UnexpectedValue If there was unexpected value when reading.
 */
static Binary*
loadTPEF(const std::string& tpefFile)
    throw (InstanceNotFound, UnreachableStream, KeyAlreadyExists, EndOfFile,
           OutOfRange, WrongSubclass, UnexpectedValue) {

    BinaryStream stream(tpefFile);
    return BinaryReader::readBinary(stream);
}


/**
 * Loads the given BEM file and creates a BinaryEncoding instance from it.
 *
 * @param bemFile The BEM file.
 * @return The newly created BinaryEncoding instance.
 * @exception SerializerException If an error occurs while reading the 
 *                                file.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state of BinaryEncoding 
 *                                        instance.
 */
static BinaryEncoding*
loadBEM(const std::string& bemFile) 
    throw (SerializerException, ObjectStateLoadingException) {

    BEMSerializer serializer;
    serializer.setSourceFile(bemFile);
    return serializer.readBinaryEncoding();
}


/**
 * Loads the given ADF file and creates a Machine instance from it.
 *
 * @param adfFile The ADF file.
 * @return The newly created Machine instance.
 * @exception SerializerException If an error occurs while reading the file.
 * @exception ObjectStateLoadingException If an error occurs while loading
 *                                        the state of Machine instance.
 */
static Machine*
loadMachine(const std::string& adfFile) 
    throw (SerializerException, ObjectStateLoadingException) {

    ADFSerializer serializer;
    serializer.setSourceFile(adfFile);
    return serializer.readMachine();
}


/**
 * Tells whether the given address space is instruction memory.
 *
 * @param as The address space.
 * @return True if the address space is instruction memory, otherwise false.
 */
static bool
isInstructionMemory(const TTAMachine::AddressSpace& as) {
    Machine* mach = as.machine();
    ControlUnit* gcu = mach->controlUnit();
    if (gcu != NULL && gcu->addressSpace() == &as) {
        return true;
    } else {
        return false;
    }
}


/**
 * Returns the name of the program image file for the given TPEF file.
 *
 * @param tpefFile Name of the TPEF file.
 * @return Name of the program image file.
 */
std::string
programImageFile(const std::string& tpefFile) {
    return FileSystem::fileNameBody(tpefFile) + ".img";
}


/**
 * Parses the given parameter which has form 'paramname=paramvalue" to
 * different strings.
 *
 * @param param The parameter.
 * @param paramName Parameter name is stored here.
 * @param paramValue Parameter value is stored here.
 * @exception InvalidData If the given parameter is not in the correct form.
 */
void
parseParameter(
    const std::string& param,
    std::string& paramName,
    std::string& paramValue)
    throw (InvalidData) {

    unsigned int separatorPos = param.find("=", 0);
    if (separatorPos == string::npos) {
        string errorMsg = 
            "Compressor parameters must be in form "
            "'parametername=parametervalue'.";
        throw InvalidData(__FILE__, __LINE__, __func__, errorMsg);
    }

    paramName = param.substr(0, separatorPos);
    paramValue = param.substr(separatorPos+1, param.length());
}   


/**
 * The main function of generatebits application.
 */
int main(int argc, char* argv[]) {

    PIGCmdLineOptions options;
    try {
        options.parse(argv, argc);
    } catch (ParserStopRequest) {
        return EXIT_SUCCESS;
    } catch (const IllegalCommandLine& exception) {
        cerr << exception.errorMessage() << endl;
        return EXIT_FAILURE;
    }
    
    string adfFile = "";
    if (options.numberOfArguments() < 1) {
        PIGCLITextGenerator textGen;
        cerr << textGen.text(PIGCLITextGenerator::TXT_ADF_REQUIRED) << endl;
        return EXIT_FAILURE;
    }
    else if (options.numberOfArguments() > 1) {
        PIGCLITextGenerator textGen;
        cerr << textGen.text(PIGCLITextGenerator::TXT_ILLEGAL_ARGS) << endl;
        return EXIT_FAILURE;
    }
    else {
        adfFile = options.argument(1);
    }
    
    string bemFile = options.bemFile();
    string piFormat = options.programImageOutputFormat();
    string diFormat = options.dataImageOutputFormat();
    int dmemMAUsPerLine = options.dataMemoryWidthInMAUs();
    int imemMAUsPerLine = options.instructionMemoryWidthInMAUs();
    string compressor = options.compressorPlugin();
    bool generateDataImages = options.generateDataImages();
    bool generateDecompressor = options.generateDecompressor();
    bool showCompressors = options.showCompressors();

    if (showCompressors) {
        std::vector<string> compressorFiles = 
            ProgramImageGenerator::availableCompressors();
        for (std::vector<string>::const_iterator iter = 
                 compressorFiles.begin();
             iter != compressorFiles.end(); iter++) {
            try {
                std::cout << "******************************************"
                          << "**********************" << endl;
                std::cout << "Compressor file: " << *iter << endl;
             
                ProgramImageGenerator::printCompressorDescription(
                    *iter, std::cout);
            } catch (const Exception& e) {
                cerr << "Error: " << e.errorMessage() << endl;
            }
            std::cout << std::endl;
        }
        return EXIT_SUCCESS;
    }       

    CodeCompressorPlugin::ParameterTable compressorParams;
    for (int i = 0; i < options.compressorParameterCount(); i++) {
        string param = options.compressorParameter(i);
        string paramName;
        string paramValue;
        try {
            parseParameter(param, paramName, paramValue);
        } catch (const Exception& e) {
            cerr << e.errorMessage() << endl;
            return EXIT_FAILURE;
        }
        CodeCompressorPlugin::Parameter newParam = {paramName, paramValue};
        compressorParams.push_back(newParam);
    }
    
    if (adfFile == "" || 
        (piFormat != "" && piFormat != "binary" && piFormat != "ascii" &&
         piFormat != "array") || 
        (diFormat != "" && diFormat != "binary" &&
         diFormat != "ascii" && diFormat != "array")) {
        options.printHelp();
        return EXIT_FAILURE;
    }

    std::vector<Binary*> tpefTable;        

    try {
        Machine* mach = loadMachine(adfFile);

        for (int i = 0; i < options.tpefFileCount(); i++) {
            string tpefFile = options.tpefFile(i);
            tpefTable.push_back(loadTPEF(tpefFile));
        }

        // create set of the Binaries
        std::set<Binary*> tpefSet;
        for (size_t i = 0; i < tpefTable.size(); i++) {
            tpefSet.insert(tpefTable[i]);
        }

        BinaryEncoding* bem = NULL;
        
        if (bemFile != "") {
            bem = loadBEM(bemFile);
        } else {
            PIGCLITextGenerator textGen;
            std::cerr 
                << textGen.text(PIGCLITextGenerator::TXT_GENERATING_BEM).
                str() << std::endl;
            BEMGenerator bemGenerator(*mach);
            bem = bemGenerator.generate();
        }
                    
        ProgramImageGenerator imageGenerator;
        if (compressor != "") {
            imageGenerator.loadCompressorPlugin(compressor);
        }
        imageGenerator.loadCompressorParameters(compressorParams);
        imageGenerator.loadMachine(*mach);
        imageGenerator.loadBEM(*bem);
        imageGenerator.loadPrograms(tpefSet);

        for (size_t i = 0; i < tpefTable.size(); i++) {

            Binary* program = tpefTable[i];
            string tpefFile = FileSystem::fileOfPath(options.tpefFile(i));
            string imageFile = programImageFile(tpefFile);
            ofstream piStream(imageFile.c_str());
            
            if (piFormat == "binary") {
                imageGenerator.generateProgramImage(
                    *program, piStream, ProgramImageGenerator::BINARY);
            } else if (piFormat == "array") {
                imageGenerator.generateProgramImage(
                    *program, piStream, ProgramImageGenerator::ARRAY,
                    imemMAUsPerLine);
            } else {
                assert(piFormat == "ascii" || piFormat == "");
                imageGenerator.generateProgramImage(
                    *program, piStream, ProgramImageGenerator::ASCII,
                    imemMAUsPerLine);
            }
            
            piStream.close();

            if (generateDataImages) {
                Machine::AddressSpaceNavigator asNav = 
                    mach->addressSpaceNavigator();
                for (int i = 0; i < asNav.count(); i++) {
                    AddressSpace* as = asNav.item(i);
                    if (!isInstructionMemory(*as)) {
                        string fileName = 
                            FileSystem::fileNameBody(tpefFile) + "_" +
                            as->name() + ".img";
                        ofstream stream(fileName.c_str());
                        if (diFormat == "binary") {
                            imageGenerator.generateDataImage(
                                *program, as->name(), stream, 
                                ProgramImageGenerator::BINARY, 0, true);
                        } else if (diFormat == "array") {
                            imageGenerator.generateDataImage(
                                *program, as->name(), stream, 
                                ProgramImageGenerator::ARRAY,
                                dmemMAUsPerLine, true);
                        } else {
                            assert(diFormat == "ascii" || diFormat == "");
                            imageGenerator.generateDataImage(
                                *program, as->name(), stream, 
                                ProgramImageGenerator::ASCII,
                                dmemMAUsPerLine, true);
                        }
                        stream.close();
                    }
                }
            }
        }

        if (generateDecompressor) {
            string decompressorFile = "decompressor.vhdl";
            bool created = FileSystem::createFile(decompressorFile);
            if (!created) {
                string errorMsg = "Unable to create file " + 
                    decompressorFile;
                throw IOException(__FILE__, __LINE__, __func__, errorMsg);
            }
            std::ofstream decompressorStream(
                decompressorFile.c_str(), std::ofstream::out);
            imageGenerator.generateDecompressor(decompressorStream);
            decompressorStream.close();
        }
        
        for (std::vector<Binary*>::iterator iter = tpefTable.begin();
             iter != tpefTable.end(); iter++) {
            delete *iter;
        }
        delete mach;
        delete bem;

        return EXIT_SUCCESS;

    } catch (const Exception& exception) {
        cerr << exception.errorMessageStack() << endl;
        return EXIT_FAILURE;
    }
}
