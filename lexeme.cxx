#include "lexeme.h"

#include <iostream>
#include <functional>

namespace sap
{
const char* const lex::type_STRINGS[] = { "SYMBOL"
                                        , "NEWLINE"
                                        , "INT CONST"
                                        , "BOOL CONST"
                                        , "STR CONST"
                                        , "RESERVED"
                                        , "IDENTIFIER"

                                        , "RULE"
                                        , "EPSILON"
                                        }; // type_STRINGS
const char* const lex::symbol_STRINGS[] = { "("
                                          , ")"
                                          , "["
                                          , "]"
                                          , "+"
                                          , "-"
                                          , "*"
                                          , "/"
                                          , "="
                                          , "<>"
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
const char* const lex::rule_STRINGS[] = { "<start>"
                                        , "<all-stmt>"
                                        , "<all-stmts>"
                                        , "<more-all-stmts"
                                        , "<sub-stmt>"
                                        , "<stmts>"
                                        , "<more-stmts>"
                                        , "<lstmt>"
                                        , "<stmt>"
                                        , "<label-def>"
                                        , "<dim-expr>"
                                        , "<assignment>"
                                        , "<id>"
                                        , "<array-index>"
                                        , "<operand-int>"
                                        , "<rightside>"
                                        , "<read-stmt>"
                                        , "<int-expr>"
                                        , "<operator-int>"
                                        , "<logic>"
                                        , "<logic-int>"
                                        , "<compare-int>"
                                        , "<logic-equals>"
                                        , "<logic-bool>"
                                        , "<operand-bool>"
                                        , "<logic-str>"
                                        , "<operand-str>"
                                        , "<if-stmt>"
                                        , "<else-part>"
                                        , "<goto-stmt>"
                                        , "<while-stmt>"
                                        , "<write-stmt>"
                                        , "<writeable>"
                                        , "<sub-call-stmt>"
                                        , "<new-identifier>"
                                        }; // rule_STRINGS
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

std::ostream& operator<<( std::ostream& out, lex::rule rhs )
{
    out << lex::rule_STRINGS[ static_cast< uint8_t >( rhs ) ];
    return out;
}

std::ostream& operator<<( std::ostream& out, lex::reserved_word rhs )
{
    out << lex::reserved_STRINGS[ static_cast< uint8_t >( rhs ) ];
    return out;
}

std::ostream& operator<<( std::ostream& out, const lex& rhs )
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
