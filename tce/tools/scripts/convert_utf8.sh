#!/bin/bash

# Copyright 2022 Google LLC
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

# This converts iso-8859-1 and windows-1252 files (western European format)
# into UTF-8 based on the file's MIME type. If conversion fails, the original 
# file is untouched.
#
# @author Sarah Clark, Google <sarahclark@google.com>

function get_type() ( 
	file --mime-encoding $1 | awk -F ':' '{ print $2 }'
)

function convert() (
	local tmp="${1}.utf8"
	iconv -f $2 -t UTF-8//IGNORE < $1 > $tmp && mv $tmp $1 && echo "$1 converted"
)

if [[ $# -eq 0 ]]; then
	echo "Usage: $0 filename [filename...]"
	exit 1
fi

for filepath in $@; do
	mtype=$(get_type $filepath)
	if [ $mtype = 'iso-8859-1' ]; then 
		convert $filepath 'ISO-8859-1' ;
	elif [ $mtype = 'windows-1252' ]; then 
		convert $filepath 'WINDOWS-1252' ;
	else echo "$filepath - skipped: $mtype"
	fi
done



