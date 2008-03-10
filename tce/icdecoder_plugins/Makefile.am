BUILD_ICDECODER_PLUGIN = ../src/procgen/ProGe/buildicdecoderplugin
basedir = ${prefix}/share/tce/icdecoder_plugins/base

nobase_base_DATA = DefaultICDecoderPlugin.so

all-local: DefaultICDecoderPlugin.so

DefaultICDecoderPlugin.so: DefaultICDecoderPlugin.cc DefaultDecoderGenerator.hh DefaultDecoderGenerator.cc DefaultICGenerator.hh DefaultICGenerator.cc CentralizedControlICGenerator.hh CentralizedControlICGenerator.cc
	${BUILD_ICDECODER_PLUGIN} DefaultICDecoderPlugin.so \
	DefaultICDecoderPlugin.cc DefaultDecoderGenerator.cc \
	DefaultICGenerator.cc CentralizedControlICGenerator.cc

clean:
	rm -f DefaultICDecoderPlugin.so
