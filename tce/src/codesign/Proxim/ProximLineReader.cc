/*
    Copyright (c) 2002-2009 Tampere University of Technology.

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
 * @file ProximLineReader.hh
 *
 * Implementation of ProximLineReader class.
 *
 * @author Veli-Pekka J‰‰skel‰inen 2005 (vjaaskel-no.spam-cs.tut.fi)
 * @note rating: red
 */

#include <iostream>
#include "ProximLineReader.hh"
#include "WxConversion.hh"
#include "Proxim.hh"
#include "SimulatorEvent.hh"
#include "ProximSimulationThread.hh"
#include "Conversion.hh"
#include "OperationBehavior.hh"
#include "OperationGlobals.hh"

const std::string ProximLineReader::DEFAULT_LOG_FILE_NAME = ".proximHistory";

/**
 * The Constructor.
 */
ProximLineReader::ProximLineReader() :
    LineReader(),
    mutex_(new wxMutex()),
    input_(new wxCondition(*mutex_)),
    outputStream_(new ProximLROutputStream(this)) {

    setInputHistoryLog(DEFAULT_LOG_FILE_NAME);
}


/**
 * The Destructor.
 */
ProximLineReader::~ProximLineReader() {
    delete mutex_;
    delete input_;
}


/**
 * Initializes the linereader.
 *
 * The FILE* pointer parameters specified in the base class are not used.
 *
 * @param defPrompt Default prompt.
 */
void
ProximLineReader::initialize(
    std::string defPrompt, FILE*, FILE*, FILE*) {

    gui_ = wxGetApp().GetTopWindow();
    prompt_ = defPrompt;
    setInitialized();

    // Set the linereader output stream as OSAL output stream.
    OperationGlobals::setOutputStream(outputStream());
}


/**
 * Waits for input from the GUI thread.
 *
 * The input_ condition timeout is set as 100ms. After the timeout
 * an empty input is returned. This is done because the worker thread
 * needs to check its stop and delete requests at regular intervals.
 *
 * @param prompt Prompt used for requesting the user input.
 */
std::string
ProximLineReader::readLine(std::string prompt)
    throw (ObjectNotInitialized, EndOfFile) {

    if (!initialized()) {
	std::string method = "ProximLineReader::readLine";
	throw ObjectNotInitialized(__FILE__, __LINE__, method);
    }

    if (prompt == "") {
	prompt = prompt_;
    }

    if (inputQueue_.empty()) {
        output(prompt);
        input_->Wait();
    }
    std::string input = inputQueue_.front();
    inputQueue_.pop();


    output(input + "\n");

    // Append read input to the command history.
    putInInputHistory(input);
    return input;
}


/**
 * This function is called by the GUI thread to set the user input.
 *
 * The input_ condtion is signaled about the user input.
 *
 * @param command The user input.
 */
void
ProximLineReader::input(std::string input) {
    inputQueue_.push(input);
    // Signal simulation about the input.
    input_->Signal();
}


/**
 * Requests answer to a char question from the GUI thread.
 *
 * @param question Question to ask from the user.
 * @param allowedChars Allowed answers.
 * @param caseSensitive True, if the answer characters are case sensitive.
 * @param defaultAnswer Default answer to the question.
 * @return Answer to the char question.
 */
char
ProximLineReader::charQuestion(
    std::string question,
    std::string allowedChars,
    bool /* (case sensitive)  UNUSED */,
    char /* (default answer) UNUSED */) throw (ObjectNotInitialized) {

    if (!initialized()) {
	throw ObjectNotInitialized(__FILE__, __LINE__);
    }


    // Char question answer is requested from the GUI thread until
    // a valid answer is received.
    std::string answer;
    do {
        if (inputQueue_.empty()) {
            output(question + " [" + allowedChars + "]? ");
            input_->Wait();
        }
        answer = inputQueue_.front();
        inputQueue_.pop();

        output(answer + "\n");

        if (answer.length() != 1) {
            continue;
        }
    } while (allowedChars.find(answer, 0) == std::string::npos);

    return answer[0];
}


/**
 * Returns an outputstream, which the worker thread can use to write text
 * to the GUI.
 *
 * @return Reference to an output stream, which can be utilized to write text
 *         to the GUI.
 */
std::ostream&
ProximLineReader::outputStream() {
    return *outputStream_;
}


/**
 *
 */
void
ProximLineReader::output(std::string output) {
    SimulatorEvent event(SimulatorEvent::EVT_SIMULATOR_OUTPUT, output);
    wxPostEvent(gui_, event);
}


/**
 * Constructor.
 *
 * @param lineReader ProximLineReader used for output.
 */
ProximLROutputStream::ProximLROutputStream(
    ProximLineReader* lineReader):
    std::ostream(new ProximLROutputBuffer(lineReader)) {
}


/**
 * The Destructor.
 */
ProximLROutputStream::~ProximLROutputStream() {
}


/**
 * The Constructor.
 *
 * @param lineReader ProximLineReader used for output.
 */
ProximLROutputBuffer::ProximLROutputBuffer(ProximLineReader* lineReader):
    std::streambuf(),
    lineReader_(lineReader),
    BUFFER_SIZE(1024)  {

    char* ptr = new char[BUFFER_SIZE];
    setp(ptr, ptr + BUFFER_SIZE);
    setg(0, 0, 0);
}


/**
 * The Destructor.
 */
ProximLROutputBuffer::~ProximLROutputBuffer() {
    sync();
}

/**
 * Puts a character at current put position.
 *
 * This function is called in case there is no room in the buffer to perform
 * the output operation.
 */
int
ProximLROutputBuffer::overflow(int c) {

    flushBuffer();

    if (c != EOF) {
	if(pbase() == epptr()) {
	    // The buffer length is zero, character is sent directly to the
	    // linereader.
	    lineReader_->output("" + char(c));
	} else {
	    // Append char to the flushed buffer.
	    sputc(c);
	}
    }
    return 0;
}

/**
 * Synchronizes the streambuffer by flushing buffer contents to the linereader.
 */
int
ProximLROutputBuffer::sync() {
    flushBuffer();
    return 0;
}


/**
 * Sends the buffer contents to the linereader.
 */
void
ProximLROutputBuffer::flushBuffer() {
    if (pbase() != pptr()) {
	// Buffer is not empty.
	int len = pptr() - pbase();
	char* buffer = new char[len+1];
	strncpy(buffer, pbase(), len);
	buffer[len] = '\0';
	lineReader_->output(std::string(buffer));
	setp(pbase(), epptr());
	delete[] buffer;
    }
}


/**
 * Sets the name of the input history log file.
 *
 * @param hisoryFileName Name of the input history log file.
 */
void
ProximLineReader::setInputHistoryLog(const std::string& historyFileName)
    throw (IOException) {
    historyFile_ = historyFileName;
    LineReader::setInputHistoryLog(historyFileName);
}


/**
 * Returns input history log file name.
 *
 * @return Name of the input history log file.
 */
std::string
ProximLineReader::inputHistoryFilename() const {
    return historyFile_;
}

