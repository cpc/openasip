#!/usr/bin/php -qC
<?php

error_reporting(E_COMPILE_ERROR | E_ERROR | E_CORE_ERROR);

/**
 * Class that represents one code file.
 */
class Code {
    // the 'real' name of the file.
    var $file_name;
    // name of the code file (given with @file tag).
    var $name;
    // the total number of lines in file
    var $total_lines;
    // the number of code lines.
    var $codelines;
    // the number of comment lines.
    var $commentlines;
    // the number of empty lines.
    var $emptylines;
    // the name of the author.
    var $author;
    // flag that is set when we are reading comment block (/**...*/)
    var $isComment;
    // The year of the code file.
    var $year;
    // the latest review date.
    var $reviewed;
    // reviewers
    var $reviewers;
    // array for function information
    var $functions;
    // keeps count of non-matched left braces
    var $left_braces;
    // the phase of the code (red, yellow, green)
    var $code_color;
    
    /**
     * Constructor.
     */
    function Code() {
        $this->file_name = "";
        $this->name = "";
        $this->total_lines = 0;
        $this->codelines = 0;
        $this->commentlines = 0;
        $this->author = "";
        $this->isComment = false;
        $this->year = 0;
        $this->reviewed = 0;
        $this->reviewers = "";
        $this->functions = array();
        $this->left_braces = 0;
        $this->code_color = "";
    }
    
