#!/bin/bash
### TCE TESTCASE
### title: ImmediateGenerator Explorer plugin test case with 3-bus machine

TEMPLATE_NAME="newTemplate"

function cleanup {
    rm -rf test{3b,4b}.dsdb 2.adf
}

function test_split {
    cleanup
    WIDTH=$1
    BUS_COUNT=$2
    explore -a ./mach/imm_test_3_bus.adf test3b.dsdb &>/dev/null
    explore -e ImmediateGenerator -s 1 -u add_it_name="${TEMPLATE_NAME}" -u width=$WIDTH -u split='true' test3b.dsdb
    explore -w 2 test3b.dsdb 1>/dev/null
    TEMPLATES="$(grep '<template name=.*>' 2.adf | wc -l)"

    if [ "${TEMPLATES}" -ne 2 ]; then
        echo "Incorrect number of templates."
        exit 1
    fi

    if [ "$(grep "<template name=\"${TEMPLATE_NAME}\">" 2.adf | wc -l)" -ne 1 ]; then
       echo "Template added was not found."
       exit 1
    fi

    awk "
        BEGIN { FS = \"[><]\"; widthSum=0 }

        /<template name=\"${TEMPLATE_NAME}\">/    { inside=1 }
        /<width>/               {
                                    if (inside > 0) {
                                        widthSum += \$3
                                        inside++
                                    }

                                }
        /<\/template>/          {
                                    templateCount = inside - 1
                                    inside=0
                                }

        END {
                if (widthSum != $WIDTH) {
                    print \"Supported template width $WIDTH was not right.\"
                    exit 1
                }
                if (templateCount != $BUS_COUNT) {
                    print \"Used incorrect amount of busses.\"
                    exit 1
                }
            }
        " 2.adf || exit 1
}

# imm_test_3_bus.adf should have the following move slot split:
# | move slot bus2: 17 | move slot bus1: 20 | move slot bus0: 19 |
# so a 20-bit immediate should use the second bus completely, while a 21-bit one
# exceeds it by one bit, and 60 exceeds the total bitcount (will also affect )
test_split 20 1
test_split 21 2
test_split 32 2
test_split 60 3
