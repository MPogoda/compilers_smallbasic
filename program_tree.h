#pragma once

#include "lexeme.h"
#include "parse.h"
#include "syntax.h"

#include <vector>
#include <iosfwd>

#include <boost/variant.hpp>

namespace sap
{
struct Assertion
{
    Assertion( const bool to_assert ) { assert( to_assert ); }
};

struct Node : private Assertion
{
    const lex::rule rule_;
    using Nodes = std::vector< Node >;
    using Value = boost::variant< lex, Nodes >;
    Value value_;

    Node( Queue& i_queue, LIterator& i_it );
    const lex& getLex() const;
    const Nodes& getNodes() const;
};

std::ostream& operator<<( std::ostream& out, const Node& root);
} // namespace sap

