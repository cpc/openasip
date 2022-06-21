#!/usr/bin/env python3

# Copyright 2022 Google LLC
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# author: Sarah Clark <sarahclark@google.com>

"""
Scans through one or more files and updates wxWindows code that causes assertion warnings
and/or errors outside of Ubuntu.

These fixes are:
- Remove vertical alignment flags in vertical sizers (example: ALIGN_CENTER_VERTICAL, ALIGN_TOP)
- Remove horizontal alignment flags in horizontal sizers (example: ALIGN_CENTER_VERTICAL, ALIGN_TOP)
- Remove alignment flags with wxGROW/wxEXPAND
- Remove explicit sizing on spinners
"""
import argparse
import logging
import re
from typing import Pattern

FLAGS_RE = re.compile(r"(wx\w+\s?\|?)+")


def main():
    """Parse the command line and process the files one by one"""
    linter_keywords = ["valign", "halign", "expand", "spinctrl", "imagelist"]

    parser = argparse.ArgumentParser(allow_abbrev=False)
    group = parser.add_mutually_exclusive_group()
    group.add_argument("-v", "--verbose", help="display debugging data)",
                       action="store_const", const=logging.DEBUG, dest='loglevel')
    group.add_argument("-q", "--quiet", help="display warnings and errors only",
                       action="store_const", const=logging.WARNING, dest='loglevel')

    parser.add_argument(
        "--lint", choices=linter_keywords, default=linter_keywords,
        help="run a specific lint (otherwise run all)", action="append")

    parser.add_argument("-n", "--dry-run", help="examine files without changing",
                        action="store_true")

    parser.add_argument("file", nargs="+", help="file to check")
    args = parser.parse_args()

    logging.basicConfig(level=args.loglevel or logging.INFO)

    if args.dry_run:
        logging.info("Dry run -- no files will be overwritten")
    for filepath in args.file:
        process_file(filepath, args.lint, args.dry_run)


def allocate_linters(linter_keys, filepath):
    """Constructs an array of linters given a set of keywords"""
    linter_classes = {"expand": RemoveAlignFromExpand(filepath),
                      "valign": RemoveMatchingAlignmentFromSizer("wxVERTICAL", filepath),
                      "halign": RemoveMatchingAlignmentFromSizer("wxHORIZONTAL", filepath),
                      "spinctrl": RemoveSizeFromSpinner(filepath),
                      "imagelist": FlagUnsizedImageList(filepath),
                      }
    linters = []
    for key in linter_keys:
        linters.append(linter_classes[key])
    return linters


def process_file(path, linter_keys, is_dry_run):
    """Runs all linters on a file and writed any changes if is_dry_run is False"""
    is_changed = False
    new_lines = []
    linters = allocate_linters(linter_keys, path)
    # Read and scan
    with UTF8ContextManager(path) as file:
        lines = file.readlines()
        for index, line in enumerate(lines):
            line_num = index + 1
            for linter in linters:
                if linter.match(line, line_num):
                    is_changed = True
                    replacement = linter.process(line, line_num)
                    if replacement is line:
                        logging.debug(
                            "% s:%d change not applied by %s", path, line_num, linter.name)
                    logging.debug("%s:%d %s matched", path,
                                  line_num, linter.name)
                    line = replacement
            new_lines.append(line)
    # Write changes
    if is_changed and not is_dry_run:
        with open(path, 'wt', encoding="utf-8") as file:
            file.writelines(new_lines)
    # Report
    if is_changed:
        if not is_dry_run:
            logging.info("%s: changed", path)
        else:
            logging.warning("%s: needs fixes", path)
    else:
        logging.info("%s: ok", path)


class UTF8ContextManager:
    """Context manager that reports encoding errors and continues"""

    def __init__(self, path) -> None:
        self.path = path
        self.file_obj = open(self.path, encoding="utf-8")

    def __enter__(self):
        logging.debug("Opening %s", self.path)
        return self.file_obj

    def __exit__(self, exc_type, exc_value, exc_tb):
        self.file_obj.close()
        if isinstance(exc_value, UnicodeDecodeError):
            logging.error(
                "%s needs to be in UTF-8", self.path)
            return True


class LintRuleBase():
    """Common code for linter rules"""

    def __init__(self, filepath="-unknown-") -> None:
        self.filepath = filepath
        self.end_comment_re = re.compile(r"((//.+)|(/\*.+(\*/)?))")

    def clean(self, line):
        """Remove comments and trim whitespace on the ends"""
        return self.end_comment_re.sub("", line, 1).strip()

    def remove_flags(self, flags: str, remove: Pattern, delimiter="|"):
        """Splits the flags string, removes flags matching a pattern, then re-assembles"""
        all_flags = map(lambda token: token.strip(), flags.split("|"))
        filtered = filter(lambda token: remove.fullmatch(
            token) is None, all_flags)
        result = delimiter.join(filtered)
        return result if len(result.strip()) > 0 else "0"


