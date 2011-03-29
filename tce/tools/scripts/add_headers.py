#!/usr/bin/env python
#
# Adds .hh and .icc to *_SOURCES of Makefile.ams recursively
#
import os, sys, os.path, re
import glob
from os.path import join

start_dir = sys.argv[1]

def find_libname():
    if not os.path.exists('Makefile.am'):
        return None
    f = open('Makefile.am')
    libname = None
    for line in f.readlines():
        match = re.search( r"(\w+)_SOURCES", line)
        if match is not None:
            if match.group(1) == 'BUILT': continue
            
            if libname is None:
                libname = match.group(1)
            elif libname != match.group(1):
                print "Found two libnames: %s and %s, ignoring." % \
                      (libname, match.group(1))
                return None
    return libname
        
        

for root, dirs, files in os.walk(start_dir):
    path = root
    old_dir = os.getcwd()
    os.chdir(path)    
    headers = glob.glob("*.hh") + glob.glob("*.icc") + glob.glob("*.h")
    # print path, headers

    if len(headers) == 0:
        os.chdir(old_dir)
        continue

    if os.path.exists('Makefile.am'):
        file_contents = open('Makefile.am').readlines()
        old_start = -1
        old_end = -1
        row_no = 0
        for row in file_contents:
            if '## headers start' in row:
                old_start = row_no
            if '## headers end' in row:
                old_end = row_no
            row_no += 1

        if old_start >= 0 and old_end >= 0:
            print "Strip out the old generated headers list."
            file_contents = \
                file_contents[0:old_start-1] + file_contents[old_end+1:]
        new_file = open('Makefile.am', 'w+')
        new_file.write("".join(file_contents))
        new_file.close()            
    

    libname = find_libname()
    if libname is None:
        print "Did not find libname for %s" % path
        os.chdir(old_dir)
        continue

    header_str = "\n## headers start\n"
    header_str += "%s_SOURCES += \\\n" % libname
    headers_per_line = 0
    for header in headers:
        if headers_per_line == 2:
            header_str += "\\\n"
            headers_per_line = 0
        
        if headers_per_line == 0:
            header_str += "\t"
        header_str += header + " "
        headers_per_line += 1

    header_str += "\n## headers end\n"
    print "appending: %s" % header_str

    f = open('Makefile.am', 'a+')
    f.write(header_str)
    f.close()

    os.chdir(old_dir)
    
    
