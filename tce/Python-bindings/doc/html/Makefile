#
# Makefile for creating html documentation for the Python bindings.
#

all::
	# Generating html documentation. If you get errors from
	# the dynamic linker, make sure you linker path (e.g.
	# LD_LIBRARY_PATH) includes the necessary TCE libraries.
	# 
	# If python does not find the TCE package, make sure you
	# have properly installed the packages.
	# 
	python generate_docs.py
	#
	# Generated html documentation.
	# NOTE: the documentation is for the TCE packages that python
	# finds in its package search path.
