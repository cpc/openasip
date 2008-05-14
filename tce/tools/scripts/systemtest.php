#!/usr/bin/php -qC
<?php

error_reporting(E_COMPILE_ERROR | E_ERROR | E_CORE_ERROR);

/**
 * Class that represents one test session
 */
class Test {
    // revision of tce that is under testing
    var $tce_rev;
    // -b option
    var $search_breaking_rev;
    // outputs broken tests to a file
    var $output_broken_tests;
    // array to store failed tests last good revision
    var $test_ok_array;
    // variable that shows how many tests are OK
    var $passed;
    // -v option
    var $verbose;
    // -s option
    var $summary;
    // -p option
    var $print_output;
    // how many test are run
    var $testcount;
    // the used tempfile
    var $tempfile;
    // the used output file
    var $difference;
    // is the output file written
    var $output_written;
    // when intialization is needed
    var $initialize;
    // flag for incorrect amount of output.txt files found
    var $error_in_outputs;
    // path to the exec_max_time executable
    var $watchdog;
    // if only one test is wanted to execute with -t option
    var $test_only;
    // array to collect executed finalize paths
    var $fin_paths;
    // don't output OK tests when using -bo switch
    var $onlyBroken;
    
    /**
     * Constructor.
     */
    function Test() {
        $this->tce_rev = -1;
        $this->search_breaking_rev = false;
        $this->output_broken_tests = false;
        $this->test_ok_array = array();
        $this->onlyBroken = true;
        $this->verbose = false;
        $this->summary = false;
        $this->print_output = false;
        $this->testcount = 0;
        $this->passed = 0;
        $this->tempfile = "";
        $this->difference = "";
        $this->output_written = false;
        $this->initialize = true;
        $this->error_in_outputs = false;
        $this->watchdog = "";
        $this->test_only = "";
        $this->fin_paths = array();
    }
    
    
    /**
     * Returns input files that has output files from test directory
     */
    function getInputs($testdir) {
        $files = listdir($testdir);
        $inputs = Array();
        $output_counter = 0;
        $handle = opendir("$testdir/");
        while (false !== ($file = readdir($handle))) {
            if ("_output.txt" != substr($file, -11) &&
                ".txt" == substr($file, -4)) {
                // get the number of input file
                $number = explode("_", $file);
                // file is considered as input file if there was "_" found
                // and corresponding output exists
                if ($number[0] != Array() &&
                    file_exists("$testdir/".$number[0]."_output.txt")) {
                    $inputs[] = $file;
                }
            } else if ("_output.txt" == substr($file, -11)) {
                $output_counter = $output_counter + 1;
            }
        }
        closedir($handle);
        sort ($inputs, SORT_NUMERIC);

        if ($output_counter > 1 && $output_counter != count($inputs)) {
            $this->error_in_outputs = true;
        } else if ($output_counter == 1 && count($inputs) == 0) {
            $inputs[] = "";
        }       
        return $inputs;
    }
    
    
    /**
     * Executes one test that may include many inputs and outputs
     */
    function doOneTest($file) {
        include $file;
        $wdir = dirname($file);
        // testdir name is the file name after removing the .testdesc suffix
        $testpath = substr($file, 0, -9);
        $testdir = explode("/", $testpath);
        $size = count($testdir) - 1;
        $testdir = $testdir[$size];
        // do initialization to a specific test
        $initpath = $testpath."/foopath";
        $this->initialize($initpath);

        $inputs = $this->getInputs($testpath);

        if ($test_description == "") {
            if ($this->output_broken_tests) {
                debugPrintToLog("Invalid test description file: $file \n");
            } else {
                echo "Invalid test description file: $file \n";
            }
            return false;
        }

        // found erroneus amount of input or output files
        if ($this->error_in_outputs == true) {
            $this->error_in_outputs = false;
            echo "$test_description...error.\n";
            return false;
        }
        
        $test_OK = true;
        if ($this->verbose) {
            echo "Testing $test_description...";
        }
        debugPrintToLog("Testing $test_description");
        
        // executable is run with all different inputs
        for ($i = 0; $i < count($inputs); $i++) {
            $number = explode("_", $inputs[$i]);
            $output = $number[0]."_output.txt";

            // Command that is used to run the binary with arguments given in
            // .testdesc. Input is given to stdin and output is forwarded to
            // tempfile. diff is used to differ the excepted output and real
            // output.
            
            // exception: there is no input file, only output
            if ($inputs[$i] == "") {
                $number[0] = "1";
                $output = "1_output.txt";
                $command = 
                    "cd ".$wdir.";".$this->watchdog." '"
                    .$test_bin." ".$bin_args
                    ." > ".$this->tempfile." 2>&1';diff "
                    .$this->tempfile." ".$testdir."/".$output;
            } else {
                // normal case when inputs exists
                $command = 
                    "cd ".$wdir.";".$this->watchdog." '"
                    .$test_bin." ".$bin_args." < ".$testdir."/"
                    .$inputs[$i]." > ".$this->tempfile." 2>&1';diff -u "
                    .$this->tempfile." ".$testdir."/".$output;
            }

            // output of diff command is saved in $out array
            unset($out);
            // echo $command."\n";
            exec($command, $out, $return_val);
            //echo "output:".$out."\n";
            //echo "retval:".$return_val."\n";
            
            // if diff returns other than zero a difference is found
            if ($out) {
                if ($test_OK == false) {
                    $failed_tests = $failed_tests.",";
                }
                
                $test_OK = false;
                $length = strlen($number[0]);
                $failed_file = substr_replace($inputs[$i], "", 0, $length);
                $failed_file = str_replace("_", " ", $failed_file);
                $failed_file = substr_replace($failed_file, "", -4);
                $failed_tests = $failed_tests."".$failed_file;
                
                // prints output if -o option was used
                if ($this->output == true) {
                    $handle = fopen($this->difference, "a");
                    fwrite($handle, "test: ".$test_description.
                           "\ninput file: ".$inputs[$i]."\n");
                    for ($j = 0; $j < count($out); $j++) {
                        fwrite($handle, $out[$j]."\n");
                    }
                    fwrite($handle, "-----------------------------------\n");
                    fclose($handle);
                    $this->output_written = true;
                }
            }

            // if option -p is given, output of the test is printed out
            if ($this->print_output) {
                echo "Output after input: ".$inputs[$i]."\n";
                $command = "cat ".$this->tempfile;
                $output = shell_exec($command);
                echo "$output";
                echo "\n";
            }
        }        
	
        if ($test_OK == true) {
            if ($this->verbose) {
                //echo "Testing $test_description...OK.\n";
                echo "OK.\n";
            }
            debugPrintToLog(" [OK].\n");
            
        } else if (count($inputs) == 1) {
            //echo "Testing $test_description....failed.\n";
            if ($this->verbose) {
                echo " [FAILED].\n";
            } else {
                echo "Testing $test_description....failed.\n";
            }
            debugPrintToLog(" [FAiLED].\n");
        } else {
            //echo "Testing $test_description....failed:$failed_tests\n";
            if ($this->verbose) {
                echo "failed:$failed_tests\n";
            } else {
                echo "Testing $test_description....failed:$failed_tests\n";
            }
            debugPrintToLog(" [FAILED]:$failed_tests\n");
        }

        $this->finalize($initpath);

        return $test_OK;
    }
    
    
    /** 
     * Runs initialize script.
     */
    function initialize($testdesc) {
        $dir = dirname($testdesc);
        if (file_exists($dir."/initialize")) {
            $command = "cd ".$dir.";".$this->watchdog." './initialize'";
            system($command);
        }
    }
    
    
    /** 
     * Runs finalize script.
     */
    function finalize($testdesc) {
        $dir = dirname($testdesc);
        for ($i = 0; $i <= sizeof($this->fin_paths); $i++) {
            if ($dir == $this->fin_paths[$i]) {
                return;
            }
        }
        if (file_exists($dir."/finalize")) {
            $command = "cd ".$dir.";".$this->watchdog." './finalize'";
            system($command);
            $this->fin_paths[] = $dir;
        }
    }
    
    
    /**
     * Returns true if both files are in same directory
     */
    function isSameDir($first, $second) {
        if (is_array($second)) {
            return false;
        }
        $current = explode("/", $first);
        $next = explode("/", $second);
        $current_size = count($current);
        unset($current[$current_size - 1]);
        $next_size = count($next);
        unset($next[$next_size - 1]);        
        if ($current !== $next) {
            return false;
        }
        return true;
    }

    
    /**
     * Executes all test description files
     */
    function doTests($files) {
        sort($files, SORT_STRING);
        for ($i = 0; $i < count($files); $i++) {
            if (is_array($files[$i])) {
                $this->doTests($files[$i]); // recursive call
            } else {
                
                // in case that -t option is used, only given test is run
                if ($this->test_only) {
                    // only files ending .testdesc can be run
                    if (ereg ("\.testdesc$", $files[$i])) {
                        // checks for particular .testdesc-file
                        if (ereg ($this->test_only."$", $files[$i])) {
                            $this->testcount++;
                            $this->initialize($files[$i]);

                            // do one test at a time and check if passed or failed
                            if ($this->doOneTest($files[$i])) {
                                $this->passed++;
                                if (count($this->test_ok_array) > 0 
                                    && !$this->onlyBroken) {
                                    $this->test_ok_array[$files[$i]] = true;
                                } 
                            } else if ($this->search_breaking_rev || 
                                       $this->output_broken_tests) {
                                $this->test_ok_array[$files[$i]] = false;
                            }

                            $this->finalize($files[$i]);
                            break;
                        }
                    }
                    continue;
                }

                // no -t option, all files ending .testdesc are run
                else if (ereg ("\.testdesc$", $files[$i])) {
                    $this->testcount++;
                    if ($this->initialize) {
                        $this->initialize($files[$i]);
                        $this->initialize = false;
                    }
                    if (!file_exists($files[$i].".disabled")) {

                        // do one test at a time and check if passed or failed
                        if ($this->doOneTest($files[$i])) {
                            $this->passed++;
                            if (count($this->test_ok_array) > 0
                                && !$this->onlyBroken) {
                                $this->test_ok_array[$files[$i]] = true;
                            } 
                        } else if ($this->search_breaking_rev ||
                                   $this->output_broken_tests) {
                            $this->test_ok_array[$files[$i]] = false;
                        }
                    } else {
                        // in case the test is disabled, we count it as
                        // passed                        
                        $this->passed++;
                    }
                }
                // if directory changes then finalize is run
                if (!$this->isSameDir($files[$i], $files[$i + 1])) {
                    $this->finalize($files[$i]);
                    $this->initialize = true;
                }
            }
        }
    }
} // test class


