BUILD_COMPRESSOR = ../src/bintools/PIG/buildcompressor
basedir = ${prefix}/share/tce/codecompressors/base

nobase_base_DATA = SimpleDictionary.so

all-local: SimpleDictionary.so

SimpleDictionary.so: simple_dictionary.cc
	${BUILD_COMPRESSOR} $@ $^

clean:
	rm -f SimpleDictionary.so
