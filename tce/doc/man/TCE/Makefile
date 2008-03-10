
# relative path to Makefile definitions and shared latex/bibtex files
DEFS = ../../latexdefs

# document title (extended document name) - must be double quoted!
DOC_NAME_TEXT = "TTA Codesign Environment User Manual"

# options passed to latex command
LATEX_OPTS = -interaction=batchmode

# default rule built by Makefile
BUILD_DEFAULT = pdf


include $(DEFS)/Makefile.defs


# Copies the generated PDF to the user manual installation directory
# and to the download page of the public web pages (also HTML generated there).
# Be sure to export WEBHOTEL_LOGIN to your login at the webhotel.cs.tut.fi.
publish: TCE.dvi html
	# dvipdfm seems to produce a better PDF 
	dvipdfm -o TCE_manual.pdf TCE.dvi
	cp TCE_manual.pdf ../../../manual/

