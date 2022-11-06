/*=============================================================================
    Copyright (c) 2001-2011 Joel de Guzman

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_CONJURE_VM_HPP)
#define BOOST_SPIRIT_CONJURE_VM_HPP

#include <vector>

namespace client
{
    ///////////////////////////////////////////////////////////////////////////
    //  The Virtual Machine
    ///////////////////////////////////////////////////////////////////////////

    class vmachine
    {
    public:

        vmachine(unsigned stackSize = 4096)
          : stack(stackSize)
        {
        }

        int execute(std::vector<int> const& code)
        {
            return execute(code, code.begin(), stack.begin());
        }

        std::vector<int> const& get_stack() const { return stack; };
        std::vector<int>& get_stack() { return stack; };

    private:

        int execute(
            std::vector<int> const& code            // the program code
          , std::vector<int>::const_iterator pc     // program counter
          , std::vector<int>::iterator frame_ptr    // start of arguments and locals
        );

        std::vector<int> stack;
    };
}

#endif

