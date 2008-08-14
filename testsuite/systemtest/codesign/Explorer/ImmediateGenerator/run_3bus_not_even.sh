#!/bin/bash
EXPLORE_BIN="../../../../../tce/src/codesign/Explorer/explore"

TEMPLATE_NAME="newTemplate"
WIDTH=32

"${EXPLORE_BIN}" -a ./data/imm_test_3_bus.adf test3bne.dsdb &>/dev/null

"${EXPLORE_BIN}" -e ImmediateGenerator -s 1 -u add_it_name="${TEMPLATE_NAME}" -u width=$WIDTH -u split='true' test3bne.dsdb 1>/dev/null

"${EXPLORE_BIN}" -w 2 test3bne.dsdb 1>/dev/null

# at least one bus, one rf and one fu should be removed
TEMPLATES="$(grep '<template name=.*>' 2.adf | wc -l)"

if [ "${TEMPLATES}" -ne 3 ]; then
    echo "Not right number of templates."
fi

if [ "$(grep "<template name=\"${TEMPLATE_NAME}\">" 2.adf | wc -l)" -ne 1 ]; then
   echo "Template added was not found."
fi

# next check that template was splitted as should

awk "
    BEGIN { FS = \"[><]\"; widthSum=0 }

    /<template name=\"${TEMPLATE_NAME}\">/    { inside=1 }
    /<width>/               { 
                                if (inside == 1) { 
                                    if (\$3 != 16) {
                                        print \"Some template slot was not of right size.\"
                                    }
                                } 
                                if (inside > 1) { 
                                    if (\$3 != 8) {
                                        print \"xSome template slot was not of right size.\"
                                    }
                                } 
                                if (inside > 0) {
                                    widthSum += \$3
                                    inside++
                                }

                            }
    /<\/template>/          { inside=0 } 

    END { 
            if (widthSum != $WIDTH) {
                print \"Supported template width $WIDTH was not right.\"
            }
        }
    " 2.adf

rm -rf 2.adf
rm -rf test3bne.dsdb

WIDTH=60

"${EXPLORE_BIN}" -a ./data/imm_test_3_bus.adf test3bne.dsdb &>/dev/null

"${EXPLORE_BIN}" -e ImmediateGenerator -s 1 -u add_it_name="${TEMPLATE_NAME}" -u width=$WIDTH -u split='true' test3bne.dsdb 1>/dev/null

"${EXPLORE_BIN}" -w 2 test3bne.dsdb 1>/dev/null

awk "
    BEGIN { FS = \"[><]\"; widthSum=0 }

    /<template name=\"${TEMPLATE_NAME}\">/    { inside=1 }
    /<width>/               { 
                                if (inside == 1) { 
                                    if (\$3 != 32) {
                                        print \"1st template slot was not of right size.\"
                                    }
                                } 
                                if (inside == 2) { 
                                    if (\$3 != 20) {
                                        print \"2nd template slot was not of right size.\"
                                    }
                                } 
                                if (inside == 3) { 
                                    if (\$3 != 8) {
                                        print \"3rd template slot was not of right size.\"
                                    }
                                } 
                                if (inside > 0) {
                                    widthSum += \$3
                                    inside++
                                }

                            }
    /<\/template>/          { inside=0 } 

    END { 
            if (widthSum != $WIDTH) {
                print \"Supported template width $WIDTH was not right.\"
            }
        }
    " 2.adf
