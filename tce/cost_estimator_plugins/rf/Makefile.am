BUILD_ESTIMATOR_PLUGIN = ../../src/codesign/Estimator/buildestimatorplugin
basedir = ${prefix}/share/tce/cost_estimation_plugins/base

nobase_base_DATA = StrictMatchRFEstimator.so InterpolatingRFEstimator.so

all-local: StrictMatchRFEstimator.so InterpolatingRFEstimator.so

StrictMatchRFEstimator.so: StrictMatchRFEstimator.cc
	${BUILD_ESTIMATOR_PLUGIN} $@ $^

InterpolatingRFEstimator.so: InterpolatingRFEstimator.cc
	${BUILD_ESTIMATOR_PLUGIN} $@ $^

clean:
	rm -f StrictMatchRFEstimator.so InterpolatingRFEstimator.so

