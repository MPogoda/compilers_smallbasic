#include "program_struct.h"

#ifdef NDEBUG
    #define DEBUG(a)
    #define DBG(a)
#else
    #include <iostream>
    #define DEBUG(a) std::cout << #a << " = " << (a) << '\n'
    #define DBG(a) std::cout << __LINE__ << " : " << a << '\n';
#endif

#include <algorithm>
#include <iterator>

#include <boost/lexical_cast.hpp>

namespace sap
{
namespace
{
struct LocalScopePrinter
{
    uint               indentLevel_ = 0;
    std::stack< uint > n_;
    std::ostream& out_;

    LocalScopePrinter( std::ostream& out ) : out_( out )  { n_.push( 0 ); }

    void print( const LocalScope& scope )
    {
        p_printLevel();
        out_ << "Available new local variables: ";
        if (n_.top() < scope.variables_.size()) {
            Identifier id = std::next( scope.variables_.cbegin(), n_.top() );
            for( ; scope.variables_.cend() != id; ++id )
                out_ << *id << ", ";
        } else out_ << "{{NONE}}";

        out_ << '\n';

        n_.push( scope.variables_.size() );
        ++indentLevel_;
        for ( const auto& sc : scope.scopes_ )
            print( sc );
        --indentLevel_;
        n_.pop();
    }

