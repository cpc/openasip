TOP_SRCDIR = ../../../..

all : InterpolatingFUEstimator InterpolatingRFEstimator

# Compile interpolating estimator plugins
InterpolatingFUEstimator :
	cd ../../../../cost_estimator_plugins/fu/; \
	rm -f InterpolatingFUEstimator.so; \
	../../src/codesign/Estimator/buildestimatorplugin \
	InterpolatingFUEstimator.so InterpolatingFUEstimator.cc > /dev/null

InterpolatingRFEstimator :
	cd ../../../../cost_estimator_plugins/rf/; \
	rm -f InterpolatingRFEstimator.so; \
	../../src/codesign/Estimator/buildestimatorplugin \
	InterpolatingRFEstimator.so InterpolatingRFEstimator.cc > /dev/null

include ${TOP_SRCDIR}/test/Makefile_test.defs
