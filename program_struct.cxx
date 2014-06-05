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
    const auto ll = boost::get< lex >( i_node.value_ );
    assert( lex::type::INT_CONST == ll.type_ );

    to_ = boost::get< uint >( ll.value_ );
    if (!GlobalScope::instance().addUsedLabel( to_ ))
        throw std::logic_error{ "Cannot use label!" };
}

SubCall parseSubCall( const Node& i_node, const LocalScope& i_scope )
{
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
{
    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( 1 == nodes.size() );

    rhs_ = parseRightside( nodes[ 0 ], i_scope );
}

Dim::Dim( const Node& i_node, LocalScope& i_scope )
    : RuleAssertion{ i_node }
{
    const auto ll = boost::get< lex >( i_node.value_ );
    assert( lex::type::IDENTIFIER == ll.type_ );

    const std::string varName = boost::get< std::string >( ll.value_ );
    var_ = i_scope.addVariable( varName );
}

Sline parseSline( const Node& i_node, LocalScope& i_scope )
{
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
    RuleAssertion< lex::rule::LABEL_DEF > assertion{ i_node };

    const auto ll = boost::get< lex >( i_node.value_ );
    if (lex::type::INT_CONST == ll.type_)
        return boost::get< uint >( ll.value_ );
    else
        assert( lex::type::EPS == ll.type_ );

    return {};
}

Line::Line( const Node& i_node, LocalScope& i_scope )
    : RuleAssertion{ i_node }
{
    const auto& nodes = boost::get< Node::Nodes >( i_node.value_ );
    assert( !nodes.empty());

    label_ = parseLabel( nodes[ 0 ] );
    if (2 == nodes.size())
        line_ = parseSline( nodes[ 1 ], i_scope );
    else
        assert( false );
}

Lines parseLines( const Node& i_node, LocalScope& i_scope )
{
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
}

While::While( const Node& i_node, LocalScope& i_scope )
    : RuleAssertion{ i_node }
    , logic_{ parseLogic( boost::get< Node::Nodes >( i_node.value_ )[ 0 ], i_scope ) }
    , localScope_( i_scope.addScope() )
    , body_( parseLines( boost::get< Node::Nodes >( i_node.value_ )[ 1 ], localScope_ ) )
{
}

std::string parseSubName( const Node& i_node, LocalScope& i_scope )
{
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
}

AnyLine parseAnyLine( const Node& i_node, LocalScope& i_scope )
{
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
    , local_{ { } }
    , body_{ parseAnyLines( boost::get< Node::Nodes >( i_node.value_ )[ 0 ], local_ ) }
{
}

} // namespace sap
