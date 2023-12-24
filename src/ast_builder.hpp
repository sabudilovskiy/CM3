#pragma once

#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <token_type.hpp>

struct ASTError: std::runtime_error{
    using std::runtime_error::runtime_error;
};

class ASTBuilder {
public:
    explicit ASTBuilder(const std::vector<Token> &tokens) : tokens(tokens), current(0) {}

    struct AstNode {
        std::string type;
        std::string value;
        std::vector<AstNode> children;
    };

    AstNode build_tree() {
        AstNode rootNode = {
                "Program", "", {}
        };

        while (!isAtEnd()) {
            rootNode.children.push_back(parseDoWhile());
            consume(TokenType::Semicolon, "Expect ';' after do-while statement.");
        }

        return rootNode;
    }

    static void printAst(const AstNode &node, int depth = 0) {
        std::string indent(depth * 2, ' '); // Обозначаем отступы

        // Вывод информации о текущем узле
        std::cout << indent << node.type << ": " << node.value << std::endl;

        // Рекурсивный обход дочерних узлов
        for (const auto &child: node.children) {
            printAst(child, depth + 1);
        }
    }


private:
    const std::vector<Token> &tokens;
    size_t current;

    Token advance() {
        if (!isAtEnd()) current++;
        return previous();
    }

    Token consume(TokenType type, const std::string &message) {
        if (check(type)) return advance();
        Error(message);
    }

    bool check(TokenType type) const {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    Token peek() const {
        return tokens[current];
    }

    Token previous() const {
        return tokens[current - 1];
    }

    bool isAtEnd() const {
        return peek().type == TokenType::EndOfStream;
    }

    AstNode parseDoWhile() {
        consume(TokenType::Do, "Expect 'do' for do-while loop.");
        AstNode doWhileNode{
                "DoWhile", "", {}
        };

        // В этом месте добавляем парсинг выражения после 'do'
        doWhileNode.children.push_back(parseExpression());

        // Парсинг 'while' и условия в круглых скобках
        consume(TokenType::While, "Expect 'while' after 'do' block.");
        consume(TokenType::LeftParen, "Expect '(' after 'while'.");
        doWhileNode.children.push_back(parseCondition());
        consume(TokenType::RightParen, "Expect ')' after 'while' condition.");

        return doWhileNode;
    }

    AstNode parseCondition() {
        AstNode conditionNode = {
                "Condition", "", {}
        };

        // Parse the whole comparison expression, considering the Assignment as well
        conditionNode.children.push_back(parseExpression());

        return conditionNode;
    }

    void Error(std::string_view error_msg) {
        std::string msg = "Error on token with number: ";
        msg
                .append(std::to_string(current))
                .append(", type of token: ")
                .append(ToStringView(peek().type))
                .append(", error: ")
                .append(error_msg);
        throw ASTError(msg);
    }

    AstNode parseExpression() {
        AstNode expressionNode = {
                "Expression", "", {}
        };

        Token lhs = consume(TokenType::Identifier, "Expect identifier on the left-hand side of condition.");
        expressionNode.children.push_back({"Identifier", lhs.value, {}});

        // Parse comparators that include =, <=, >=, and := tokens
        Token op;
        if (check(TokenType::LessEqual) ||
            check(TokenType::GreaterEqual) ||
            check(TokenType::Equal) ||
            check(TokenType::Assignment)) {
            op = advance();
        } else {
            Error("Expect comparison operator in condition.");
        }
        expressionNode.children.push_back({"Operator", op.value, {}});

        // Accept both an identifier or a hexadecimal number to the right-hand side of the comparison
        if (check(TokenType::Identifier) || check(TokenType::HexNumber)) {
            Token rhs = advance(); // Advances and gets the next token
            std::string rhsType = rhs.type == TokenType::Identifier ? "Identifier" : "HexNumber";
            expressionNode.children.push_back({rhsType, rhs.value, {}});
        } else {
            Error("Expect identifier or hexadecimal number on the right-hand side of condition.");
        }

        return expressionNode;
    }
};