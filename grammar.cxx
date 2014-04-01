#include "grammar.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include "lexeme.h"

#include <stdexcept>
#include <sstream>
namespace sap
{
namespace qi = boost::spirit::qi;
using lines = std::deque< std::string >;

struct LinesGrammar : qi::grammar< Iterator, lines() >
{
    LinesGrammar() : base_type( start_ )
    {
        using namespace qi::labels;
        using qi::eol;
        using qi::ascii::char_;

        line_  %= *(char_ - eol) >> eol;
        start_ %= *line_;

    }

    qi::rule< Iterator, std::string() > line_;
    qi::rule< Iterator, lines() >       start_;
} linesGrammar;

struct symbols_ : qi::symbols< char, lex::symbol >
{
    symbols_()
    {
        using symbol = lex::symbol;
        // assert enumerations are continious.
        // because i've created them
        constexpr uint8_t MAX{ static_cast< uint8_t >( symbol::COUNT ) };
        for ( uint8_t s = 0; s != MAX; ++s)
            add( lex::symbol_STRINGS[ s ], static_cast< symbol >( s ) );
    }
} symbols;

struct reserved_words_ : qi::symbols< char, lex::reserved_word >
{
    reserved_words_()
    {
        using rword = lex::reserved_word;
        constexpr uint8_t MAX{ static_cast< uint8_t >( rword::COUNT ) };
        for ( uint8_t s = 0; s != MAX; ++s )
            add( lex::reserved_STRINGS[ s ], static_cast< rword >( s ) );
    }
} reserved_words;

struct MyGrammar : qi::grammar< Iterator, Lexems(), qi::ascii::space_type >
{
    qi::rule< Iterator, lex::type()  , qi::ascii::space_type > symbol_type;
    qi::rule< Iterator, lex::symbol(), qi::ascii::space_type > symbol;
    qi::rule< Iterator, lex()        , qi::ascii::space_type > symbol_rule;

    qi::rule< Iterator, lex::type()         , qi::ascii::space_type > reserved_type;
    qi::rule< Iterator, lex::reserved_word(), qi::ascii::space_type > reserved;
    qi::rule< Iterator, lex()               , qi::ascii::space_type > reserved_rule;

    qi::rule< Iterator, lex::type(), qi::ascii::space_type > identifier_type;
    qi::rule< Iterator, std::string()                      > identifier;
    qi::rule< Iterator, lex()      , qi::ascii::space_type > identifier_rule;

    qi::rule< Iterator, lex::type(), qi::ascii::space_type > constant_type;
    qi::rule< Iterator, std::string()                      > string_constant;
    qi::rule< Iterator, double()   , qi::ascii::space_type > double_constant;
    qi::rule< Iterator, bool()     , qi::ascii::space_type > bool_constant;
    qi::rule< Iterator, lex()      , qi::ascii::space_type > constant_rule;

    qi::rule< Iterator, lex(), qi::ascii::space_type > visible_lexeme;

    qi::rule< Iterator, void()  , qi::ascii::space_type > empty_line;
    qi::rule< Iterator, Lexems(), qi::ascii::space_type > non_empty_line;

    qi::rule< Iterator, Lexems(), qi::ascii::space_type > start;

    MyGrammar( ) : base_type( start )
    {
        using qi::ascii::char_;
        using qi::ascii::alnum;
        using qi::ascii::alpha;
        using qi::labels::_val;
        using qi::labels::_1;
        using qi::lit;
        using qi::eps;
        using qi::eol;
        using qi::double_;

        // dummy, sets lex.type_ field
        symbol_type  = eps          [ _val = lex::type::SYMBOL ];
        // match symbols against symbol-table
        symbol      %= symbols;
        symbol_rule %= symbol_type
                    >> symbol;

        // dummy, sets lex.type_ field
        reserved_type    = eps              [ _val = lex::type::RESERVED ];
        // match against symbol table
        reserved        %= reserved_words;
        reserved_rule   %= reserved_type
                        >> reserved;

        // dummy, sets lex.type_ field
        identifier_type  = eps                  [ _val = lex::type::IDENTIFIER ];
        identifier      %= char_("_a-zA-Z")
                        >> *char_("a-zA-Z1-9_");
        identifier_rule  = identifier_type
                        >> identifier;

        constant_type    = eps                      [ _val = lex::type::CONST ];
        double_constant %= double_;
        // bool constant is either True or False
        bool_constant    = lit("\"true\"")              [ _val = true ]
                         | lit("\"false\"")             [ _val = false ];
        // string constant is any number of characters between " "
        string_constant %= lit("\"")
                        >> *(char_ - char_('"'))
                        >> lit("\"");
        // constant is either double, or bool, or string constant
        constant_rule   %= constant_type
                        >> ( double_constant
                           | bool_constant
                           | string_constant
                           );

        // matches all lexemes, that a visible (not newline )
        // it's either reserved word, or constant, or identifier, or symbol
        visible_lexeme  %= reserved_rule
                        |  constant_rule
                        |  identifier_rule
                        |  symbol_rule
                        ;

        // Matches anything, but doesn't set anything (it's type is void)
        empty_line       = eps;
        // Matches typical line (one or more lexems )
        non_empty_line  %= +visible_lexeme;   // some visible_lexeme

        start   = non_empty_line    [ _val = _1 ]
                | empty_line;
    }
} myGrammar; // class MyGrammar

Lexems parse( const Container input )
{
    Lexems result;
    Iterator beg{ std::begin( input ) };
    Iterator end{ std::end( input ) };

    lines ls;
    if ( (! boost::spirit::qi::parse( beg, end, linesGrammar, ls ) )
      || (0 != std::distance( beg, end ) ) ) {
        throw std::logic_error{ "Lines aren't properly terminated." };
    }

    uint i{ 0 };
    for (const auto& line : ls ) {
        ++i;
        Lexems tempResult;
        beg = std::begin( line );
        end = std::end( line );
        if ( (!qi::phrase_parse( beg, end, myGrammar, qi::ascii::space, tempResult ) )
          || (0 != std::distance( beg, end ) ) ) {
            std::stringstream ss;
            uint column = std::distance( std::begin( line ), beg);
            ss << "Cannot parse at position (" << i << ", " << column + 1 << "):\n"
               << line << '\n';
            while (0 != column-- )
                ss << ' ';
            ss << "^\n";
            throw std::invalid_argument{ ss.str() };
        }
        for (const auto& lexem : tempResult ) {
            result.push_back( lexem );
        }

        lex newLine;
        newLine.type_ = lex::type::NEWLINE;
        newLine.value_ = i;
        result.push_back( newLine );
    }

    return std::move( result );
}
} // namespace sap
