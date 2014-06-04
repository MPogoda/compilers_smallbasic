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

namespace sap
{
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
            return true;
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
    if ( isUsed ) {
        if ( used_[ i_name ] != i_type ) {
            return false;
        } else {
            return true;
        }
    }

    used_.insert( { i_name, i_type } );
    return true;
}

bool GlobalScope::has( const std::string& i_name ) const
{
    const bool isDeclared = declared_.find( i_name ) != declared_.cend();
    const bool isUsed = used_.find( i_name ) != used_.cend();

    return isDeclared || isUsed;
}

bool GlobalScope::checkNames() const
{
    return used_.empty();
}

bool GlobalScope::addDeclaredLabel( uint i_label )
{
    if (declaredLabels_.find( i_label ) != declaredLabels_.cend()) {
        return false;
    } else {
        declaredLabels_.insert( i_label );
        return true;
    }
}

bool GlobalScope::addUsedLabel( uint i_label )
{
    if (usedLabels_.find( i_label ) == declaredLabels_.cend()) {
        declaredLabels_.insert( i_label );
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

bool LocalScope::useVariable( const std::string& i_name ) const
{
    const bool isLocal = std::find( variables_.cbegin(), variables_.cend(), i_name ) != variables_.cend();

    if (isLocal) return true;
    else return GlobalScope::instance().use( i_name, IDType::Variable );
}

bool LocalScope::addVariable( const std::string& i_name )
{
    const bool isLocal = std::find( variables_.cbegin(), variables_.cend(), i_name ) != variables_.cend();
    const bool isGlobal = GlobalScope::instance().has( i_name );

    if (isLocal || isGlobal) return false;

    variables_.push_back( i_name );
    return true;
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
LocalScope& LocalScope::addScope()
{
    scopes_.emplace_back( variables_ );

    return scopes_.back();
}

Id parseId( const Node& i_node, const LocalScope& i_scope );

OperandInt::OperandInt( const Node& i_node, const LocalScope& i_scope )
    : RuleAssertion{ i_node }
{
    if (const auto plex = boost::get< lex >( &i_node.value_ )) {
        assert( lex::type::INT_CONST == plex->type_ );
        value_ = boost::get< uint >( plex->value_ );
    } else {
        const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
        assert( 1 == nodes.size() );

        value_ = parseId( nodes[ 0 ], i_scope );
    }
}

ArrayElement::ArrayElement( const Node& i_node, const LocalScope& i_scope )
    : RuleAssertion{ i_node }
{
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

    rhs_ = OperandInt{ nodes[ 1 ], i_scope };
}

Id parseId( const Node& i_node, const LocalScope& i_scope )
{
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
{
    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( !nodes.empty());
    lhs_ = OperandInt{ nodes[ 0 ], i_scope };
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
{
    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( !nodes.empty());
    lhs_ = OperandBool{ nodes[ 0 ], i_scope };
    if (nodes.size() == 3) {
        cmp_ = parseCmp( nodes[ 1 ] );
        rhs_ = OperandBool{ nodes[ 2], i_scope };
    } else if (nodes.size() != 1) {
        throw std::logic_error{"A"};
    }
}

LogicInt::LogicInt( const Node& i_node, const LocalScope& i_scope )
    : RuleAssertion{ i_node }
{
    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( 3 == nodes.size() );

    lhs_ = OperandInt{ nodes[ 0 ], i_scope };
    cmp_ = parseCmp( nodes[ 1 ] );
    rhs_ = OperandInt{ nodes[ 2], i_scope };
}

LogicStr::LogicStr( const Node& i_node, const LocalScope& i_scope )
    : RuleAssertion{ i_node }
{
    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( 3 == nodes.size() );

    lhs_ = OperandStr{ nodes[ 0 ], i_scope };
    cmp_ = parseCmp( nodes[ 1 ] );
    rhs_ = OperandStr{ nodes[ 2], i_scope };
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
{
    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( 2 == nodes.size() );

    lhs_ = parseId( nodes[ 0 ], i_scope );
    rhs_ = parseRightside( nodes[ 1 ], i_scope );

    if (const auto lhs_id = boost::get< std::string>( &lhs_ )) {
        i_scope.declareVariable( *lhs_id );
    } else {
        const auto arr = boost::get< ArrayElement >( lhs_ );
        i_scope.declareArray( arr.lhs_ );
    }
}

} // namespace sap
