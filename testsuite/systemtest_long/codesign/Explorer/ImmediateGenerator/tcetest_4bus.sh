#!/bin/bash
### TCE TESTCASE
### title: ImmediateGenerator Explorer plugin test case with 4-bus machine

TEMPLATE_NAME="newTemplate"
WIDTH=32
BUS_COUNT=2

rm -f test4b.dsdb 2.adf
explore -a ./mach/imm_test_4_bus.adf test4b.dsdb
explore -e ImmediateGenerator -s 1 -u add_it_name="${TEMPLATE_NAME}" -u width=$WIDTH -u split='true' test4b.dsdb
explore -w 2 test4b.dsdb

# at least one bus, one rf and one fu should be removed
TEMPLATES="$(grep '<template name=.*>' 2.adf | wc -l)"

if [ "${TEMPLATES}" -ne 2 ]; then
    echo "Not right number of templates."
    exit 1
fi

if [ "$(grep "<template name=\"${TEMPLATE_NAME}\">" 2.adf | wc -l)" -ne 1 ]; then
   echo "Template added was not found."
   exit 1
fi

# next check that template was splitted as should

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

