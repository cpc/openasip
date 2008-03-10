BUILD_ESTIMATOR_PLUGIN = ../../src/codesign/Estimator/buildestimatorplugin
basedir = ${prefix}/share/tce/cost_estimation_plugins/base

nobase_base_DATA = StrictMatchFUEstimator.so InterpolatingFUEstimator.so

all-local: StrictMatchFUEstimator.so InterpolatingFUEstimator.so

StrictMatchFUEstimator.so: StrictMatchFUEstimator.cc
	${BUILD_ESTIMATOR_PLUGIN} StrictMatchFUEstimator.so \
	StrictMatchFUEstimator.cc

InterpolatingFUEstimator.so: InterpolatingFUEstimator.cc
	${BUILD_ESTIMATOR_PLUGIN} InterpolatingFUEstimator.so \
	InterpolatingFUEstimator.cc

clean:
	rm -f StrictMatchFUEstimator.so InterpolatingFUEstimator.so

