// src/ast_nodes.cppm
export module algol126.ast;
import algol126.tokens;
import std;

export struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void print(int indent) const = 0;
};

export struct LiteralNode : public ASTNode {
    std::string value;
    TokenType type;
    LiteralNode(std::string val, TokenType t) : value(val), type(t) {}
    void print(int indent) const override { std::println("{:>{}}Literal/Ident: {}", "", indent, value); }
};

export struct AssignmentNode : public ASTNode {
    std::string var_name;
    std::shared_ptr<ASTNode> value_expr;
    bool is_export;
    std::string type_string;
    bool is_null_safe;
    AssignmentNode(std::string name, std::shared_ptr<ASTNode> val, bool exp = false) 
        : var_name(name), value_expr(val), is_export(exp), is_null_safe(true) {}
    void print(int indent) const override {
        std::println("{:>{}}{}{}Assign: {} :=", "", indent, (is_null_safe ? "" : "[Nullable] "), (is_export ? "Export " : ""), var_name);
        if (value_expr) value_expr->print(indent + 2);
    }
};

export struct PrintNode : public ASTNode {
    std::shared_ptr<ASTNode> expr;
    void print(int indent) const override {
        std::println("{:>{}}Native print(()) Command:", "", indent);
        if (expr) expr->print(indent + 2);
    }
};

export struct InputNode : public ASTNode {
    std::string var_name;
    void print(int indent) const override { std::println("{:>{}}Native input(()) Reading Target: {}", "", indent, var_name); }
};

export struct StructNode : public ASTNode {
    std::string name;
    std::vector<std::pair<std::string, std::string>> fields;
    void print(int indent) const override {
        std::println("{:>{}}Struct Memory Layout Def: {}", "", indent, name);
        for (const auto& [f_type, f_name] : fields) std::println("{:>{}}|- Field: {} {}", "", indent + 2, f_type, f_name);
    }
};

export struct ProcNode : public ASTNode {
    std::string name;
    std::vector<std::pair<std::string, std::string>> params;
    std::string ret_type;
    std::vector<std::shared_ptr<ASTNode>> body;
    void print(int indent) const override {
        std::println("{:>{}}Proc Definition Statement: {}() -> {}", "", indent, name, ret_type);
        for (const auto& stmt : body) stmt->print(indent + 2);
    }
};

export struct ArrayDeclNode : public ASTNode {
    std::string var_name;
    std::string type_string;
    std::string size;
    void print(int indent) const override {
        std::println("{:>{}}Flat Buffer Declaration: {} {}[{}]", "", indent, type_string, var_name, size);
    }
};

export struct ForLoopNode : public ASTNode {
    std::string iterator;
    std::shared_ptr<ASTNode> from_expr;
    std::shared_ptr<ASTNode> to_expr;
    std::vector<std::shared_ptr<ASTNode>> body;
    void print(int indent) const override {
        std::println("{:>{}}For Loop ({}):", "", indent, iterator);
        for (const auto& stmt : body) stmt->print(indent + 4);
    }
};

export struct TemplateStructNode : public ASTNode {
    std::string name;
    std::string placeholder_type_name;
    std::vector<std::pair<std::string, std::string>> fields; // {type, name}
    void print(int indent) const override {
        std::println("{:>{}}Template Generic Shape Def: {}<{}>", "", indent, name, placeholder_type_name);
    }
};

export struct ScaleBlockNode : public ASTNode {
    std::vector<std::shared_ptr<ASTNode>> body;
    void print(int indent) const override {
        std::println("{:>{}}Scale Unsafe Turbo Block:", "", indent);
        for (const auto& stmt : body) stmt->print(indent + 2);
    }
};

export struct BlockNode : public ASTNode {
    std::vector<std::shared_ptr<ASTNode>> statements;
    void print(int indent) const override {
        std::println("{:>{}}Block (begin ... end):", "", indent);
        for (const auto& stmt : statements) stmt->print(indent + 2);
    }
};
