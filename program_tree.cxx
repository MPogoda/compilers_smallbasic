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
        case 44:
        case 45:
        case 46:
        case 47:
        case 48:
        case 49:
        case 50:
        case 51:
        case 52:
        case 53:
        case 54:
        case 55:
        case 56:
        case 57:
        case 58:
        case 59:
        case 60:
        case 61:
        case 62:
        case 63:
        case 64:
        case 65:
        case 66:
        case 67:
        case 68:
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
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
        case 48:
        case 49:
        case 50:
        case 51:
        case 52:
        case 53:
        case 54:
        case 55:
        case 56:
        case 57:
        case 58:
        case 59:
        case 60:
        case 61:
        case 62:
        case 63:
        case 64:
        case 65:
        case 66:
        case 67:
        case 68:
        default:
            assert( !"No such rule!" );
    }
}

lex skip( LIterator& it )
{
    return *it++;
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
} // namespace sap
