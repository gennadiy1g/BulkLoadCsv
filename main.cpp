#include <boost/program_options.hpp>
#include <iostream>
#include <stdio.h>

namespace bpo = boost::program_options;

int main(int argc, char** argv)
{
    try {
        bpo::options_description hiddenOptions("Hidden options");
        hiddenOptions.add_options()("input-file", bpo::value<std::string>(), "input file")("output-table", bpo::value<std::string>(), "input file");

        bpo::options_description visibleOptions("Allowed options");
        visibleOptions.add_options()("help,?", "print usage information and exit")("separator,S", "field separator character")("quote,Q", "string quote character")("driver,D", "ODBC driver name (default MonetDB ODBC Driver)")("host,H", "name of the host on which the server runs (default: localhost)")("port,P", "portnumber of the server (default: 50000)")("uid,U", "user to connect as (default monetdb)")("pwd,P", "user's password (default monetdb)");

        bpo::options_description commandLineOptions;
        commandLineOptions.add(visibleOptions).add(hiddenOptions);

        bpo::positional_options_description positionalOptions;
        positionalOptions.add("input-file", -1);

        bpo::variables_map variablesMap;
        store(bpo::command_line_parser(argc, argv).options(commandLineOptions).positional(positionalOptions).run(), variablesMap);
        notify(variablesMap);

        if (variablesMap.count("help")) {
            std::cout << visibleOptions << "\n";
            return 0;
        }
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
