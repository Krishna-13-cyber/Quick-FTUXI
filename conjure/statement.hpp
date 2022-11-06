/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_CONJURE_STATEMENT_HPP)
#define BOOST_SPIRIT_CONJURE_STATEMENT_HPP

#include "expression.hpp"

namespace client { namespace parser
{
    ///////////////////////////////////////////////////////////////////////////////
    //  The statement grammar
    ///////////////////////////////////////////////////////////////////////////////
    template <typename Iterator>
    struct statement : qi::grammar<Iterator, ast::statement_list(), skipper<Iterator> >
    {
        statement(error_handler<Iterator>& error_handler);

        expression<Iterator> expr;
        qi::rule<Iterator, ast::statement_list(), skipper<Iterator> >
            statement_list, compound_statement;

        qi::rule<Iterator, ast::statement(), skipper<Iterator> > statement_;
        qi::rule<Iterator, ast::button, skipper<Iterator> > statement_ , statement_;
        qi::rule<Iterator, ast::toggle, skipper<Iterator> > statement_, statement_;
        qi::rule<Iterator, ast::input_box, skipper<Iterator> > statement_, statement_;
        qi::rule<Iterator, std::string(), skipper<Iterator> > identifier;
    };
}}

#endif


