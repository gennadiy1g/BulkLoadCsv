#include <boost/program_options.hpp>
#include <iostream>
#include <stdio.h>

namespace bpo = boost::program_options;

int main(int argc, char** argv)
{
    try {
        bpo::options_description hiddenOptions("Hidden options");
        hiddenOptions.add_options()("file-name", bpo::value<std::string>(), "file name")("table-name", bpo::value<std::string>(), "table name");

        bpo::options_description visibleOptions("Allowed options");
        visibleOptions.add_options()("help,?", "Print usage information and exit.");
        visibleOptions.add_options()("separator,S", "Field separator character.")("quote,Q", "String quote character.");
        visibleOptions.add_options()("driver,D", "ODBC driver name (default MonetDB ODBC Driver)");
        visibleOptions.add_options()("host,H", "name of the host on which the server runs (default: localhost)")("port,P", "portnumber of the server (default: 50000)");
        visibleOptions.add_options()("uid,U", "user to connect as (default monetdb)")("pwd,P", "user's password (default monetdb)");

        bpo::options_description commandLineOptions;
        commandLineOptions.add(visibleOptions).add(hiddenOptions);

        bpo::positional_options_description positionalOptions;
        positionalOptions.add("file-name", 0).add("table-name", 1);

        bpo::variables_map variablesMap;
        store(bpo::command_line_parser(argc, argv).options(commandLineOptions).positional(positionalOptions).run(), variablesMap);
        notify(variablesMap);

        if (variablesMap.count("help")) {
            std::cout << visibleOptions << std::endl;
            return 0;
        }
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
