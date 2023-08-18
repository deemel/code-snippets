Tool to automate exporting of MS Access tables and data into PostgreSQL.  Access table schemas are used to create equivalent PostgreSQL table schemas.  2 new columns (date_created, date_modified) are added to PostgreSQL tables for change tracking purposes.

Current data type conversions are as follow.

| Access Data Type     | PostgreSQL Data Type |
|----------------------|----------------------|
| Boolean              | BOOLEAN              |
| Long Integer         | INT                  |
| Integer              | SMALLINT             |
| Single               | REAL                 |
| Double               | DOUBLE PRECISION     |
| Currency             | REAL                 |
| DateTime             | TIMESTAMP            |
| Text                 | VARCHAR              |
| Memo/Hyperlink (255) | VARCHAR (8192)       |

Access table data are saved as CSV files and are bulk loaded into PostgreSQL tables via COPY command.
