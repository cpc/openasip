
# If 'needSQLite' variable is defined, we'll try to find the command line
# client program of the SQLite and set it to $TCE_SQLITE_BIN.
if test -n "$needSQLite";
    then
    TCE_SQLITE_BIN=$(which sqlite3)
    if test -z $TCE_SQLITE_BIN;
        then
        TCE_SQLITE_BIN=$(which sqlite)

        if test -z $TCE_SQLITE_BIN;
            then
            echo Unable to find SQLite binary.
            exit 1
        fi
    fi
fi
