#!/usr/bin/env python2
# -*- coding: utf-8 -*-
"""Finds last llvm svn version which has passed the most buildbot tests.

Checks last 100 revisions and.

author Mikael Lepistö 2009
"""

import urllib2
import BeautifulSoup

soup = BeautifulSoup.BeautifulSoup(
    urllib2.urlopen('http://google1.osuosl.org:8011/grid?width=100').read())

revcount = len(soup.find('tr').findAll('td'))

oks_per_column = [[0,0]] + [[0, int(i.contents[0]) ]
                            for i in soup.findAll('td', 'sourcestamp')]

for i in soup.findAll('tr'):
	col = 0
	for row in i.findAll('td'):
		link = row.find('a')
		if link and link.string == u'OK':
			oks_per_column[col][0] += 1
		col += 1

print max(oks_per_column)[1]
