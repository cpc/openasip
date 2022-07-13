
# relative path to Makefile definitions and shared latex/bibtex files
DEFS = ../../latexdefs

# document title (extended document name) - must be double quoted!
DOC_NAME_TEXT = "OpenASIP User Manual"

# options passed to latex command
LATEX_OPTS = -interaction=batchmode

# default rule built by Makefile
BUILD_DEFAULT = pdf

include $(DEFS)/Makefile.defs

publish: OpenASIP.dvi
	# dvipdfm seems to produce a better PDF 
	dvipdfm -o OpenASIP_manual.pdf OpenASIP.dvi
	cp OpenASIP_manual.pdf ../../../manual/

publish_to_web: OpenASIP.pdf
	scp -r OpenASIP.pdf ${USERNAME}@${HOST}:/home/tce/public_html/user_manual

distclean: veryclean
