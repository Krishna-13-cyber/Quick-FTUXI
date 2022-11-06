#define BOOST_SPIRIT_DEBUG
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <iomanip>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/phoenix/phoenix.hpp>

namespace AST {
    std::vector <std::string> v;
    namespace Cmd {
       // std::vector <std::string> v;
        struct Component  { std::string comp; };
        struct Color  { std::string col; };
        struct Function  { std::string func; };
    }

    using Command = boost::variant<Cmd::Component, Cmd::Color, Cmd::Function>;
    using Commands = std::vector<Command>;
}


BOOST_FUSION_ADAPT_STRUCT(AST::Cmd::Component, comp)
BOOST_FUSION_ADAPT_STRUCT(AST::Cmd::Color, col)
BOOST_FUSION_ADAPT_STRUCT(AST::Cmd::Function, func)

namespace AST { namespace Cmd { // For demo/debug
    std::vector <std::string> v;
    std::ostream& operator<<(std::ostream& os, Component const& cmd)  { return os << "Component " << boost::fusion::as_vector(cmd);  } 
    std::ostream& operator<<(std::ostream& os, Color const& cmd)  { return os << "Color " << boost::fusion::as_vector(cmd);  }
    std::ostream& operator<<(std::ostream& os, Function const& cmd)  { return os << "Function " << boost::fusion::as_vector(cmd);  }
} }

namespace qi = boost::spirit::qi;

template <typename It>
struct ScriptGrammar : qi::grammar<It, AST::Commands()>
{
    ScriptGrammar() : ScriptGrammar::base_type(start) {
        using namespace AST;
        std::vector <Component > component;
        std::vector <Color> color;
        std::vector <Function > function;
        using namespace qi;
        start   = skip(space) [ script ];
        script  = command % ";";
        command = Component|Color|Function;

        Component  = "Component"  >> comp;
       // v.push_back(Componen.comp);
        Color  = "Color"  >> col;
      //  v.push_back(Color.col);
        Function  = "Function"  >> func;
      //  v.push_back(Function.func);
        comp  = +~char_(";");
        col  = +~char_(";");
        func  = +~char_(";");
        BOOST_SPIRIT_DEBUG_NODES((start)(script)(command)(Component)(Color)(Function)(comp)(col)(func))
    }
    private:
    using Skipper = qi::space_type;
    qi::rule<It, AST::Commands(), Skipper> script;
    qi::rule<It, AST::Command(), Skipper> command;
    qi::rule<It, AST::Cmd::Component(), Skipper>  Component;
    qi::rule<It, AST::Cmd::Color(), Skipper>  Function;
    qi::rule<It, AST::Cmd::Function(), Skipper>  Color;
    // lexeme
    qi::rule<It, AST::Commands()>  start;
    qi::rule<It, std::string()>  comp;
    qi::rule<It, std::string()>  col;
    qi::rule<It, std::string()>  func;
};

int main() {    
    std::string const testInput = "Component Button;Color Blue;Function Hello;";

    typedef std::string::const_iterator iter;

    iter start = testInput.begin(), end = testInput.end();

    AST::Commands script;

    bool val = qi::parse(start, testInput.end(), ScriptGrammar<iter>(), script);

    if (val) {
        std::cout << "Parsed " << script.size() << " commands\n";
        std::copy(script.begin(), script.end(), std::ostream_iterator<AST::Command>(std::cout, ";"));
    } 
    else {
        std::cout << "Parse failed\n";
    }

    if (start != end)
        std::cout << "Remaining unparsed input: '" << std::string(start, end) << "'\n";
}
