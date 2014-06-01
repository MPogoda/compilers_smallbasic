#include "syntax.h"

#ifdef NDEBUG
    #define DEBUG(a)
    #define DBG(a)
#else
    #include <iostream>
    #define DEBUG(a) std::cout << #a << " = " << (a) << '\n'
    #define DBG(a) std::cout << __LINE__ << " : " << a << '\n';
#endif
namespace sap
{
void push_rule( Stack& st, const int rule_number )
{
    DEBUG( rule_number );
    switch (rule_number) {
        case 1: st.push( { lex::type::RULE, lex::rule::ALL_STMTS } ); break;
        case 2: st.push( { lex::type::RULE, lex::rule::SUB_STMT } ); break;
        case 3: st.push( { lex::type::RULE, lex::rule::LSTMT } ); break;
        case 4: st.push( { lex::type::RULE, lex::rule::MORE_ALL_STMTS } );
                st.push( { lex::type::RULE, lex::rule::ALL_STMT } );
                break;
        case 5: st.push( { lex::type::RULE, lex::rule::ALL_STMTS } ); break;
        case 6: st.push( { lex::type::EPS, false } ); break;
        case 7: st.push( { lex::type::NEWLINE, 0u } );
                st.push( { lex::type::RESERVED, lex::reserved_word::ENDSUB } );
                st.push( { lex::type::RULE, lex::rule::STMTS } );
                st.push( { lex::type::NEWLINE, 0u } );
                st.push( { lex::type::RULE, lex::rule::NEW_IDENTIFIER } );
                st.push( { lex::type::RESERVED, lex::reserved_word::SUB } );
                break;
        case 8: st.push( { lex::type::RULE, lex::rule::MORE_STMTS } );
                st.push( { lex::type::RULE, lex::rule::LSTMT } );
                break;
        case 9: st.push( { lex::type::RULE, lex::rule::STMTS } ); break;
        case 10: st.push( { lex::type::EPS, false } ); break;
        case 11: st.push( { lex::type::RULE, lex::rule::STMT } );
                 st.push( { lex::type::RULE, lex::rule::LABEL_DEF } );
                 break;
        case 12: st.push( { lex::type::NEWLINE, 0u } );
                 st.push( { lex::type::RULE, lex::rule::LABEL_DEF } );
                 break;
        case 13: st.push( { lex::type::RULE, lex::rule::DIM_EXPR } ); break;
        case 14: st.push( { lex::type::RULE, lex::rule::ASSIGNMENT } ); break;
        case 15: st.push( { lex::type::RULE, lex::rule::IF_STMT } ); break;
        case 16: st.push( { lex::type::RULE, lex::rule::GOTO_STMT } ); break;
        case 17: st.push( { lex::type::RULE, lex::rule::WHILE_STMT } ); break;
        case 18: st.push( { lex::type::RULE, lex::rule::WRITE_STMT } ); break;
        case 19: st.push( { lex::type::RULE, lex::rule::SUB_CALL_STMT } ); break;
        case 21: st.push( { lex::type::SYMBOL, lex::symbol::COLON } );
                 st.push( { lex::type::INT_CONST, 0u } );
                 break;
        case 22: st.push( { lex::type::EPS, false } ); break;
        case 23: st.push( { lex::type::NEWLINE, 0u } );
                 st.push( { lex::type::IDENTIFIER, "" } );
                 st.push( { lex::type::RESERVED, lex::reserved_word::DIM } );
                 break;
        case 24: st.push( { lex::type::NEWLINE, 0u } );
                 st.push( { lex::type::RULE, lex::rule::RIGHTSIDE } );
                 st.push( { lex::type::SYMBOL, lex::symbol::EQUAL } );
                 st.push( { lex::type::RULE, lex::rule::ID } );
                 break;
        case 25: st.push( { lex::type::RULE, lex::rule::ARRAY_INDEX } );
                 st.push( { lex::type::RULE, lex::rule::NEW_IDENTIFIER } );
                 break;
        case 26: st.push( { lex::type::IDENTIFIER, "" } ); break;
        case 27: st.push( { lex::type::SYMBOL, lex::symbol::R_BRACKET } );
                 st.push( { lex::type::RULE, lex::rule::OPERAND_INT } );
                 st.push( { lex::type::SYMBOL, lex::symbol::L_BRACKET } );
                 break;
        case 28: st.push( { lex::type::RULE, lex::rule::ID } ); break;
        case 29: st.push( { lex::type::INT_CONST, 0u } ); break;
        case 30: st.push( { lex::type::RULE, lex::rule::READ_STMT } ); break;
        case 31: st.push( { lex::type::RULE, lex::rule::INT_EXPR } ); break;
        case 32: st.push( { lex::type::RULE, lex::rule::LOGIC } ); break;
        case 33: st.push( { lex::type::STR_CONST, "" } ); break;
        case 34: st.push( { lex::type::SYMBOL, lex::symbol::R_PARENTHESIS } );
                 st.push( { lex::type::SYMBOL, lex::symbol::L_PARENTHESIS } );
                 st.push( { lex::type::RESERVED, lex::reserved_word::READ } );
                 break;
        case 35: st.push( { lex::type::RULE, lex::rule::OPERAND_INT });
                 st.push( { lex::type::RULE, lex::rule::OPERATOR_INT });
                 st.push( { lex::type::RULE, lex::rule::OPERAND_INT });
                 break;
        case 36: st.push( { lex::type::RULE, lex::rule::OPERAND_INT }); break;
        case 37: st.push( { lex::type::SYMBOL, lex::symbol::PLUS}); break;
        case 38: st.push( { lex::type::SYMBOL, lex::symbol::MINUS}); break;
        case 39: st.push( { lex::type::SYMBOL, lex::symbol::SLASH}); break;
        case 40: st.push( { lex::type::SYMBOL, lex::symbol::STAR}); break;
        case 41: st.push( { lex::type::RULE, lex::rule::LOGIC_INT }); break;
        case 42: st.push( { lex::type::RULE, lex::rule::LOGIC_BOOL }); break;
        case 43: st.push( { lex::type::RULE, lex::rule::LOGIC_STR }); break;
        case 44: st.push( { lex::type::RULE, lex::rule::OPERAND_INT });
                 st.push( { lex::type::RULE, lex::rule::COMPARE_INT });
                 st.push( { lex::type::RULE, lex::rule::OPERAND_INT });
                 break;
        case 45: st.push( { lex::type::SYMBOL, lex::symbol::LESS}); break;
        case 46: st.push( { lex::type::SYMBOL, lex::symbol::GREATER}); break;
        case 20: st.push( { lex::type::SYMBOL, lex::symbol::EQUAL }); break;
        case 47: st.push( { lex::type::SYMBOL, lex::symbol::NOT_EQUAL });
                 break;
        case 48: st.push( { lex::type::SYMBOL, lex::symbol::EQUAL}); break;
        case 49: st.push( { lex::type::SYMBOL, lex::symbol::NOT_EQUAL });
                 break;
        case 50: st.push( { lex::type::RULE, lex::rule::OPERAND_BOOL });
                 st.push( { lex::type::RULE, lex::rule::LOGIC_EQUALS });
                 st.push( { lex::type::RULE, lex::rule::OPERAND_BOOL });
                 break;
        case 51: st.push( { lex::type::RULE, lex::rule::OPERAND_BOOL }); break;
        case 52: st.push( { lex::type::RULE, lex::rule::ID }); break;
        case 53: st.push( { lex::type::BOOL_CONST, false }); break;
        case 54: st.push( { lex::type::RULE, lex::rule::OPERAND_STR });
                 st.push( { lex::type::RULE, lex::rule::LOGIC_EQUALS });
                 st.push( { lex::type::RULE, lex::rule::OPERAND_STR });
                 break;
        case 55: st.push( { lex::type::RULE, lex::rule::ID }); break;
        case 56: st.push( { lex::type::STR_CONST, "" }); break;
        case 57: st.push( { lex::type::NEWLINE, 0u });
                 st.push( { lex::type::RESERVED, lex::reserved_word::ENDIF });
                 st.push( { lex::type::RULE, lex::rule::ELSE_PART });
                 st.push( { lex::type::RULE, lex::rule::STMTS });
                 st.push( { lex::type::NEWLINE, 0u });
                 st.push( { lex::type::RESERVED, lex::reserved_word::THEN });
                 st.push( { lex::type::RULE, lex::rule::LOGIC });
                 st.push( { lex::type::RESERVED, lex::reserved_word::IF });
                 break;
        case 58: st.push( { lex::type::RULE, lex::rule::STMTS });
                 st.push( { lex::type::NEWLINE, 0u });
                 st.push( { lex::type::RESERVED, lex::reserved_word::ELSE });
                 break;
        case 59: st.push( { lex::type::EPS, false }); break;
        case 60: st.push( { lex::type::NEWLINE, 0u });
                 st.push( { lex::type::INT_CONST, 0u });
                 st.push( { lex::type::RESERVED, lex::reserved_word::GOTO });
                 break;
        case 61: st.push( { lex::type::NEWLINE, 0u });
                 st.push( { lex::type::RESERVED, lex::reserved_word::ENDWHILE });
                 st.push( { lex::type::RULE, lex::rule::STMTS });
                 st.push( { lex::type::NEWLINE, 0u });
                 st.push( { lex::type::RULE, lex::rule::LOGIC });
                 st.push( { lex::type::RESERVED, lex::reserved_word::WHILE });
                 break;
        case 62: st.push( { lex::type::NEWLINE, 0u });
                 st.push( { lex::type::SYMBOL, lex::symbol::R_PARENTHESIS });
                 st.push( { lex::type::RULE, lex::rule::RIGHTSIDE });
                 st.push( { lex::type::SYMBOL, lex::symbol::L_PARENTHESIS });
                 st.push( { lex::type::RESERVED, lex::reserved_word::WRITE });
                 break;
        case 66: st.push( { lex::type::NEWLINE, 0u });
                 st.push( { lex::type::SYMBOL, lex::symbol::R_PARENTHESIS });
                 st.push( { lex::type::SYMBOL, lex::symbol::L_PARENTHESIS });
                 st.push( { lex::type::IDENTIFIER, "" });
                 break;
        case 67: st.push( { lex::type::IDENTIFIER, "" } ); break;
        default:
            assert( !"NO SUCH RULE!" );
    }
}

bool operator==( const lex& lhs, const lex& rhs )
{
    if (( lex::type::EPS == lhs.type_ ) || ( lex::type::EPS == rhs.type_ )) return true;
    if (lhs.type_ != rhs.type_) return false;
    switch (lhs.type_) {
        case lex::type::SYMBOL:
            return boost::get< lex::symbol >( lhs.value_ ) == boost::get< lex::symbol >( rhs.value_ );
        case lex::type::RESERVED:
            return boost::get< lex::reserved_word >( lhs.value_ ) == boost::get< lex::reserved_word >( rhs.value_);
        case lex::type::RULE:
            return boost::get< lex::rule >( lhs.value_ ) == boost::get< lex::rule >( rhs.value_ );
        case lex::type::NEWLINE:
        case lex::type::INT_CONST:
        case lex::type::BOOL_CONST:
        case lex::type::STR_CONST:
        case lex::type::IDENTIFIER:
            return true;
        default:
            assert( !"Wrong lexeme!");
    }
    return false;
}

Queue parse( const Table& table, LIterator begin, const LIterator end, Stack ss)
{
    Queue result;
    while (ss.size() > 1) {
        const bool atEnd = begin == end;
        DEBUG( ss.top() );
        if (!atEnd) DEBUG(*begin);

        if (ss.top().type_ == lex::type::COUNT ) {
            break;
        } else if (ss.top().type_ == lex::type::EPS ) {
            ss.pop();
        } else if (ss.top() == *begin) {
            ++begin;
            ss.pop();
        } else if (lex::type::RULE == ss.top().type_) {
            const auto table_top = table.find( ss.top() );
            if (table.end() == table_top) throw std::logic_error{ "Cannot find rule!" };
            std::pair< MIterator, MIterator > eq;
            if (!atEnd) eq = table_top->second.equal_range( *begin );
            std::pair< MIterator, MIterator > eq_eps = table_top->second.equal_range( { lex::type::EPS, false } );

            int length{ 0 };
            if (!atEnd) length = std::distance( eq.first, eq.second );
            const int length_eps = std::distance( eq_eps.first, eq_eps.second );
            const int sum_length = length + length_eps;

            if (0 == sum_length) throw std::logic_error{ "asd" };
            ss.pop();

            if (1 == sum_length) {
                const uint rule = (length == 0) ? eq_eps.first->second : eq.first->second;

                result.push( rule );
                push_rule( ss, rule );
            } else { // length > 1
                if (!atEnd)
                for (; eq.second != eq.first; ++eq.first) {
                    Stack newSS{ ss };
                    const uint rule = eq.first->second;
                    push_rule( newSS, rule );
                    try {
                        Queue subQueue = parse( table, begin, end, std::move( newSS ) );
                        result.push( rule );

                        while (!subQueue.empty()) {
                            result.push( subQueue.front());
                            subQueue.pop();
                        }
                        return result;
                    } catch (std::logic_error& ex) {
                        DBG( "Failed rule" );
                        DEBUG( rule );
                        continue;
                    }
                }

                for (; eq_eps.second != eq_eps.first; ++eq_eps.first) {
                    Stack newSS{ ss };
                    const uint rule = eq_eps.first->second;
                    push_rule( newSS, rule );
                    try {
                        Queue subQueue = parse( table, begin, end, std::move( newSS ) );
                        result.push( rule );
                        while (!subQueue.empty()) {
                            result.push( subQueue.front());
                            subQueue.pop();
                        }
                        return result;
                    } catch (std::logic_error& ex) {
                        DBG( "Failed rule" );
                        DEBUG( rule );
                        continue;
                    }
                }


                throw std::logic_error{ "Cannot find rule" };
            }

        } else {
            throw std::logic_error{ "Cannot find rule!" };
        }
    }

    if (0 != std::distance( begin, end)) throw std::logic_error{ "Cannot perform syntax parsing!" };

    return result;
}

Table createTable()
{
    Table result;

    using map = std::unordered_multimap< lex, int >;
    map tmp;

    tmp.insert( { { { lex::type::RESERVED, lex::reserved_word::DIM }, 1 }
                , { { lex::type::IDENTIFIER, "" }, 1 }
                , { { lex::type::INT_CONST, 0u }, 1 }
                , { { lex::type::RESERVED, lex::reserved_word::IF }, 1 }
                , { { lex::type::RESERVED, lex::reserved_word::GOTO }, 1 }
                , { { lex::type::RESERVED, lex::reserved_word::WHILE }, 1 }
                , { { lex::type::RESERVED, lex::reserved_word::WRITE }, 1 }
                , { { lex::type::RESERVED, lex::reserved_word::SUB }, 1 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::START }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::RESERVED, lex::reserved_word::DIM }, 3 }
                , { { lex::type::IDENTIFIER, "" }, 3 }
                , { { lex::type::INT_CONST, 0u }, 3 }
                , { { lex::type::RESERVED, lex::reserved_word::IF }, 3 }
                , { { lex::type::RESERVED, lex::reserved_word::GOTO }, 3 }
                , { { lex::type::RESERVED, lex::reserved_word::WHILE }, 3 }
                , { { lex::type::RESERVED, lex::reserved_word::WRITE }, 3}
                , { { lex::type::RESERVED, lex::reserved_word::SUB }, 2 }
                , { { lex::type::IDENTIFIER, "" }, 2 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::ALL_STMT }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::RESERVED, lex::reserved_word::DIM }, 4 }
                , { { lex::type::IDENTIFIER, "" }, 4 }
                , { { lex::type::INT_CONST, 0u }, 4 }
                , { { lex::type::RESERVED, lex::reserved_word::IF }, 4 }
                , { { lex::type::RESERVED, lex::reserved_word::GOTO }, 4 }
                , { { lex::type::RESERVED, lex::reserved_word::WHILE }, 4 }
                , { { lex::type::RESERVED, lex::reserved_word::WRITE }, 4}
                , { { lex::type::RESERVED, lex::reserved_word::SUB }, 4 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::ALL_STMTS }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::RESERVED, lex::reserved_word::DIM }, 5 }
                , { { lex::type::IDENTIFIER, "" }, 5 }
                , { { lex::type::INT_CONST, 0u }, 5 }
                , { { lex::type::RESERVED, lex::reserved_word::IF }, 5 }
                , { { lex::type::RESERVED, lex::reserved_word::GOTO }, 5 }
                , { { lex::type::RESERVED, lex::reserved_word::WHILE }, 5 }
                , { { lex::type::RESERVED, lex::reserved_word::WRITE }, 5}
                , { { lex::type::RESERVED, lex::reserved_word::SUB }, 5 }
                , { { lex::type::EPS, 0u }, 6 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::MORE_ALL_STMTS }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::RESERVED, lex::reserved_word::SUB }, 7 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::SUB_STMT }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::RESERVED, lex::reserved_word::DIM }, 8 }
                , { { lex::type::IDENTIFIER, "" }, 8 }
                , { { lex::type::INT_CONST, 0u }, 8 }
                , { { lex::type::RESERVED, lex::reserved_word::IF }, 8 }
                , { { lex::type::RESERVED, lex::reserved_word::GOTO }, 8 }
                , { { lex::type::RESERVED, lex::reserved_word::WHILE }, 8 }
                , { { lex::type::RESERVED, lex::reserved_word::WRITE }, 8}
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::STMTS }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::RESERVED, lex::reserved_word::DIM }, 9 }
                , { { lex::type::IDENTIFIER, "" }, 9 }
                , { { lex::type::INT_CONST, 0u }, 9 }
                , { { lex::type::RESERVED, lex::reserved_word::IF }, 9 }
                , { { lex::type::RESERVED, lex::reserved_word::GOTO }, 9 }
                , { { lex::type::RESERVED, lex::reserved_word::WHILE }, 9 }
                , { { lex::type::RESERVED, lex::reserved_word::WRITE }, 9}
                , { { lex::type::EPS, false }, 10 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::MORE_STMTS }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::INT_CONST, 0u }, 11 }
                , { { lex::type::EPS, false }, 11 }
                , { { lex::type::INT_CONST, 0u }, 12 }
                , { { lex::type::EPS, false }, 12 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::LSTMT }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::RESERVED, lex::reserved_word::DIM }, 13 }
                , { { lex::type::IDENTIFIER, "" }, 14 }
                , { { lex::type::RESERVED, lex::reserved_word::IF }, 15 }
                , { { lex::type::RESERVED, lex::reserved_word::GOTO }, 16 }
                , { { lex::type::RESERVED, lex::reserved_word::WHILE }, 17 }
                , { { lex::type::RESERVED, lex::reserved_word::WRITE }, 18 }
                , { { lex::type::IDENTIFIER, "" }, 19 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::STMT }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::INT_CONST, 0u }, 21 }
                , { { lex::type::EPS, false }, 22 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::LABEL_DEF }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { lex::type::RESERVED, lex::reserved_word::DIM }, 23 } );
    result.insert( { { lex::type::RULE, lex::rule::DIM_EXPR }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { lex::type::IDENTIFIER, "" }, 24 } );
    result.insert( { { lex::type::RULE, lex::rule::ASSIGNMENT }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::IDENTIFIER, "" }, 25 }
                , { { lex::type::IDENTIFIER, "" }, 26 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::ID }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::SYMBOL, lex::symbol::L_BRACKET }, 27 } } );
    result.insert( { { lex::type::RULE, lex::rule::ARRAY_INDEX }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::IDENTIFIER, "" }, 28 }
                , { { lex::type::INT_CONST, 0u }, 29 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::OPERAND_INT }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::IDENTIFIER, "" }, 31 }
                , { { lex::type::RESERVED, lex::reserved_word::READ }, 30 }
                , { { lex::type::INT_CONST, 0u }, 31 }
                , { { lex::type::INT_CONST, 0u }, 32 }
                , { { lex::type::BOOL_CONST, false }, 32 }
                , { { lex::type::STR_CONST, "" }, 32 }
                , { { lex::type::IDENTIFIER, "" }, 32 }
                , { { lex::type::STR_CONST, "" }, 33 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::RIGHTSIDE }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert(  { { lex::type::RESERVED, lex::reserved_word::WRITE }, 34 } );
    result.insert( { { lex::type::RULE, lex::rule::READ_STMT }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::IDENTIFIER, "" }, 35 }
                , { { lex::type::INT_CONST, 0u }, 35 }
                , { { lex::type::INT_CONST, 0u }, 36 }
                , { { lex::type::IDENTIFIER, "" }, 36 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::INT_EXPR }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::SYMBOL, lex::symbol::PLUS }, 37 }
                , { { lex::type::SYMBOL, lex::symbol::MINUS }, 38 }
                , { { lex::type::SYMBOL, lex::symbol::SLASH }, 39 }
                , { { lex::type::SYMBOL, lex::symbol::STAR }, 40 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::OPERATOR_INT }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::INT_CONST, 0u }, 41 }
                , { { lex::type::IDENTIFIER, "" }, 41 }
                , { { lex::type::BOOL_CONST, false }, 42 }
                , { { lex::type::IDENTIFIER, "" }, 42 }
                , { { lex::type::STR_CONST, "" }, 43 }
                , { { lex::type::IDENTIFIER, "" }, 43 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::LOGIC }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::INT_CONST, 0u }, 44 }
                , { { lex::type::IDENTIFIER, "" }, 44 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::LOGIC_INT }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::SYMBOL, lex::symbol::LESS }, 45 }
                , { { lex::type::SYMBOL, lex::symbol::GREATER }, 46 }
                , { { lex::type::SYMBOL, lex::symbol::NOT_EQUAL }, 47 }
                , { { lex::type::SYMBOL, lex::symbol::EQUAL }, 20 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::COMPARE_INT }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::SYMBOL, lex::symbol::NOT_EQUAL }, 49 }
                , { { lex::type::SYMBOL, lex::symbol::EQUAL }, 48 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::LOGIC_EQUALS }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::BOOL_CONST, false }, 50 }
                , { { lex::type::IDENTIFIER, "" }, 50 }
                , { { lex::type::IDENTIFIER, "" }, 51 }
                , { { lex::type::BOOL_CONST, false }, 51 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::LOGIC_BOOL }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::BOOL_CONST, false }, 53 }
                , { { lex::type::IDENTIFIER, "" }, 52 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::OPERAND_BOOL }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::STR_CONST, "" }, 54 }
                , { { lex::type::IDENTIFIER, "" }, 54 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::LOGIC_STR }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::STR_CONST, "" }, 56 }
                , { { lex::type::IDENTIFIER, "" }, 55 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::OPERAND_STR }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { lex::type::RESERVED, lex::reserved_word::IF }, 57 });
    result.insert( { { lex::type::RULE, lex::rule::IF_STMT }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { { lex::type::RESERVED, lex::reserved_word::ELSE }, 58 }
                , { { lex::type::EPS, false }, 59 }
                }
            );
    result.insert( { { lex::type::RULE, lex::rule::ELSE_PART }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { lex::type::RESERVED, lex::reserved_word::GOTO }, 60 });
    result.insert( { { lex::type::RULE, lex::rule::GOTO_STMT }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { lex::type::RESERVED, lex::reserved_word::WHILE }, 61 });
    result.insert( { { lex::type::RULE, lex::rule::WHILE_STMT }
                   , std::move( tmp ) } );
    tmp.clear();


    tmp.insert( { { lex::type::RESERVED, lex::reserved_word::WRITE }, 62 });
    result.insert( { { lex::type::RULE, lex::rule::WRITE_STMT }
                   , std::move( tmp ) } );
    tmp.clear();

    tmp.insert( { { lex::type::IDENTIFIER, "" }, 66 });
    result.insert( { { lex::type::RULE, lex::rule::SUB_CALL_STMT }
                   , std::move( tmp ) } );
    tmp.clear();

    tmp.insert( { { lex::type::IDENTIFIER, "" }, 67 });
    result.insert( { { lex::type::RULE, lex::rule::NEW_IDENTIFIER }
                   , std::move( tmp ) } );

    return result;
}
} // namespace sap
