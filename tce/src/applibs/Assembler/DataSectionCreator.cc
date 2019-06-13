/*
    Copyright (c) 2002-2009 Tampere University.

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
 * @file DataSectionCreator.hh
 *
 * Definition of DataSectionCreator class.
 *
 * @author Mikael Lepistö 2005 (tmlepist-no.spam-cs.tut.fi)
 * @note rating: yellow
 */

#include <map>
#include <vector>

#include "DataSectionCreator.hh"
#include "LabelManager.hh"
#include "Assembler.hh"

#include "MapTools.hh"

#include "Binary.hh"
#include "Section.hh"
#include "DataSection.hh"
#include "MachineResourceManager.hh"
#include "MathTools.hh"

using namespace TPEF;

/**
 * Constructor.
 *
 * @param resourceManager Resource manager for TPEF to create.
 */
DataSectionCreator::DataSectionCreator(
    MachineResourceManager& resourceManager, Assembler* /*parent*/) :
    resources_(resourceManager), currentStartAddress_(0),
    isStartAddressDefined_(false) {
}

/**
 * Destructor.
 */
DataSectionCreator::~DataSectionCreator() {
    cleanup();
}

/**
 * Sets fixed address for the next data area definition.
 *
 * @param address Start address of next data area definition.
 */
void DataSectionCreator::setAreaStartAddress(UValue address) {
    currentStartAddress_ = address;
    isStartAddressDefined_ = true;
}

/**
 * Adds data line.
 *
 * @param origLine Parsed data line to copy to creator.
 */
void
DataSectionCreator::addDataLine(const DataLine &origLine) {

    // make copy of original
    DataLine* line = new DataLine;

    line->dataSpace = origLine.dataSpace;
    line->width = origLine.width;
    line->initData = origLine.initData;
    line->labels = origLine.labels;
    line->asmLineNumber = origLine.asmLineNumber;

    // find out section where to we add line by
    // address and address space
    assert(line != NULL);

    InternalSection* foundSection = NULL;
    for (int foundIndex = sections_.size() - 1;
         foundIndex >= 0; foundIndex--) {

        InternalSection& section = sections_[foundIndex];

        // try to find last suitable section for the data line
        if (section.addressSpace == line->dataSpace) {

            // we always create new section for datalines with fixed address,
            // otherwise we just check, if last created section is fine for
            // currently added line

            if (section.isInitialized != line->initData.empty() &&
                !isStartAddressDefined_) {

                // section with same type of data lines was found
                foundSection = &section;
                break;

            } else {
                // section with same type of data lines
                // was not found -> create new section
                break;
            }
        }
    }

    // create new section if suitable is not found
    if (foundSection == NULL) {
        InternalSection newSection;

        newSection.isInitialized = !line->initData.empty();
        newSection.fixedStartAddress = isStartAddressDefined_;
        newSection.startAddress = currentStartAddress_;
        newSection.addressSpace = line->dataSpace;
        newSection.length = 0;

        sections_.push_back(newSection);

        // set found section
        foundSection = &sections_.back();

        isStartAddressDefined_ = false;
        currentStartAddress_ = 0;
    }

    assert(foundSection != NULL);

    // add line to section
    foundSection->lines.push_back(line);
}

/**
 * Frees all internally allocated data.
 */
void
DataSectionCreator::cleanup() {

    for (unsigned int i = 0; i < sections_.size(); i++) {
        InternalSection &section = sections_[i];

        for (unsigned int j = 0; j < section.lines.size(); j++) {
            section.lines[j]->initData.clear();
            section.lines[j]->labels.clear();
            delete section.lines[j];
        }

        section.lines.clear();
    }

    sections_.clear();

    currentStartAddress_ = 0;
    isStartAddressDefined_ = false;
}

/**
 * Writes created sections to given binary.
 *
 * All data stored inside creator are freed after this call, unless
 * exception is thrown.
 *
 * In case of exception the creator restores its state to what it was before
 * calling this method, and this method can be run again.
 *
 * @param tpef Binary to which created sections should be added.
 * @param labels LabelManager where to add data labels and relocations.
 * @exception CompileError If there is any errors during compiling.
 */
