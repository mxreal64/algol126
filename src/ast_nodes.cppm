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

export struct BinaryOpNode : public ASTNode {
    std::shared_ptr<ASTNode> left;
    TokenType op;
    std::shared_ptr<ASTNode> right;

    BinaryOpNode(std::shared_ptr<ASTNode> l, TokenType o, std::shared_ptr<ASTNode> r) 
        : left(l), op(o), right(r) {}

    void print(int indent) const override {
        std::println("{:>{}}BinaryOp:", "", indent);
        if (left) left->print(indent + 2);
        if (right) right->print(indent + 2);
    }
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

export struct IfStatementNode : public ASTNode {
    std::shared_ptr<ASTNode> condition;
    std::vector<std::shared_ptr<ASTNode>> then_branch;
    std::vector<std::pair<std::shared_ptr<ASTNode>, std::vector<std::shared_ptr<ASTNode>>>> elif_branches;
    std::vector<std::shared_ptr<ASTNode>> else_branch;

    IfStatementNode() : condition(nullptr) {}

    void print(int indent) const override {
        std::println("{:>{}}IfStatement:", "", indent);
        if (condition) condition->print(indent + 2);
    }
};

export struct IndexAccessNode : public ASTNode {
    std::shared_ptr<ASTNode> base;
    std::shared_ptr<ASTNode> index_expr;

    IndexAccessNode(std::shared_ptr<ASTNode> b, std::shared_ptr<ASTNode> idx) 
        : base(b), index_expr(idx) {}

    void print(int indent) const override {
        std::println("{:>{}}IndexAccess:", "", indent);
        if (base) base->print(indent + 2);
        if (index_expr) index_expr->print(indent + 2);
    }
};


export struct PointerDerefNode : public ASTNode {
    std::shared_ptr<ASTNode> expr;
    PointerDerefNode(std::shared_ptr<ASTNode> e) : expr(e) {}
};

export struct MemberAccessNode : public ASTNode {
    std::shared_ptr<ASTNode> base;
    std::string field_name;

    MemberAccessNode(std::shared_ptr<ASTNode> b, std::string f) 
        : base(b), field_name(f) {}

    void print(int indent) const override {
        std::println("{:>{}}MemberAccess: .{}", "", indent, field_name);
        if (base) base->print(indent + 2);
    }
};

export struct NullCheckNode : public ASTNode {
    std::shared_ptr<ASTNode> expr;

    NullCheckNode(std::shared_ptr<ASTNode> e) 
        : expr(e) {}

    void print(int indent) const override {
        std::println("{:>{}}NullCheck (?)", "", indent);
        if (expr) expr->print(indent + 2);
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
