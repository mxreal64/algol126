// ALGOL 126 Compiler Core (A126C)
// Copyright (c) 2026 mxreal64
//
// This Source Code Form is subject to the terms of the Mozilla Public 
// License, v. 2.0. If a copy of the MPL was not distributed with this 
// file, You can obtain one at http://mozilla.org.

// src/parser.cppm
export module algol126.parser;
import algol126.tokens;
import algol126.lexer;
import algol126.ast;
import std;

export class Parser {
private:
    std::vector<Token> tokens;
    std::size_t cursor = 0;
    std::unordered_map<std::string, std::shared_ptr<TemplateStructNode>> template_registry;
    Token peek() const { return cursor < tokens.size() ? tokens[cursor] : Token{TokenType::END_OF_FILE, "EOF", 0}; 
    }
    bool match(TokenType t) { 
    if (peek().type == t) { 
    cursor++; 
    return true; 
    } return false; 
    }
    Token consume(TokenType t, std::string_view m) { if (peek().type != t) throw std::runtime_error(std::format("Parser Error [Line {}]: {}", peek().line, m)); return tokens[cursor++];
     }
public:
    Parser(const std::vector<Token>& t) : tokens(t) {}
    std::shared_ptr<ASTNode> parse_program() {
        consume(TokenType::BEGIN, "Missing file 'begin' wrapper.");
        auto r = std::make_shared<BlockNode>();
        while (peek().type != TokenType::END && peek().type != TokenType::END_OF_FILE) r->statements.push_back(parse_statement());
        consume(TokenType::END, "Missing file 'end' wrapper."); 
        match(TokenType::SEMICOLON); 
        return r;
    }
private:
    std::shared_ptr<ASTNode> parse_statement() {
        if (match(TokenType::IMPORT)) {
            Token p_tok = consume(TokenType::STRING_LITERAL, "Expected path."); 
            consume(TokenType::SEMICOLON, "Missing ';'.");
            std::filesystem::path m_path(p_tok.value); 
            if (!std::filesystem::exists(m_path)) m_path = std::string(p_tok.value) + ".a26";
            if (std::filesystem::exists(m_path)) {
                std::ifstream m_file(m_path); 
                std::stringstream buf; 
                buf << m_file.rdbuf(); 
                Lexer m_lex; 
                auto m_toks = m_lex.tokenize(buf.str());
                if (!m_toks.empty() && m_toks.back().type == TokenType::END_OF_FILE) m_toks.pop_back();
                tokens.insert(tokens.begin() + cursor, m_toks.begin(), m_toks.end());
            } else throw std::runtime_error("Module Ingestion Fault: " + std::string(p_tok.value));
            return std::make_shared<BlockNode>();
        }
        if (match(TokenType::TEMPLATE)) {
            consume(TokenType::L_PAREN, "Expected '('."); consume(TokenType::TYPE, "Expected 'type'.");
            std::string tp{consume(TokenType::IDENTIFIER, "Expected placeholder.").value}; 
            consume(TokenType::R_PAREN, "Expected ')'.");
            consume(TokenType::STRUCT, "Expected 'struct'."); 
            auto tn = std::make_shared<TemplateStructNode>();
            tn->name = std::string{consume(TokenType::IDENTIFIER, "Expected name.").value}; tn->placeholder_type_name = tp; 
            consume(TokenType::L_BRACE, "Expected '{'.");
            while (peek().type != TokenType::R_BRACE && peek().type != TokenType::END_OF_FILE) {
                std::string ft{consume(peek().type, "Expected field type.").value}; 
                std::string fn{consume(TokenType::IDENTIFIER, "Expected field name.").value};
                consume(TokenType::SEMICOLON, "Missing internal ';'."); tn->fields.emplace_back(ft, fn);
            }
            consume(TokenType::R_BRACE, "Missing '}'."); consume(TokenType::SEMICOLON, "Missing ';'.");
            template_registry[tn->name] = tn; return tn;
        }
        if (peek().type == TokenType::STRUCT) {
            cursor++; auto s = std::make_shared<StructNode>(); s->name = std::string{consume(TokenType::IDENTIFIER, "Expected struct name.").value}; consume(TokenType::L_BRACE, "Expected '{'.");
            while (peek().type != TokenType::R_BRACE && peek().type != TokenType::END_OF_FILE) {
                std::string ft{consume(peek().type, "Expected field type.").value};
                std::string fn{consume(TokenType::IDENTIFIER, "Expected field name.").value};
                s->fields.emplace_back(ft, fn); consume(TokenType::SEMICOLON, "Missing field ';'.");
            }
            consume(TokenType::R_BRACE, "Missing closing '}'."); consume(TokenType::SEMICOLON, "Missing struct ';'."); return s;
        }
        if (match(TokenType::PROC)) {
            auto f = std::make_shared<ProcNode>(); f->name = std::string{consume(TokenType::IDENTIFIER, "Expected function name.").value};
            consume(TokenType::EQUAL, "Expected '='."); consume(TokenType::L_PAREN, "Expected '('.");
            while (peek().type != TokenType::R_PAREN && peek().type != TokenType::END_OF_FILE) {
                std::string pt{consume(peek().type, "Expected parameter type.").value};
                std::string pn{consume(TokenType::IDENTIFIER, "Expected parameter name.").value};
                f->params.emplace_back(pt, pn);
            }
            consume(TokenType::R_PAREN, "Expected ')'."); f->ret_type = std::string{consume(peek().type, "Expected return type.").value}; consume(TokenType::L_BRACE, "Expected '{'.");
            while (peek().type != TokenType::R_BRACE && peek().type != TokenType::END_OF_FILE) f->body.push_back(parse_statement());
            consume(TokenType::R_BRACE, "Expected closing '}' boundary."); consume(TokenType::SEMICOLON, "Missing function ';'."); return f;
        }
        // 1. Check control loops FIRST
        if (match(TokenType::FOR)) {
            auto l = std::make_shared<ForLoopNode>(); 
            l->iterator = std::string{consume(TokenType::IDENTIFIER, "Expected iterator variable.").value}; 
            consume(TokenType::FROM, "Expected 'from'."); l->from_expr = parse_expression(); 
            consume(TokenType::TO, "Expected 'to'."); l->to_expr = parse_expression();
            consume(TokenType::DO, "Expected 'do'."); 
            while (peek().type != TokenType::OD && peek().type != TokenType::END_OF_FILE) l->body.push_back(parse_statement());
            consume(TokenType::OD, "Missing 'od'."); 
            consume(TokenType::SEMICOLON, "Missing ';'."); 
            return l;
        }
        if (match(TokenType::SCALE)) {
            auto s = std::make_shared<ScaleBlockNode>(); while (peek().type != TokenType::ELACS && peek().type != TokenType::END_OF_FILE) s->body.push_back(parse_statement());
            consume(TokenType::ELACS, "Missing 'elacs'."); 
            consume(TokenType::SEMICOLON, "Missing ';'."); 
            return s;
        }
        if (match(TokenType::PRINT)) {
            consume(TokenType::L_PAREN, "Expected (('"); 
            consume(TokenType::L_PAREN, "Expected (('"); 
            auto p = std::make_shared<PrintNode>(); 
            p->expr = parse_expression(); 
            consume(TokenType::R_PAREN, "Expected ))"); 
            consume(TokenType::R_PAREN, "Expected ))"); 
            consume(TokenType::SEMICOLON, "Missing ';'."); 
            return p;
        }
        if (match(TokenType::IF)) {
            auto node = std::make_shared<IfStatementNode>();
            node->condition = parse_expression();
            consume(TokenType::DO, "Expected 'do' after condition.");
            
            while (peek().type != TokenType::ELIF && peek().type != TokenType::ELSE && 
                   peek().type != TokenType::FI && peek().type != TokenType::END_OF_FILE) {
                node->then_branch.push_back(parse_statement());
            }
            
            while (match(TokenType::ELIF)) {
                auto elif_cond = parse_expression();
                consume(TokenType::DO, "Expected 'do' after elif condition.");
                std::vector<std::shared_ptr<ASTNode>> elif_body;
                while (peek().type != TokenType::ELIF && peek().type != TokenType::ELSE && 
                       peek().type != TokenType::FI && peek().type != TokenType::END_OF_FILE) {
                    elif_body.push_back(parse_statement());
                }
                node->elif_branches.emplace_back(elif_cond, elif_body);
            }
            if (match(TokenType::ELSE)) {
                while (peek().type != TokenType::FI && peek().type != TokenType::END_OF_FILE) {
                    node->else_branch.push_back(parse_statement());
                }
            }
            consume(TokenType::FI, "Expected 'fi' to terminate conditional block.");
            consume(TokenType::SEMICOLON, "Missing trailing ';'.");
            return node;
        }

        // ===================================================================
        // UNIFIED PROPERTY ASSIGNMENT, MUTATION, AND TYPE ALLOCATION ENGINE
        // ===================================================================
        bool ie = match(TokenType::EXPORT), io = match(TokenType::QUESTION), ir = match(TokenType::REF); 
        Token tt = peek();

        if ((tt.type >= TokenType::INT8 && tt.type <= TokenType::VEC4) || tt.type == TokenType::IDENTIFIER) {
            // 1. Process MUTATIONS or REASSIGNMENTS first to protect memory bounds (e.g., hero.health := 100; or step_counter := x;)
            if (tt.type == TokenType::IDENTIFIER && cursor + 1 < tokens.size() && 
               (tokens[cursor + 1].type == TokenType::WALRUS || tokens[cursor + 1].type == TokenType::DOT)) {
                
                Token id = consume(TokenType::IDENTIFIER, "Expected tracking variable.");
                std::string target_name_str{id.value};

                // Accumulate compound property access chains iteratively
                while (peek().type == TokenType::DOT) {
                    cursor++;
                    Token field = consume(TokenType::IDENTIFIER, "Expected member property field name.");
                    target_name_str += "." + std::string(field.value);
                }

                consume(TokenType::WALRUS, "Expected assignment walrus (:=).");
                auto v = parse_expression(); 
                consume(TokenType::SEMICOLON, "Missing trailing ';'.");
                
                auto n = std::make_shared<AssignmentNode>(target_name_str, v, false);
                n->type_string = "";
                return n;
            }

            // 2. Process NEW TYPE DECLARATIONS (e.g., player_t hero := {}; or int32 a := 10;)
            cursor++; // Safely consumes type keyword or structural blueprint identifier string
            std::string act_t{tt.value};
            std::string raw_ident{tt.value};

            // Process specialized template layouts if applicable
            if (template_registry.contains(raw_ident) && peek().type == TokenType::L_PAREN) {
                cursor++; 
                std::string spec_t{consume(peek().type, "Expected concrete specialization type.").value}; 
                consume(TokenType::R_PAREN, "Expected ')'.");
                
                auto t_base = template_registry[raw_ident]; 
                auto concrete = std::make_shared<StructNode>();
                Token id = consume(TokenType::IDENTIFIER, "Expected tracking identifier.");
                concrete->name = std::string{id.value};
                
                for (const auto& [f_t, f_n] : t_base->fields) {
                    concrete->fields.emplace_back(f_t == t_base->placeholder_type_name ? spec_t : f_t, f_n);
                }
                act_t = raw_ident + "_" + spec_t;
                std::string var_name_str{id.value};

                if (match(TokenType::SEMICOLON)) {
                    auto n = std::make_shared<AssignmentNode>(
                        std::string{var_name_str}, 
                        std::make_shared<LiteralNode>(std::string{"{}"}, TokenType::IDENTIFIER), 
                        ie
                    );
                    n->type_string = act_t; 
                    n->is_null_safe = !io; 
                    return n;
                }

                consume(TokenType::WALRUS, "Expected assignment walrus (:=).");
                auto v = parse_expression(); 
                consume(TokenType::SEMICOLON, "Missing ';'.");

                auto n = std::make_shared<AssignmentNode>(std::string{var_name_str}, v, ie); 
                n->type_string = act_t; 
                n->is_null_safe = !io; 
                return n;
            }

            // Standard variable declaration processing path
            Token id = consume(TokenType::IDENTIFIER, "Expected tracking identifier.");
            std::string var_name_str{id.value};
            consume(TokenType::WALRUS, "Expected assignment walrus (:=).");

            if (match(TokenType::INPUT)) {
                consume(TokenType::L_PAREN, "Expected (('"); 
                consume(TokenType::L_PAREN, "Expected (('");
                consume(TokenType::R_PAREN, "Expected ))"); 
                consume(TokenType::R_PAREN, "Expected ))");
                consume(TokenType::SEMICOLON, "Missing input tracking ';'.");

                auto inp = std::make_shared<InputNode>();
                inp->var_name = var_name_str;

                auto n = std::make_shared<AssignmentNode>(std::string{var_name_str}, inp, ie);
                n->type_string = (ir ? "void*" : act_t);
                return n;
            }

            auto v = parse_expression();
            consume(TokenType::SEMICOLON, "Missing trailing statement ';'.");

            auto n = std::make_shared<AssignmentNode>(std::string{var_name_str}, v, ie);
            n->type_string = (ir ? act_t + "*" : act_t);
            n->is_null_safe = !io;
            return n;
        }

        // 3. Fallback: Parse standalone raw expression statements
        auto e = parse_expression();
        match(TokenType::SEMICOLON);
        return e;
    }

