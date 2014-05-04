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
        , INT_CONST
        , BOOL_CONST
        , STR_CONST
        , RESERVED
        , IDENTIFIER

        , RULE

        , EPS
        , COUNT
        }; // enum class type

    enum class rule : uint8_t
        { START
        , ALL_STMT
        , ALL_STMTS
        , MORE_ALL_STMTS
        , SUB_STMT
        , STMTS
        , MORE_STMTS
        , LSTMT
        , STMT
        , LABEL_DEF
        , DIM_EXPR
        , ASSIGNMENT
        , ID
        , ARRAY_INDEX
        , OPERAND_INT
        , RIGHTSIDE
        , READ_STMT
        , INT_EXPR
        , OPERATOR_INT
        , LOGIC
        , LOGIC_INT
        , COMPARE_INT
        , LOGIC_EQUALS
        , LOGIC_BOOL
        , OPERAND_BOOL
        , LOGIC_STR
        , OPERAND_STR
        , IF_STMT
        , ELSE_PART
        , GOTO_STMT
        , WHILE_STMT
        , WRITE_STMT
        , WRITEABLE
        , SUB_CALL_STMT

        , COUNT
    }; // enum class rule

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

    typedef boost::variant< symbol, reserved_word, rule, std::string, uint, bool > value;
    value value_;

    static const char* const type_STRINGS       [ static_cast< uint8_t >( type::COUNT )         ];
    static const char* const rule_STRINGS       [ static_cast< uint8_t >( rule::COUNT )         ];
    static const char* const symbol_STRINGS     [ static_cast< uint8_t >( symbol::COUNT )       ];
    static const char* const reserved_STRINGS   [ static_cast< uint8_t >( reserved_word::COUNT )];

}; // struct lex

std::ostream& operator<<( std::ostream& out, lex::type          rhs );
std::ostream& operator<<( std::ostream& out, lex::symbol        rhs );
std::ostream& operator<<( std::ostream& out, lex::rule        rhs );
std::ostream& operator<<( std::ostream& out, lex::reserved_word rhs );
std::ostream& operator<<( std::ostream& out, const lex&           rhs );

bool operator==( const lex& lhs, const lex& rhs );
} // namespace sap
BOOST_FUSION_ADAPT_STRUCT(
    sap::lex,
    (sap::lex::type, type_)
    (sap::lex::value, value_)
)
