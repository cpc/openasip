BUILD_ICDECODER_PLUGIN = ../src/procgen/ProGe/buildicdecoderplugin
basedir = ${prefix}/share/tce/icdecoder_plugins/base

nobase_base_DATA = DefaultICDecoderPlugin.so

all-local: DefaultICDecoderPlugin.so

DefaultICDecoderPlugin.so: DefaultDecoderGenerator.hh DefaultICGenerator.hh CentralizedControlICGenerator.hh
DefaultICDecoderPlugin.so: DefaultICDecoderPlugin.cc DefaultDecoderGenerator.cc DefaultICGenerator.cc CentralizedControlICGenerator.cc
	${BUILD_ICDECODER_PLUGIN} $@ $^

clean:
	rm -f DefaultICDecoderPlugin.so
