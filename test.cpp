#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/lex_lexertl.hpp>

#include <iostream>
#include <fstream>
#include <string>

#include "example.hpp"

using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
//  Token definition
///////////////////////////////////////////////////////////////////////////////
template <typename Lexer>
struct example1_tokens : lex::lexer<Lexer>
{
    example1_tokens()
    {
        // define tokens and associate them with the lexer
        identifier = "[a-zA-Z_][a-zA-Z0-9_]*";
        button_ = "Button";
        toggle_ = "Toggle";
        this->self += Toggle_ | Button_ | identifier;        
        this->self = lex::char_(',') | '{' | '}' | ';';

        // any token definition to be used as the skip parser during parsing 
        // has to be associated with a separate lexer state (here 'WS') 
        this->white_space = "[ \\t\\n]+";

        this->self("WS") = white_space;
    }

    lex::token_def<> button_, toggle_;
    lex::token_def<> identifier, white_space;
};

///////////////////////////////////////////////////////////////////////////////
//  Grammar definition
///////////////////////////////////////////////////////////////////////////////
template <typename Iterator, typename Lexer>
struct example1_grammar 
  : qi::grammar<Iterator, qi::in_state_skipper<Lexer> >
{
    template <typename TokenDef>
    example1_grammar(TokenDef const& tok)
      : example1_grammar::base_type(program)
    {
        using boost::spirit::_val;

        program 
            =  +block
            ;

        block
            =   '{' >> *statement >> '}'
            ;

        statement 
            =   button
            |   toggle
            ;

        button 
            =   (tok.button_ >> ',' >> *(tok.identifier >> -ascii::char_(',')) >> ';')
                
            ;

        toggle
            =   (tok.toggle_ >> ',' >> *(tok.identifier >> -ascii::char_(',')) >> ';')
                [
                   
                ]
            ;
    }
        qi::rule<Iterator, qi::in_state_skipper<Lexer> > program, block, statement;
        qi::rule<Iterator, qi::in_state_skipper<Lexer> > button, toggle;
        lex::token_def<std::string> identifier;
};

///////////////////////////////////////////////////////////////////////////////
int main()
{
    // iterator type used to expose the underlying input stream
    typedef std::string::iterator base_iterator_type;

    typedef lex::lexertl::token<
        base_iterator_type, boost::mpl::vector<unsigned int, std::string> 
    > token_type;
//]
    // Here we use the lexertl based lexer engine.
    typedef lex::lexertl::lexer<token_type> lexer_type;

    // This is the token definition type (derived from the given lexer type).
    typedef example1_tokens<lexer_type> example1_tokens;

    // this is the iterator type exposed by the lexer 
    typedef example1_tokens::iterator_type iterator_type;

    // this is the type of the grammar to parse
    typedef example1_grammar<iterator_type, example1_tokens::lexer_def> example1_grammar;

    // now we use the types defined above to create the lexer and grammar
    // object instances needed to invoke the parsing process
    example1_tokens tokens;                         // Our lexer
    example1_grammar calc(tokens);                  // Our parser

    std::string str (read_from_file("example1.input"));

    // At this point we generate the iterator pair used to expose the
    // tokenized input stream.
    std::string::iterator it = str.begin();
    iterator_type iter = tokens.begin(it, str.end());
    iterator_type end = tokens.end();
        
    // Parsing is done based on the token stream, not the character 
    // stream read from the input.
    // Note how we use the lexer defined above as the skip parser. It must
    // be explicitly wrapped inside a state directive, switching the lexer 
    // state for the duration of skipping whitespace.
    bool r = qi::phrase_parse(iter, end, calc, qi::in_state("WS")[tokens.self]);

    if (r && iter == end)
    {
        std::cout << "-------------------------\n";
        std::cout << "Parsing succeeded\n";
        std::cout << "-------------------------\n";
    }
    else
    {
        std::cout << "-------------------------\n";
        std::cout << "Parsing failed\n";
        std::cout << "-------------------------\n";
    }

    std::cout << "Bye... :-) \n\n";
    return 0;
}