    std::shared_ptr<ASTNode> parse_primary() {
        Token t = peek();
        std::shared_ptr<ASTNode> node = nullptr;
        if (match(TokenType::IDENTIFIER) || match(TokenType::INT_LITERAL) || match(TokenType::REAL_LITERAL) || match(TokenType::STRING_LITERAL)) {
            if (t.type == TokenType::IDENTIFIER && peek().type == TokenType::L_PAREN) {
                std::string cb = std::string(t.value) + "("; 
                cursor++; 
                bool fa = true;
                while (peek().type != TokenType::R_PAREN && peek().type != TokenType::END_OF_FILE) {
                    Token at = tokens[cursor++]; 
                    if (at.type == TokenType::IDENTIFIER || at.type == TokenType::INT_LITERAL || at.type == TokenType::REAL_LITERAL) { 
                        if (!fa) cb += ", "; 
                        cb += std::string(at.value); 
                        fa = false; 
                    }
                }
                consume(TokenType::R_PAREN, "Missing closing parenthesis."); 
                cb += ")"; 
                node = std::make_shared<LiteralNode>(cb, TokenType::IDENTIFIER);
            } else {
                node = std::make_shared<LiteralNode>(std::string(t.value), t.type);
            }
        } else if (match(TokenType::VEC4)) {
            consume(TokenType::L_PAREN, "Missing arguments."); 
            while (peek().type != TokenType::R_PAREN && peek().type != TokenType::END_OF_FILE) cursor++;
            consume(TokenType::R_PAREN, "Missing bracket constraint."); 
            node = std::make_shared<LiteralNode>(std::string("0"), TokenType::VEC4);
        } else if (match(TokenType::VOID)) { 
            node = std::make_shared<LiteralNode>(std::string("0"), TokenType::VOID); 
        } else if (match(TokenType::L_BRACE)) {
            // Uninstantiated struct initializer brace track handling ({})
            while (peek().type != TokenType::R_BRACE && peek().type != TokenType::END_OF_FILE) {
                cursor++;
            }
            consume(TokenType::R_BRACE, "Expected closing '}' inside layout instantiation block.");
            node = std::make_shared<LiteralNode>(std::string{"0"}, TokenType::INT_LITERAL); // Safely fall back to a clean 0 initialization token state
        } else {
            throw std::runtime_error("Unexpected statement parser expression fault: " + std::string(t.value));
        }
        while (true) {
            if (match(TokenType::L_BRACKET)) {
                auto idx_expr = parse_expression();
                consume(TokenType::R_BRACKET, "Expected closing ']' bracket.");
                node = std::make_shared<IndexAccessNode>(node, idx_expr);
            }
            else if (match(TokenType::DOT)) {
                Token field = consume(TokenType::IDENTIFIER, "Expected field or swizzle identifier after '.'.");
                node = std::make_shared<MemberAccessNode>(node, std::string{field.value});
            }
            else if (match(TokenType::QUESTION)) {
                node = std::make_shared<NullCheckNode>(node);
            }
            else {
                break;
            }
        }
        return node;
    }
    std::shared_ptr<ASTNode> parse_expression() {
        std::shared_ptr<ASTNode> l = parse_primary();
        while (peek().type >= TokenType::PLUS && peek().type <= TokenType::GREATER_EQUAL) {
            Token ot = tokens[cursor++]; // Consume operator token safely
            auto r = parse_primary();    // Enforce left-associative sequence bindings
            l = std::make_shared<BinaryOpNode>(l, ot.type, r);
        }
        return l;
    }
};
