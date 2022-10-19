/*=============================================================================
    Copyright (c) 2001-2010 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
///////////////////////////////////////////////////////////////////////////////
//
//  A mini XML-like parser
//
//  [ JDG March 25, 2007 ]   spirit2
//
///////////////////////////////////////////////////////////////////////////////

#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/fusion.hpp>
#include <boost/phoenix/stl.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

namespace client
{
    namespace fusion = boost::fusion;
    namespace phoenix = boost::phoenix;
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    ///////////////////////////////////////////////////////////////////////////
    //  Our mini XML tree representation
    ///////////////////////////////////////////////////////////////////////////
    //[tutorial_xml1_structures
    struct button;

    typedef
        boost::variant<
            boost::recursive_wrapper<button>
          , std::string
          , std::string
        >
    gui_node;

    struct button
    {
        std::string Function;                         
        std::string Color;
        std::vector<gui_node> children;       
    };
    //]
}

// We need to tell fusion about our button struct
// to make it a first-class fusion citizen
//[tutorial_xml1_adapt_structures
BOOST_FUSION_ADAPT_STRUCT(
    client::button,
    (std::string, Function)
    (std::string, Color)
    (std::vector<client::gui_node>, children)
)
//]

namespace client
{
    ///////////////////////////////////////////////////////////////////////////
    //  Print out the mini xml tree
    ///////////////////////////////////////////////////////////////////////////
    // int const tabsize = 4;

    // void tab(int indent)
    // {
    //     for (int i = 0; i < indent; ++i)
    //         std::cout << ' ';
    // }

    ///////////////////////////////////////////////////////////////////////////
    //  Our mini XML grammar definition
    ///////////////////////////////////////////////////////////////////////////
    //[tutorial_xml1_grammar
    
    template <typename Iterator>
    struct grammar : qi::grammar<Iterator, button(), ascii::space_type>
    {
        grammar() : grammar::base_type(xml)
        {
            using qi::lit;
            using qi::lexeme;
            using ascii::char_;
            using ascii::string;
            using namespace qi::labels;

            using phoenix::at_c;
            using phoenix::push_back;

            text = lexeme[+(char_ - '<')        [_val += _1]];
            node = (xml | text)                 [_val = _1];

            start_tag =
                    '<'
                >>  !lit('/')
                >>  lexeme[+(char_ - '>')       [_val += _1]]
                >>  '>'
            ;

            end_tag =
                    "</"
                >>  lit(_r1)
                >>  '>'
            ;

            xml =
                    start_tag                   [at_c<0>(_val) = _1]
                >>  *node                       [push_back(at_c<1>(_val), _1)]
                >>  end_tag(at_c<0>(_val))
            ;
        }

        qi::rule<Iterator, button(), ascii::space_type> xml;
        qi::rule<Iterator, gui_node(), ascii::space_type> node;
        qi::rule<Iterator, std::string(), ascii::space_type> text;
        qi::rule<Iterator, std::string(), ascii::space_type> start_tag;
        qi::rule<Iterator, void(std::string), ascii::space_type> end_tag;
    };
    //]
}

///////////////////////////////////////////////////////////////////////////////
//  Main program
///////////////////////////////////////////////////////////////////////////////
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

    std::string storage; // We will read the contents here.
    in.unsetf(std::ios::skipws); // No white space skipping!
    std::copy(
        std::istream_iterator<char>(in),
        std::istream_iterator<char>(),
        std::back_inserter(storage));

    typedef client::grammar<std::string::const_iterator> grammar;
    grammar xml; // Our grammar
    client::button ast; // Our tree

    using boost::spirit::ascii::space;
    std::string::const_iterator iter = storage.begin();
    std::string::const_iterator end = storage.end();
    bool r = phrase_parse(iter, end, xml, space, ast);

    if (r && iter == end)
    {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        std::cout << "-------------------------\n";
        //client::mini_xml_printer printer;
        //printer(ast);
        return 0;
    }
    else
    {
        std::string::const_iterator some = iter + std::min(30, int(end - iter));
        std::string context(iter, (some>end)?end:some);
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "stopped at: \"" << context << "...\"\n";
        std::cout << "-------------------------\n";
        return 1;
    }
}



