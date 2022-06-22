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

""" Test module for wx_linter"""
import unittest

from wx_linter import RemoveAlignFromExpand, RemoveMatchingAlignmentFromSizer, RemoveSizeFromSpinner


class TestFixAlignExpand(unittest.TestCase):
    """Removes alignment flags with wxGROW and wxEXPAND"""

    def test_match_expand(self):
        """Rule recognizes grow and expand flags"""
        rule = RemoveAlignFromExpand("")
        self.assertTrue(rule.match("This is a test of wxEXPAND"))
        self.assertTrue(rule.match("This is a test of wxGROW"))
        self.assertFalse(rule.match("This is a test"))

    def test_repair_expand(self):
        """Cannot have alignment flags w/ wxWXPAND or wxGROW"""
        rule = RemoveAlignFromExpand()
        self.assertEqual(rule.process(
            "Fix wxEXPAND|wxALIGN_RIGHT flags"), "Fix wxEXPAND flags")
        self.assertEqual(rule.process(
            "Fix wxEXPAND|wxALIGN_RIGHT|wxALL flags"), "Fix wxEXPAND|wxALL flags")


class TestFixAlignInVSizer(unittest.TestCase):
    """Removes vertical alighment flags in wxVERTICAL sizers"""

    def setUp(self):
        self.rule = RemoveMatchingAlignmentFromSizer("wxVERTICAL")

    def test_line_cleaning(self):
        """Check comment removal and whitespace trimming"""
        self.assertEqual(self.rule.clean("This is a test"), "This is a test")
        self.assertEqual(self.rule.clean(
            "This is a test // no comment"), "This is a test")
        self.assertEqual(self.rule.clean(
            "  This is a test // no comment"), "This is a test")
        self.assertEqual(self.rule.clean(
            "  This is a test /* no comment"), "This is a test")

    def test_indentation_count(self):
        """Check { and } tracking"""
        self.assertEqual(self.rule.check_indentation(""), 0)  # default
        self.assertEqual(self.rule.check_indentation("{"), 1)
        self.assertEqual(self.rule.check_indentation("}"), 0)

    def test_match_align(self):
        """Test matching an item with its flags on the same line"""
        # This matcher expects to be in a block
        self.assertFalse(self.rule.match("{"))
        # Match primed
        self.assertFalse(self.rule.match(
            "    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );"))
        # Match
        self.assertTrue(self.rule.match(
            "    item0->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );"))

    def test_matches_static_box(self):
        """Test matching an item with its flags on the same line"""
        # This matcher expects to be in a block
        self.assertFalse(self.rule.match("{"))
        # Match primed
        self.assertFalse(self.rule.match(
            "    wxStaticBoxSizer *item1 = new wxStaticBoxSizer( item0, wxVERTICAL );"))
        # Match
        self.assertTrue(self.rule.match(
            "    item1->Add( item7, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );"))

    def test_match_align_split(self):
        """Test matching an item with its flags on a second line"""
        # This matcher expects to be in a block
        self.assertFalse(self.rule.match("{"))
        # Match primed
        self.assertFalse(self.rule.match(
            "    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );"))
        # Match
        self.assertFalse(self.rule.match(
            "    item0->Add( item7, 0,"))
        self.assertTrue(self.rule.match(
            "wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );"))

    def test_ignores_other_direction(self):
        """Test matching an item with its flags on a second line"""
        # This matcher expects to be in a block
        self.rule.match("{")
        self.rule.match("wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );")
        self.rule.match("wxBoxSizer *item1 = new wxBoxSizer( wxHORIZONTAL );")
        # Match
        self.assertTrue(self.rule.match(
            "item0->Add( item7, 0,  wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );"))
        self.assertFalse(self.rule.match(
            "item1->Add( item8, 0,  wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );"))

    def test_scopes_match_to_context(self):
        """Test matching an item with its flags on a second line"""
        # This matcher expects to be in a block
        self.rule.match("{")
        self.rule.match("wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );")
        self.assertTrue(self.rule.match(
            "item0->Add( item7, 0,  wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );"))
        self.rule.match("}")
        self.assertFalse(self.rule.match(
            "item0->Add( item7, 0,  wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5 );"))

    def test_repair_valign(self):
        """Ensure all the flags are removed for a vertical sizer"""
        self.assertEqual(self.rule.process(
            "    item0->Add( item7, 0, " +
            "wxALIGN_BOTTOM|wxGROW|wxALIGN_CENTER_VERTICAL|wxALL|wxALIGN_TOP, 5 );"),
            "    item0->Add( item7, 0, wxGROW|wxALL, 5 );")

    def test_repair_h_align(self):
        """Ensure all the flags are removed for a horizontal sizer"""
        rule = RemoveMatchingAlignmentFromSizer("wxHORIZONTAL")
        self.assertEqual(rule.process(
            "    item0->Add( item7, 0, " +
            "wxALIGN_LEFT|wxGROW|wxALIGN_CENTER_HORIZONTAL|wxALL|wxALIGN_RIGHT, 5 );"),
            "    item0->Add( item7, 0, wxGROW|wxALL, 5 );")

    def test_empty_result(self):
        """Ensure all the flags are removed for a horizontal sizer"""
        rule = RemoveMatchingAlignmentFromSizer("wxHORIZONTAL")
        self.assertEqual(rule.process(
            "    item0->Add( item7, 0, " +
            "wxALIGN_LEFT|wxALIGN_CENTER_HORIZONTAL|wxALIGN_RIGHT, 5 );"),
            "    item0->Add( item7, 0, 0, 5 );")


class TestFixSizeOnSpinner(unittest.TestCase):
    """Removes limited wxSize() on wxSpinCtrl"""

    def setUp(self):
        self.rule = RemoveSizeFromSpinner()

    def test_match_size(self):
        """Rule recognizes grow and expand flags"""
        self.assertTrue(self.rule.match("    wxSpinCtrl *bitWidth = new wxSpinCtrl( parent, " +
                                        "ID_BIT_WIDTH, wxT(\"1\"), wxDefaultPosition, " +
                                        "wxSize(100,-1), 0, 1, 10000, 1);"))
        self.assertFalse(self.rule.match("    wxSpinCtrl *bitWidth = new wxSpinCtrl( parent, " +
                                         "ID_BIT_WIDTH, wxT(\"1\"), wxDefaultPosition, " +
                                         "wxSize(-1,-1), 0, 1, 10000, 1);"))

    def test_match_multiline(self):
        """Rule handles size on a subsequent line"""
        # Test multiline
        self.assertFalse(self.rule.match(
            "    wxSpinCtrl *bitWidth = new wxSpinCtrl( parent, ID_BIT_WIDTH, wxT(\"1\"), "))
        self.assertTrue(self.rule.match(
            "wxDefaultPosition, wxSize(100,-1), 0, 1, 10000, 1);"))

    def test_repair_size(self):
        """Cannot have alignment flags w/ wxWXPAND or wxGROW"""
        self.assertEqual(self.rule.process("wxSize(100, 100)"), "wxSize(-1,-1)")
        self.assertEqual(self.rule.process("wxSize(-1, -1)"), "wxSize(-1,-1)")
        self.assertEqual(self.rule.process("wxSize()"), "wxSize(-1,-1)")


if __name__ == '__main__':
    unittest.main()