/**
 * Generates an array that contains all files in current directory
 * and its subdirectories.
 */
function listdir($base) {
    if(is_dir($base)) {
        $dh = opendir($base);
        while (false !== ($dir = readdir($dh))) {
            if (is_dir($base ."/". $dir) && $dir !== '.' && 
                $dir !== '..') {
                $subbase = $base ."/". $dir;
                $subfilelist = listdir($subbase);
                $filelist[] = $subfilelist;
            } elseif(is_file($base ."/". $dir) && $dir !== '.' && 
                     $dir !== '..') {
                $filelist[] = $base ."/". $dir;
            }
        }
        closedir($dh);
    }
    return $filelist;
}


/**
 * Prints the help.
 */
function printUsage($difference, $argv) {
    $progname = basename($argv[0]);
    echo "Usage: $progname [OPTION]...\n";
    echo "\t-bo\tif some tests failed, output failed tests to a file.\n";
    echo "\t-b\tif some tests failed, searches last good revision for those tests.\n";
    echo "\t-v\tprints a message also on successful test completion.\n";
    echo "\t-s\tprints a summary of all the tests run.\n";
    echo "\t-o\toutputs the difference found between program output and\n";
    echo "\t\texpected output into file $difference.\n";
    echo "\t-p\tprints the test output on the screen.\n";
    echo "\t-t file\texecutes only given file (E.g. simulation.testdesc). \n";
} 

