#pragma once

#include "program_tree.h"
#include "program_code.h"

#include <vector>
#include <string>
#include <list>
#include <iosfwd>
#ifndef NDEBUG
    #include <iostream>
#endif

#include <iosfwd>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace sap
{
using Identifiers = std::list< std::string >;
using Identifier  = Identifiers::const_iterator;

using Labels      = std::set< uint >;
using Label       = boost::optional< Labels::const_iterator >;

enum class IDType { Procedure, Array, Variable };
using TypeMap = std::map< std::string, IDType >;

struct LocalScope
{
    using LocalScopes = std::list< LocalScope >;

    Identifiers variables_;
    LocalScopes scopes_;

    LocalScope( const Identifiers& i_blocked = {});
    Identifier addVariable( const std::string& i_name );
    bool useVariable( const std::string& i_name ) const;
    bool declareVariable( const std::string& i_name ) const;
    bool useArray( const std::string& i_name ) const;
    bool declareArray( const std::string& i_name ) const;
    bool useProc( const std::string& i_name ) const;
    bool declareProc( const std::string& i_name ) const;
    LocalScope& addScope();

    void reset();
}; // struct LocalScope

std::ostream& operator<<( std::ostream& out, const LocalScope& scope );

struct GlobalScope
{
    LocalScope scope_;

    TypeMap declared_;
    TypeMap used_;

    Labels      declaredLabels_;
    Labels      usedLabels_;

    bool declare( const std::string& i_name, IDType i_type );
    bool use( const std::string& i_name, IDType i_type );
    bool has( const std::string& i_name ) const;

    bool addDeclaredLabel( uint i_label );
    bool addUsedLabel( uint i_label );

    uint addLabel( );
    static GlobalScope& instance();

    void reset();
private:
    GlobalScope() = default;
    GlobalScope( const GlobalScope& ) = delete;
    GlobalScope( GlobalScope&& ) = delete;
    GlobalScope& operator=( const GlobalScope& ) = delete;
    GlobalScope& operator=( GlobalScope&& ) = delete;
}; // struct Scope

std::ostream& operator<<( std::ostream& out, const GlobalScope& scope );

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
using Id = boost::variant< std::string, boost::recursive_wrapper< ArrayElement > >;
// using Id = boost::variant< std::string, ArrayElement >;

struct OperandInt : private RuleAssertion< lex::rule::OPERAND_INT >
{
    using Value = boost::variant< int, Id >;
    Value value_;

    OperandInt( const Node& i_node, const LocalScope& i_scope );
    void operator()( ProgramCode& i_code ) const;
}; // struct OperantInt

struct ArrayElement : private RuleAssertion< lex::rule::ID >
{
    std::string lhs_;
    OperandInt  rhs_;

    ArrayElement( const Node& i_node, const LocalScope& i_scope );
    void operator()( ProgramCode& i_code ) const;
}; // struct ArrayElement

struct OperandBool : private RuleAssertion< lex::rule::OPERAND_BOOL >
{
    using Value = boost::variant< bool, Id >;
    Value value_;

    OperandBool( const Node& i_node, const LocalScope& i_scope );
    void operator()( ProgramCode& i_code ) const;
}; // struct OperandBool

struct OperandStr : private RuleAssertion< lex::rule::OPERAND_STR >
{
    using Value = boost::variant< std::string, Id >;
    Value value_;

    OperandStr( const Node& i_node, const LocalScope& i_scope );
    void operator()( ProgramCode& i_code ) const;
}; // struct OperandStr

struct IntExpr : private RuleAssertion< lex::rule::INT_EXPR >
{
    enum class Op { PLUS, MINUS, MULT, DIV };
    OperandInt                      lhs_;
    boost::optional< Op >           op_;
    boost::optional< OperandInt >   rhs_;

    IntExpr( const Node& i_node, const LocalScope& i_scope );
    void operator()( ProgramCode& i_code ) const;
}; // struct IntExpr

enum class Cmp { EQ, NE, LE, GE };

struct LogicBool : private RuleAssertion< lex::rule::LOGIC_BOOL >
{
    OperandBool                     lhs_;
    boost::optional< Cmp >          cmp_;
    boost::optional< OperandBool >  rhs_;

    LogicBool( const Node& i_node, const LocalScope& i_scope );
    void operator()( ProgramCode& i_code ) const;
}; // struct LogicBool

struct LogicInt : private RuleAssertion< lex::rule::LOGIC_INT >
{
    OperandInt  lhs_;
    Cmp         cmp_;
    OperandInt  rhs_;

    LogicInt( const Node& i_node, const LocalScope& i_scope );
    void operator()( ProgramCode& i_code ) const;
}; // struct LogicInt

struct LogicStr : private RuleAssertion< lex::rule::LOGIC_STR >
{
    OperandStr  lhs_;
    Cmp         cmp_;
    OperandStr  rhs_;

    LogicStr( const Node& i_node, const LocalScope& i_scope );
    void operator()( ProgramCode& i_code ) const;
}; // struct LogicStr

using Logic = boost::variant< LogicBool, LogicInt, LogicStr >;

struct Input : private RuleAssertion< lex::rule::READ_STMT >
{
    Input( const Node& i_node );
    void operator()( ProgramCode& i_code ) const;
};

using Rightside = boost::variant< Logic, IntExpr, Input, std::string >;

struct Assignment : private RuleAssertion< lex::rule::ASSIGNMENT >
{
    Id          lhs_;
    Rightside   rhs_;
    Assignment( const Node& i_node, const LocalScope& i_scope );
    void operator()( ProgramCode& i_code ) const;
}; // struct Assignment

struct Goto : private RuleAssertion< lex::rule::GOTO_STMT >
{
    uint to_;
    Goto( const Node& i_node );
    void operator()( ProgramCode& i_code ) const;
}; // struct Goto

struct Write : private RuleAssertion< lex::rule::WRITE_STMT >
{
    Rightside rhs_;
    Write( const Node& i_node, const LocalScope& i_scope );
    void operator()( ProgramCode& i_code ) const;
}; // struct Write

using SubCall = std::string;

struct If;
struct While;

struct Dim : private RuleAssertion< lex::rule::DIM_EXPR >
{
    Identifier var_;

    Dim( const Node& i_node, LocalScope& i_scope );
}; // struct Dim

using Sline = boost::variant< Assignment, SubCall, Goto, Write, boost::recursive_wrapper< If >, boost::recursive_wrapper< While >, Dim >;
// using Sline = boost::variant< Assignment, SubCall, Goto, Write, If , While , Dim >;

struct Line : private RuleAssertion< lex::rule::LSTMT >
{
    boost::optional< uint >     label_;
    boost::optional< Sline >    line_;

    Line( const Node& i_node, LocalScope& i_scope );
    void operator()( ProgramCode& i_code ) const;
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
    void operator()( ProgramCode& i_code ) const;
}; // struct If

struct While : private RuleAssertion< lex::rule::WHILE_STMT >
{
    Logic       logic_;
    LocalScope& localScope_;
    Lines       body_;

    While( const Node& i_node, LocalScope& i_scope );
    void operator()( ProgramCode& i_code ) const;
}; // struct While

struct SubDecl : private RuleAssertion< lex::rule::SUB_STMT >
{
    const std::string name_;
    LocalScope& local_;
    Lines       body_;

    SubDecl( const Node& i_node, LocalScope& i_scope );
    void operator()( ProgramCode& i_code ) const;
}; // struct SubDecl

using AnyLine = boost::variant< Line, SubDecl >;
using AnyLines = std::vector< AnyLine >;
struct Program : private RuleAssertion< lex::rule::START >
{
    AnyLines    body_;

    Program( const Node& i_node );
    void operator()( ProgramCode& i_code ) const;
}; // struct Program
} // namespace sap
