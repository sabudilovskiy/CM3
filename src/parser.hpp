#pragma once

#include <string>
#include <functional>
#include <map>
#include "token_type.hpp"
#include "constexpr_string.hpp"

class Parser {
public:
    using State = TokenType;
    using SymbolCategory = std::function<bool(std::string)>;

private:
    std::map<State, std::vector<std::pair<SymbolCategory, State>>> transitions;
    State initialState;
    State currentState;
    State invalidState;

public:
    Parser(const State& start, const State& invalid) :
            initialState(start),
            currentState(start),
            invalidState(invalid){}

    // Принимает начальное состояние, функцию категории символа и конечное состояние
    void addTransition(const State& from, SymbolCategory symbolCat, const State& to) {
        transitions[from].emplace_back(symbolCat, to);
    }


    // Проверяет, является ли входная строка валидной согласно заданным переходам
    std::vector<Token> parse(const std::vector<std::string>& input) {
        currentState = initialState; // Начинаем с начального состояния
        std::vector<Token> parsed{};
        for (std::size_t number_lex = 0; number_lex < input.size() && currentState != invalidState; number_lex++){
            auto& lex = input[number_lex];
            bool transitionFound = false;

            // Поиск перехода для текущего символа
            for (const auto& trans : transitions[currentState]) {
                if (trans.first(lex)) {
                    currentState = trans.second;
                    transitionFound = true;
                    break;
                }
            }

            if (!transitionFound) {
                currentState = invalidState;
            }
            parsed.emplace_back(currentState, lex);
        }
        if (currentState != invalidState){
            parsed.emplace_back(TokenType::EndOfStream);
        }
        return parsed;
    }
};

template <ConstexprString str>
constexpr auto Match(){
    return [](const std::string& s){
        return s == str.AsStringView();
    };
};

template <ConstexprString str>
constexpr auto matcher = Match<str>();

bool is_hex_digit(char ch){
    return '0' <= ch && ch <= '9' || 'a' <= ch  && ch <= 'f'  || 'A' <= ch  && ch <= 'F';
}

bool is_graph(char ch){
    return 'a' <= ch && ch <= 'z' ||  'A' <= ch && ch <= 'Z' || ch == '_';
}

auto MatchIdentifier(const std::string& str){
    if (str.empty()){
        return false;
    }
    if (!is_graph(str.front())){
        return false;
    }
    for (char ch: str){
        if (!isdigit(ch) && !is_graph(ch) ){
            return false;
        }
    }
    return true;
}

auto MatchHexNumber(const std::string& s){
    if (!s.starts_with("0x")){
        return false;
    }
    std::string_view raw_value = {s.data() + 2, s.size() - 2};
    if (raw_value.empty()){
        return false;
    }
    for (auto ch: raw_value){
        if (!is_hex_digit(ch) )
        {
            return false;
        }
    }
    return true;
}

auto CreateParser(){
    Parser parser(TokenType::Empty, TokenType::Invalid);

    parser.addTransition(TokenType::Empty, matcher<"do">, TokenType::Do);
    parser.addTransition(TokenType::Do, MatchIdentifier, TokenType::Identifier);
    parser.addTransition(TokenType::Identifier, matcher<"<=">, TokenType::LessEqual);
    parser.addTransition(TokenType::Identifier, matcher<"=">, TokenType::Equal);
    parser.addTransition(TokenType::Identifier, matcher<"=>">, TokenType::GreaterEqual);
    parser.addTransition(TokenType::LessEqual, MatchHexNumber, TokenType::HexNumber);
    parser.addTransition(TokenType::Equal, MatchHexNumber, TokenType::HexNumber);
    parser.addTransition(TokenType::GreaterEqual, MatchHexNumber, TokenType::HexNumber);
    parser.addTransition(TokenType::HexNumber, matcher<"while">, TokenType::While);

    parser.addTransition(TokenType::While, matcher<"(">, TokenType::LeftParen);
    parser.addTransition(TokenType::LeftParen, MatchIdentifier, TokenType::Identifier);
    parser.addTransition(TokenType::Identifier, matcher<":=">, TokenType::Assignment);
    parser.addTransition(TokenType::Assignment, MatchHexNumber, TokenType::HexNumber);
    parser.addTransition(TokenType::HexNumber, matcher<")">, TokenType::RightParen);
    parser.addTransition(TokenType::RightParen, matcher<";">, TokenType::Semicolon);

    parser.addTransition(TokenType::Semicolon, matcher<"do">, TokenType::Do);
    return parser;
}
