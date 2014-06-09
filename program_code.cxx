#include "program_code.h"

namespace sap
{
ProgramCode::Item ProgramCode::push( const std::string& i_token )
{
    code_.push_back( i_token );
    return std::next( code_.begin(), code_.size() - 1);
}
}
