// #include <QApplication>
//
// #include "main_window.h"
//
#include "lexeme.h"
#include "grammar.h"
#include "syntax.h"

#include <fstream>
int main( int /* argc */, char* /* argv */[] )
{
    // QApplication app{ argc, argv };
    // sap::MainWindow window;
    // window.show();
    // return app.exec();

    using namespace sap;
    std::ifstream in{ "../test.ba" };
    in.unsetf( std::ios::skipws );
    const std::string str{ std::istreambuf_iterator< char >{ in }
                         , std::istreambuf_iterator< char >{} };
        Lexems lexems = sap::parse( str );

        const Table table = createTable();

        Stack ss;
        ss.push( { sap::lex::type::COUNT, false } );
        ss.push( { sap::lex::type::RULE, sap::lex::rule::START } );
        Queue result = parse( table, lexems.begin(), lexems.end(), std::move( ss ) );

        while (!result.empty()) {
            std::cout << result.front() << " → ";
            result.pop();
        }
        std::cout << std::endl;

        return 0;
}
