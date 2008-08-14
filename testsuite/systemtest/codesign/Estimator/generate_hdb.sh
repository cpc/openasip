#!/bin/bash
# Initialize the test HDB.
rm -f data/test.hdb
../../../../tce/src/procgen/HDB/createhdb data/test.hdb
sqlite3 data/test.hdb <<EOF
BEGIN;

INSERT INTO cost_function_plugin 
VALUES(1, 'jepjep', 'StrictMatchFUEstimator', 
       '../../../../tce/cost_estimator_plugins/fu/StrictMatchFUEstimator.so', 
       'fu');

INSERT INTO cost_function_plugin 
VALUES(2, 'jepjep', 'DefaultICDecoder', 
       '../../../../tce/icdecoder_plugins/DefaultICDecoderPlugin.so', 'icdec');

INSERT INTO cost_function_plugin 
VALUES(3, 'jepjep', 'StrictMatchRFEstimator', 
       '../../../../tce/cost_estimator_plugins/rf/StrictMatchRFEstimator.so', 
       'rf');

INSERT INTO cost_estimation_data(id, plugin_reference, name, value) 
VALUES(NULL, 2, 'input_sub_socket 2 1 32', 
'throughput_delay=20 area=20 active_energy=50 idle_energy=2 control_delay=30');

INSERT INTO cost_estimation_data(id, plugin_reference, name, value) 
VALUES(NULL, 2, 'input_sub_socket 1 1 32', 
'throughput_delay=5 area=50.2 active_energy=30 idle_energy=3 control_delay=3');

INSERT INTO cost_estimation_data(id, plugin_reference, name, value) 
VALUES(NULL, 2, 'input_sub_socket 2 1 5', 
'throughput_delay=5 area=50.2 active_energy=30 idle_energy=3 control_delay=3');

INSERT INTO cost_estimation_data(id, plugin_reference, name, value) 
VALUES(NULL, 2, 'output_sub_socket 1 2 8', 
'throughput_delay=5 area=50.2 active_energy=30 idle_energy=3 control_delay=3');

INSERT INTO cost_estimation_data(id, plugin_reference, name, value) 
VALUES(NULL, 2, 'output_sub_socket 1 2 5', 
'throughput_delay=5 area=50.2 active_energy=30 idle_energy=3 control_delay=3');

INSERT INTO cost_estimation_data(id, plugin_reference, name, value) 
VALUES(NULL, 2, 'output_sub_socket 1 2 32', 
'throughput_delay=5 area=50.2 active_energy=30 idle_energy=3 control_delay=3');

INSERT INTO cost_estimation_data(id, plugin_reference, name, value) 
VALUES(NULL, 2, 'output_sub_socket 1 1 32', 
'throughput_delay=5 area=50.2 active_energy=30 idle_energy=3 control_delay=3');

INSERT INTO cost_estimation_data(id, plugin_reference, name, value) 
VALUES(NULL, 2, 'sub_bus 4 8 5',
'throughput_delay=7.2 area=5.0 active_energy=6.0 idle_energy=7.2');

INSERT INTO cost_estimation_data(id, plugin_reference, name, value) 
VALUES(NULL, 2, 'sub_bus 3 6 3',
'throughput_delay=7.2 area=5.0 active_energy=6.0 idle_energy=7.2');

INSERT INTO cost_estimation_data(id, plugin_reference, name, value) 
VALUES(NULL, 2, 'sub_bus 2 6 24',
'throughput_delay=7.2 area=5.0 active_energy=6.0 idle_energy=7.2');

INSERT INTO fu VALUES(1, NULL, 1);
INSERT INTO rf VALUES(1, NULL, 3);

INSERT INTO cost_estimation_data(plugin_reference, fu_reference, name, value) 
VALUES(1, 1, 'area', '1234.0');
INSERT INTO cost_estimation_data(plugin_reference, rf_reference, name, value) 
VALUES(3, 1, 'area', '4284.0');

INSERT INTO cost_estimation_data(plugin_reference, fu_reference, name, value) 
VALUES(1, 1, 'output_delay', '2001');
INSERT INTO cost_estimation_data(plugin_reference, fu_reference, name, value) 
VALUES(1, 1, 'input_delay', '6701');
INSERT INTO cost_estimation_data(plugin_reference, fu_reference, name, value) 
VALUES(1, 1, 'computation_delay', '1670');

INSERT INTO cost_estimation_data(plugin_reference, fu_reference, name, value) 
VALUES(1, 1, 'operation_execution_energy ADD', '100');
INSERT INTO cost_estimation_data(plugin_reference, fu_reference, name, value) 
VALUES(1, 1, 'operation_execution_energy STQ', '300');
INSERT INTO cost_estimation_data(plugin_reference, fu_reference, name, value) 
VALUES(1, 1, 'operation_execution_energy LDQ', '350');
INSERT INTO cost_estimation_data(plugin_reference, fu_reference, name, value) 
VALUES(1, 1, 'fu_idle_energy', '10');


INSERT INTO cost_estimation_data(plugin_reference, rf_reference, name, value) 
VALUES(3, 1, 'output_delay', '2431');
INSERT INTO cost_estimation_data(plugin_reference, rf_reference, name, value) 
VALUES(3, 1, 'input_delay', '641');
INSERT INTO cost_estimation_data(plugin_reference, rf_reference, name, value) 
VALUES(3, 1, 'computation_delay', '6451');

INSERT INTO cost_estimation_data(plugin_reference, rf_reference, name, value) 
VALUES(3, 1, 'rf_access_energy 1 0', '100');
INSERT INTO cost_estimation_data(plugin_reference, rf_reference, name, value) 
VALUES(3, 1, 'rf_access_energy 0 2', '250');
INSERT INTO cost_estimation_data(plugin_reference, rf_reference, name, value) 
VALUES(3, 1, 'rf_idle_energy', '20');

COMMIT;
.quit
EOF