void
DataSectionCreator::finalize(
    Binary& tpef, LabelManager& labels, bool littleEndian) {
    // completed tpef data sections.
    std::vector<TPEF::Section*> completed;

    // find out start addresses of each section and resolve label values
    std::map<std::string, UValue> addressSpaceUsedMAUs;
    std::vector<std::string> addedLabels;

    try {

        resolveDataAreaSizesAndLabelAddresses(
            addressSpaceUsedMAUs, addedLabels, labels);

        // create TPEF sections and add initialization data + relocation 
        // elements
        for (unsigned int i = 0; i < sections_.size(); i++) {
            InternalSection  &section = sections_[i];

            Section* newSection = NULL;

            // if initialized, generate initialization data
            if (section.isInitialized) {
                if (littleEndian) {
                    newSection = Section::createSection(Section::ST_LEDATA);
                } else {
                    newSection = Section::createSection(Section::ST_DATA);
                }
            } else {
                newSection = Section::createSection(Section::ST_UDATA);
            }

            // add section to completed sections
            completed.push_back(newSection);

            newSection->setName(resources_.stringToChunk(""));

            assert(section.fixedStartAddress);
            newSection->setStartingAddress(section.startAddress);

            ASpaceElement* aSpace = NULL;

            // wrap IllegalMachine to
            // CompileError exception with linenumber
            try {
                aSpace = 
                    resources_.findDataAddressSpace(section.addressSpace);

            } catch (IllegalMachine& e) {
                CompileError error(
                    __FILE__, __LINE__, __func__, e.errorMessage());

                error.setCodeFileLineNumber(section.lines[0]->asmLineNumber);
                error.setCause(e);

                throw error;

            } catch (InvalidData& e) {
                CompileError error(
                    __FILE__, __LINE__, __func__,
                    "Can't write data to instruction memory.");

                error.setCodeFileLineNumber(section.lines[0]->asmLineNumber);
                error.setCause(e);

                throw error;
            }

            assert(aSpace != NULL);
            newSection->setASpace(aSpace);

            // sets data section length in MAUs
            RawSection *rawSection = dynamic_cast<RawSection*>(newSection);
            assert(rawSection != NULL);
            rawSection->setLengthInMAUs(section.length);                   

            // aSpace had to be set before writing initialization data of 
            // section
            if (section.isInitialized) {
                DataSection *dataSection = 
                    dynamic_cast<DataSection*>(newSection);

                UValue currentMAUIndex = 0;

                for (unsigned int j = 0; j < section.lines.size(); j++) {
                    DataLine *line = section.lines[j];
                    currentMAUIndex += writeDataLineToTPEF(
                        line, dataSection, labels, currentMAUIndex);
                }
            }
        }
        
    } catch (CompileError& e) {

        labels.clearLastRelocations();

        // clean added labels
        for (unsigned int i = 0; i < addedLabels.size(); i++) {
            labels.removeLabel(addedLabels[i]);
        }

        // clean already created stuff and throw error forward
        for (UValue j = 0; j < completed.size(); j++) {
            delete completed[j];
        }

        completed.clear();

        throw e;

    }

    // really add relocations to manager
    labels.commitLastRelocations();

    // add completed sections to binary
    for (UValue i = 0; i < completed.size(); i++) {
        tpef.addSection(completed[i]);
    }

    cleanup();
}

/**
 * Resolves data area definition sizes, addresses and
 * writes resolved data labels to label manager.
 *
 * @param addressSpaceUsedMAUs Table for updating number of MAUs that are
 *                             used for each section.
 * @param addedLabels List of all labels added to label manager for
 *                    error cleanup.
 * @param labels Label bookkeeper.
 */
void
DataSectionCreator::resolveDataAreaSizesAndLabelAddresses(
    std::map<std::string, UValue>& addressSpaceUsedMAUs,
    std::vector<std::string>& addedLabels, LabelManager& labels) {
    // calculate sizes and addresses for each data section and
    // add values for labels

    for (unsigned int i = 0; i < sections_.size(); i++) {
        InternalSection  &section = sections_[i];
        UValue spaceLastAddress;

        if (!MapTools::containsKey(
                addressSpaceUsedMAUs, section.addressSpace)) {

            addressSpaceUsedMAUs[section.addressSpace] = 0;
        }

        // last used address of address space of current section
        spaceLastAddress = addressSpaceUsedMAUs[section.addressSpace];

        // check if we have to look, if current address is ok or
        // we set the address
        if (section.fixedStartAddress) {
            if (spaceLastAddress > section.startAddress) {

                CompileError error(
                    __FILE__, __LINE__, __func__,
                    "Can't start data section from address: " +
                    Conversion::toString(section.startAddress) +
                    " first unused address in address space: " +
                    section.addressSpace +
                    " is: " + Conversion::toString(spaceLastAddress));

                error.setCodeFileLineNumber(section.lines[0]->asmLineNumber);

                throw error;
            }
        } else {
            section.startAddress = spaceLastAddress;
        }

        section.fixedStartAddress = true;

        // add also labels to label map
        UValue dataLineAddress = section.startAddress;

        for (unsigned int j = 0; j < section.lines.size(); j++) {
            DataLine *currentLine = section.lines[j];

            // wrap IllegalMachine and ObjectAlreadyExists exceptions
            // to CompileError exception with linenumber
            try {

                for (unsigned int k = 0;
                     k < currentLine->labels.size(); k++) {

                    labels.addLabel(
                        *resources_.findDataAddressSpace(
                            section.addressSpace),
                        currentLine->labels[k], dataLineAddress);
                    
                    // for cleanup 
                    addedLabels.push_back(currentLine->labels[k]);
                }

                dataLineAddress += currentLine->width;

            } catch (InvalidData& e) {
                CompileError error(
                    __FILE__, __LINE__, __func__,
                    "Can't write data to instruction memory.");

                error.setCodeFileLineNumber(currentLine->asmLineNumber);
                error.setCause(e);

                throw error;

            } catch (Exception& e) {

                CompileError error(
                    __FILE__, __LINE__, __func__, e.errorMessage());

                error.setCodeFileLineNumber(currentLine->asmLineNumber);
                error.setCause(e);

                throw error;
            }
        }

        section.length = dataLineAddress - section.startAddress;
        addressSpaceUsedMAUs[section.addressSpace] = dataLineAddress;
#if 0
        // some useful dedug info about created data sections and mapping
        std::cerr << "Section length: " << section.length
                  << "\tSection start:  " << section.startAddress
                  << std::endl;
#endif
        // NOTE: here we can check if we can merge two sections together.
        //       If both are initialized or uninitialized and second
        //       section's start address is same that first section
        //       start address + first section size then merging can
        //       be done.
    }
}

