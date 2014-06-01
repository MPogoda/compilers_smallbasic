#include "program_tree.h"

#include <iostream>

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
namespace
{
lex::rule getRule( const uint i_rule )
{
    switch (i_rule) {
        case 1:
            return lex::rule::START;
        case 2:
        case 3:
            return lex::rule::ALL_STMT;
        case 4:
            return lex::rule::ALL_STMTS;
        case 5:
        case 6:
            return lex::rule::MORE_ALL_STMTS;
        case 7:
            return lex::rule::SUB_STMT;
        case 8:
            return lex::rule::STMTS;
        case 9:
        case 10:
            return lex::rule::MORE_STMTS;
        case 11:
        case 12:
            return lex::rule::LSTMT;
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
            return lex::rule::STMT;
            /* case 20 */
        case 21:
        case 22:
            return lex::rule::LABEL_DEF;
        case 23:
            return lex::rule::DIM_EXPR;
        case 24:
            return lex::rule::ASSIGNMENT;
        case 25:
        case 26:
            return lex::rule::ID;
        case 27:
            return lex::rule::ARRAY_INDEX;
        case 28:
        case 29:
            return lex::rule::OPERAND_INT;
        case 30:
        case 31:
        case 32:
        case 33:
            return lex::rule::RIGHTSIDE;
        case 34:
            return lex::rule::READ_STMT;
        case 35:
        case 36:
            return lex::rule::INT_EXPR;
        case 37:
        case 38:
        case 39:
        case 40:
            return lex::rule::OPERATOR_INT;
        case 41:
        case 42:
        case 43:
            return lex::rule::LOGIC;
        case 44:
            return lex::rule::LOGIC_INT;
        case 45:
        case 46:
        case 47:
        case 20:
            return lex::rule::COMPARE_INT;
        case 48:
        case 49:
            return lex::rule::LOGIC_EQUALS;
        case 50:
        case 51:
            return lex::rule::LOGIC_BOOL;
        case 52:
        case 53:
            return lex::rule::OPERAND_BOOL;
        case 54:
            return lex::rule::LOGIC_STR;
        case 55:
        case 56:
            return lex::rule::OPERAND_STR;
        case 57:
            return lex::rule::IF_STMT;
        case 58:
        case 59:
            return lex::rule::ELSE_PART;
        case 60:
            return lex::rule::GOTO_STMT;
        case 61:
            return lex::rule::WHILE_STMT;
        case 62:
            return lex::rule::WRITE_STMT;
        case 63:
        case 64:
        case 65:
            return lex::rule::WRITEABLE;
        case 66:
            return lex::rule::SUB_CALL_STMT;
        case 67:
            return lex::rule::NEW_IDENTIFIER;
        default:
            assert( !"No such rule!" );
    }
}

int getCount( const uint i_rule )
{
    switch (i_rule) {
        case 1:
        case 2:
        case 3:
            return 1;
        case 4:
            return 2;
        case 5:
            return 1;
        case 6:
            return -1;
        case 7:
        case 8:
            return 2;
        case 9:
            return 1;
        case 10:
            return -1;
        case 11:
            return 2;
        case 12:
            return 1;
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
            return 1;
        case 21:
            return 0;
        case 22:
            return -1;
        case 23:
            return 0;
        case 24:
        case 25:
            return 2;
        case 26:
            return 0;
        case 27:
        case 28:
            return 1;
        case 29:
            return 0;
        case 30:
        case 31:
        case 32:
            return 1;
        case 33:
            return 0;
        case 34:
            return -1;
        case 35:
            return 3;
        case 36:
            return 1;
        case 37:
        case 38:
        case 39:
        case 40:
            return 0;
        case 41:
        case 42:
        case 43:
            return 1;
        case 44:
            return 3;
        case 45:
        case 46:
        case 47:
        case 20:
        case 48:
        case 49:
            return 0;
        case 50:
            return 3;
        case 51:
        case 52:
            return 1;
        case 53:
            return 0;
        case 54:
            return 3;
        case 55:
            return 1;
        case 56:
            return 0;
        case 57:
            return 3;
        case 58:
            return 1;
        case 59:
            return -1;
        case 60:
            return 0;
        case 61:
            return 2;
        case 62:
        case 63:
        case 64:
            return 1;
        case 65:
        case 66:
        case 67:
            return 0;
        default:
            assert( !"No such rule!" );
    }
}

// should we skip lexeme
bool shouldSkip( const lex i_lhs ) {
    switch (i_lhs.type_) {
        case lex::type::SYMBOL:
            switch (boost::get< lex::symbol >( i_lhs.value_ )) {
                case lex::symbol::L_PARENTHESIS:
                case lex::symbol::R_PARENTHESIS:
                case lex::symbol::L_BRACKET:
                case lex::symbol::R_BRACKET:
                case lex::symbol::COLON:
                    return true;
                case lex::symbol::PLUS:
                case lex::symbol::MINUS:
                case lex::symbol::STAR:
                case lex::symbol::SLASH:
                case lex::symbol::EQUAL:
                case lex::symbol::LESS:
                case lex::symbol::GREATER:
                case lex::symbol::NOT_EQUAL:
                    return false;
                default:
                    assert(!"No such symbol!");
            }
        case lex::type::INT_CONST:
        case lex::type::BOOL_CONST:
        case lex::type::STR_CONST:
        case lex::type::IDENTIFIER:
            return false;
        case lex::type::NEWLINE:
        case lex::type::RESERVED:
            return true;
        default:
            assert(!"No such lexeme!");
    }

    return false;
}

lex skip( LIterator& i_it )
{
    while (shouldSkip( *i_it ))
        ++i_it;
    return *i_it++;
}

class NodePrinter
{
public:
    void print( const Node& root )
    {
        p_printLevel();

        out_ << root.rule_;
        if (const lex* lhs = boost::get< lex >( &root.value_ )) {
            out_ << " == " << *lhs << '\n';
        } else if (const Node::Nodes* nodes = boost::get< Node::Nodes >( &root.value_)) {
            ++level_;
            out_ << '\n';
            for (const auto& nd : *nodes ) {
                print( nd );
            }
            --level_;
        }
    }