/**
 * return last ok revision number
 */
function getLastOkRev() {
    global $tce_last_ok_revision_filename; 
    $tceLORFileName = $tce_last_ok_revision_filename;
    $tceLastOkRev = file_get_contents($tceLORFileName);
    preg_match_all("/[0-9]+\n/", $tceLastOkRev, $matches);
    return $matches[0][0];
}

/**
 * returns Last Changed Revision
 */
function getLastChangedRev() {
    exec("svn info " . getenv('TCE_ROOT'), $svnOut, $svnReturnVal);
    // at leat 10 lines in svn info output
    if (count($svnOut) < 10) {
        print "Systemtest error: Couldn't get svn info from " 
            . getenv('TCE_ROOT') . "\n";
        exit;
    }
    preg_match("/[0-9]+$/", $svnOut[8], $matches);
    return $matches[0];
}

/**
 * returns current Revision of tce root
 */
function getCurrentRev() {
    exec("svn info " . getenv('TCE_ROOT'), $svnOut, $svnReturnVal);
    // at leat 10 lines in svn info output
    if (count($svnOut) < 10) {
        print "Systemtest error: Couldn't get svn info from " 
            . getenv('TCE_ROOT') . "\n";
        exit;
    }
    preg_match("/[0-9]+$/", $svnOut[4], $matches);
    return $matches[0];
}