/**
 * Write one data line definition to TPEF data section.
 *
 * @param line Data line to write.
 * @param dataSection Section where to write data.
 * @param label Label manager to resolve label values.
 * @param currentMAUIndex MAU index to start of data line in TPEF.
 * @return MAU count that were written to TPEF.
 * @exception CompileError the data line contains errors.
 */
UValue
DataSectionCreator::writeDataLineToTPEF(
    DataLine* line, DataSection* dataSection, LabelManager& labels,
    UValue currentMAUIndex) {
    UValue writtenMAUs = 0;

    // read maus from init data until inited data of line ends
    for (unsigned int k = 0; k < line->initData.size(); k++) {
        InitDataField &data = line->initData[k];
        
        UValue value = 0;
        bool isSigned = false;

        // if expression resolve value
        if(data.litOrExpr.isExpression) {
            value = labels.resolveExpressionValue(
                line->asmLineNumber, data.litOrExpr);
        } else {
            value = data.litOrExpr.value;
            isSigned = data.litOrExpr.isSigned;
        }

        UValue usedMAUs =
            sizeInWords(value, dataSection->aSpace()->MAU(), isSigned);

        // if data width is explicitely set use that width
        if (data.width != 0) {
            if (usedMAUs > data.width) {

                CompileError error(
                    __FILE__, __LINE__, __func__,
                    "Init field contains too long value. Reserved MAUs: " +
                    Conversion::toString(data.width) +
                    " Needed MAUs: " +
                    Conversion::toString(usedMAUs));

                error.setCodeFileLineNumber(line->asmLineNumber);

                throw error;
            }

            usedMAUs = data.width;
        }

        // check if data line width is enough for writing next value
        if (writtenMAUs + usedMAUs > line->width) {

            CompileError error(
                __FILE__, __LINE__, __func__,
                "Data line contains too much init data. Reserved MAUs: " +
                Conversion::toString(line->width) +
                " Already inited MAUs: " +
                Conversion::toString(writtenMAUs));

            error.setCodeFileLineNumber(line->asmLineNumber);

            throw error;
        }

        // select extending for writing data to data section
        if (isSigned) {
            dataSection->writeValue(
                currentMAUIndex + writtenMAUs, 
                usedMAUs, static_cast<SValue>(value));

        } else {
            dataSection->writeValue(
                currentMAUIndex + writtenMAUs, usedMAUs, value);
        }

        // if expression then add relocation.
        if(data.litOrExpr.isExpression) {
            Chunk *srcChunk =
                dataSection->chunk(
                    dataSection->MAUsToBytes(
                        currentMAUIndex + writtenMAUs));

            labels.addRelocation(
                *dataSection, *srcChunk,
                labels.aSpaceElement(data.litOrExpr.expression.label),
                value, usedMAUs * dataSection->aSpace()->MAU());
        }

        writtenMAUs += usedMAUs;
    }

    if (writtenMAUs  < line->width) {
        dataSection->writeValue(currentMAUIndex+writtenMAUs, 1,
                                static_cast<unsigned long>(0));
        writtenMAUs++;
    }

    // increase index to data section
    return line->width;
}

/**
 * Compute the minimum number of words of given bit width that are necessary
 * to encode an integer value.
 *
 * @param value Integer value to encode.
 * @param width Bit width of the words.
 * @param isSigned Tells whether the value had a sign ('+'/'-') in front of
 *        it.
 */
int
DataSectionCreator::sizeInWords(UValue value, int width, bool isSigned)
    const {

    // maximum word width allowed: 32 bits
    assert(static_cast<unsigned int>(width) <= sizeof(value) * BYTE_BITWIDTH);

    int neededBits = 0;
    
    if (isSigned) {
        neededBits = MathTools::requiredBitsSigned(value);
    } else {
        neededBits = MathTools::requiredBits(value);     
    }
    
    int wordCount = static_cast<int>(
        ceil(static_cast<double>(neededBits) / 
             static_cast<double>(width)));
  
    return wordCount;
}

