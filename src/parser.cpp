#include <boost/fusion/adapted/struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <ctime>

typedef std::string::const_iterator It;

namespace structs {
    typedef boost::fusion::vector< string, string, string > console;

    struct Component
    {
        std::string name;
    };

    struct Record {
        Component name;
        std::string color;
        std::string function;
    };

    typedef std::vector<Record> Records;
}

BOOST_FUSION_ADAPT_STRUCT(structs::Record,
        (Component, name)
        (std::string, color)
        (std::string, function))

namespace boost { namespace spirit { namespace traits
{
    template<>
    struct transform_attribute<structs::Component, structs::console, qi::domain>
    {
        typedef structs::console type;

        static type pre(structs::Component) { return type(); }
        static void fail(structs::Component&) { }
        static void post(structs::Component& component, type const& v)
        {
            std::tm time = { fusion::at_c<5>(v), fusion::at_c<4>(v), fusion::at_c<3>(v),
                             fusion::at_c<2>(v), fusion::at_c<1>(v) - 1900, fusion::at_c<0>(v), 0, 0, 0 };


        }
    };
} } }

namespace qi = boost::spirit::qi;

namespace QiParsers {
    template <typename It>
    struct Parser : qi::grammar<It, structs::Records()> {

        Parser() : Parser::base_type(start) {
            using namespace qi;

            month.add
                ("Jan", 0)
                ("Feb", 1)
                ("Mar", 2)
                ("Apr", 3)
                ("May", 4)
                ("Jun", 5)
                ("Jul", 6)
                ("Aug", 7)
                ("Sep", 8)
                ("Oct", 9)
                ("Nov", 10)
                ("Dec", 11);

            date = repeat(4)[digit] >> '-' >> month >> '-' >> repeat(2)[digit] >> ' ' >> 
                   repeat(2)[digit] >> ':' >> repeat(2)[digit] >> ':' >> repeat(2)[digit] >> '.' >> repeat(6)[digit];

            line = '[' >> date >> ']'
                >> " - " >> double_ >> " s"
                >> " => String: "  >> raw[+graph];

            start = line % eol;
        }

      private:
        qi::symbols<char, int> month;

        qi::rule<It, structs::console()> date;
        qi::rule<It, structs::Record()> line;
        qi::rule<It, structs::Records()> start;

    };
}

structs::Records parse_string(It b, It e)
{
    static const QiParsers::Parser<It> parser;

    structs::Records records;
    parse(b, e, parser, records);

    return records;
}

static const std::string input =
"[2018-Mar-13 13:13:59.580482] - 0.200 s => String: Test_1\n\
[2018-Mar-14 13:13:59.580482] - 0.400 s => String: Test_2\n\
[2018-Mar-15 13:13:59.580482] - 0.600 s => String: Test_3\n";

int main() {
    const auto records = parse_string(input.begin(), input.end());

    return 0;
}