    /**
     * Returns true if line is an empty line.
     */
    function emptyLine($line) {
        if (strlen($line) == 0) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Returns true if line is a comment line.
     */
    function commentLine($line) {

        if (!$this->isComment) {
            if (substr($line, 0, 3) == "/**" || substr($line, 0, 2) == "/*") {
                $this->isComment = true;
                // now we must check if this is one-line C comment (ends */).
                rtrim($line);
                $line_length = strlen($line);
                if (substr($line, $line_length - 3, 2) == "*/") {
                    $this->isComment = false;
                }
                return true;
            }
        }

        // we have read '/**' earlier
        if ($this->isComment) {
            if (substr($line, 0, 2) == "*/") {
                $this->isComment = false;
            }
            return true;
        } else if (substr($line, 0, 2) == "//") {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Stores the name of the author if it is found.
     */
    function authorLine($line) {
        
        $tok = strtok($line, " ");
        if ($tok == "*") {
            $tok = strtok(" ");
            if ($tok == "@author") {
                $first = strtok(" ");
                $second = strtok(" ");
                $this->author = "$first $second";
                $this->year = strtok(" ");
            } 
        }
    }

    /**
     * Stores the name of the file is it is found.
     */
    function nameLine($line) {
        $tok = strtok($line, " ");
        if ($tok == "*") {
            $tok = strtok(" ");
            if ($tok == "@file") {
                $this->name = rtrim(strtok(" "));
            } 
        }
    }

    /**
     * Checks whether the line is a review line.
     */
    function reviewLine($line) {
        $tok = strtok($line, " ");
        if ($tok == "*") {
            $tok = strtok(" ");
            if ($tok == "@note") {
                $tok = strtok(" ");
                if ($tok == "reviewed") {
                    $day = strtok(" ");
                    $month = strtok(" ");
                    $year = strtok(" ");
                    $this->reviewed = strtotime("$day $month $year");
                    // 'by' is read off
                    strtok(" ");
                    // the reviewers
                    $this->reviewers = strtok("\n");
                }
            } 
        }
    }

    /**
     * Checks whether the line is a rating line.
     */
    function ratingLine($line) {
        $tok = strtok($line, " ");
        if ($tok == "*") {
            $tok = strtok(" ");
            if ($tok == "@note") {
                $tok = strtok(" ");
                if ($tok == "rating:") {
                    $tok = strtok(" \n");
                    $this->code_color = $tok;
                }
            }
        }
    }

    /**
     *
     */
    function functionCheck($line, $line_number) {
        if ($this->left_braces == 0) {
            if (preg_match("/.*\) {\$/", $line) ||
                preg_match("/.*\) const {\$/", $line)) {
                $func = new FunctionInfo();
                $func->start_line = $line_number;
                array_push($this->functions, $func);
                $this->left_braces = $this->left_braces + 1;
            }
        } else {
            if (preg_match("/^}/", $line)) {
                $this->left_braces = $this->left_braces - 1;
                if ($this->left_braces == 0) {
                    $funcs = count($this->functions);
                    $this->functions[$funcs - 1]->lines = 
                        $line_number - $this->functions[$funcs - 1]->start_line;
                }
            } 
            if (preg_match("/{\$/", $line)) {
                $this->left_braces = $this->left_braces + 1;
            }
        }
    }

    /**
     * Parses one line.
     */
    function parseLine($data, $line_number) {
        $line = ltrim($data);
        if ($this->commentLine($line)) {
            $this->commentlines++;
        } else if ($this->emptyLine($line)) {
            $this->emptylines++; 
        } else {
            $this->codelines++;
        }
        
        $this->functionCheck($line, $line_number);
        $this->authorLine($line);
        $this->nameLine($line);
        $this->reviewLine($line);
        $this->ratingLine($line);
    }

    /**
     * Total lines are calculated when the file is read.
     */
    function calculateTotalLines() {
        $this->total_lines = $this->commentlines + $this->codelines + 
            $this->emptylines;
    }

    /**
     * Dumps the contents of Code object.
     */
    function dump() {
        echo "name: $this->file_name\n"; 
        echo "code: $this->codelines lines\n"; 
	echo "comments: $this->commentlines lines\n";
        echo "empty: $this->emptylines\n";
        echo "author: $this->author\n";
        echo "year: $this->year\n";
        echo "reviewed: ", date(r, $this->reviewed), "\n";
        echo "reviewers: $this->reviewers\n";
        echo "\n";
    }
}

class FunctionInfo {
    // Starting line of the function. 
    var $start_line;
    // The length of function.
    var $lines;

    function FunctionInfo() {
        $this->start_line = 0;
        $this->lines = 0;
    }
}

/**
 * Stores and processes all code file information.
 */
class CodeStorage {

    // Array containing all code file information.
    var $codes;
    // Is set to true if warnings are printed.
    var $warnings;
    // Is set to true if style matters are checked.
    var $style_check;
    
    /**
     * Constructor.
     */
    function CodeStorage() {
        $this->codes = array();
        $this->warnings = false;
    }

    /**
     * Prints the information about code files.
     */
    function lineCount() {
        $how_many = count($this->codes);
        $code_lines = 0;
        $comment_lines = 0;
        $empty_lines = 0;
        for ($i = 0; $i < $how_many; $i++) {
            $code_lines += $this->codes[$i]->codelines;
            $comment_lines += $this->codes[$i]->commentlines;
            $empty_lines += $this->codes[$i]->emptylines;
        }
       
        $total = $code_lines + $comment_lines + $empty_lines;
        $codepc = 0;
        $commentpc = 0;
        $emptypc = 0;
        if ($total != 0) {
            $codepc = round($code_lines / $total * 100, 1);
            $commentpc = round($comment_lines / $total * 100, 1);
            $emptypc = round($empty_lines / $total * 100, 1);
        }
       
        echo "Code lines:\t $code_lines\t ($codepc%)\n";
        echo "Comment lines:\t $comment_lines\t ($commentpc%)\n";
        echo "Empty lines:\t $empty_lines\t ($emptypc%)\n";
        echo "------------------------------\n";
        echo "Total:\t $total\n";
    }

    /**
     * Prints user statistics (developer statistics).
     */
    function userStats() {
        $how_many = count($this->codes);
        $total = 0;
        $files = array();
        $auth_lines = array();
        for ($i = 0; $i < $how_many; $i++) {
            $total += $this->codes[$i]->total_lines;
            $auth = $this->codes[$i]->author;
            if ($auth == "") {
                $auth = "unknown";
            }
            if (array_key_exists($auth, $files)) {
                $files[$auth]++;
            } else {
                $files[$auth] = 1;
            }
            $auth_lines[$auth] += $this->codes[$i]->total_lines;
        }

        arsort($auth_lines);

        $file_number = count($auth_lines);
        $persons = array_keys($auth_lines);
        $person_number = count($persons);
        $longest = 0;
        
        for ($k = 0; $k < $person_number; $k++) {
            $person = $persons[$k];
            if (strlen($person) > $longest) {
                $longest = strlen($person);
            }
        }

        for ($j = 0; $j < $file_number; $j++) {
            $person = $persons[$j];
            if ($j == 0) {
                echo termcolored("                     /\  /\  /\ \n", "YELLOW");
                echo termcolored("                    /  \/  \/  \ \n", "YELLOW");
                echo termcolored("                   | __------__ | \n", "YELLOW");
				echo termcolored("                   /~          ~\ \n", "LIGHT_GREEN");
                echo termcolored("                  |    //^\\\//^\|\n", "LIGHT_GREEN");   
                echo termcolored("                /~~\  ||  ", "LIGHT_GREEN");
				echo termcolored("o", "LIGHT_RED");
				echo termcolored("| |", "LIGHT_GREEN");
				echo termcolored("o", "LIGHT_RED");
				echo termcolored("|:~\ \n", "LIGHT_GREEN");
                echo termcolored("               | |6   ||___|_|_||:|\n", "LIGHT_GREEN");
                echo termcolored("                \__.  /      o  \/'\n", "LIGHT_GREEN");
                echo termcolored("                 |   (       O   )\n", "LIGHT_GREEN");
                echo termcolored("        /~~~~\    `\  \         /\n", "LIGHT_GREEN");
                echo termcolored("       | |~~\ |     )  ~------~`\ \n", "LIGHT_GREEN");
                echo termcolored("      /' |  | |   /     ____ /~~~)\ \n", "LIGHT_GREEN");
                echo termcolored("     (_/'   | | |     /'    |    ( |\n", "LIGHT_GREEN");
                echo termcolored("            | | |     \    /   __)/ \ \n", "LIGHT_GREEN");
                echo termcolored("            \  \ \      \/    /' \   `\ \n", "LIGHT_GREEN");
                echo termcolored("              \  \|\        /   | |\___|\n", "LIGHT_GREEN");
                echo termcolored("                \ |  \____/     | |\n", "LIGHT_GREEN");
                echo termcolored("                /^~>  \        _/ <\n", "LIGHT_GREEN");
                echo termcolored("               |  |         \       \ \n", "LIGHT_GREEN");
                echo termcolored("               |  | \        \       \ \n", "LIGHT_GREEN");
                echo termcolored("               -^-\  \       |        )\n", "LIGHT_GREEN");
                echo termcolored("                   `\_______/^\______/\n", "LIGHT_GREEN");
                echo "-------------------------------\n";
                echo "*KING OF THE CODE MONKIES:*\n";
            }
            
            echo "$person:";
            $spaces = $longest - strlen($person);
            for ($z = 0; $z < $spaces; $z++) {
                echo " ";
            }
            echo "\t$files[$person] files and ";
            echo "$auth_lines[$person] lines\n";
            if ($j == 0) {
                echo "\n";
            }
        }
        echo "-------------------------------\n";
        echo "total:\t $how_many files and $total lines\n"; 
    }

    /**
     * Print warnings.
     */
    function printWarnings($code) {
        if ($code->author == "") {
            echo "Error: Missing @author tag in $code->file_name\n";
        }
        if ($code->name == "") {
            echo "Error: Missing @file tag in $code->file_name\n";
        } else {
            $real_name = substr(strrchr($code->file_name, "/"), 1);
            if ($code->name != $real_name) {
                echo "Error: Name in @file tag not the real name of file: ",
                    "$code->file_name\n";
            }
        }
        if ($code->reviewed < 0) {
            echo "Error: review date in wrong format $code->file_name\n";
        }
        if ($code->code_color != "red" && $code->code_color != "yellow" &&
            $code->code_color != "green" && $code->code_color != "") {
            echo "Error: code color is illegal in file $code->file_name\n";
        }
        if ($code->code_color == "") {
            echo "Error: missing rating tag in file $code->file_name\n";
        }
        if (!is_numeric($code->year)) {
            echo "Error: missing year in the author tag in file ",
                "$code->file_name\n";
        }
    }

    /**
     * Checks that the style of the line is ok.
     */
    function checkStyle($code, $line, $line_number) {
        $length = strlen($line);
        if ($length > 80) {
            echo "Warning: too long line ($length) in file ",
                "$code->file_name in line $line_number\n";
        }
        if (strpos($line, "\t") !== false) {
            echo "Warning: file $code->file_name has a tabulator in line ",
                "$line_number\n";
        }
    }

    function checkFunctions($code) {
        $how_many = count($code->functions);
        for ($i = 0; $i < $how_many; $i++) {
            if (substr($code->file_name, $len-3, 3) != ".hh") {
                $length = $code->functions[$i]->lines;
                if ($length > 60) {
                    $start = $code->functions[$i]->start_line;
                    echo "Warning: too long function ($length lines) in file ",
                        "$code->file_name in line $start\n";
                }
            }
        }
    }

    /**
     * Analyzes and stores the information of code files,
     * that are given in a possibly multi-dimensinal array.
     */
    function processCode($codearray) {

        $numberOf = count($codearray);
        for ($i = 0; $i < $numberOf; $i++) {
            if (is_array($codearray[$i])) {
                $this->processCode($codearray[$i]);
            } else {
                
                if (isCodeFile($codearray[$i])) {
                                       
                    $fp = fopen($codearray[$i], "r");
                    $file = new Code();
                    $file->file_name = $codearray[$i];
                    $line_nmbr = 1;
                    while ($data = fgets($fp, 4096)) {
                        if ($this->style_check) {
                            $this->checkStyle($file, $data, $line_nmbr);
                        }
                        $file->parseLine($data, $line_nmbr);
                        $line_nmbr = $line_nmbr + 1;
                    }
                    fclose($fp);
                    if ($this->warnings) {
                        $this->printWarnings($file);
                    }
                    if ($this->style_check) {
                        $this->checkFunctions($file);
                    }
                    $file->calculateTotalLines();
                    array_push($this->codes, $file);
                }   
            }
        }
    }

    /**
     * Prints the code files in the format needed for review document.
     */
    function printReview() {
        $numberOf = count($this->codes);
        for ($i = 0; $i < $numberOf; $i++) {
            $name = $this->codes[$i]->name;
            $lines = $this->codes[$i]->total_lines;
            $author = $this->codes[$i]->author;
            $reviewed = $this->codes[$i]->reviewed;
            if ($name == "") {
                $name = $this->codes[$i]->file_name;
            }
            if ($author == "") {
                $author = "unknown";
            }
            if ($reviewed == "") {
                $reviewed = "-";
            } else {
                $reviewed = date("d.m.y", $reviewed);
            }
            echo "$name,$lines,$author,$reviewed\n";
        }
    }

}

/**
 * Generates an array that contains all files in current directory
 * and its subdirectories.
 */
function recursive_listdir($base) {
    if(is_dir($base)) {
        $dh = opendir($base);
        while (false !== ($dir = readdir($dh))) {
            if (is_dir($base ."/". $dir) && $dir !== '.' && 
                $dir !== '..') {
                $subbase = $base ."/". $dir;
                $subfilelist = recursive_listdir($subbase);
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
 * Compares empty line percentages of two code lines.
 */
function emptyCompare($code1, $code2) {

    $code1pc = $code1->emptylines/$code1->total_lines;
    $code2pc = $code2->emptylines/$code2->total_lines;
    
    if ($code1pc == $code2pc) {
        return 0;
    }

    return ($code1pc < $code2pc) ? -1 : 1;
}

/**
 * Compares comment line percentages of two code lines.
 */
function commentCompare($code1, $code2) {

    $code1pc = $code1->commentlines/$code1->total_lines;
    $code2pc = $code2->commentlines/$code2->total_lines;
    
    if ($code1pc == $code2pc) {
        return 0;
    }

    return ($code1pc < $code2pc) ? -1 : 1;
}

/**
 * Compares line numbers of two code lines.
 */
function lineCompare($code1, $code2) {
    
    if ($code1->total_lines == $code2->total_lines) {
        return 0;
    }
    return $code1->total_lines < $code2->total_lines ? 1 : -1;
}

/**
 * Prints ten files having most empty lines.
 */
function topTenEmpty($codes) {
    usort($codes, "emptyCompare");
    echo "-----------------------------\n";
    echo "Least empty lines:\n";
    reset($codes);
    $file = current($codes);
    for ($i = 1; $i < 11; $i++) {
        $emptypc = round(($file->emptylines/$file->total_lines)*100, 1);
        echo "$i. $file->file_name (empty lines $emptypc%)\n";
        $file = next($codes);
    }
    echo "-----------------------------\n";
}

/**
 * Prints ten files having most lines.
 */
function topTenLines($codes) {
    usort($codes, "lineCompare");
    echo "-----------------------------\n";
    echo "Top ten code lines:\n";
    reset($codes);
    $file = current($codes);
    for ($i = 1; $i < 11; $i++) {
        $lines = $file->total_lines;
        echo "$i. $file->file_name ($lines code lines)\n";
        $file = next($codes);
    }
    echo "-----------------------------\n";
}

/**
 * Prints ten files having most comment lines.
 */
function topTenComments($codes) {
    usort($codes, "commentCompare");
    echo "-----------------------------\n";
    echo "Least comment lines:\n";
    reset($codes);
    $file = current($codes);
    for ($i = 1; $i < 11; $i++) {
        $commentpc = round(($file->commentlines/$file->total_lines) * 100, 1);
        echo "$i. $file->file_name (comment lines $commentpc%)\n";
        $file = next($codes);
    }
    echo "-----------------------------\n";
}

/**
 * Count the number of lines in file.
 */
function countLines($file) {
    $handle = fopen($file, "r");
    $lines = 0;
    if ($handle) {
        while (!feof($handle)) {
            $buffer = fgets($handle, 4096);
            $lines++;
        }
        fclose($handle);
    } else {
        echo "cannot open file $file\n";
    }
    return $lines;
}

/**
 * Calculates differences between two files using diff.
 */
function calculateDiff($file1, $file2) {
    $dumpfile = tempnam("/tmp", "DAA");
    $command = sprintf("diff -w %s %s > %s", $file1, $file2, $dumpfile);
    system($command);
    $handle = fopen($dumpfile, "r");
    $diff = 0;
    if ($handle) {
        while (!feof($handle)) {
            $buffer = fgets($handle, 4096);
            if (is_numeric(substr($buffer, 0, 1))) {
                
                $first = 0;
                $second = 0;
                $left = 1;
                $right = 0;

                // the format of the lines we are interested is of this kind:
                // <num>[,<num>](a|c|d)<num>[,<num>]
                // A number followed by possibly another number separated
                // with comma. Then a, c, or d. The again number followed
                // by possibly another number separated by comma.
                $pos = strpos($buffer, ",");
                $tok = strtok($buffer, ",");
                if ($pos != false) {
                    $tok = strtok($buffer, ",");
                    if (is_numeric($tok)) {
                        // tok was a number
                        $first = $tok;
                        if ($tok = strtok("c")) {
                            $second = $tok;
                        } else if ($tok = strtok("d")) {
                            $second = $tok;
                        } else if ($tok = strtok("a")) {
                            $second = $tok;
                        } else {
                            echo "Problems with parsing...\n";
                        }
                        
                        $left = $second - $first + 1;
                        
                        if ($tok = strtok(",")) {
                            $first = $tok;
                            $second = strtok("\n");
                            $right = $second - $first + 1;
                        } else {
                            $right = 1;
                        }
                    } else {
                        $left = 1;
                        // tok was something like 13d14
                        $pos = 0;
                        while (substr($tok, $pos, 1) != "a" &&
                               substr($tok, $pos, 1) != "c" &&
                               substr($tok, $pos, 1) != "d") {
                            $pos++;
                        }
                        $first = substr($tok, $pos + 1, strlen($tok));
                        $second = strtok("\n");
                        $right = $second - $first + 1;
                    }
                }
             
                $diff += $left > $right ? $left : $right;
            }
        }
    } else {
        echo "Cannot open $dumpfile\n";
    }
    return $diff;
}

/**
 * Prints the changes occurred to file after latest review.
 */
function printChanges($codes) {
    $how_many = count($codes);
    // first header is printed
    echo "File,Author,Lines,Lines changed since last review,"; 
    echo "Last review date,Reviewer 1,Reviewer 2,Reviewer 3,Reviewer 4\n";
    for ($i = 0; $i < $how_many; $i++) {
        $file = $codes[$i]->file_name;
        $auth = $codes[$i]->author;
        $lines = $codes[$i]->total_lines;
        if ($codes[$i]->reviewed <= 0) {
            echo "$file,$auth,$lines,-,-,-,-,-,-\n";
        } else {
            $tmpfname = tempnam("/tmp", "FOO");
            $review = $codes[$i]->reviewed - 86400;
            $date = date("m/d/y", $review);
            $command = sprintf("cvs -Q up -p -D %s %s > %s", $date, $file,
                               $tmpfname);
            system($command);
            
            $line_change = calculateDiff($file, $tmpfname);
            $rev = date("d.m.y", $codes[$i]->reviewed);
            $revs = $codes[$i]->reviewers;
            echo "$file,$auth,$lines,$line_change,$rev,$revs\n";
           
            unlink($tmpfname);
        }
    }
}

/**
 * Prints info about files that are not reviewed.
 */
function printVirgins($codes) {
    echo "File,Author,Lines\n";
    $how_many = count($codes);
    for ($i = 0; $i < $how_many; $i++) {
        $file = $codes[$i]->file_name;
        $auth = $codes[$i]->author;
        if ($auth == "") {
            $auth = "unknown";
        }
        $lines = $codes[$i]->total_lines;
        if ($codes[$i]->reviewed <= 0) {
            echo "$file,$auth,$lines\n";
        }
    }
}

/**
 * Prints code colors.
 */
function printColors($codes) {
    $reds = array();
    $yellows = array();
    $greens = array();
    $how_many = count($codes);
    for ($i = 0; $i < $how_many; $i++) {
        $color = $codes[$i]->code_color;
        if ($color == "red" || $color == "") {
            array_push($reds, $codes[$i]);
        } 
        if ($color == "yellow") {
            array_push($yellows, $codes[$i]);
        } 
        if ($color == "green") {
            array_push($greens, $codes[$i]);
        }
    }

    echo "********************\n";
    echo "Codes labeled as red\n";
    echo "********************\n";
    for ($j = 0; $j < count($reds); $j++) {
        if ($reds[$j]->name != "") {
            $name = $reds[$j]->file_name;
            echo "$name\n";
        }
    }

    echo "\n";
    echo "***********************\n";
    echo "Codes labeled as yellow\n";
    echo "***********************\n";
    for ($k = 0; $k < count($yellows); $k++) {
        if ($yellows[$k]->name != "") {
            $name = $yellows[$k]->file_name;
            echo "$name\n";
        }
    }
    
    echo "\n";
    echo "**********************\n";
    echo "Codes labeled as green\n";
    echo "**********************\n";
    for ($l = 0; $l < count($greens); $l++) {
        if ($greens[$l]->name != "") {
            $name = $greens[$l]->file_name;
            echo "$name\n";
        }
    }
    echo "\n";    

    $red_nmbr = count($reds);
    $yellow_nmbr = count($yellows);
    $green_nmbr = count($greens);

    $red_pc = round($red_nmbr/$how_many * 100, 1);
    $yellow_pc = round($yellow_nmbr/$how_many * 100, 1);
    $green_pc = round($green_nmbr/$how_many * 100, 1);
    echo "red code:\t $red_nmbr files ($red_pc%)\n";
    echo "yellow code:\t $yellow_nmbr files ($yellow_pc%)\n";
    echo "green code:\t $green_nmbr files ($green_pc%)\n";
    echo "------------------\n";
    echo "total: $how_many files\n";
}

/**
 * Checks whether a file is a code file.
 */
function isCodeFile($file_name) {
    $len = strlen($file_name);
    if (substr($file_name, $len-4, 4) == ".icc" ||
        substr($file_name, $len-3, 3) == ".cc" ||
        substr($file_name, $len-3, 3) == ".hh") {
        return true;
    } else {
        return false;
    }
}

/**
 * Prints user statistics such that code lines, comment lines and empty
 * lines for individual users are shown.
 */
function printUserStats($codes) {
	$code_lines = array();
	$comment_lines = array();
    $empty_lines = array();
	$cnt = count($codes);
    for ($i = 0; $i < $cnt; $i++) {
        $auth = $codes[$i]->author;
        if ($auth == "") {
            $auth = "unknown";
        }

	    if (array_key_exists($auth, $code_lines)) {
		    $code_lines[$auth] += $codes[$i]->codelines;
            $comment_lines[$auth] += $codes[$i]->commentlines;
            $empty_lines[$auth] += $codes[$i]->emptylines;
		} else {
            $code_lines[$auth] = $codes[$i]->codelines;
            $comment_lines[$auth] = $codes[$i]->commentlines;
            $empty_lines[$auth] = $codes[$i]->emptylines;
        }
    }
	arsort($code_lines);
    $persons = array_keys($code_lines);
    $cnt = count($persons);
    for ($j = 0; $j < $cnt; $j++) {
        $person = $persons[$j];
        $total = $code_lines[$person] + $comment_lines[$person] +
            $empty_lines[$person];
	    $code_percent = round($code_lines[$person]/$total * 100, 1);
        $comment_percent = round($comment_lines[$person]/$total * 100, 1);
        $empty_percent = round($empty_lines[$person]/$total * 100, 1);
        echo "**************************************\n";
        echo "$person:\n";
        echo "code lines:\t $code_lines[$person]\t ($code_percent%)\n";
        echo "comment lines:\t $comment_lines[$person]\t ($comment_percent%)\n";
        echo "empty lines:\t $empty_lines[$person]\t ($empty_percent%)\n";
    	echo "total:\t\t $total\n";
        echo "**************************************\n\n";
	}
}

$_colors = array(
       'LIGHT_RED'      => "[1;31m",
       'LIGHT_GREEN'    => "[1;32m",
       'YELLOW'    => "[1;33m",
       'LIGHT_BLUE'    => "[1;34m",
       'MAGENTA'    => "[1;35m",
       'LIGHT_CYAN'    => "[1;36m",
       'WHITE'    => "[1;37m",
       'NORMAL'    => "[0m",
       'BLACK'    => "[0;30m",
       'RED'        => "[0;31m",
       'GREEN'    => "[0;32m",
       'BROWN'    => "[0;33m",
       'BLUE'        => "[0;34m",
       'CYAN'        => "[0;36m",
       'BOLD'        => "[1m",
       'UNDERSCORE'    => "[4m",
       'REVERSE'    => "[7m",

);

function termcolored($text, $color="NORMAL", $back=1){
   global $_colors;
   $out = $_colors["$color"];
   if($out == ""){ $out = "[0m"; }
   if($back){
       return chr(27)."$out$text".chr(27)."[0m";
   }else{
       echo chr(27)."$out$text".chr(27)."[0m";
   }//fi
}// end function

/**
 * Prints help menu.
 */
function printUsage($argv) {
    echo "Usage: $argv[0] [option]...\n";
    echo "\t-w\t\t\tprints warning if missing tags are found.\n";
    echo "\t--least-empty\t\tprints 10 files having the least empty lines.\n";
    echo "\t--top-lines\t\tprints 10 files having most code lines.\n";
    echo "\t--least-comments\tprints 10 files with least comment lines.\n";
    echo "\t--changes\t\tprints review statistics.\n";
    echo "\t--review\t\tprints file, author, and line number as CSV\n";
    echo "\t--authors\t\tprints user statistics\n";
    echo "\t--virgin-only\t\tprints statistics of files not yet reviewed\n";
    echo "\t--style\t\t\tprints style warnings\n";
    echo "\t--status\t\tprints the statistics of code file statuses\n";
	echo "\t--userstats\t\tprints the statistics for individual authors\n";
}

// options that the script recognizes
$options = array(1 => "-w", 2=> "--least-empty",
                 3 => "--review", 4 => "--top-lines",
                 5 => "--least-comments", 6 => "--changes",
                 7 => "--authors", 8 => "--help", 9 => "--virgin-only",
                 10 => "--style", 11 => "--status", 12 => "--userstats");
$dir = ".";
$storage = new CodeStorage;
$warnings = false;
$least_empty = false;
$top_lines = false;
$least_comments = false;
$changes = false;
$review = false;
$authors = false;
$virgin_only = false;
$style = false;
$status = false;
$userstats = false;
$help = false;
$option_errors = false;

if ($argc == 1) {
    $files = recursive_listdir($dir);
    $storage->processCode($files);
    $storage->lineCount();
} else {
    for ($j = 1; $j < $argc; $j++) {
        if (!in_array($argv[$j], $options)) {
            echo "Unknown option: $argv[$j]\n";
    	    $option_errors = true;    
	    } else {
            switch ($argv[$j]) {
            case "-w":
                $warnings = true;
		        break;
            case "--least-empty":
                $least_empty = true;
		        break;
            case "--review":
                $review = true;
		        break;
            case "--top-lines":
                $top_lines = true;
		        break;
            case "--least-comments":
                $least_comments = true;
		        break;
            case "--changes":
                $changes = true;
		        break;
            case "--authors":
                $authors = true;
		        break;
            case "--virgin-only":
                $virgin_only = true;
		        break;
            case "--style":
                $style = true;
		        break;
            case "--status":
                $status = true;
		        break;
            case "--help":
                $help = true;
		        break;
            case "--userstats":
                $userstats = true;
                break;
            }
        }
    }

    if ($style) {
        $storage->style_check = true;
    }
    if ($warnings) {
        $storage->warnings = true;
    }
    if ($help || $option_errors) {
        printUsage($argv);
    } else {

        $files = recursive_listdir($dir);
        $storage->processCode($files);

        if ($least_empty) {
            topTenEmpty($storage->codes);
        }
        if ($top_lines) {
            topTenLines($storage->codes);
        }
        if ($least_comments) {
            topTenComments($storage->codes);
        }
        if ($changes) {
            printChanges($storage->codes);   
        }
        if ($review) {
            $storage->printReview();
        }
        if ($authors) {
            $storage->userStats();
        }
        if ($virgin_only) {
            printVirgins($storage->codes);
        }
        if ($status) {
            printColors($storage->codes);
        }
	    if ($userstats) {
            printUserStats($storage->codes);
        }
    }
}


?>
