#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

namespace qi = boost::spirit::qi;
namespace phx = boost::phoenix;

template <typename Parser, typename ... Args>
void ParseOrDie(const std::string& input, const Parser& p, Args&& ... args)
{
    std::string::const_iterator begin = input.begin(), end = input.end();
    bool ok = qi::parse(begin, end, p, std::forward<Args>(args) ...);
    if (!ok || begin != end) {
        std::cout << "Unparseable: "
                  << std::quoted(std::string(begin, end)) << std::endl;
        throw std::runtime_error("Parse error");
    }
}

struct Button
{
    int order;
    std::string button = "Button";
    std::string function;

    Button() { }
    Button(int order, std::string button, std::string function)
            : order(order), button(button), function(function) { }

};

struct Toggle
{
    int order;
    std::string toggle = "Toggle";
    std::string function;

    Toggle() { }
    Toggle(int order, std::string toggle, std::string function)
            : order(order), toggle(toggle), function(function) { }

};

class Grammar : public qi::grammar<
    std::string::const_iterator, Button()>
{
public:
    using Iterator = std::string::const_iterator;

Grammar() : Grammar::base_type(start)
    {
        start = (qi::int_  >> ',' >> button >> ',' >> function)
            [qi::_val = phx::construct <Button>(qi::_1, qi::_2, qi::_3) ];
    }

    qi::rule<Iterator, std::string()> button;
    qi::rule<Iterator, std::string()> function;
    qi::rule<Iterator, Button()> start;
};

void testing(std::istream& input)
{
    std::string line;
    Grammar g;
    while (std::getline(input, line)) {
    Button data;
        ParseOrDie(line, g, data);
        std::cout << "Hello"<< std::endl;
    }
}

BOOST_FUSION_ADAPT_STRUCT(
Button,
    (int, order)
    (std::string, button)
    (std::string, function)
)

BOOST_FUSION_ADAPT_STRUCT(
Toggle,
    (int, order)
    (std::string, toggle)
    (std::string, function)
)

int main(int argc, char* argv[])
{
    testing(std::cin);
    return 0;
}
