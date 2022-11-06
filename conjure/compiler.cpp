/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "compiler.hpp"
#include "vm.hpp"
#include <boost/foreach.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/assert.hpp>
#include <boost/lexical_cast.hpp>
#include <set>

namespace client { namespace code_gen
{
    vector <string> Component;
    vector <string> Function;
    vector <string> Attribute;

    int const* function::find_var(std::string const& name) const
    {
        std::map<std::string, int>::const_iterator i = variables.find(name);
        if (i == variables.end())
            return 0;
        return &i->second;
    }

    void function::add_var(std::string const& name)
    {
        std::size_t n = variables.size();
        variables[name] = n;
    }

    void function::link_to(std::string const& name, std::size_t address)
    {
        function_calls[address] = name;
    }

    void function::print_assembler() const
    {
        std::vector<int>::const_iterator pc = code.begin() + address;

        std::vector<std::string> locals(variables.size());
        typedef std::pair<std::string, int> pair;
        BOOST_FOREACH(pair const& p, variables)
        {
            locals[p.second] = p.first;
            std::cout << "      local       "
                << p.first << ", @" << p.second << std::endl;
        }

        std::map<std::size_t, std::string> lines;
        std::set<std::size_t> jumps;

        std::cout << "start:" << std::endl;
        typedef std::pair<std::size_t, std::string> line_info;
        BOOST_FOREACH(line_info const& l, lines)
        {
            std::size_t pos = l.first;
            if (jumps.find(pos) != jumps.end())
                std::cout << pos << ':' << std::endl;
            std::cout << l.second << std::endl;
        }

        std::cout << "end:" << std::endl << std::endl;
    }

    bool compiler::operator()(unsigned int x)
    {
        BOOST_ASSERT(current != 0);
        current->op(op_int, x);
        return true;
    }

    bool compiler::operator()(ast::identifier const& x)
    {
        BOOST_ASSERT(current != 0);
        int const* p = current->find_var(x.name);
        if (p == 0)
        {
            error_handler(x.id, "Undeclared variable: " + x.name);
            return false;
        }
        current->op(op_load, *p);
        return true;
    }

    bool compiler::operator()(ast::button const& x)
    {
        BOOST_ASSERT(current != 0);
        Component.push_back("Button");
        if ((*this)(x.body1))
            Attribute.push_back(x.body1);
        if ((*this)(x.body2))
            Function.push_back(x.body2);   
            return true;
    }

    bool compiler::operator()(ast::toggle const& x)
    {
        BOOST_ASSERT(current != 0);
        Component.push_back("Toggle");
        if ((*this)(x.body1))
            Attribute.push_back(x.body1);
        if ((*this)(x.body2))
            Function.push_back(x.body2);   
            return true;
    }
    
    bool compiler::operator()(ast::input_box const& x)
    {
        
        BOOST_ASSERT(current != 0);
        Component.push_back("Input_Box");
        if ((*this)(x.body1))
            Attribute.push_back(x.body1);
        if ((*this)(x.body2))
            Function.push_back(x.body2);   
            return true;
    }


    bool compiler::operator()(ast::expression const& x)
    {
        BOOST_ASSERT(current != 0);
        if (!boost::apply_visitor(*this, x.first))
            return false;
        std::list<ast::operation>::const_iterator rbegin = x.rest.begin();
        if (!compile_expression(0, rbegin, x.rest.end()))
            return false;
        return true;
    }


    bool compiler::operator()(ast::statement const& x)
    {
        BOOST_ASSERT(current != 0);
        return boost::apply_visitor(*this, x);
    }

    bool compiler::operator()(ast::statement_list const& x)
    {
        BOOST_ASSERT(current != 0);
        BOOST_FOREACH(ast::statement const& s, x)
        {
            if (!(*this)(s))
                return false;
        }
        return true;
    }


    bool compiler::operator()(ast::function const& x)
    {
        void_return = x.return_type == "void";
        if (functions.find(x.function_name.name) != functions.end())
        {
            error_handler(x.function_name.id, "Duplicate function: " + x.function_name.name);
            return false;
        }
        boost::shared_ptr<code_gen::function>& p = functions[x.function_name.name];
        p.reset(new code_gen::function(code, x.args.size()));
        current = p.get();
        current_function_name = x.function_name.name;

        // op_stk_adj 0 for now. we'll know how many variables
        // we'll have later and add them
        current->op(op_stk_adj, 0);
        BOOST_FOREACH(ast::identifier const& arg, x.args)
        {
            current->add_var(arg.name);
        }

        if (!(*this)(x.body))
            return false;
        (*current)[1] = current->nvars();   // now store the actual number of variables
                                            // this includes the arguments
        return true;
    }

    void compiler::print_assembler() const
    {
        typedef std::pair<std::string, boost::shared_ptr<code_gen::function> > pair;
        BOOST_FOREACH(pair const& p, functions)
        {
            std::cout << ";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;" << std::endl;
            std::cout << p.second->get_address() << ": function " << p.first << std::endl;
            p.second->print_assembler();
        }
    }

    boost::shared_ptr<code_gen::function>
    compiler::find_function(std::string const& name) const
    {
        function_table::const_iterator i = functions.find(name);
        if (i == functions.end())
            return boost::shared_ptr<code_gen::function>();
        else
            return i->second;
    }
}}

