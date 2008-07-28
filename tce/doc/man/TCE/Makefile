
# relative path to Makefile definitions and shared latex/bibtex files
DEFS = ../../latexdefs

# document title (extended document name) - must be double quoted!
DOC_NAME_TEXT = "TTA Codesign Environment User Manual"

# options passed to latex command
LATEX_OPTS = -interaction=batchmode

# default rule built by Makefile
BUILD_DEFAULT = pdf


include $(DEFS)/Makefile.defs

publish: TCE.dvi html
	# dvipdfm seems to produce a better PDF 
	dvipdfm -o TCE_manual.pdf TCE.dvi
	cp TCE_manual.pdf ../../../manual/

publish_to_web: TCE.pdf TCE.ps html
	scp -r TCE.pdf TCE.ps TCE ${LINTULA_USERNAME}@webhotel.cs.tut.fi:/export/services/tce.cs.tut.fi/public_html/user_manual
