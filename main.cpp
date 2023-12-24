#include <iostream>
#include <vector>
#include <string>
#include <token_type.hpp>
#include <sstream>
#include <regex>
#include "ast_builder.hpp"
#include "parser.hpp"

std::string InputExpr(){
    std::cout << "Введите выражение для разбора: ";
    std::string expr;
    std::getline(std::cin, expr);
    return expr;
}

std::vector<std::string> ParseToLexs(const std::string& input) {
    // Используем регулярное выражение для поиска слов
    std::regex regex("\\s+");

    // Создаем итератор начала итерации по регулярному выражению
    std::sregex_token_iterator iter(input.begin(), input.end(), regex, -1);

    // Создаем итератор конца итерации по регулярному выражению
    std::sregex_token_iterator end;

    // Копируем результаты в вектор строк
    std::vector<std::string> result(iter, end);

    return result;
}

void PrintLexs(const std::vector<std::string>& lexs){
    std::cout << "Лексемы: \n";
    std::size_t lex_num = 0;
    for (auto& l: lexs){
        std::cout << lex_num << ") " << l << '\n';
        lex_num++;
    }
}

void PrintTokens(const std::vector<Token>& toks){
    std::cout << "Токены: \n";
    std::size_t t_num = 0;
    for (auto& t: toks){
        std::cout << t_num << ") Тип: " << ToStringView(t.type) << ", значение: " << t.value << '\n';
        t_num++;
    }
}

bool InvalidTokens(const std::vector<Token>& toks){
    if (toks.empty()){
        return true;
    }
    if (toks.back().type != TokenType::EndOfStream){
        return true;
    }
    return false;
}

struct ParseError{

};

// do a := 0x5 while ( a <= 0x4 ) ;
// do a := 0x5 ) ;
// do a := 6 while ( a <= 5 ) ;
int main() try {
    std::string expr = InputExpr();
    auto lexs = ParseToLexs(expr);
    PrintLexs(lexs);
    auto tokens = CreateParser().parse(lexs);
    PrintTokens(tokens);
    if (InvalidTokens(tokens)){
        throw ParseError{};
    }
    ASTBuilder ast_builder(tokens);
    ASTBuilder::AstNode ast = ast_builder.build_tree();
    ASTBuilder::printAst(ast);
}
catch (const ParseError&){
    std::cerr << "Parse error\n";
}
catch (const ASTError &e) {
    std::cerr << "AST error: " << e.what() << std::endl;
}