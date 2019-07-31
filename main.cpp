#include <boost/program_options.hpp>
#include <iostream>
#include <stdio.h>

#include "utilities.h"

namespace bpo = boost::program_options;

int main(int argc, char** argv)
{
    try {
        initLocalization();

        bpo::options_description hiddenOptions("Hidden options");
        hiddenOptions.add_options() //
            ("file-name", bpo::wvalue<std::string>(), "file name") //
            ("table-name", bpo::value<std::string>()->default_value(""), "table name");

        bpo::options_description visibleOptions("Allowed options");
        visibleOptions.add_options() //
            ("help,?", "Print usage information and exit.") //
            ("separator,S", bpo::wvalue<std::string>()->default_value(","), "Field separator character.") //
            ("quote,Q", bpo::wvalue<std::string>()->default_value("\""), "String quote character.") //
            ("port,P", bpo::value<int>()->default_value(50000), "Port number of the server.") //
            ("uid,U", bpo::value<std::string>()->default_value("monetdb"), "User to connect as.") //
            ("pwd", bpo::value<std::string>()->default_value("monetdb"), "Password.") //
            ("print-sql", "Scan the file and print the generated SQL commands before executing them.") //
            ("dry-run,D", "Scan the file and print the generated SQL commands but do not execute them.");

        bpo::options_description commandLineOptions;
        commandLineOptions.add(visibleOptions).add(hiddenOptions);

        bpo::positional_options_description positionalOptions;
        positionalOptions.add("file-name", 0).add("table-name", 1);

        bpo::variables_map variablesMap;
        store(bpo::command_line_parser(argc, argv).options(commandLineOptions).positional(positionalOptions).run(), variablesMap);
        notify(variablesMap);

        if (variablesMap.count("help")) {
            std::cout << "Usage: BulkLoadCsv [OPTION]... FILE [TABLE]\n\n"
                         "Bulk load a delimited text file FILE into a table TABLE in a MonetDB database.\n\n"
                         "Scan the delimited text file FILE and detect data types, lengths and NULL (NOT NULL)\n"
                         "constraints of the columns. Then generate and execute DROP TABLE, CREATE TABLE and \n"
                         "COPY INTO SQL commands. The first line of the file FILE must contain names of the columns.\n\n";
            std::cout << visibleOptions << std::endl;
            return 0;
        }
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
