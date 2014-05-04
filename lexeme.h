#pragma once

#include <boost/variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <iosfwd>

namespace sap
{
struct lex
{
    enum class type : uint8_t
        { SYMBOL
        , NEWLINE
        , CONST
        , RESERVED
        , IDENTIFIER

        , COUNT
        }; // enum class type

    enum class symbol : uint8_t
        { L_PARENTHESIS         // (
        , R_PARENTHESIS         // )
        , L_BRACKET             // [
        , R_BRACKET             // ]
        , PLUS                  // +
        , MINUS                 // -
        , STAR                  // *
        , SLASH                 // /
        , EQUAL                 // =
        , LESS                  // <
        , GREATER               // >
        , COLON                 // :

        , COUNT
        }; // enum class symbol

    enum class reserved_word : uint8_t
        { SUB
        , ENDSUB
        , WHILE
        , ENDWHILE
        , IF
        , THEN
        , ELSE
        , ENDIF
        , WRITE
        , READ
        , GOTO
        , DIM

        , COUNT
        }; // enum class reserved_words

    type type_;

    typedef boost::variant< symbol, reserved_word, std::string, uint, bool > value;
    value value_;

    static const char* const type_STRINGS       [ static_cast< uint8_t >( lex::type::COUNT )            ];
    static const char* const symbol_STRINGS     [ static_cast< uint8_t >( lex::symbol::COUNT )          ];
    static const char* const reserved_STRINGS   [ static_cast< uint8_t >( lex::reserved_word::COUNT )   ];

}; // struct lex

std::ostream& operator<<( std::ostream& out, lex::type          rhs );
std::ostream& operator<<( std::ostream& out, lex::symbol        rhs );
std::ostream& operator<<( std::ostream& out, lex::reserved_word rhs );
std::ostream& operator<<( std::ostream& out, lex                rhs );
} // namespace sap
BOOST_FUSION_ADAPT_STRUCT(
    sap::lex,
    (sap::lex::type, type_)
    (sap::lex::value, value_)
)
