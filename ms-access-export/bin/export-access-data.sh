#!/bin/bash

DIR_BASE="$HOME/git/ms_access_export"
DIR_BIN="$DIR_BASE/bin"
DIR_BIN_MDB="$DIR_BIN/mdbtools/src/util"
DIR_SRC="$DIR_BASE/src"
DIR_DATA="$DIR_BASE/data"
DIR_SCRIPTS="$DIR_BASE/scripts"
PG_HOST="pg-host"
PG_PORT="5432"
PG_USER="$POSTGRESQL_USERNAME" # set outside of script
PG_PASSWD="$POSTGRESQL_PASSWORD" # set outside of script
PG_DB="pg-db"
CMD_PSQL="psql -h $PG_HOST -p $PG_PORT -U $PG_USER -d $PG_DB -a -f "
FILE_CREATE_FUNC_UPDATE_DATE_MODIFIED="$DIR_BIN/update_date_modified.create.psql"
CMD_MDB_SCHEMA="mdb-schema -T"
CMD_MDB_EXPORT="mdb-export"
COL_DATE_CREATED="date_created"
COL_DATE_MODIFIED="date_modified"

PATH=$PATH:$DIR_BIN_MDB
export PATH

for i in $DIR_SCRIPTS $DIR_DATA
do
  rm -rf $i
  mkdir -p $i
done

#
# create PostgreSQL update_date_modified() function
#
PGPASSWORD=$PG_PASSWD $CMD_PSQL "$FILE_CREATE_FUNC_UPDATE_DATE_MODIFIED"

readarray -t files < <(ls -1 $DIR_SRC/*.accdb)
for file in "${files[@]}"
do
  schema=$(basename "$file" | cut -d'.' -f1)
  filename_psql_schema_create="$DIR_SCRIPTS/${schema}.schema-create.psql"

  #
  # create PostgreSQL table schema
  #
  cat <<EOF > "$filename_psql_schema_create"
DROP SCHEMA IF EXISTS "$schema" CASCADE;
CREATE SCHEMA "$schema";
EOF

  PGPASSWORD=$PG_PASSWD $CMD_PSQL "$filename_psql_schema_create"

  readarray -t tables < <(mdb-tables -1 "$file")
  for table in "${tables[@]}"
  do
    filename_schema="$DIR_DATA/${schema}.${table}.schema"
    filename_export="$DIR_DATA/${schema}.${table}.export"
    filename_psql_table_create="$DIR_SCRIPTS/${schema}.${table}.table-create.psql"
    filename_psql_copy="$DIR_SCRIPTS/${schema}.${table}.copy.psql"

    echo
    echo ========== $file, $schema, $table
    echo

    #
    # - dump Access table's schema
    # - export Access table's data
    #
    # NB: MDB Tools can be found at https://github.com/mdbtools/mdbtools
    #
    $CMD_MDB_SCHEMA "$table" "$file" | grep -v "\-\-" > "$filename_schema"
    $CMD_MDB_EXPORT "$file" "$table" > "$filename_export"

    sed -i "/^\s*$/d" "$filename_schema"

    # resolve PostgreSQL complaint about date/time field value out of range: "01/00/00 00:00:00"
    sed -i "s/\"01\/00\/00 00:00:00\"//g" "$filename_export"

    #
    # create PostgreSQL table
    #
    cat <<EOF > "$filename_psql_table_create"
SET search_path TO "$schema";

DROP TABLE IF EXISTS "$table";

EOF

    cat "$filename_schema" >> "$filename_psql_table_create"
    sed -i "s/\[/\"/g" "$filename_psql_table_create"
    sed -i "s/\]/\"/g" "$filename_psql_table_create"
    sed -i "s/Boolean/BOOLEAN/g" "$filename_psql_table_create"
    sed -i "s/Long Integer/INT/g" "$filename_psql_table_create"
    sed -i "s/Integer/SMALLINT/g" "$filename_psql_table_create"
    sed -i "s/Single/REAL/g" "$filename_psql_table_create"
    sed -i "s/Double/DOUBLE PRECISION/g" "$filename_psql_table_create"
    sed -i "s/Currency/REAL/g" "$filename_psql_table_create"
    sed -i "s/DateTime/TIMESTAMP/g" "$filename_psql_table_create"
    sed -i "s/Text/VARCHAR/g" "$filename_psql_table_create"
    sed -i "s/Memo\/Hyperlink (255)/VARCHAR (8192)/g" "$filename_psql_table_create"
    sed -i "s/);/,\n\t\"$COL_DATE_CREATED\"\t\t\tTIMESTAMP DEFAULT CURRENT_TIMESTAMP,\n\t\"$COL_DATE_MODIFIED\"\t\t\tTIMESTAMP DEFAULT CURRENT_TIMESTAMP\n);/g" "$filename_psql_table_create"
    sed -i "N;s/\n,/,/g;P;D" "$filename_psql_table_create"

    cat <<EOF >> "$filename_psql_table_create"

CREATE TRIGGER update_date_modified
  BEFORE UPDATE
  ON "$table"
  FOR EACH ROW
  EXECUTE FUNCTION public.update_date_modified();
EOF

    #
    # import data into PostgreSQL
    #
    readarray -t columns < <(grep -P '\t' "$filename_psql_table_create" | grep -v -e "$COL_DATE_CREATED" -e "$COL_DATE_MODIFIED" | cut -d'"' -f2)
    columns2=""
    for column in "${columns[@]}"
    do
      columns2="$columns2\"$column\","
    done
    columns2=${columns2:0:-1}

    cat <<EOF >> "$filename_psql_table_create"

\COPY "$table"($columns2) FROM '$filename_export' DELIMITER ',' CSV HEADER;
EOF

    PGPASSWORD=$PG_PASSWD $CMD_PSQL "$filename_psql_table_create"
  done
done

exit $?
