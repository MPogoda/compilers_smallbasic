#pragma once

#include "program_tree.h"

#include <vector>
#include <string>
#include <list>
#include <iosfwd>
#ifndef NDEBUG
    #include <iostream>
#endif

#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace sap
{
using Identifiers = std::list< std::string >;
using Identifier  = Identifiers::const_iterator;

using Labels      = std::set< uint >;
using Label       = boost::optional< Labels::const_iterator >;

struct GlobalScope
{
    Identifiers declaredProcedures_;
    Identifiers usedProcedures_;

    Identifiers declaredVariables_;
    Identifiers usedVariables_;

    Identifiers declaredArrays_;
    Identifiers usedArrays_;

    Labels      declaredLabels_;
    Labels      usedLabels_;

    bool addDeclaredProcedure( const std::string& i_name );
    bool addUsedProcedure( const std::string& i_name );
    bool addDeclaredVariable( const std::string& i_name );
    bool addUsedVariable( const std::string& i_name );
    bool addDeclaredArray( const std::string& i_name );
    bool addUsedArray( const std::string& i_name );

    bool checkNames() const;

    bool addDeclaredLabel( uint i_label );
    bool addUsedLabel( uint i_label );

    static GlobalScope& instance();

    bool isNameFree( const std::string& i_name ) const;
private:
    GlobalScope();
    GlobalScope( const GlobalScope& ) = delete;
    GlobalScope( GlobalScope&& ) = delete;
    GlobalScope& operator=( const GlobalScope& ) = delete;
    GlobalScope& operator=( GlobalScope&& ) = delete;
}; // struct Scope

struct LocalScope
{
    using LocalScopes = std::list< LocalScope >;

    Identifiers variables_;
    LocalScopes scopes_;

    bool addVariable( const std::string& i_name );
    LocalScope& addScope();
}; // struct LocalScope

template < lex::rule Rule >
struct RuleAssertion
{
    RuleAssertion( const Node& i_node )
    {
        #ifndef NDEBUG
            std::cout << "Parsing node with " << Rule << ';'
                    << "Actual " << i_node.rule_ << '\n';
        #endif
        assert( Rule == i_node.rule_ );
    }
};

struct ArrayElement;
using Id = boost::variant< std::string, ArrayElement >;

struct OperandInt : private RuleAssertion< lex::rule::OPERAND_INT >
{
    using Value = boost::variant< int, Id >;
    Value value_;

    OperandInt( const Node& i_node, LocalScope& i_scope );
}; // struct OperantInt

struct ArrayElement : private RuleAssertion< lex::rule::ID >
{
    std::string lhs_;
    Id          rhs_;

    ArrayElement( const Node& i_node, LocalScope& i_scope );
}; // struct ArrayElement

struct OperandBool : private RuleAssertion< lex::rule::OPERAND_BOOL >
{
    using Value = boost::variant< bool, Id >;
    Value value_;

    OperandBool( const Node& i_node, LocalScope& i_scope );
}; // struct OperandBool

struct OperandStr : private RuleAssertion< lex::rule::OPERAND_STR >
{
    using Value = boost::variant< std::string, Id >;
    Value value_;

    OperandStr( const Node& i_node, LocalScope& i_scope );
}; // struct OperandStr

struct IntExpr : private RuleAssertion< lex::rule::INT_EXPR >
{
    enum class Op { PLUS, MINUS, MULT, DIV };
    OperandInt                      lhs_;
    boost::optional< Op >           op_;
    boost::optional< OperandInt >   rhs_;

    IntExpr( const Node& i_node, LocalScope& i_scope );
}; // struct IntExpr

enum class Cmp { EQ, NE, LE, GE };

struct LogicBool : private RuleAssertion< lex::rule::LOGIC_BOOL >
{
    OperandBool                     lhs_;
    boost::optional< Cmp >          cmp_;
    boost::optional< OperandBool >  rhs_;

    LogicBool( const Node& i_node, LocalScope& i_scope );
}; // struct LogicBool

struct LogicInt : private RuleAssertion< lex::rule::LOGIC_INT >
{
    OperandInt  lhs_;
    Cmp         cmp_;
    OperandInt  rhs_;

    LogicInt( const Node& i_node, LocalScope& i_scope );
}; // struct LogicInt

struct LogicStr : private RuleAssertion< lex::rule::LOGIC_STR >
{
    OperandStr  lhs_;
    Cmp         cmp_;
    OperandStr  rhs_;

    LogicStr( const Node& i_node, LocalScope& i_scope );
}; // struct LogicStr

using Logic = boost::variant< LogicBool, LogicInt, LogicStr >;

struct Input : private RuleAssertion< lex::rule::READ_STMT >
{
    Input( const Node& i_node );
};

using Rightside = boost::variant< Logic, IntExpr, Input, std::string >;

struct Assignment : private RuleAssertion< lex::rule::ASSIGNMENT >
{
    std::string lhs_;
    Rightside   rhs_;
    Assignment( const Node& i_node, LocalScope& i_scope );
}; // struct Assignment

struct Goto : private RuleAssertion< lex::rule::GOTO_STMT >
{
    uint to_;
    Goto( const Node& i_node );
}; // struct Goto

struct Write : private RuleAssertion< lex::rule::WRITE_STMT >
{
    Rightside rhs_;
    Write( const Node& i_node, LocalScope& i_scope );
}; // struct Write

using SubCall = std::string;

struct If;
struct While;

using Sline = boost::variant< Assignment, SubCall, Goto, Write, If, While >;

struct Line : private RuleAssertion< lex::rule::LSTMT >
{
    boost::optional< uint > label_;
    Sline                   line_;

    Line( const Node& i_node, LocalScope& i_scope );
}; // private Line

using Lines = std::vector< Line >;
struct If : private RuleAssertion< lex::rule::IF_STMT >
{
    Logic       logic_;
    LocalScope& thenScope_;
    Lines       then_;
    LocalScope& elseScope_;
    Lines       else_;

    If( const Node& i_node, LocalScope& i_scope );
}; // struct If

struct While : private RuleAssertion< lex::rule::WHILE_STMT >
{
    Logic       logic_;
    LocalScope& localScope_;
    Lines       body_;

    While( const Node& i_node, LocalScope& i_scope );
}; // struct While

struct SubDecl : private RuleAssertion< lex::rule::SUB_STMT >
{
    LocalScope& local_;
    Lines       body_;

    SubDecl( const Node& i_node, LocalScope& i_scope );
}; // struct SubDecl

using AnyLine = boost::variant< Line, SubDecl >;
using AnyLines = std::vector< AnyLine >;
struct Program : private RuleAssertion< lex::rule::START >
{
    LocalScope  local_;
    AnyLines    body_;

    Program( const node& i_node );
}; // struct Program
} // namespace sap
