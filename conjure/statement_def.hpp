/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "statement.hpp"
#include "error_handler.hpp"
#include "annotation.hpp"

namespace client { namespace parser
{
    template <typename Iterator>
    statement<Iterator>::statement(error_handler<Iterator>& error_handler)
      : statement::base_type(statement_list), expr(error_handler)
    {
        qi::_1_type _1;
        qi::_2_type _2;
        qi::_3_type _3;
        qi::_4_type _4;

        qi::_val_type _val;
        qi::raw_type raw;
        qi::lexeme_type lexeme;
        qi::alpha_type alpha;
        qi::alnum_type alnum;
        qi::lit_type lit;

        using qi::on_error;
        using qi::on_success;
        using qi::fail;
        using boost::phoenix::function;

        typedef function<client::error_handler<Iterator> > error_handler_function;
        typedef function<client::annotation<Iterator> > annotation_function;

        statement_list =
            +statement_
            ;

        statement_ =
            |   button
            |   toggle
            |   input_box
            ;

        identifier =
                !expr.keywords
            >>  raw[lexeme[(alpha | '_') >> *(alnum | '_')]]
            ;

        button =
                lit("Button")
            >   '('
            >    statement_ // Color
            >    ','
            >    statement_ // Function 
            >   ')'
            ;

        toggle =
                lit("Toggle")
            >   '('
            >    statement_
            >    ','
            >    statement_
            >   ')'
            ;

        input_box =
                lit("Input Box")
            >   '('
            >    statement_
            >    ','
            >    statement_
            >   ')'
            ;
            
        compound_statement =
            '{' >> -statement_list >> '}'
            ;

        // Debugging and error handling and reporting support.
        BOOST_SPIRIT_DEBUG_NODES(
            (statement_list)
            (statement_)
            (button)
            (toggle)
            (compound_statement)
        );

        // Error handling: on error in statement_list, call error_handler.
        on_error<fail>(statement_list,
            error_handler_function(error_handler)(
                "Error! Expecting ", _4, _3));

        // Annotation: on success in variable_declaration,
        // button and return_statement, call annotation.
        on_success(variable_declaration,
            annotation_function(error_handler.iters)(_val, _1));
        on_success(button,
            annotation_function(error_handler.iters)(_val, _1));
        on_success(return_statement,
            annotation_function(error_handler.iters)(_val, _1));
    }
}}


