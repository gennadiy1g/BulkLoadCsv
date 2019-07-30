#include <boost/program_options.hpp>
#include <iostream>
#include <stdio.h>

namespace bpo = boost::program_options;

int main(int argc, char** argv)
{
    try {
        bpo::options_description hiddenOptions("Hidden options");
        hiddenOptions.add_options() //
            ("file-name", bpo::value<std::string>(), "file name") //
            ("table-name", bpo::value<std::string>()->default_value(""), "table name");

        bpo::options_description visibleOptions("Allowed options");
        visibleOptions.add_options() //
            ("help,?", "Print usage information and exit.") //
            ("separator,S", bpo::value<char>()->default_value(','), "Field separator character.") //
            ("quote,Q", bpo::value<char>()->default_value('"'), "String quote character.") //
            ("driver,D", bpo::value<std::string>()->default_value("MonetDB ODBC Driver"), "ODBC driver name.") //
            ("host,H", bpo::value<std::string>()->default_value("localhost"), "Name of the host on which the server runs.") //
            ("port,P", bpo::value<int>()->default_value(50000), "Port number of the server.") //
            ("uid,U", bpo::value<std::string>()->default_value("monetdb"), "User to connect as.") //
            ("pwd,P", bpo::value<std::string>()->default_value("monetdb"), "Password.");

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
