/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  Not a calculator anymore, right? :-)
//
//  [ JDG April 10, 2007 ]      spirit2
//  [ JDG February 18, 2011 ]   Pure attributes. No semantic actions.
//
///////////////////////////////////////////////////////////////////////////////

#include "function.hpp"
#include "skipper.hpp"
#include "vm.hpp"
#include "compiler.hpp"
#include <boost/lexical_cast.hpp>
#include <fstream>

int main(int argc, char **argv)
{
    char const* filename;
    if (argc > 1)
    {
        filename = argv[1];
    }
    else
    {
        std::cerr << "Error: No input file provided." << std::endl;
        return 1;
    }

    std::ifstream in(filename, std::ios_base::in);

    if (!in)
    {
        std::cerr << "Error: Could not open input file: "
            << filename << std::endl;
        return 1;
    }

    std::string source_code; // We will read the contents here.
    in.unsetf(std::ios::skipws); // No white space skipping!
    std::copy(
        std::istream_iterator<char>(in),
        std::istream_iterator<char>(),
        std::back_inserter(source_code));

    typedef std::string::const_iterator iterator_type;
    iterator_type iter = source_code.begin();
    iterator_type end = source_code.end();


    client::ast::function_list ast;             // Our AST

    client::error_handler<iterator_type>
        error_handler(iter, end);               // Our error handler
    client::parser::function<iterator_type>
        function(error_handler);                // Our parser
    client::parser::skipper<iterator_type>
        skipper;                                // Our skipper
    client::code_gen::compiler
        compiler(error_handler);                // Our compiler


    bool success = phrase_parse(iter, end, +function, skipper, ast);

    std::cout << "-------------------------\n";

    if (success && iter == end)
    {
        std::cout << "PASSED\n";
    }
    return 0;
}


