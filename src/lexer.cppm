// ALGOL 126 Compiler Core (A126C)
// Copyright (c) 2026 mxreal64
//
// This Source Code Form is subject to the terms of the Mozilla Public 
// License, v. 2.0. If a copy of the MPL was not distributed with this 
// file, You can obtain one at http://mozilla.org.

// src/lexer.cppm
export module algol126.lexer;
import algol126.tokens;
import std;

export class Lexer {
private:
    std::unordered_map<std::string, TokenType> keywords;

    void init_keywords() {
        keywords["begin"] = TokenType::BEGIN;       keywords["end"] = TokenType::END;
        keywords["if"] = TokenType::IF;             keywords["elif"] = TokenType::ELIF;
        keywords["else"] = TokenType::ELSE;         keywords["fi"] = TokenType::FI;
        keywords["for"] = TokenType::FOR;           keywords["from"] = TokenType::FROM;
        keywords["to"] = TokenType::TO;             keywords["do"] = TokenType::DO;
        keywords["od"] = TokenType::OD;             keywords["scale"] = TokenType::SCALE;
        keywords["elacs"] = TokenType::ELACS;       keywords["vec4"] = TokenType::VEC4;
        keywords["ref"] = TokenType::REF;           keywords["void"] = TokenType::VOID;
        keywords["struct"] = TokenType::STRUCT;     keywords["proc"] = TokenType::PROC;
        keywords["template"] = TokenType::TEMPLATE; keywords["type"] = TokenType::TYPE;
        keywords["import"] = TokenType::IMPORT;     keywords["export"] = TokenType::EXPORT;
        keywords["print"] = TokenType::PRINT;       keywords["input"] = TokenType::INPUT;
        
        // Populate full primitive register widths
        keywords["int8"]   = TokenType::INT8;       keywords["uint8"]  = TokenType::UINT8;
        keywords["int16"]  = TokenType::INT16;      keywords["uint16"] = TokenType::UINT16;
        keywords["int32"]  = TokenType::INT32;      keywords["uint32"] = TokenType::UINT32;
        keywords["int64"]  = TokenType::INT64;      keywords["uint64"] = TokenType::UINT64;
        keywords["real32"] = TokenType::REAL32;     keywords["real64"] = TokenType::REAL64;
    }

public:
    Lexer() { init_keywords(); }

    std::vector<Token> tokenize(std::string_view source) {
        std::vector<Token> tokens;
        std::size_t i = 0; int line = 1;

        while (i < source.length()) {
            char c = source[i];
            if (c == '\n') { line++; i++; continue; }
            if (std::isspace(c)) { i++; continue; }

            if (c == '#') {
                i++;
                while (i < source.length() && source[i] != '#') { if (source[i] == '\n') line++; i++; }
                if (i < source.length()) i++; continue;
            }

            // Section 5 Character Relational Scanning Lookaheads
            if (c == ':' && i + 1 < source.length() && source[i + 1] == '=') {
                tokens.emplace_back(TokenType::WALRUS, ":=", line); i += 2; continue;
            }
            if (c == '=' && i + 1 < source.length() && source[i + 1] == '=') {
                tokens.emplace_back(TokenType::EQUAL, "==", line); i += 2; continue;
            }
            if (c == '!' && i + 1 < source.length() && source[i + 1] == '=') {
                tokens.emplace_back(TokenType::NOT_EQUAL, "!=", line); i += 2; continue;
            }
            if (c == '<' && i + 1 < source.length() && source[i + 1] == '=') {
                tokens.emplace_back(TokenType::LESS_EQUAL, "<=", line); i += 2; continue;
            }
            if (c == '>' && i + 1 < source.length() && source[i + 1] == '=') {
                tokens.emplace_back(TokenType::GREATER_EQUAL, ">=", line); i += 2; continue;
            }

            // Single Character Switch Core
            switch (c) {
                case ';': tokens.emplace_back(TokenType::SEMICOLON, ";", line); i++; continue;
                case '(': tokens.emplace_back(TokenType::L_PAREN, "(", line); i++; continue;
                case ')': tokens.emplace_back(TokenType::R_PAREN, ")", line); i++; continue;
                case '{': tokens.emplace_back(TokenType::L_BRACE, "{", line); i++; continue;
                case '}': tokens.emplace_back(TokenType::R_BRACE, "}", line); i++; continue;
                case '[': tokens.emplace_back(TokenType::L_BRACKET, "[", line); i++; continue;
                case ']': tokens.emplace_back(TokenType::R_BRACKET, "]", line); i++; continue;
                case '.': tokens.emplace_back(TokenType::DOT, ".", line); i++; continue;
                case '?': tokens.emplace_back(TokenType::QUESTION, "?", line); i++; continue;
                case '=': tokens.emplace_back(TokenType::EQUAL, "=", line); i++; continue;
                case '+': tokens.emplace_back(TokenType::PLUS, "+", line); i++; continue;
                case '-': tokens.emplace_back(TokenType::MINUS, "-", line); i++; continue;
                case '*': tokens.emplace_back(TokenType::MULTIPLY, "*", line); i++; continue;
                case '/': tokens.emplace_back(TokenType::DIVIDE, "/", line); i++; continue;
                case '%': tokens.emplace_back(TokenType::MODULO, "%", line); i++; continue;
                case '<': tokens.emplace_back(TokenType::LESS, "<", line); i++; continue;
                case '>': tokens.emplace_back(TokenType::GREATER, ">", line); i++; continue;
            }

            if (c == '"') {
                std::string s; i++;
                while (i < source.length() && source[i] != '"') { if (source[i] == '\n') line++; s += source[i++]; }
                if (i < source.length()) i++;
                tokens.emplace_back(TokenType::STRING_LITERAL, s, line); continue;
            }
            
            // Identifiers & flat keyword scanner
            if (std::isalpha(c) || c == '_') {
                std::string s;
                while (i < source.length() && (std::isalnum(source[i]) || source[i] == '_')) {
                    s += source[i++];
                }
                
                // FIXED: Ensure we only match exact keywords from the table, otherwise it's a raw variable identifier!
                if (keywords.contains(s)) {
                    tokens.emplace_back(keywords[s], s, line);
                } else {
                    tokens.emplace_back(TokenType::IDENTIFIER, s, line);
                }
                continue;
            }

            if (std::isdigit(c)) {
                std::string n;
                while (i < source.length() && std::isdigit(source[i])) n += source[i++];
                tokens.emplace_back(TokenType::INT_LITERAL, n, line); continue;
            }
            i++;
        }
        tokens.emplace_back(TokenType::END_OF_FILE, "EOF", line);
        return tokens;
    }
};
