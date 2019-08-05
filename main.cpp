// https://sourceforge.net/p/mingw-w64/wiki2/Unicode%20apps/
#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef UNICODE
#define UNICODE
#endif

#include <boost/program_options.hpp>
#include <iostream>
#include <stdio.h>

#include "utilities.h"

namespace bpo = boost::program_options;

extern "C" int wmain(int argc, wchar_t** argv)
{
    try {
        initLocalization();

        // Running Examples under Microsoft Windows
        // https://www.boost.org/doc/libs/1_69_0/libs/locale/doc/html/running_examples_under_windows.html
        std::wcout << argc << ' ' << argv[0] << ' ' << (argc >= 2 ? argv[1] : L"") << std::endl;

        bpo::options_description hiddenOptions("Hidden options");
        hiddenOptions.add_options() //
            ("file-name", bpo::wvalue<std::wstring>(), "file name") //
            ("table-name", bpo::wvalue<std::wstring>()->default_value(L"", ""), "table name");

        bpo::options_description visibleOptions("Allowed options");
        visibleOptions.add_options() //
            ("help,?", "Print usage information and exit.") //
            ("separator,S", bpo::wvalue<std::wstring>()->default_value(L",", ","), "Field separator character.") //
            ("quote,Q", bpo::wvalue<std::wstring>()->default_value(L"\"", "\""), "String quote character.") //
            ("port,P", bpo::value<int>()->default_value(50000), "Port number of the server.") //
            ("uid,U", bpo::value<std::string>()->default_value("monetdb"), "User to connect as.") //
            ("pwd", bpo::value<std::string>()->default_value("monetdb"), "Password.") //
            ("print-sql", "Scan the file and print the generated SQL commands before executing them.") //
            ("dry-run,D", "Scan the file and print the generated SQL commands but do not execute them.");

        bpo::options_description commandLineOptions;
        commandLineOptions.add(visibleOptions).add(hiddenOptions);

        bpo::positional_options_description positionalOptions;
        positionalOptions.add("file-name", 1).add("table-name", 2);

        bpo::variables_map variablesMap;
        store(bpo::wcommand_line_parser(argc, argv).options(commandLineOptions).positional(positionalOptions).run(), variablesMap);
        notify(variablesMap);

        if (variablesMap.count("help") || (!variablesMap.count("file-name"))) {
            std::cout << "Usage: BulkLoadCsv [OPTION]... FILE [TABLE]\n\n"
                         "Bulk load a delimited text file FILE into a table TABLE in a MonetDB database.\n\n"
                         "Scan the delimited text file FILE and detect data types, lengths and NULL (NOT NULL)\n"
                         "constraints of the columns. Then generate and execute DROP TABLE, CREATE TABLE and \n"
                         "COPY INTO SQL commands. The first line of the file FILE must contain names of the columns.\n\n";
            std::cout << visibleOptions << std::endl;
            return 0;
        }

        if (variablesMap.count("file-name")) {
            std::wcout << L"Scanning " << variablesMap["file-name"].as<std::wstring>() << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