class RemoveAlignFromExpand(LintRuleBase):
    """Remove alignment flags within wxEXPAND and wxGROW"""

    def __init__(self, filepath="-unknown-") -> None:
        self.name = "RemoveAlignFromExpand"
        self.pattern = re.compile(r"(wxGROW|wxEXPAND)")
        self.flags_re = re.compile(r"(wx\w+\s?\|)*wx\w+")
        self.remove_re = re.compile(r"wxALIGN_\w+")
        super().__init__(filepath)

    def match(self, line, _=0):
        """Finds the pattern in the line."""
        return self.pattern.search(line)

    def process(self, line, _=0) -> str:
        """Accepts a line containing layout flags and removes the extraneous ones"""
        matched = self.flags_re.search(line)
        if matched is None:
            return line
        flags = matched.string[matched.start(): matched.end()]
        prefix = line[:matched.start()]
        suffix = line[matched.end():]
        replacement = self.remove_flags(flags, self.remove_re)
        return prefix + replacement + suffix


class RemoveMatchingAlignmentFromSizer(LintRuleBase):
    """Remove alignment flags matching the axis of the sizer"""

    def __init__(self, direction, filepath="-unknown-") -> None:
        self.name = "RemoveMatchingAlignmentFromSizer (" + direction + ")"
        self.active_tests = []
        self.in_candidate = False
        self.indentation = 0
        self.sizer_re = re.compile(
            r"wx(Static)?BoxSizer[\s*]+(\w+)\s*=\s*new\s+wx(Static)?BoxSizer" +
            r"\s*\(\s*(\w+,\s*)?" + direction + r"\s*\)")
        self.flags_re = re.compile(r"(wx\w+\s?\|)*wx\w+")
        self.remove_re = re.compile(
            r"(wxALIGN_CENTER_VERTICAL|wxALIGN_TOP|wxALIGN_BOTTOM)" if direction == "wxVERTICAL"
            else r"(wxALIGN_CENTER_HORIZONTAL|wxALIGN_LEFT|wxALIGN_RIGHT)")
        super().__init__(filepath)

    def check_indentation(self, cleaned_line):
        """Looks for { and } to increment and decrement self.indentation"""
        if cleaned_line.endswith('{'):
            self.indentation += 1
        elif cleaned_line.endswith('}'):
            self.indentation -= 1
        return self.indentation

    def is_new_sizer(self, line):
        """Returns a match object if line contains a new wxSizer, else None"""
        return self.sizer_re.search(line)

    def match(self, line, _=0):
        """Scans each line in order to detect content that needs fixing."""
        cleaned_line = self.clean(line)
        self.check_indentation(cleaned_line)
        if self.indentation <= 0:
            self.indentation = 0
            self.active_tests = []
            return None
        # Scan for new wxSizer (wx{DIRECTION})
        new_statement = self.sizer_re.search(cleaned_line)
        if new_statement:
            item = new_statement.group(2)
            test = f"{item}->Add"
            self.active_tests.append(test)
        elif any(lambda item: item.search(cleaned_line) for item in self.active_tests):
            # We may be in a multi-line statement
            self.in_candidate = True
        if self.in_candidate:
            if self.remove_re.search(cleaned_line):
                self.in_candidate = False
                return line
            if cleaned_line.endswith(";"):
                self.in_candidate = False
        return None

    def process(self, line, _=0) -> str:
        """Removes the specified flags from the line"""
        matched = self.flags_re.search(line)
        flags = matched.string[matched.start(): matched.end()]
        prefix = line[:matched.start()]
        suffix = line[matched.end():]
        replacement = self.remove_flags(flags, self.remove_re)
        return prefix + replacement + suffix


class RemoveSizeFromSpinner(LintRuleBase):
    """Remove size restrictions on spin controls"""

    def __init__(self, filepath="-unknown-") -> None:
        self.name = "RemoveSizeFromSpinner"
        self.in_candidate = False
        self.spinner_re = re.compile(r"new\s+wxSpinCtrl")
        self.size_re = re.compile(r"wxSize\(([-]?\d+),\s*([-]?\d+)\)")
        super().__init__(filepath)

    def match(self, line, _=0):
        """Scans each line in order to detect content that needs fixing."""
        cleaned_line = self.clean(line)
        new_statement = self.spinner_re.search(cleaned_line)
        if new_statement:
            # We may be in a multi-line statement
            self.in_candidate = True
        if self.in_candidate:
            size_param = self.size_re.search(cleaned_line)
            if size_param and (size_param.group(1) != "-1" or size_param.group(2) != "-1"):
                self.in_candidate = False
                return line
            if cleaned_line.endswith(";"):
                self.in_candidate = False
        return None

    def process(self, line, _=0) -> str:
        """Replaces with unbounded wxSize()"""
        matched = self.size_re.search(line)
        if matched is None:
            return line
        prefix = line[:matched.start()]
        suffix = line[matched.end():]
        return prefix + "wxSize()" + suffix


class FlagUnsizedImageList(LintRuleBase):
    """Warn if wxImageList is created without size data"""

    def __init__(self, filepath="-unknown-") -> None:
        self.name = "RemoveSizeFromSpinner"
        self.filepath = filepath
        self.image_list_re = re.compile(r"new\s+wxImageList\(\s*\)")
        super().__init__(filepath)

    def match(self, line, _=0):
        """Checks for an unsized image list ctor"""
        cleaned_line = self.clean(line)
        return self.image_list_re.search(cleaned_line)

    def process(self, line, index=0) -> str:
        """Replaces with unbounded wxSize()"""
        if self.match(line, index):
            logging.warning(
                "%s:%d new wxImageList(...) found without x, y values", self.filepath, index)
        return line


if __name__ == '__main__':
    main()
