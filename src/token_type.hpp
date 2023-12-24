#pragma once
#include <string_view>

enum class TokenType {
    Empty,
    Do,
    While,
    Identifier,
    HexNumber,
    LessEqual,   // <=
    GreaterEqual, // >=
    Equal, // =
    Assignment,  // :=
    Semicolon,  // ;
    LeftParen,   // (
    RightParen,  // )
    EndOfStream,
    Invalid
};

std::string_view ToStringView(TokenType token_type) {
#define CASE(X) case X : return #X;
    using
    enum TokenType;
    switch (token_type) {
        CASE(Empty);
        CASE(Do);
        CASE(While);
        CASE(Identifier);
        CASE(HexNumber);
        CASE(LessEqual);
        CASE(GreaterEqual);
        CASE(Equal);
        CASE(Assignment);
        CASE(Semicolon);
        CASE(LeftParen);
        CASE(RightParen);
        CASE(EndOfStream);
        CASE(Invalid);
    }
#undef CASE
}

struct Token {
    TokenType type;
    std::string value;
};