/**
 * Changes tce repo to revision given as parameter and compiles it with make.
 */
function updateTceToRev($revision) {
    $cmd = "svn up -r $revision " . getenv('TCE_ROOT') . " 2>/dev/null";
    exec($cmd, $svnOut, $svnReturnVal);
    if (!$svnOut) {
        return false;
    }

    $cmd = "make -C " . getenv('TCE_ROOT') . " 1>/dev/null";
    exec($cmd, $makeOut, $makeReturnVal);
    if ($makeOut) {
        printToLog("Error in systemtest: errors when compiling tce revision: $revision.\n");
        printToLog("$makeOut\n");
        return false;
    }

    return true;
}

/**
 * Checks if there is change in revision, returns true if so.
 */
function checkIfChangesInRev($revision) {
    $cmd = "svn info -r $revision " . getenv('TCE_ROOT') . " 2>/dev/null";
    exec($cmd, $svnOut, $svnReturnVal);
    // at leat 10 lines in svn info output
    if (count($svnOut) < 9) {
        print "Systemtest error: Couldn't get svn info from " 
            . getenv('TCE_ROOT') . "\n";
        exit;
    }
    preg_match("/[0-9]+$/", $svnOut[4], $matchesR);
    preg_match("/[0-9]+$/", $svnOut[7], $matchesLCR);
    if ($matchesR[0] == $matchesLCR) {
        return true;
    } else {
        return false;
    }
}

function writeFailedTestsToFile($filename, $tests) {
    // open file
    if (is_writable($filename)) {
        if (!$handle = fopen($filename, 'w')) {
            echo "Cannot open file ($filename) to write.";
            return false;
        }
    } else {
        if (!$handle = fopen($filename, 'w')) {
            echo "Cannot open file ($filename) to write.";
            return false;
        }
    }

    $content = "";
    foreach ($tests as $i => $value) {
        if (!$value) {
            $content = $content . "$i\n";
        } else {
            $content = $content . "$i " . getCurrentRev() . "\n";
        }
    }
    
    // Write $content opened file
    if (fwrite($handle, $content) === FALSE) {
        echo "Cannot write to file ($filename)";
        return false;
    }

    fclose($handle);
    
    return true;

}

/**
 * writes bisect systemtests results to a given file.
 * creates the file if it doesn't exist, otherwise appends.
 */
function writeResultsToFile($filename, $results) {

    // open file
    if (is_writable($filename)) {
        if (!$handle = fopen($filename, 'a')) {
            echo "Cannot open file ($filename) to append.";
            return false;
        }
    } else {
        if (!$handle = fopen($filename, 'w')) {
            echo "Cannot open file ($filename) to write.";
            return false;
        }
    }

    $last_ok_rev = 0; 
    foreach ($results as $i => $value) {
        if ($last_ok_rev < $value) {
            $last_ok_rev = $value;
        }
        $content = $content . "$i Last ok/breaking revision: $value";
        while (checkIfChangesInRev(--$value)) {}
        $content = $content . "/$value\n";
    }
    $content = "Last ok revision $last_ok_rev\n" . $content;
 
    // Write $content opened file
    if (fwrite($handle, $content) === FALSE) {
        echo "Cannot write to file ($filename)";
        return false;
    }

    fclose($handle);
    
    return true;
}

/**
 * Prints to log file
 */
function printToLog($print, $usePrint_r) {
    global $systemtest_log;
    $filename = $systemtest_log;
    
    // open file
    if (is_writable($filename)) {
        if (!$handle = fopen($filename, 'a')) {
            echo "Cannot open file ($filename) to append.";
            return;
        }
    } else {
        if (!$handle = fopen($filename, 'w')) {
            echo "Cannot open file ($filename) to write.";
            return;
        }
    }
    
    if ($usePrint_r) {
        $print = print_r($print, true);
    }
    
    // Write to opened file
    if (fwrite($handle, $print) === FALSE) {
        echo "Cannot write to file ($filename)";
        return;
    }

    fclose($handle);
}

