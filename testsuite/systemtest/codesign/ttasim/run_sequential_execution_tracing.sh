#!/bin/bash
exe=../../../../tce/src/codesign/ttasim/ttasim
sqlite=`which sqlite3`

if [ "_$sqlite" = "_" ];
    then
    echo "No sqlite found!"
    exit 1
fi

# Execute without execution trace, make sure there was not one
# before.
rm -f data/hello.trace
$exe <<EOF
prog data/hello
run
quit
EOF

function dump_trace {
    if [ -a data/hello.trace ];
        then
        $sqlite data/hello.trace <<EOF
SELECT * FROM instruction_execution;
EOF
    fi
}

function dump_instruction_execution_profile {
    if [ -a data/hello.trace ];
        then
        $sqlite data/hello.trace <<EOF
SELECT instruction_execution_count.address AS address, 
       instruction_execution_count.count AS count, 
       procedure_address_range.procedure_name AS procedure 
FROM instruction_execution_count, procedure_address_range 
WHERE address >= procedure_address_range.first_address AND 
      address <= procedure_address_range.last_address
ORDER BY count DESC, address ASC;
EOF
    fi
}

function dump_procedure_execution_times {
    if [ -a data/hello.trace ];
        then
        $sqlite data/hello.trace <<EOF
SELECT procedure_address_range.procedure_name AS procedure, 
      SUM(instruction_execution_count.count) AS cycles 
FROM instruction_execution_count, procedure_address_range 
WHERE address >= procedure_address_range.first_address AND
      address <= procedure_address_range.last_address
GROUP BY procedure
ORDER BY cycles DESC;
EOF
    fi
}

function dump_procedure_transfers {
    if [ -a data/hello.trace ];
        then
        $sqlite data/hello.trace <<EOF
SELECT procedure_transfer.cycle AS cycle, 
       procedure_transfer.address AS address, 
       procedure_transfer.type AS is_exit,
       procedure_address_range.procedure_name AS procedure
FROM procedure_transfer,procedure_address_range 
WHERE address >= procedure_address_range.first_address AND
      address <= procedure_address_range.last_address
ORDER BY cycle;
EOF
    fi
}

function assert_file_found {
    if [ -a $1 ];
        then
        return 0;
    else
        echo "$1 not found!";
        return 1;
    fi
}

# Dump the trace. Should not be possible because execution
# trace was disabled and no trace should have been created.
dump_trace

# Execute with execution trace, make sure there was not one
# before.
rm -f data/hello.trace
$exe <<EOF
setting execution_trace true
setting procedure_transfer_tracking true
setting profile_data_saving on
prog data/hello
run
quit
EOF

assert_file_found data/hello.trace
dump_trace
dump_procedure_execution_times
dump_instruction_execution_profile
dump_procedure_transfers

