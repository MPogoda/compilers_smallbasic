#pragma once

#include <boost/functional/hash.hpp>

#include "lexeme.h"
#include "grammar.h"

#include <unordered_map>
#include <stack>

namespace std
{
template <>
struct hash< sap::lex >
{
    size_t operator()( const sap::lex& lhs ) const
    {
        using pair = std::pair< sap::lex::type, sap::lex::value >;

        return boost::hash< pair >{}( {lhs.type_, lhs.value_ } );
    }
};
} // namespace std

namespace sap
{
using Table = std::unordered_map< lex, std::unordered_multimap< lex, int > >;
using MIterator = std::unordered_multimap< lex, int >::const_iterator;
using Stack = std::stack< lex >;
using LIterator = Lexems::const_iterator;

void push_rule( Stack& st, const int rule_number );

bool parse( const Table& table, LIterator begin, const LIterator end, Stack& ss );
Table createTable();
} // namespace sap

// struct array;
//
// typedef boost::variant< int64_t
//                       , std::string
//                       , boost:: recursive_helper< array >
//                       >
//         array_index;
//
// struct array
// {
//     std::string name_;
//     array_index index_;
// }; // struct array
// std::ostream& operator<<(std::ostream& os, array& rhs )
// {
//
//     return os;
// }
//
// typedef boost::variant< int64_t
//                       , std::string
//                       , array >
//         operand_int;
//
// enum class operator_int : uint8_t { PLUS, MINUS, DIV, MULT };
//
// typedef boost::variant< std::string
//                       , array
//                       >
//         operand_str;
// typedef boost::variant< bool
//                       , std::string
//                       , array
//                       >
//         operand_bool;
//
// typedef operand_str leftside;
//
// template < typename Operand, typename Operator, typename Operand2 = Operand >
// struct bin_op
// {
//     Operand left_;
//     Operator op_;
//     Operand2 right_;
// }; // struct bin_op
//
// typedef boost::variant< operand_int
//                       , bin_op< operand_int, operator_int >
//                       >
//         int_expr;
//
// enum class compare_int : uint8_t { LT, GT, EQ, NE };
// enum class compare_other : uint8_t { EQ, NE };
//
// typedef bin_op< operand_int, compare_int > logic_int;
// typedef boost::variant< operand_bool
//                       , bin_op< operand_bool, compare_other >
//                       >
//         logic_bool;
// typedef bin_op< operand_str, compare_other > logic_str;
//
// typedef boost::variant< logic_int
//                       , logic_bool
//                       , logic_str
//                       >
//         logic;
//
// struct read
// {
//     double dummy_;
// };
//
// typedef boost::variant< read
//                       , int_expr
//                       , logic
//                       , std::string
//                       >
//         rightside;
//
// typedef std::pair< leftside, rightside > assignment;
//
// struct unary_stmt
// {
//     enum class type : uint8_t { DIM, LABEL, GOTO, NOP };
//     type type_;
//     std::string arg_;
// }; // unary_stmt
//
// struct stmts;
//
// struct if_stmt
// {
//     logic logic_;
//     stmts then_;
//     stmts else_;
// };
//
// struct while_stmt
// {
//     logic logic_;
//     stmts do_;
// };
//
// struct write_stmt
// {
//     typedef boost::variant< int_expr, logic, string_constant > writeable;
//     writeable w_;
// };
//
// struct sub_call
// {
//     std::string sub_;
// };
//
// struct sub_stmt
// {
//     std::string id_;
//     stmts body_;
// };
//
// struct stmt
// {
//     unary_stmt label_;
//     typedef boost::variant< unary_stmt
//                           , assignment
//                           , boost_recursive_helper< if_stmt >
//                           , boost_recursive_helper< while_stmt >
//                           , write_stmt
//                           , sub_call
//             > body;
//     body body_
// };
// } // namespace sap
//
// BOOST_FUSION_ADAPT_STRUCT(
//     sap::array,
//     (std::string, name_)
//     (sap::array_index, index_)
// )
//
// BOOST_FUSION_ADAPT_STRUCT(
//     sap::while_stmt,
//     (logic, logic_)
//     (sap::stmts, do_)
// )
// BOOST_FUSION_ADAPT_STRUCT(
//     sap::if_stmt,
//     (logic, logic_)
//     (sap::stmts, then_)
//     (sap::stmts, else_)
// )
// BOOST_FUSION_ADAPT_STRUCT(
//     sap::read,
//     (double, dummy_)
// )
//
// BOOST_FUSION_ADAPT_STRUCT(
//     sap::unary_stmt,
//     (sap::unary_stmt::type, type_)
//     (std::string, arg_)
// )
//
// BOOST_FUSION_ADAPT_TPL_STRUCT(
//     (Operand)(Operator)(Operand2),
//     (sap::bin_op)(Operand)(Operator)(Operand2)
//     (Operand, left_)
//     (Operator, op_)
//     (Operand2, right_)
// )
//
