#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/fusion/adapted.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <fstream>

namespace Format
{
    struct component
    {
        std::string              name;
        std::string              color;
        std::string              function;

        friend std::ostream& operator<<(std::ostream& os, component const& o);
    };

    typedef std::vector<component> file_data;
}

BOOST_FUSION_ADAPT_STRUCT(Format::component,      
        (std::string, name)
        (std::string, color)
        (std::string, function)
)

namespace Format
{
    std::ostream& operator<<(std::ostream& os, component const& o)
    {
        using namespace boost::spirit::karma;
        return os << format_delimited(
                "\n    Component:" << no_delimit [ '"' << string << '"' ] << "\n    {"
                << *("\n       Color:" <<  string)
                << *("\n        function:" << string)
                << "\n    }"
                , ' ', o);
    }
}

namespace /*anon*/
{
    namespace phx=boost::phoenix;
    namespace qi =boost::spirit::qi;

    template <typename Iterator> struct skipper
        : public qi::grammar<Iterator>
    {
        skipper() : skipper::base_type(start, "skipper")
        {
            using namespace qi;

            comment = '#' >> *(char_ - eol) >> (eol|eoi);
            start   = comment | qi::space;

            BOOST_SPIRIT_DEBUG_NODE(start);
            BOOST_SPIRIT_DEBUG_NODE(comment);
        }

      private:
        qi::rule<Iterator> start, comment;
    };

    template <typename Iterator> struct parser
        : public qi::grammar<Iterator, Format::file_data(), skipper<Iterator> >
    {
        parser() : parser::base_type(start, "parser")
        {
            using namespace qi;
            using phx::push_back;
            using phx::at_c;

            name  = eps >> lexeme [ *~char_('"')];
            color   = eps >> lexeme [ *~char_('"')];
            function   = eps >> lexeme [ *~char_('"')];

            namef = eps >> "" >> name;
            colorf = eps >> "" >> color;
            functionf = eps >> "" >> function;

            component = eps >> "Component" 
                >> name [ at_c<0>(_val) = _1 ] >> '{' >> color [ at_c<1>(_val) = _1 ] >> function [ at_c<2>(_val) = _1 ] >> '}';
            //set   = name >> color >> function '{'>> *colorf >> *functionf >> '}';
            start = *component;
        }

      private:
        qi::rule<Iterator, std::string()        , skipper<Iterator> > name,color,function,namef,colorf,functionf;
        qi::rule<Iterator, Format::component()  , skipper<Iterator> > component;
        qi::rule<Iterator, Format::file_data()  , skipper<Iterator> > start;
    };
}

namespace Parser {

    bool parsefile(const std::string& spec, Format::file_data& data)
    {
        std::ifstream in(spec.c_str());
        in.unsetf(std::ios::skipws);

        std::string v;
        v.reserve(4096);
        v.insert(v.end(), std::istreambuf_iterator<char>(in.rdbuf()), std::istreambuf_iterator<char>());

        if (!in) 
            return false;

        typedef char const * iterator_type;
        iterator_type first = &v[0];
        iterator_type last = first+v.size();

        try
        {
            parser<iterator_type>  p;
            skipper<iterator_type> s;
            bool r = qi::phrase_parse(first, last, p, s, data);

            r = r && (first == last);

            if (!r)
                std::cerr << spec << ": parsing failed at: \"" << std::string(first, last) << "\"\n";
            return r;
        }
        catch (const qi::expectation_failure<char const *>& e)
        {
            std::cerr << "FIXME: expected " << e.what_ << ", got '" << std::string(e.first, e.last) << "'" << std::endl;
            return false;
        }
    }
}

int main()
{
    Format::file_data data;
    bool ok = Parser::parsefile("input.txt", data);

    std::cout << "# component sets exported automatically by karma\n\n";

    for (auto& set : data)
        std::cout << set;
}