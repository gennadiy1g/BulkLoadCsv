# BulkLoadCsv

A command line program to bulk load a delimited text file into a new table in a MonetDB database as easy as possible.

## Why this program is useful

This program is useful if:

 * Delimited text files are provided without corresponding SQL scripts to import them.
 * Data types, lengths, precisions, scales and NULL constraints of all columns are not known and are not trivial to get.
 * It is not desirable to import all columns as [TEXT](https://www.monetdb.org/Documentation/Manuals/SQLreference/BuiltinTypes) NULL.

 BulkLoadCsv will:
 1. Scan the given delimited text file and detect data types, lengths, precisions, scales and NULL constraints of all columns. 
 1. Generate and execute DROP TABLE, CREATE TABLE and COPY INTO commands.

## Limitations

* At this time, BulkLoadCsv is available only for 64-bit Windows.
* Data types from the following subset of [built-in SQL types](https://www.monetdb.org/Documentation/Manuals/SQLreference/BuiltinTypes) are detected: 
FLOAT, DECIMAL, TINYINT, SMALLINT, INT, BIGINT, BOOLEAN, CHAR, VARCHAR.
* Data types from the following subset of [temporal types](https://www.monetdb.org/Documentation/SQLreference/Temporal) are detected:
TIMESTAMP (formatted as YYYY-MM-DD HH.MI.SS.sss), DATE (formatted as YYYY-MM-DD), TIME (formatted as HH.MI.SS.sss).

## Prerequisites

* If MonetDB server is running on localhost, [MonetDB ODBC Driver](https://www.monetdb.org/downloads/Windows/Apr2019-SP1/MonetDB-ODBC-Installer-x86_64-20190830.msi) must be installed.
* If MonetDB server is running on remote computer, [mclient](https://www.monetdb.org/Documentation/mclient-man-page) must be installed on the localhost in the default directory ("C:\Program Files\MonetDB\MonetDB5\mclient.bat"), and MonetDB on both computers must be [Apr2019 release](https://www.monetdb.org/downloads/Windows/Apr2019-SP1) (11.33.3) or later.
* The first line of the delimited text file must contain names of the columns.

## Getting started

Usage: BulkLoadCsv [OPTION]... FILE [NEW_TABLE]

Bulk load a delimited text file FILE into a new table NEW_TABLE in a MonetDB database.
The first line of the file must contain names of the columns.

Scan the FILE and detect data types, lengths, precisions, scales and NULL constraints of all columns.
Then generate and execute DROP TABLE IF EXISTS NEW_TABLE, CREATE TABLE NEW_TABLE and
COPY INTO NEW_TABLE commands.

Allowed options:

Short [long] (=default)|Comment
-|-
-? [ --help ] | Print usage information and exit.
-S [ --separator ] arg (=,) | Field separator character.
--separator_unicode arg | Field separator character, Unicode code point in decimal or hexadecimal form.
-Q [ --quote ] arg (=") | String quote character.
--quote_unicode arg | String quote character, Unicode code point in decimal or hexadecimal form.
-H [ --host ] arg (=localhost) | Name of the host on which the server runs (requires Apr2019 release (11.33.3) or later of MonetDB).
-P [ --port ] arg (=50000) | Port number of the server.
-U [ --uid ] arg (=monetdb) | User to connect as.
--pwd arg (=monetdb) | Password.
--print-sql | Scan the file and print the generated SQL commands before executing them.
-D [ --dry-run ] | Scan the file and print the generated SQL commands but do not execute them.

If the given file has been successfully imported, exit status is set to 0, otherwise exit status is set to 1.

## Examples

* Import specified comma separated file into MonetDB database on server running on localhost and listening on port 50000:

    `BulkLoadCsv.exe "C:\Users\FooUser\Documents\BulkLoadCsv\test data\parsing_results_1.csv"`

* Same as above, but explicitly specify field separator character:

    `BulkLoadCsv.exe  --separator "," "C:\Users\FooUser\Documents\BulkLoadCsv\test data\parsing_results_1.csv"`

* Same as above, but specify field separator character as Unicode code point in decimal form (see [List of Unicode characters](https://en.wikipedia.org/wiki/List_of_Unicode_characters#Basic_Latin) for more on  Unicode code points):

    `BulkLoadCsv.exe --separator_unicode 44 "C:\Users\FooUser\Documents\BulkLoadCsv\test data\parsing_results_1.csv"`

* Same as above, but specify field separator character as Unicode code point in hexadecimal form:

    `BulkLoadCsv.exe --separator_unicode 0x2C "C:\Users\FooUser\Documents\BulkLoadCsv\test data\parsing_results_1.csv"`

## Credit and licenses for embedded code

This project includes code from outside sources:

* [Boost](https://www.boost.org/LICENSE_1_0.txt)

* [Nanodbc](https://nanodbc.github.io/nanodbc/doxygen/index.html#license)