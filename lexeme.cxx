#include "lexeme.h"

#include <iostream>

namespace sap
{
const char* const lex::type_STRINGS[] = { "SYMBOL"
                                        , "NEWLINE"
                                        , "CONST"
                                        , "RESERVED"
                                        , "IDENTIFIER"
                                        }; // type_STRINGS
const char* const lex::symbol_STRINGS[] = { "."
                                          , "("
                                          , ")"
                                          , "]"
                                          , "["
                                          , "+"
                                          , "-"
                                          , "*"
                                          , "/"
                                          , "="
                                          , "<"
                                          , ">"
                                          , ":"
                                          }; // symbol_STRINGS
const char* const lex::reserved_STRINGS[] = { "Sub"
                                            , "EndSub"
                                            , "While"
                                            , "EndWhile"
                                            , "If"
                                            , "Then"
                                            , "Else"
                                            , "EndIf"
                                            , "TextWindow.Write"
                                            , "TextWindow.Read"
                                            , "Goto"
                                            , "Dim"
                                            }; // reserved_STRINGS

std::ostream& operator<<( std::ostream& out, lex::type rhs )
{
    out << lex::type_STRINGS[ static_cast< uint8_t >( rhs ) ];
    return out;
}

std::ostream& operator<<( std::ostream& out, lex::symbol rhs )
{
    out << lex::symbol_STRINGS[ static_cast< uint8_t >( rhs ) ];
    return out;
}

std::ostream& operator<<( std::ostream& out, lex::reserved_word rhs )
{
    out << lex::reserved_STRINGS[ static_cast< uint8_t >( rhs ) ];
    return out;
}

std::ostream& operator<<( std::ostream& out, lex rhs )
{
    out << "["
        << "type: '"
        << rhs.type_
        << '\''
        << ", value: '"
        << rhs.value_
        << '\''
        << "]";
    return out;
}
} // namespace sap