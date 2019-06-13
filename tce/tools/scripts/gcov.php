#!/usr/bin/php -qC
<?php
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
 * Class that represents one test session
 */
class Test {
    // file that shows which files have coverage less than the limit
    var $LESSTHAN_FILE;
    // the used tempfile2
    var $tempfile;
    var $tempfile2;
    // percentage limit
    var $limit;
    // list of excluded directories
    var $excludeDirs;
    // list of exluded files
    var $excludeFiles;
    // gcov executable
    var $gcov;
    /**
     * Constructor.
     */
    function Test() {
	$this->LESSTHAN_FILE = "lessthan.txt";
        $this->tempfile = "";
	$this->tempfile2 = "";
	$this->limit = 90;
	$this->gcov = "";
	
	// Add excluded directories here.
	$this->excludeDirs = array("procgen/ProDe",
				   "applibs/wxToolkit",
				   "codesign/osal/OSEd",
				   "codesign/Proxim",
				   "applibs/wxToolkit/MachineWindow",
				   "applibs/wxToolkit/GUIOptions");
	// Add excluded files here.
	$this->excludeFiles = array("tools/Application.cc",
				    "tools/RelationalDBConnection.cc",
				    "tools/SimValue.cc",
				    "tools/RelationalDBQueryResult.cc",
				    "base/osal/OperationBehavior.cc");
    }    
    
    /**
     * Returns true if the file name given as first argument is in the list of
     * excluded files. File should contain relative path to tce/src.
     */
    function exclude($file) {
	// Check if the file is in one of the excluded directories.
	$dirOfFile=dirname($file);
	foreach($this->excludeDirs as $dir) {
	    if ("./".$dir == $dirOfFile) {
		return true;
	    }
	}
	foreach($this->excludeFiles as $fileX) {
	    if ("./".$fileX == $file) {
		return true;
	    }
	}
	return false; 
    }

    /**
     * Function to test if a file is excluded
     */
    function test_exclude($file) {
	if ($this->exclude($file)) {
	    echo "yes\n";
	} else {
	    echo "no\n";
	}
    }

    /**
     * Function that excludes all files that begin with 'Null' 
     */
    function nullExclude() {
	exec('find $(dirname $0) -path */Null*cc', $nullFiles);
	foreach($nullFiles as $nullFile) {
	    $nullFile = substr($nullFile, 2);
	    //echo $nullFile."\n";
	    array_push($this->excludeFiles, $nullFile);
	}
    }

    /**
     * Funtion that makes code, tests and runs gcov to all files (not excluded ones)
     */
    function run_gcov() {
	// makes all in tce/
	echo "## Building code base...\n";
	exec('make > /dev/null 2>&1');
	
	$tceRoot = getcwd();
	chdir('test');
	
	// makes all in tce/test
	echo "## Running unit tests...\n";
	exec('make > /dev/null 2>&1');
	
	chdir($tceRoot);
	chdir('systemtest');
	
	// runs the systems tests
	echo "## Running system test scripts...\n";
	exec('../tools/scripts/systemtest.php > /dev/null');
	
	chdir($tceRoot);
	
	// removes old file
	if (file_exists($this->LESSTHAN_FILE)) {
	    unlink($this->LESSTHAN_FILE);
	}
	chdir('src');
	
	exec('find -name *.cc', $files);

	echo "## Collecting data...\n";

	$this->nullExclude();
	
	foreach($files as $file) {
	    // test if the file is in excluded files or dirs list
	    if ($this->exclude($file)) {
		continue;
	    }
	    
	    $path = dirname($file);
	    $file = basename($file);
	    
	    $tceSrc = getcwd();
	    chdir($path);

	    // run gcov to all wanted files
	    exec($this->gcov.' '.$file.' 2> /dev/null |grep \
                 '.$file.'$ >> '.$this->tempfile2);
	    chdir($tceSrc);
	}
	chdir($tceRoot);

	// writes lines that have too low percentage value to the lessthan file
	$lines = file($this->tempfile2);
	$handle = fopen($this->LESSTHAN_FILE, 'a');
	foreach($lines as $line) {
	    $percent = substr($line, 0, 3);
	    if ($percent < $this->limit) {
		fwrite($handle, $line);
	    }
	}
	fclose($handle);


	// Count of files that are not tested at all (coverage 0.00%).
	$notTestedCount = 0;
	$lessThan = file($this->LESSTHAN_FILE);
	foreach($lessThan as $line) {
	    if (substr($line, 0, 4) == "0.00") {
		$notTestedCount += 1;
	    }
	}
	// Count of files that are tested but their coverage is too low.
	$lessThanCount = (count($lessThan) - $notTestedCount);

	// If we have files that have low coverage ratio, let's print it to screen
	// so compile test script can grab the printout.
	if ($notTestedCount > 0) {
	    echo $notTestedCount." files that are not unit tested at all found!\n\n";
	    passthru('cat '.$this->LESSTHAN_FILE.'| grep -x "0.00%.*"');
	}
	if ($lessThanCount > 0) {
	    echo "\n".$lessThanCount." files with less than ".$this->limit;
	    echo "% code line unit test coverage found!\n\n";
	    passthru('cat '.$this->LESSTHAN_FILE.' | grep -xv "0.00%.*" | sort -n');
	}
    }    
    
    /**
     * Prints the help.
     */
    function printUsage($argv) {
	echo "Usage: Run configure with '--with-gcov' option before using ";
	echo basename($argv[0])."!\n";
	echo "       Run '".basename($argv[0])."' in the tce root directory ";
	echo "to get the test coverage results\n       from unit tests.\n";
	echo "       Percentage values of files that has coverage values less ";
	echo "than ".$this->limit."% are\n       printed in file \"lessthan.txt\".";
	echo "\n";
    } 
}

$test = new Test;
$test->gcov = getenv('GCOV');

if ($test->gcov == "") {
    $test->gcov = shell_exec('which gcov');
    $test->gcov = substr($test->gcov, 0, strlen($test->gcov) - 1);
}

if ($test->gcov == "") {
    echo "gcov not found! Stopping.";
    return;
}

// tempfile for test output
$user = getenv("USER");
exec("mktemp /tmp/gcovtest.".$user.".XXXXXXX", $TEMPFILE, $var);
exec("mktemp /tmp/gcovtest.".$user.".XXXXXXX", $TEMPFILE2, $var);
$BASEDIR = getcwd();

$test->tempfile = $TEMPFILE[0];
$test->tempfile2 = $TEMPFILE2[0];

// get path to ..../tce/tools/scripts directory
chdir(dirname($argv[0]));
$path = getcwd();

// change back to old directory
chdir($BASEDIR);

if ($argc == 1) {
    $test->run_gcov();
} else if ($argc == 2) {
    if ($argv[1] == '--help') {
	$test->printUsage($argv);
    } else if ($argv[1] == '-h') {
	$test->printUsage($argv);
    } else {
	$test->test_exclude($argv[1]);
    }
} else {
    $test->printUsage($argv);
}

// removes the tempfile
system("rm $TEMPFILE[0]");
system("rm $TEMPFILE2[0]");
?>
