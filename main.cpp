// https://sourceforge.net/p/mingw-w64/wiki2/Unicode%20apps/
#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef UNICODE
#define UNICODE
#endif

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/locale.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <memory>

#include "MonetDBBulkLoader.h"
#include "log.h"
#include "utilities.h"

namespace bpo = boost::program_options;

using namespace std::literals;

// Function used to check that 'opt1' and 'opt2' are not specified at the same time.
void conflictingOptions(const bpo::variables_map& variablesMap,
    const char* opt1, const char* opt2)
{
    if (variablesMap.count(opt1) && !variablesMap[opt1].defaulted()
        && variablesMap.count(opt2) && !variablesMap[opt2].defaulted())
        throw std::logic_error("Conflicting options '"s + opt1 + "' and '"s + opt2 + "'."s);
}

extern "C" int wmain(int argc, wchar_t** argv)
{
    bool loggingInitialized { false };

    try {
        initLocalization();
        initLogging();
        loggingInitialized = true;
        ColumnInfo::initializeLocales();

        bpo::options_description hiddenOptions("Hidden options");
        hiddenOptions.add_options() //
            ("file-name", bpo::wvalue<std::wstring>(), "file name") //
            ("table-name", bpo::wvalue<std::wstring>()->default_value(L"", ""), "table name");

        bpo::options_description visibleOptions("Allowed options");
        visibleOptions.add_options() //
            ("help,?", "Print usage information and exit.") //
            ("separator,S", bpo::wvalue<wchar_t>()->default_value(L',', ","), "Field separator character.") //
            ("separator_unicode", bpo::wvalue<std::wstring>(), "Field separator character, Unicode code point.") //
            ("quote,Q", bpo::wvalue<wchar_t>()->default_value(L'"', "\""), "String quote character.") //
            ("quote_unicode", bpo::wvalue<std::wstring>(), "String quote character, Unicode code point.") //
            ("host,H", bpo::wvalue<std::wstring>()->default_value(L"localhost", "localhost"), "Name of the host on which the server runs (requires Apr2019 release (11.33.3) of MonetDB).") //
            ("port,P", bpo::value<int>()->default_value(50000), "Port number of the server.") //
            ("uid,U", bpo::wvalue<std::wstring>()->default_value(L"monetdb", "monetdb"), "User to connect as.") //
            ("pwd", bpo::wvalue<std::wstring>()->default_value(L"monetdb", "monetdb"), "Password.") //
            ("print-sql", "Scan the file and print the generated SQL commands before executing them.") //
            ("dry-run,D", "Scan the file and print the generated SQL commands but do not execute them.");

        bpo::options_description commandLineOptions;
        commandLineOptions.add(visibleOptions).add(hiddenOptions);

        bpo::positional_options_description positionalOptions;
        positionalOptions.add("file-name", 1).add("table-name", 1);

        bpo::variables_map variablesMap;
        store(bpo::wcommand_line_parser(argc, argv).options(commandLineOptions).positional(positionalOptions).run(), variablesMap);
        notify(variablesMap);

        if (variablesMap.count("help") || (!variablesMap.count("file-name"))) {
            std::cout << "Usage: BulkLoadCsv [OPTION]... FILE [NEW_TABLE]\n\n"
                         "Bulk load a delimited text file FILE into a new table NEW_TABLE in a MonetDB database.\n"
                         "The first line of the file must contain names of the columns.\n\n"
                         "Scan the FILE and detect data types, lengths and NULL (NOT NULL) constraints of its columns.\n"
                         "Then generate and execute DROP TABLE IF EXISTS NEW_TABLE, CREATE TABLE NEW_TABLE and \n"
                         "COPY INTO NEW_TABLE SQL commands.\n\n";
            std::cout << visibleOptions << std::endl;
            return EXIT_SUCCESS;
        } else {

            conflictingOptions(variablesMap, "separator", "separator_unicode");
            conflictingOptions(variablesMap, "quote", "quote_unicode");

            auto factoryLambda = [&variablesMap]() -> std::unique_ptr<BulkLoader> const {
                auto host = boost::locale::conv::utf_to_utf<char>(variablesMap["host"].as<std::wstring>());
                boost::trim(host);
                if (variablesMap.count("host") && !variablesMap["host"].defaulted()
                    && !boost::is_iequal()(host, boost::asio::ip::host_name()) && (host != "127.0.0.1"s) && !boost::is_iequal()(host, "localhost"s)) {
                    return std::make_unique<MclientMonetDBBulkLoader>(variablesMap["file-name"].as<std::wstring>());
                } else {
                    return std::make_unique<NanodbcMonetDBBulkLoader>(variablesMap["file-name"].as<std::wstring>());
                }
            };

            auto bulkLoader = factoryLambda();

            std::vector<ConnectionParameter> connectionParameters;
            if (variablesMap.count("port") && !variablesMap["port"].defaulted()) {
                connectionParameters.push_back(std::make_pair(ConnectionParameterName::Port, std::to_wstring(variablesMap["port"].as<int>())));
            }
            if (variablesMap.count("uid") && !variablesMap["uid"].defaulted()) {
                connectionParameters.push_back(std::make_pair(ConnectionParameterName::User, variablesMap["uid"].as<std::wstring>()));
            }
            if (variablesMap.count("pwd") && !variablesMap["pwd"].defaulted()) {
                connectionParameters.push_back(std::make_pair(ConnectionParameterName::Password, variablesMap["pwd"].as<std::wstring>()));
            }
            if (connectionParameters.size()) {
                bulkLoader->setConnectionParameters(connectionParameters);
            }

            wchar_t separator, quote;
            if (variablesMap.count("separator_unicode")) {
                separator = std::stol(variablesMap["separator_unicode"].as<std::wstring>(), nullptr, 0);
            } else {
                separator = variablesMap["separator"].as<wchar_t>();
            }
            if (variablesMap.count("quote_unicode")) {
                quote = std::stol(variablesMap["quote_unicode"].as<std::wstring>(), nullptr, 0);
            } else {
                quote = variablesMap["quote"].as<wchar_t>();
            }
            if ((separator == L'\\') || (quote == L'\\')) {
                throw std::invalid_argument("Backslash character ('\\' or 0x5c in ASCII encoding) is a reserved character, and cannot be used as a field_separator or a string_quote!");
            }
            if (separator == quote) {
                throw std::invalid_argument("The same character cannot be used as both: a field_separator and a string_quote!");
            }

            // Running Examples under Microsoft Windows
            // https://www.boost.org/doc/libs/1_69_0/libs/locale/doc/html/running_examples_under_windows.html
            std::wcout << "Scanning " << variablesMap["file-name"].as<std::wstring>() << std::endl;
            bulkLoader->parse(separator, quote);
            std::cout << bulkLoader->parsingResults().numLines() << " lines, "
                      << bulkLoader->parsingResults().numMalformedLines() << " malformed lines, "
                      << bulkLoader->parsingResults().columns().size() << " columns" << std::endl;

            if (variablesMap.count("dry-run") || variablesMap.count("print-sql")) {
                auto tableName { bulkLoader->getTableName(variablesMap["table-name"].as<std::wstring>()) };
                auto dropCommand { bulkLoader->generateDropTableCommand(tableName) };
                std::wcout << dropCommand << ";\n";
                auto createCommand { bulkLoader->generateCreateTableCommand(tableName) };
                std::wcout << createCommand << ";\n";
                auto copyCommand { bulkLoader->generateCopyIntoCommand(tableName) };
                std::wcout << copyCommand << ";" << std::endl;
            }
            if (!variablesMap.count("dry-run")) {
                auto rejectedRecords = bulkLoader->load(variablesMap["table-name"].as<std::wstring>());
                if (rejectedRecords.value_or(0) > 0) {
                    std::cout << "Sever rejected " << rejectedRecords.value() << " records" << std::endl;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;

        if (loggingInitialized) {
            auto& gLogger = GlobalLogger::get();
            BOOST_LOG_SEV(gLogger, bltrivial::fatal) << e.what();
        }

        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