    void p_printLevel() const
    {
        for (uint i = 0; indentLevel_ != i; ++i)
            out_ << ' ' << ' ';
    }
};
}
std::ostream& operator<<( std::ostream& out, const LocalScope& scope )
{
    LocalScopePrinter{ out }.print( scope );

    return out;
}

std::ostream& operator<<( std::ostream& out, const IDType type )
{
    switch (type) {
        case IDType::Procedure: out << "Procedure"; break;
        case IDType::Array: out << "Array"; break;
        case IDType::Variable: out << "Variable"; break;
    }

    return out;
}
std::ostream& operator<<( std::ostream& out, const GlobalScope& scope )
{
    out << "Available global definitions:\n";
    for (const auto& it : scope.declared_) {
        out << "  " << it.first << " : " << it.second << '\n';
    }

    if (!scope.used_.empty()) {
        out << "NOT DECLARED, BUT USED DECLARATIONS:\n";

        for (const auto& it : scope.used_) {
            out << "  " << it.first << " : " << it.second << '\n';
        }
    }

    out << "Available labels: ";
    for (const auto i : scope.declaredLabels_ ) out << i << ", ";
    out << '\n';

    if (!scope.usedLabels_.empty()) {
        out << "NOT DECLARED LABELS: ";
        for (const auto i : scope.usedLabels_ ) out << i << ", ";
        out << '\n';
    }

    out << scope.scope_;

    return out;
}

bool GlobalScope::declare( const std::string& i_name, const IDType i_type )
{
    const bool isDeclared = declared_.find( i_name ) != declared_.cend();
    if ( isDeclared ) {
        if ( declared_[ i_name ] != i_type ) {
            return false;
        } else {
            if (i_type == IDType::Procedure)
                return false;
            else
                return true;
        }
    }

    const bool isUsed = used_.find( i_name ) != used_.cend();
    if ( isUsed ) {
        if ( used_[ i_name ] != i_type ) {
            return false;
        } else {
            used_.erase( used_.find( i_name ) );
        }
    }

    declared_.insert( { i_name, i_type } );
    return true;
}

bool GlobalScope::use( const std::string& i_name, const IDType i_type )
{
    const bool isDeclared = declared_.find( i_name ) != declared_.cend();
    if ( isDeclared ) {
        if ( declared_[ i_name ] != i_type ) {
            return false;
        } else {
            return true;
        }
    }

    const bool isUsed = used_.find( i_name ) != used_.cend();
    DEBUG( isUsed );
    DEBUG( i_name );
    if ( isUsed ) {
        if ( used_[ i_name ] != i_type ) {
            return false;
        } else {
            return true;
        }
    }

    DEBUG( used_.size() );
    used_.insert( { i_name, i_type } );
    DEBUG( used_.size() );
    return true;
}

bool GlobalScope::has( const std::string& i_name ) const
{
    const bool isDeclared = declared_.find( i_name ) != declared_.cend();
    const bool isUsed = used_.find( i_name ) != used_.cend();

    return isDeclared || isUsed;
}

bool GlobalScope::addDeclaredLabel( uint i_label )
{
    if (declaredLabels_.find( i_label ) != declaredLabels_.cend()) {
        return false;
    } else {
        declaredLabels_.insert( i_label );
        if (usedLabels_.find( i_label ) != usedLabels_.cend()) {
            usedLabels_.erase( usedLabels_.find( i_label ) );
        }
        return true;
    }
}

bool GlobalScope::addUsedLabel( uint i_label )
{
    if (declaredLabels_.find( i_label ) != declaredLabels_.cend()) {
        return true;
    }
    if (usedLabels_.find( i_label ) == usedLabels_.cend()) {
        usedLabels_.insert( i_label );
    }

    return true;
}

GlobalScope& GlobalScope::instance()
{
    static GlobalScope scope;

    return scope;
}

LocalScope::LocalScope( const Identifiers& i_blocked )
    : variables_{ i_blocked }
{
}

void LocalScope::reset()
{
    Identifiers{}.swap( variables_ );
    LocalScopes{}.swap( scopes_ );
}

void GlobalScope::reset()
{
    scope_.reset();
    TypeMap{}.swap( declared_ );
    TypeMap{}.swap( used_ );
    Labels{}.swap( declaredLabels_ );
    Labels{}.swap( usedLabels_ );
}

bool LocalScope::useVariable( const std::string& i_name ) const
{
    const bool isLocal = std::find( variables_.cbegin(), variables_.cend(), i_name ) != variables_.cend();

    if (isLocal) return true;
    else return GlobalScope::instance().use( i_name, IDType::Variable );
}

Identifier LocalScope::addVariable( const std::string& i_name )
{
    const bool isLocal = std::find( variables_.cbegin(), variables_.cend(), i_name ) != variables_.cend();
    const bool isGlobal = GlobalScope::instance().has( i_name );

    if (isLocal || isGlobal) throw std::logic_error{ "Cannot add local variable!" };

    variables_.push_back( i_name );
    return std::next( variables_.cbegin(), variables_.size() - 1 );
}

bool LocalScope::useArray( const std::string& i_name ) const
{
    const bool isLocal = std::find( variables_.cbegin(), variables_.cend(), i_name ) != variables_.cend();

    if (isLocal) return false;

    const bool result = GlobalScope::instance().use( i_name, IDType::Array );

    return result;
}

bool LocalScope::declareArray( const std::string& i_name ) const
{
    const bool isLocal = std::find( variables_.cbegin(), variables_.cend(), i_name ) != variables_.cend();

    if (isLocal) return false;

    const bool result = GlobalScope::instance().declare( i_name, IDType::Array );

    return result;
}

bool LocalScope::declareVariable( const std::string& i_name ) const
{
    const bool isLocal = std::find( variables_.cbegin(), variables_.cend(), i_name ) != variables_.cend();

    if (isLocal) return true;

    const bool result = GlobalScope::instance().declare( i_name, IDType::Variable );

    return result;
}

bool LocalScope::useProc( const std::string& i_name ) const
{
    const bool isLocal = std::find( variables_.cbegin(), variables_.cend(), i_name ) != variables_.cend();

    if (isLocal) return false;

    const bool result = GlobalScope::instance().use( i_name, IDType::Procedure );

    return result;
}

bool LocalScope::declareProc( const std::string& i_name ) const
{
    const bool isLocal = std::find( variables_.cbegin(), variables_.cend(), i_name ) != variables_.cend();

    if (isLocal) return false;

    const bool result = GlobalScope::instance().declare( i_name, IDType::Procedure );

    return result;
}

LocalScope& LocalScope::addScope()
{
    scopes_.emplace_back( variables_ );

    return scopes_.back();
}

Id parseId( const Node& i_node, const LocalScope& i_scope );
OperandInt::Value parseInt( const Node& i_node, const LocalScope& i_scope )
{
    DBG( '!' );
    if (const auto plex = boost::get< lex >( &i_node.value_ )) {
        assert( lex::type::INT_CONST == plex->type_ );
        return boost::get< uint >( plex->value_ );
    } else {
        const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
        assert( 1 == nodes.size() );

        return parseId( nodes[ 0 ], i_scope );
    }
}

OperandInt::OperandInt( const Node& i_node, const LocalScope& i_scope )
    : RuleAssertion{ i_node }
    , value_{ parseInt( i_node, i_scope ) }
{
    DBG( '!' );
}

ArrayElement::ArrayElement( const Node& i_node, const LocalScope& i_scope )
    : RuleAssertion{ i_node }
    , rhs_{ boost::get< Node::Nodes >(boost::get< Node::Nodes >( i_node.value_ )[ 1].value_)[0], i_scope }
{
    DBG( '!' );
    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( 2 == nodes.size() );

    {
        const auto& node = nodes[ 0 ];
        RuleAssertion< lex::rule::NEW_IDENTIFIER > assertion{ node };

        const auto ll = boost::get< lex >( node.value_ );
        assert( lex::type::IDENTIFIER == ll.type_ );

        lhs_ = boost::get< std::string >( ll.value_ );
        if (!i_scope.useArray( lhs_ ))
            throw std::logic_error{ "Cannot use array!" };
    }
}

Id parseId( const Node& i_node, const LocalScope& i_scope )
{
    DBG( '!' );
    RuleAssertion< lex::rule::ID > assertion{ i_node };

    if (const auto plex = boost::get< lex >( &i_node.value_ )) {
        assert( lex::type::IDENTIFIER == plex->type_ );
        const std::string result = boost::get< std::string >( plex->value_ );
        if (!i_scope.useVariable( result ))
            throw std::logic_error{ "Cannot use variable!" };

        return result;
    } else {
        return ArrayElement{ i_node, i_scope };
    }
}

OperandBool::OperandBool( const Node& i_node, const LocalScope& i_scope )
    : RuleAssertion{ i_node }
{
    DBG( '!' );
    if (const auto plex = boost::get< lex >( &i_node.value_ )) {
        assert( lex::type::BOOL_CONST == plex->type_ );
        value_ = boost::get< bool >( plex->value_ );
    } else {
        const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
        assert( 1 == nodes.size() );

        value_ = parseId( nodes[ 0 ], i_scope );
    }
}

OperandStr::OperandStr( const Node& i_node, const LocalScope& i_scope )
    : RuleAssertion{ i_node }
{
    DBG( '!' );
    if (const auto plex = boost::get< lex >( &i_node.value_ )) {
        assert( lex::type::STR_CONST == plex->type_ );
        value_ = boost::get< std::string >( plex->value_ );
    } else {
        const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
        assert( 1 == nodes.size() );

        value_ = parseId( nodes[ 0 ], i_scope );
    }
}

IntExpr::Op parseOp( const Node& op_node )
{
    DBG( '!' );
    RuleAssertion< lex::rule::OPERATOR_INT > assertion{ op_node };

    const lex op_lexeme = boost::get< lex >( op_node.value_ );
    assert( lex::type::SYMBOL == op_lexeme.type_ );
    switch (boost::get< lex::symbol >(op_lexeme.value_)) {
        case lex::symbol::PLUS:
            return IntExpr::Op::PLUS;
            break;
        case lex::symbol::MINUS:
            return IntExpr::Op::MINUS;
            break;
        case lex::symbol::STAR:
            return IntExpr::Op::MULT;
            break;
        case lex::symbol::SLASH:
            return IntExpr::Op::DIV;
            break;
        default:
            DEBUG( op_lexeme);
            assert( !"Fail!" );
    }
}

IntExpr::IntExpr( const Node& i_node, const LocalScope& i_scope )
    : RuleAssertion{ i_node }
    , lhs_{ boost::get< Node::Nodes >( i_node.value_ )[ 0 ], i_scope }
{
    DBG( '!' );
    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( !nodes.empty());

    if (nodes.size() == 3) {
        op_ = parseOp( nodes[ 1 ] );
        rhs_ = OperandInt{ nodes[ 2], i_scope };
    } else if (nodes.size() != 1) {
        throw std::logic_error{"A"};
    }
}

Cmp parseCmp( const Node& i_node )
{
    DBG( '!' );
    assert( (lex::rule::LOGIC_EQUALS == i_node.rule_) || (lex::rule::COMPARE_INT == i_node.rule_ ));

    const bool HAS_ORDER = lex::rule::COMPARE_INT == i_node.rule_;

    const lex op_lexeme = boost::get< lex >( i_node.value_ );
    assert( lex::type::SYMBOL == op_lexeme.type_ );

    switch (boost::get< lex::symbol >(op_lexeme.value_)) {
        case lex::symbol::LESS:
            assert( HAS_ORDER );
            return Cmp::LE;
            break;
        case lex::symbol::GREATER:
            assert( HAS_ORDER );
            return Cmp::GE;
            break;
        case lex::symbol::EQUAL:
            return Cmp::EQ;
            break;
        case lex::symbol::NOT_EQUAL:
            return Cmp::NE;
            break;
        default:
            DEBUG( op_lexeme);
            assert( !"Fail!" );
    }
}

LogicBool::LogicBool( const Node& i_node, const LocalScope& i_scope )
    : RuleAssertion{ i_node }
    , lhs_{ boost::get< Node::Nodes >( i_node.value_ )[ 0 ], i_scope }
{
    DBG( '!' );
    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( !nodes.empty());

    if (nodes.size() == 3) {
        cmp_ = parseCmp( nodes[ 1 ] );
        rhs_ = OperandBool{ nodes[ 2], i_scope };
    } else if (nodes.size() != 1) {
        throw std::logic_error{"A"};
    }
}

LogicInt::LogicInt( const Node& i_node, const LocalScope& i_scope )
    : RuleAssertion{ i_node }
    , lhs_{ boost::get< Node::Nodes >( i_node.value_ )[ 0 ], i_scope }
    , cmp_{ parseCmp( boost::get< Node::Nodes >( i_node.value_ )[ 1 ]) }
    , rhs_{ boost::get< Node::Nodes >( i_node.value_ )[ 2 ], i_scope }
{
    DBG( '!' );
    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( 3 == nodes.size() );

    // lhs_ = OperandInt{ nodes[ 0 ], i_scope };
    // cmp_ = parseCmp( nodes[ 1 ] );
    // rhs_ = OperandInt{ nodes[ 2], i_scope };
}

LogicStr::LogicStr( const Node& i_node, const LocalScope& i_scope )
    : RuleAssertion{ i_node }
    , lhs_{ boost::get< Node::Nodes >( i_node.value_ )[ 0 ], i_scope }
    , cmp_{ parseCmp( boost::get< Node::Nodes >( i_node.value_ )[ 1 ]) }
    , rhs_{ boost::get< Node::Nodes >( i_node.value_ )[ 2 ], i_scope }
{
    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( 3 == nodes.size() );

    // lhs_ = OperandStr{ nodes[ 0 ], i_scope };
    // cmp_ = parseCmp( nodes[ 1 ] );
    // rhs_ = OperandStr{ nodes[ 2], i_scope };
}

Logic parseLogic( const Node& i_node, const LocalScope& i_scope )
{
    DBG( '!' );
    RuleAssertion< lex::rule::LOGIC > assertion{ i_node };

    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( 1 == nodes.size() );

    const auto& child = nodes[ 0 ];
    switch (child.rule_) {
        case lex::rule::LOGIC_INT:
            return LogicInt{ child, i_scope };
        case lex::rule::LOGIC_BOOL:
            return LogicBool{ child, i_scope };
        case lex::rule::LOGIC_STR:
            return LogicStr{ child, i_scope };
        default:
            DEBUG( child.rule_ );
            assert( !"Wrong child node!" );
    }
}

Input::Input( const Node& i_node )
    : RuleAssertion{ i_node }
{
    DBG( '!' );
    assert( lex::type::EPS == boost::get< lex >( i_node.value_ ).type_ );
}

Rightside parseRightside( const Node& i_node, const LocalScope& i_scope )
{
    DBG( '!' );
    RuleAssertion< lex::rule::RIGHTSIDE > assertion{ i_node };

    if (const auto nodes = boost::get< Node::Nodes >( &i_node.value_ ) ) {
        assert( 1 == nodes->size() );

        const auto& child = (*nodes)[ 0 ];
        switch (child.rule_) {
            case lex::rule::LOGIC:
                return parseLogic( child, i_scope );
            case lex::rule::INT_EXPR:
                return IntExpr{ child, i_scope };
            case lex::rule::READ_STMT:
                return Input{ child };
            default:
                DEBUG( child.rule_ );
                assert( !"Wrong child node!" );
        }
    } else {
        const auto ll = boost::get< lex >( i_node.value_ );
        assert( lex::type::STR_CONST == ll.type_ );

        return boost::get< std::string >( ll.value_ );
    }
}

Assignment::Assignment( const Node& i_node, const LocalScope& i_scope )
    : RuleAssertion{ i_node }
    , lhs_{ parseId( boost::get< Node::Nodes >( i_node.value_ )[ 0 ], i_scope ) }
    , rhs_{ parseRightside( boost::get< Node::Nodes >( i_node.value_ )[ 1 ], i_scope ) }
{
    DBG( '!' );
    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( 2 == nodes.size() );

    // lhs_ = parseId( nodes[ 0 ], i_scope );
    // rhs_ = parseRightside( nodes[ 1 ], i_scope );
    //
    bool result = false;
    if (const auto lhs_id = boost::get< std::string>( &lhs_ )) {
        result = i_scope.declareVariable( *lhs_id );
    } else {
        const auto arr = boost::get< ArrayElement >( lhs_ );
        result = i_scope.declareArray( arr.lhs_ );
    }

    if (!result)
        throw std::logic_error{ "Cannot declare variable or array!" };
}

Goto::Goto( const Node& i_node )
    : RuleAssertion{ i_node }
{
    DBG( '!' );
    const auto ll = boost::get< lex >( i_node.value_ );
    assert( lex::type::INT_CONST == ll.type_ );

    to_ = boost::get< uint >( ll.value_ );
    if (!GlobalScope::instance().addUsedLabel( to_ ))
        throw std::logic_error{ "Cannot use label!" };
}

SubCall parseSubCall( const Node& i_node, const LocalScope& i_scope )
{
    DBG( '!' );
    RuleAssertion< lex::rule::SUB_CALL_STMT > assertion{ i_node };

    const auto ll = boost::get< lex >( i_node.value_ );
    assert( lex::type::IDENTIFIER == ll.type_ );

    const std::string subName = boost::get< std::string >( ll.value_ );

    if (!i_scope.useProc( subName ))
        throw std::logic_error{ "Cannot call procedure!" };

    return subName;
}

Write::Write( const Node& i_node, const LocalScope& i_scope )
    : RuleAssertion{ i_node }
    , rhs_{ parseRightside( boost::get< Node::Nodes >( i_node.value_ )[ 0 ], i_scope ) }
{
    DBG( '!' );
    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( 1 == nodes.size() );

    // rhs_ = parseRightside( nodes[ 0 ], i_scope );
}

Dim::Dim( const Node& i_node, LocalScope& i_scope )
    : RuleAssertion{ i_node }
{
    DBG( '!' );
    const auto ll = boost::get< lex >( i_node.value_ );
    assert( lex::type::IDENTIFIER == ll.type_ );

    const std::string varName = boost::get< std::string >( ll.value_ );
    var_ = i_scope.addVariable( varName );
}

Sline parseSline( const Node& i_node, LocalScope& i_scope )
{
    DBG( '!' );
    const RuleAssertion< lex::rule::STMT > assertion{ i_node };

    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( 1 == nodes.size() );

    const auto& child = nodes[ 0 ];
    switch (child.rule_) {
        case lex::rule::DIM_EXPR:
            return Dim{ child, i_scope };
        case lex::rule::ASSIGNMENT:
            return Assignment{ child, i_scope };
        case lex::rule::WRITE_STMT:
            return Write{ child, i_scope };
        case lex::rule::IF_STMT:
            return If{ child, i_scope };
        case lex::rule::WHILE_STMT:
            return While{ child, i_scope };
        case lex::rule::SUB_CALL_STMT:
            return parseSubCall( child, i_scope );
        case lex::rule::GOTO_STMT:
            return Goto{ child };
        default:
            DEBUG( child.rule_ );
            assert( !"Wrong child node!" );
    }
}

boost::optional< uint > parseLabel( const Node& i_node )
{
    DBG( '!' );
    RuleAssertion< lex::rule::LABEL_DEF > assertion{ i_node };

    const auto ll = boost::get< lex >( i_node.value_ );
    if (lex::type::INT_CONST == ll.type_) {
        const auto la = boost::get< uint >( ll.value_ );
        if (!GlobalScope::instance().addDeclaredLabel( la ))
            throw std::logic_error{ "CANNOT REDEFINE LABEL!" };

        return la;
    } else
        assert( lex::type::EPS == ll.type_ );

    return {};
}

boost::optional< Sline > parseSlineO( const Node& i_node, LocalScope& i_scope )
{
    DBG( '!' );
    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( !nodes.empty());

    if (2 == nodes.size())
        return parseSline( nodes[ 1 ], i_scope );
    else
        assert( false );

    return {};
}


Line::Line( const Node& i_node, LocalScope& i_scope )
    : RuleAssertion{ i_node }
    , label_{ parseLabel( boost::get< Node::Nodes >( i_node.value_)[0] ) }
    , line_{ parseSlineO( i_node, i_scope ) }
{
    DBG( '!' );
    // const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    // assert( !nodes.empty());
    //
    // label_ = parseLabel( nodes[ 0 ] );
    // if (2 == nodes.size())
    //     line_ = parseSline( nodes[ 1 ], i_scope );
    // else
    //     assert( false );
}

Lines parseLines( const Node& i_node, LocalScope& i_scope )
{
    DBG( '!' );
    const RuleAssertion< lex::rule::STMTS > assertion{ i_node };

    Lines result;

    auto pnode = &i_node;
    while (const auto* nodes = boost::get< Node::Nodes >( &pnode->value_ )) {
        assert( 2 == nodes->size() );

        result.emplace_back( (*nodes)[ 0 ], i_scope );

        const RuleAssertion< lex::rule::MORE_STMTS > assertion{ (*nodes)[1] };
        pnode = &(*nodes)[1];
        if (const auto subnodes = boost::get< Node::Nodes >( &pnode->value_)) {
            assert( subnodes->size() == 1);
            pnode = &(*subnodes)[ 0 ];
            const RuleAssertion< lex::rule::STMTS > assertion( *pnode );
        } else {
            break;
        }
    }

    assert( lex::type::EPS == boost::get< lex >( pnode->value_ ).type_ );

    return result;
}


Lines parseElse( const Node& i_node, LocalScope& i_scope )
{
    DBG( '!' );
    const RuleAssertion< lex::rule::ELSE_PART > assertion{ i_node };

    if (const auto nodes = boost::get< Node::Nodes >( &i_node.value_ )) {
        assert( 1 == nodes->size() );
        return parseLines( (*nodes)[ 0 ], i_scope );
    }

    assert( lex::type::EPS == boost::get< lex >( i_node.value_ ).type_ );

    return {};
}

If::If( const Node& i_node, LocalScope& i_scope )
    : RuleAssertion{ i_node }
    , logic_{ parseLogic( boost::get< Node::Nodes >( i_node.value_ )[ 0 ], i_scope ) }
    , thenScope_( i_scope.addScope() )
    , then_( parseLines( boost::get< Node::Nodes >( i_node.value_ )[ 1 ], thenScope_ ) )
    , elseScope_( i_scope.addScope() )
    , else_( parseElse( boost::get< Node::Nodes >( i_node.value_ )[ 2 ], elseScope_ ) )
{
    DBG( '!' );
}

While::While( const Node& i_node, LocalScope& i_scope )
    : RuleAssertion{ i_node }
    , logic_{ parseLogic( boost::get< Node::Nodes >( i_node.value_ )[ 0 ], i_scope ) }
    , localScope_( i_scope.addScope() )
    , body_( parseLines( boost::get< Node::Nodes >( i_node.value_ )[ 1 ], localScope_ ) )
{
    DBG( '!' );
}

std::string parseSubName( const Node& i_node, LocalScope& i_scope )
{
    DBG( '!' );
    const RuleAssertion< lex::rule::NEW_IDENTIFIER > assertion{ i_node };

    const auto ll = boost::get< lex >( i_node.value_ );
    assert( lex::type::IDENTIFIER == ll.type_ );

    const std::string subName = boost::get< std::string >( ll.value_ );
    if (!i_scope.declareProc( subName ))
        throw std::logic_error{ "Cannot declare procedure!" };

    return subName;
}

SubDecl::SubDecl( const Node& i_node, LocalScope& i_scope )
    : RuleAssertion{ i_node }
    , name_{ parseSubName( boost::get< Node::Nodes >( i_node.value_ )[ 0], i_scope ) }
    , local_{ i_scope.addScope() }
    , body_{ parseLines( boost::get< Node::Nodes >( i_node.value_ )[ 1 ], local_ ) }
{
    DBG( '!' );
}

AnyLine parseAnyLine( const Node& i_node, LocalScope& i_scope )
{
    DBG( '!' );
    const RuleAssertion< lex::rule::ALL_STMT > assertion{ i_node };

    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( 1 == nodes.size() );

    const auto& child = nodes[ 0 ];
    if (lex::rule::SUB_STMT == child.rule_)
        return SubDecl{ child, i_scope };
    else
        return Line{ child, i_scope };
}

AnyLines parseAnyLines( const Node& i_node, LocalScope& i_scope )
{
    DBG( '!' );
    const RuleAssertion< lex::rule::ALL_STMTS > assertion{ i_node };

    AnyLines result;

    auto pnode = &i_node;
    while (const auto* nodes = boost::get< Node::Nodes >( &pnode->value_ )) {
        assert( 2 == nodes->size() );

        result.emplace_back( parseAnyLine( (*nodes)[ 0 ], i_scope ) );

        const RuleAssertion< lex::rule::MORE_ALL_STMTS > assertion{ (*nodes)[1] };
        pnode = &(*nodes)[1];
        if (const auto subnodes = boost::get< Node::Nodes >( &pnode->value_)) {
            assert( subnodes->size() == 1);
            pnode = &(*subnodes)[ 0 ];
            const RuleAssertion< lex::rule::ALL_STMTS > assertion( *pnode );
        } else {
            break;
        }
    }

    assert( lex::type::EPS == boost::get< lex >( pnode->value_ ).type_ );

    return result;
}

Program::Program( const Node& i_node )
    : RuleAssertion{ i_node }
    , body_( parseAnyLines( boost::get< Node::Nodes >( i_node.value_ )[ 0 ]
                          , GlobalScope::instance().scope_ ) )
{
    DBG( '!' );
}

void getId( const Id& id, ProgramCode& code)
{
    if (const auto s = boost::get< std::string >( &id )) {
        code.push( *s );
    } else if (const auto s = boost::get< ArrayElement >( &id )) {
        (*s)( code );
    } else
        assert( !"Cannot generate code for ID!" );
}

void ArrayElement::operator()( ProgramCode& code ) const
{
    code.push( lhs_ );
    rhs_( code );
    code.push( "]" );
}

void OperandInt::operator()( ProgramCode& code ) const
{
    if (const auto s = boost::get< int >( &value_ )) {
        code.push( boost::lexical_cast< std::string >( *s ) );
    } else if (const auto s = boost::get< Id >( &value_ )) {
        getId( *s, code );
    } else
        assert( !"Cannot generate code for OperandInt!" );
}

void OperandBool::operator()( ProgramCode& code ) const
{
    if (const auto s = boost::get< bool >( &value_ )) {
        code.push( (*s) ? "true" : "false" );
    } else if (const auto s = boost::get< Id >( &value_ )) {
        getId( *s, code );
    } else
        assert( !"Cannot generate code for OperandBool!" );
}

void OperandStr::operator()( ProgramCode& code ) const
{
    if (const auto s = boost::get< std::string >( &value_ )) {
        std::stringstream ss;
        ss << '"' << *s << '"';
        code.push( ss.str() );
    } else if (const auto s = boost::get< Id >( &value_ )) {
        getId( *s, code );
    } else
        assert( !"Cannot generate code for OperandStr!" );
}

std::string getOp( IntExpr::Op i_op )
{
    switch (i_op) {
        case IntExpr::Op::PLUS: return "+";
        case IntExpr::Op::MINUS: return "-";
        case IntExpr::Op::MULT: return "*";
        case IntExpr::Op::DIV: return "/";
        default: assert( !"No such OP!" );
    }
}

void IntExpr::operator()( ProgramCode& code ) const
{
    lhs_( code );
    if ( op_ && rhs_ ) {
        (*rhs_)( code );
        code.push( getOp( *op_ ) );
    }
}

std::string getCmp( Cmp i_cmp )
{
    switch (i_cmp) {
        case Cmp::EQ: return "==";
        case Cmp::NE: return "!=";
        case Cmp::LE: return "<";
        case Cmp::GE: return ">";
        default: assert( !"No such OP!" );
    }
}

void LogicBool::operator()( ProgramCode& code ) const
{
    lhs_( code );
    if ( cmp_ && rhs_ ) {
        (*rhs_)( code );
        code.push( getCmp( *cmp_ ) );
    }
}

void LogicInt::operator()( ProgramCode& code ) const
{
    lhs_( code );
    rhs_( code );
    code.push( getCmp( cmp_ ) );
}

void LogicStr::operator()( ProgramCode& code ) const
{
    lhs_( code );
    rhs_( code );
    code.push( getCmp( cmp_ ) );
}

void getLogic( const Logic& logic, ProgramCode& code )
{
    if (const auto d = boost::get< LogicBool >( &logic )) {
        (*d)( code );
    } else if (const auto d = boost::get< LogicInt >( &logic )) {
        (*d)( code );
    } else if (const auto d = boost::get< LogicStr >( &logic )) {
        (*d)( code );
    } else assert( !"Cannot generate code for LOGIC element!" );
}

void Input::operator()( ProgramCode& code ) const
{
    code.push( "READ" );
}

void getRightside( const Rightside& rhs, ProgramCode& code)
{
    if (const auto d = boost::get< Logic >( &rhs )) {
        getLogic( *d, code );
    } else if (const auto d = boost::get< IntExpr >( &rhs )) {
        (*d)( code );
    } else if (const auto d = boost::get< Input >( &rhs )) {
        (*d)( code );
    } else if (const auto d = boost::get< std::string >( &rhs )) {
        std::stringstream ss;
        ss << '"' << *d << '"';
        code.push( ss.str() );
    } else assert( !"Cannot generate code for RIGHTSIDE element!" );
}

void Assignment::operator()( ProgramCode& code ) const
{
    getId( lhs_, code );
    getRightside( rhs_, code );
    code.push( ":=" );
}

void Write::operator()( ProgramCode& code ) const
{
    getRightside( rhs_, code );
    code.push( "Write" );
}

void Goto::operator()( ProgramCode& code ) const
{
    code.push( boost::lexical_cast< std::string >( to_ ) );
    code.push( "BR" );
}

void getSubCall( const SubCall& s, ProgramCode& code )
{
    code.push( s );
    code.push( "Fn" );
}

void getSline( const Sline& line, ProgramCode& code )
{
    if (const auto d = boost::get< Assignment >( &line )) {
        ( *d )( code );
    } else if (const auto d = boost::get< SubCall >( &line )) {
        getSubCall(*d, code );
    } else if (const auto d = boost::get< Goto >( &line )) {
        (*d)(code);
    } else if (const auto d = boost::get< Write >( &line )) {
        (*d)(code);
    } else if (const auto d = boost::get< If >( &line )) {
        (*d)(code);
    } else if (const auto d = boost::get< While >( &line )) {
        (*d)(code);
    } else if (boost::get< Dim >( &line )) {
        // NOP
    } else assert( !"Cannot generate code for Sline element!" );
}

void Line::operator()( ProgramCode& code ) const
{
    if (label_) {
        std::stringstream ss;
        ss << *label_ << ":";
        code.push( ss.str() );
    }

    if (line_) {
        getSline( *line_, code );
    }
}

void getLines( const Lines& lines, ProgramCode& code )
{
    code.push( "BLOCK" );
    for ( const auto& line : lines ) {
        line( code );
    }
    code.push( "BLCKEND" );
}

uint GlobalScope::addLabel()
{
    const uint last = declaredLabels_.empty() ? 0 :*declaredLabels_.rbegin();
    const uint result = last + 1;

    if (addDeclaredLabel( result ) ) return result;
    else return 0;
}

void If::operator()( ProgramCode& code ) const
{
    getLogic( logic_, code );
    const uint elseLabel = GlobalScope::instance().addLabel();

    code.push( boost::lexical_cast< std::string >( elseLabel ) );
    code.push( "if" );
    code.push( "false" );

    getLines( then_, code );

    const uint ifLabel = GlobalScope::instance().addLabel();

    code.push( boost::lexical_cast< std::string >( ifLabel ) );
    code.push( "BR" );

    {
        std::stringstream ss;
        ss << elseLabel << ':';
        code.push( ss.str() );
    }

    getLines( else_, code );

    {
        std::stringstream ss;
        ss << ifLabel << ':';
        code.push( ss.str() );
    }
}

void While::operator()( ProgramCode& code ) const
{
    const uint logicLabel = GlobalScope::instance().addLabel();
    {
        std::stringstream ss;
        ss << logicLabel << ':';
        code.push( ss.str() );
    }

    getLogic( logic_, code );

    const uint elseLabel = GlobalScope::instance().addLabel();

    code.push( boost::lexical_cast< std::string >( elseLabel ) );
    code.push( "if" );
    code.push( "false" );

    getLines( body_, code );

    code.push( boost::lexical_cast< std::string >( logicLabel ) );
    code.push( "BR" );

    {
        std::stringstream ss;
        ss << elseLabel << ':';
        code.push( ss.str() );
    }
}

void SubDecl::operator()( ProgramCode& code ) const
{
    {
        std::stringstream ss;
        ss << name_ << ':';
        code.push( ss.str() );
    }
    getLines( body_, code );
}

void getAnyLine( const AnyLine& line, ProgramCode& code )
{
    if (const auto d = boost::get< Line >( &line )) {
        ( *d )( code );
    } else if (const auto d = boost::get< SubDecl >( &line )) {
        (*d)( code );
    } else assert( !"Cannot generate code for AnyLine element!" );
}

void Program::operator()( ProgramCode& code ) const
{
    code.push( "BEGIN_OF_PROGRAM" );

    for (const auto& line : body_ )
        getAnyLine( line, code );

    code.push( "END_OF_PROGRAM" );
}
} // namespace sap