    NodePrinter( std::ostream& i_out ) : out_( i_out ) { }
private:
    uint level_ = 0;
    std::ostream& out_;

    void p_printLevel() const
    {
        for (uint i = 0; level_ != i; ++i) {
            out_ << "> ";
        }
    }
};
} // namespace
Node::Node( Queue& i_queue, LIterator& i_it )
    : Assertion{ !i_queue.empty() }
    , rule_( getRule( i_queue.front() ) )
{
    const auto rule = i_queue.front();
    i_queue.pop();

    const auto N = getCount( rule );
    if (0 > N) {
        DBG( "It's EPSILON!" );
        value_ = lex{ lex::type::EPS, false };
    } else if (0 == N) {
        DBG( "It's LEAF!" );
        value_ = skip( i_it );
    } else {
        DBG( "It's NOT Leaf!" );
        DEBUG( N );
        auto nodes = Nodes( );
        nodes.reserve( N );

        for (int i = 0; N != i; ++i) {
            nodes.emplace_back( Node{ i_queue, i_it } );
            if ((0 == i) && (lex::rule::ASSIGNMENT == rule_)) {
                skip( i_it );
            }
        }

        value_ = nodes;
    }
}

const lex& Node::getLex() const
{
    return boost::get< lex >( value_ );
}

const Node::Nodes& Node::getNodes() const
{
    return boost::get< Nodes >( value_ );
}

std::ostream& operator<<( std::ostream& i_out, const Node& i_node )
{
    NodePrinter{ i_out }.print( i_node );

    return i_out;
}
} // namespace sap
