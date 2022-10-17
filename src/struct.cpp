//#define BOOST_SPIRIT_DEBUG // before including Spirit
#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/fusion/adapted.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <fstream>

namespace Format
{
    struct int_point   { int x, y, z;   }; 
    struct float_point { float x, y, z; }; 

    struct figure
    {
        std::string              component;
        std::string              color;
        std::string              function;
       // std::vector<int_point>   int_points;
       // std::vector<float_point> float_points;

        friend std::ostream& operator<<(std::ostream& os, figure const& o);
    };

    struct figure_set
    {
        std::string           component;
        std::string           color;
        std::string           function;
        std::set<std::string> includes;
        std::vector<figure>   figures;

        friend std::ostream& operator<<(std::ostream& os, figure_set const& o);
    };

    typedef std::vector<figure_set> file_data;
}

BOOST_FUSION_ADAPT_STRUCT(Format::int_point,   
        (int, x)(int, y)(int, z))
BOOST_FUSION_ADAPT_STRUCT(Format::float_point, 
        (float, x)(float, y)(float, z))
BOOST_FUSION_ADAPT_STRUCT(Format::figure,      
        (std::string, component)
        (std::string, color)
        (std::string, function)
        (std::vector<Format::int_point>, int_points)
        (std::vector<Format::float_point>, float_points))
BOOST_FUSION_ADAPT_STRUCT(Format::figure_set,  
        (std::string, component)
        (std::string, color)
        (std::string, function)
        (std::set<std::string>, includes)
        (std::vector<Format::figure>, figures))

namespace Format
{
    std::ostream& operator<<(std::ostream& os, figure const& o)
    {
        using namespace boost::spirit::karma;
        return os << format_delimited(
                "\n    figure" << no_delimit [ '"' << string << '"' ] << "\n    {"
                << *("\n       i" << int_ << int_ << int_)
                << *("\n       f" << float_ << float_ << float_)
                << "\n    }"
                , ' ', o);
    }

    std::ostream& operator<<(std::ostream& os, figure_set const& o)
    {
        using namespace boost::spirit::karma;
        return os << format_delimited(
                "\nset" << no_delimit [ '"' << string << '"' ] << "\n{"
                << *("\n    component " << no_delimit [ '"' << string << '"' ])
                << *("\n    color " << no_delimit [ '"' << string << '"' ])
                << *("\n    function" << no_delimit [ '"' << string << '"' ])
                << *stream
                << "\n}"
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

            component   = eps >> lexeme [ '"' >> *~char_('"') >> '"' ];
            color       = eps >> lexeme [ '"' >> *~char_('"') >> '"' ];
            function    = eps >> lexeme [ '"' >> *~char_('"') >> '"' ];

            include = eps >> "component" >> component;
            include1 = eps >> "color" >> color;
            include2 = eps >> "function" >> function;

            ipoints = eps >> "i"       >> int_         >> int_   >> int_;
            fpoints = eps >> "f"       >> float_       >> float_ >> float_;

            figure  = eps >> "figure" 
                >> component        [ at_c<0>(_val) = _1 ] >> color       [ at_c<1>(_val) = _1 ] >> function [ at_c<2>(_val) = _1 ] >>'{' >> 
                *(
                        ipoints [ push_back(at_c<3>(_val), _1) ]
                      | fpoints [ push_back(at_c<4>(_val), _1) ]
                 ) >> '}';
            set     = eps >> "set" >> component >> color >> function >>'{' >> *include >> *include1 >> *include2 >> *figure >> '}';
            start   = *set;
        }

      private:
        qi::rule<Iterator, std::string()        , skipper<Iterator> > component, color, function, include, include1, include2;
        qi::rule<Iterator, Format::int_point()  , skipper<Iterator> > ipoints;
        qi::rule<Iterator, Format::float_point(), skipper<Iterator> > fpoints;
        qi::rule<Iterator, Format::figure()     , skipper<Iterator> > figure;
        qi::rule<Iterator, Format::figure_set() , skipper<Iterator> > set;
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

    std::cerr << "Parse " << (ok?"success":"failed") << std::endl;
    std::cout << "# figure sets exported automatically by karma\n\n";

    for (auto& set : data)
        std::cout << set;
}