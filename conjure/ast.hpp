/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_CONJURE_AST_HPP)
#define BOOST_SPIRIT_CONJURE_AST_HPP

#include <boost/variant/recursive_variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/optional.hpp>
#include <list>

namespace client { 
    namespace ast
{
    ///////////////////////////////////////////////////////////////////////////
    //  The AST
    ///////////////////////////////////////////////////////////////////////////
    struct tagged
    {
        int id; // Used to annotate the AST with the iterator position.
                // This id is used as a key to a map<int, Iterator>
                // (not really part of the AST.)
    };

    struct nil {};
    struct expression;

    struct identifier : tagged
    {
        identifier(std::string const& name = "") : name(name) {}
        std::string name;
    };

    struct button;
    struct toggle;
    struct input_box;


    typedef boost::variant<
          , boost::recursive_wrapper<button>
          , boost::recursive_wrapper<toggle>
          , boost::recursive_wrapper<input_box>
        >
    statement;

    struct statement_list : std::list<statement> {};

    struct button
    {
        statement body1;
        statement body2;
    };

    struct input_box
    {
        statement body1;
        statement body2;
    };

    struct toggle
    {
        statement body1;
        statement body2;
    };

    inline std::ostream& operator<<(std::ostream& out, nil)
    {
        out << "nil"; return out;
    }

    inline std::ostream& operator<<(std::ostream& out, identifier const& id)
    {
        out << id.name; return out;
    }
}}


BOOST_FUSION_ADAPT_STRUCT(
    client::ast::button,
    (client::ast::statement, body1)
    (client::ast::statement, body2)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::toggle,
    (client::ast::statement, body1)
    (client::ast::statement, body2)
)

BOOST_FUSION_ADAPT_STRUCT(
    client::ast::input_box,
    (client::ast::statement, body1)
    (client::ast::statement, body2)
)


#endif
