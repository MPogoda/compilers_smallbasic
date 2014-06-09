#pragma once

#include <string>
#include <deque>

namespace sap
{
struct ProgramCode
{
    using List = std::deque< std::string >;
    using Item = List::iterator;

    List code_;

    Item push( const std::string& i_token );
};
}