/**
 *  Prints to log file if $DEBUG is true
 */
function debugPrintToLog($print, $usePrint_r) {
    global $DEBUG, $systemtest_log, $systemtest_debug_log;
    if ($DEBUG) {
        $systemtest_log = $systemtest_debug_log;
        printToLog($print, $usePrint_r);
    }
}

/**
 * Gets last working revisions for failed test cases.
 * Expects TCE_ROOT and HOME env variables as set
 */
function bisectRevision($test) {
    global $systemtest_bisect_result_filename;

    $rev_interval_left = getLastOkRev() + 1; // all ok +1
    $rev_interval_right = getLastChangedRev() - 1; // all fail -1

    // sanity check
    if (($rev_interval_left - 1) == ($rev_interval_right + 1)) {
        exit(1);
    }

    // if only one possible revision that breaks the tests
    if (($rev_interval_left - 1) == $rev_interval_right) {
        return;
    }

    // tested revisions systemtests status, true (ok), false (fail)
    $tested_revisions = array();
    // to filter tests which latest ok rev is allready found
    $rev_found_filter = array(); 
    
    // initialize known (border) revisions
    $tested_revisions[$rev_interval_right+1] = $test->test_ok_array;
    $tested_revisions[$rev_interval_left-1] = $test->test_ok_array;
    foreach ($tested_revisions[$rev_interval_left-1] as $i => $value) {
        $tested_revisions[$rev_interval_left-1][$i] = true;
    }

    // $files (broken tests) to be tested initialization
    unset($files);
    foreach ($test->test_ok_array as $i => $value) {
        $files[] = $value;
    }

    printToLog(date("r") . "\n");

    // let's start from right
    $test->tce_rev = $rev_interval_right;
    $tests_run_count = 0; // for debug statistics
    $loop_count = 0; // for debug statistics
    $results_filename = $systemtest_bisect_result_filename;
    while (count($files) > 0) {
        
        // get next revision which is to be tested
        while (!checkIfChangesInRev($test->tce_rev) && $test->tce_rev-- >= $rev_interval_left) {}
        
        // sanity check
        if ($test->tce_rev < $rev_interval_left) {
            print "Error in systemtest: last ok revision was not ok or didn't contain any changes.";
            exit(1);
        }

        // updates revision of tce
        if (!updateTceToRev($test->tce_rev)) {
            printToLog("Systemtest error: svn update to revision $test->tce_rev failed\n");
            exit(1);
        }
        
        if (!$tested_revisions[$test->tce_rev]) {
            $test->doTests($files);

            $tested_revisions[$test->tce_rev] = $test->test_ok_array;
            $tests_run_count++; // for debug statistics
        } else {
            printToLog("\nSystemtest error: Revision was allready tested 
                         (Someone has written shitty code)!\n");
        }

        unset($files);
        foreach ($test->test_ok_array as $i => $value) {
            // the breaking revision is the last revision where changes were 
            // made
            if ($tested_revisions[$test->tce_rev][$i]) {
                $rev_found_filter[$i] = $test->tce_rev;
                unset($test->test_ok_array[$i]);
            } else {
                // add to $files for new test run
                $files[] = $i;
            }
        }

        $loop_count++; // for debug statistics
    }

    printToLog($rev_found_filter, true);
    writeResultsToFile($results_filename, $rev_found_filter);
    return;
    
    // now test_last_good_rev should contain failed tests and good revs for 
    // them, write this info to a file, that is to be read by compiletest.sh 
    // and mailed 

    // update tce to most reacent revision
    if (!updateTceToRev("HEAD")) {
        printToLog("Systemtest error: svn update to revision HEAD failed\n");
        exit(1);
    }

}

function createTestFilesArrayFromFile($filename) {
    // open file
    if (is_readable($filename)) {
        if (!$handle = fopen($filename, 'r')) {
            echo "Cannot open file ($filename) to read.";
            return false;
        }
    } else {
        //echo "File ($filename) was not readable.";
        return false;
    }

    $files = array();
    $temp = "";
    if ($handle) {
        while (!feof($handle)) {
            $temp = stream_get_line($handle, 4096, "\n");
            if (!ereg (" [0-9]+$", $temp)) {
                $files[] = $temp;
            }
        }

        fclose($handle);
    }

    if (count($files) == 0) {
        return true;
    }

    return $files;
}


//
// MAIN PART
//

// tempfile for test output
$user = getenv("USER");
exec("mktemp /tmp/systemtest.".$user.".XXXXXXX", $TEMPFILE, $var);

$dir = ".";
$test = new Test;

// output file name
$DIFFERENCE = "difference.txt";

// variables used as globals
// last ok tce revision info file (in svn Last Changed format)
$tce_last_ok_revision_filename = getenv('HOME') . "/tce_last_ok_revision";
$systemtest_bisect_result_filename = getenv('HOME') . "/systemtest_bisect_results";
$systemtest_log = getenv('HOME') . "/systemtest.log";
$systemtest_debug_log = getenv('HOME') . "/systemtest_debug.log";
$DEBUG = false;

// file name where to store failed tests (used with -bo)
$broken_systests_filename = "broken_system_tests.temp";

$test->difference = $DIFFERENCE;
$test->tempfile = $TEMPFILE[0];

// save current directory
$cur_dir = getcwd();

// get path to ..../tce/tools/scripts directory
chdir(dirname($argv[0]));
$path = getcwd();
$test->watchdog = $path."/exec_max_time 300000";

// change back to old directory
chdir($cur_dir);

// Check command line parameters.

// options that the script recognizes
$options = array(1 => "-v", 2=> "-s", 3 => "-o", 4 => "-p", 5 => "-t",
                 6 => "--help", 7 => "-h", 8 => "-b", 9 => "-bo");
if ($argc != 1) {
    for ($j = 1; $j < $argc; $j++) {
        if ($skip) {
            $test->test_only = $argv[$j];
            $skip = false;
        } else if (!in_array($argv[$j], $options)) {
            echo "Unknown option: $argv[$j]\n";
            printUsage($DIFFERENCE, $argv);
            return;
        } else {
            switch ($argv[$j]) {
            case "-b":
                $test->search_breaking_rev = true;
                break;
            case "-bo":
                $test->output_broken_tests = true;
                break;
            case "-v":
                $test->verbose = true;
                break;
            case "-s":
                $test->summary = true;
                break;
            case "-o":
                $test->output = true;
                // removes old difference.txt
                if (file_exists($DIFFERENCE)) {
                    system("rm $DIFFERENCE");
                }
                break;
            case "-p":
                $test->print_output = true;
                break;
            case "-t":
                $skip = true;
                break;
            case "--help":
                printUsage($DIFFERENCE, $argv);
                return;
            case "-h":
                printUsage($DIFFERENCE, $argv);
                return;
            }
        }
    }
}

debugPrintToLog("[=== systemtest starting ===]\n");
    
if ($test->output_broken_tests) {
    $files = createTestFilesArrayFromFile($broken_systests_filename);
    if (!$files) {
        $files = listdir($dir);
        debugPrintToLog("Testing ALL tests.\n");
    } else {
        foreach ($files as $i) {
            $test->test_ok_array[$i] = false;
            $test->onlyBroken = false;
        }
    debugPrintToLog("Tests to be tested:\n");
    debugPrintToLog($files, true);
    }
} else {
    debugPrintToLog("Testing ALL tests (no -bo).\n");
    $files = listdir($dir);
}

$test->doTests($files);

if ($test->output_broken_tests && count($test->test_ok_array) != 0) {
    writeFailedTestsToFile($broken_systests_filename, $test->test_ok_array);
}

// get working revs for failed tests on HEAD rev
if ($test->search_breaking_rev && count($test->test_ok_array) != 0) {
    bisectRevision($test);    
}

// summary printing
if ($test->summary == true) {
    $percentage = round(($test->passed/$test->testcount)*100,2);
    echo "\nResult: $test->passed/$test->testcount ($percentage%) ";
    echo "of tests OK.\n";
}

if ($test->output_written == true) {
    echo "\nDifferences found in inputs and outputs are stored into ";
    echo "$DIFFERENCE\n";
}

// removes the tempfile
system("rm $TEMPFILE[0]");

?>
