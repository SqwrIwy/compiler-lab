#pragma once

#include <iostream>
#include <memory>
#include <cassert>
using namespace std;

static int func_cnt = 0;
static int val_cnt = 0;

class BaseAST
{
public:
    virtual ~BaseAST() = default;

    virtual void print_AST() const {}

    virtual string IR_string(shared_ptr<string> id) const = 0;
};

class CompUnitAST : public BaseAST
{
public:
    unique_ptr<BaseAST> func_def;

    void print_AST() const override
    {
        cout << "CompUnitAST { ";
        func_def->print_AST();
        cout << " }";
    }

    string IR_string(shared_ptr<string> id) const override
    {
        return func_def->IR_string(nullptr);
    }
};

class FuncDefAST : public BaseAST
{
public:
    unique_ptr<BaseAST> func_type;
    string ident;
    unique_ptr<BaseAST> block;

    void print_AST() const override
    {
        cout << "FuncDefAST { ";
        func_type->print_AST();
        cout << ", " << ident << ", ";
        block->print_AST();
        cout << " }";
    }

    string IR_string(shared_ptr<string> id) const override
    {
        return "fun @" + ident + "(): " + func_type->IR_string(nullptr) + "\n{\n" + block->IR_string(nullptr) + "}\n";
    }
};

class FuncTypeAST : public BaseAST
{
public:
    string _int;

    void print_AST() const override
    {
        cout << "FuncTypeAST { ";
        cout << _int;
        cout << " }";
    }

    string IR_string(shared_ptr<string> id) const override
    {
        assert(_int.compare(string("int")) == 0);
        return "i32";
    }
};

class BlockAST : public BaseAST
{
public:
    unique_ptr<BaseAST> stmt;

    void print_AST() const override
    {
        cout << "BlockAST { ";
        stmt->print_AST();
        cout << " }";
    }

    string IR_string(shared_ptr<string> id) const override
    {
        return "\%entry:\n" + stmt->IR_string(nullptr);
    }
};

class StmtAST : public BaseAST
{
public:
    string _return;
    unique_ptr<BaseAST> exp;

    void print_AST() const override
    {
        cout << "StmtAST { ";
        cout << _return << ", ";
        // exp->print_AST();
        cout << " }";
    }

    string IR_string(shared_ptr<string> id) const override
    {
        assert(_return.compare(string("return")) == 0);
        auto exp_id = make_shared<string>();
        string s = exp->IR_string(exp_id);
        return s + "ret " + *exp_id + "\n";
    }
};

class ExpAST : public BaseAST
{
public:
    unique_ptr<BaseAST> lor_exp;

    string IR_string(shared_ptr<string> id) const override
    {
        return lor_exp->IR_string(id);
    }
};

class PrimaryExpAST_0 : public BaseAST
{
public:
    unique_ptr<BaseAST> exp;

    string IR_string(shared_ptr<string> id) const override
    {
        return exp->IR_string(id);
    }
};

class PrimaryExpAST_1 : public BaseAST
{
public:
    unique_ptr<BaseAST> number;

    string IR_string(shared_ptr<string> id) const override
    {
        *id = number->IR_string(nullptr);
        return "";
    }
};

class NumberAST : public BaseAST
{
public:
    int int_const;

    void print_AST() const override
    {
        cout << "NumberAST { ";
        cout << int_const;
        cout << " }";
    }

    string IR_string(shared_ptr<string> id) const override
    {
        return to_string(int_const);
    }
};

class UnaryExpAST_0 : public BaseAST {
public:
    unique_ptr<BaseAST> primary_exp;

    string IR_string(shared_ptr<string> id) const override
    {
        return primary_exp->IR_string(id);
    }
};

class UnaryExpAST_1 : public BaseAST
{
public:
    unique_ptr<BaseAST> unary_op;
    unique_ptr<BaseAST> unary_exp;

    string IR_string(shared_ptr<string> id) const override
    {
        auto unary_exp_id = make_shared<string>();
        string s = unary_exp->IR_string(unary_exp_id);
        char op = unary_op->IR_string(nullptr)[0];
        if (op == '+')
        {
            *id = *unary_exp_id;
            return s;
        }
        if (op == '-')
        {
            *id = "\%" + to_string(val_cnt++);
            return s + *id + " = sub 0, " + *unary_exp_id + "\n";
        }
        if (op == '!')
        {
            *id = "\%" + to_string(val_cnt++);
            return s + *id + " = eq 0, " + *unary_exp_id + "\n";
        }
        assert(false);
    }
};

class UnaryOpAST : public BaseAST
{
public:
    string op;

    string IR_string(shared_ptr<string> id) const override
    {
        return op;
    }
};

class MulExpAST_0 : public BaseAST
{
public:
    unique_ptr<BaseAST> unary_exp;

    string IR_string(shared_ptr<string> id) const override
    {
        return unary_exp->IR_string(id);
    }
};

class MulExpAST_1 : public BaseAST
{
public:
    unique_ptr<BaseAST> mul_exp;
    char op;
    unique_ptr<BaseAST> unary_exp;

    string IR_string(shared_ptr<string> id) const override
    {
        auto mul_exp_id = make_shared<string>();
        auto unary_exp_id = make_shared<string>();
        string s1 = mul_exp->IR_string(mul_exp_id);
        string s2 = unary_exp->IR_string(unary_exp_id);
        string s3;
        *id = "%" + to_string(val_cnt++);
        switch (op)
        {
        case '*':
        {
            s3 = *id + " = mul " + *mul_exp_id + ", " + *unary_exp_id + "\n";
            break;
        }

        case '/':
        {
            s3 = *id + " = div " + *mul_exp_id + ", " + *unary_exp_id + "\n";
            break;
        }

        case '%':
        {
            s3 = *id + " = mod " + *mul_exp_id + ", " + *unary_exp_id + "\n";
            break;
        }
        
        default:
        {
            assert(false);
        }
        }
        return s1 + s2 + s3;
    }
};

class AddExpAST_0 : public BaseAST
{
public:
    unique_ptr<BaseAST> mul_exp;

    string IR_string(shared_ptr<string> id) const override
    {
        return mul_exp->IR_string(id);
    }
};

class AddExpAST_1 : public BaseAST
{
public:
    unique_ptr<BaseAST> add_exp;
    char op;
    unique_ptr<BaseAST> mul_exp;

    string IR_string(shared_ptr<string> id) const override
    {
        auto add_exp_id = make_shared<string>();
        auto mul_exp_id = make_shared<string>();
        string s1 = add_exp->IR_string(add_exp_id);
        string s2 = mul_exp->IR_string(mul_exp_id);
        string s3;
        *id = "%" + to_string(val_cnt++);
        switch (op)
        {
        case '+':
        {
            s3 = *id + " = add " + *add_exp_id + ", " + *mul_exp_id + "\n";
            break;
        }

        case '-':
        {
            s3 = *id + " = sub " + *add_exp_id + ", " + *mul_exp_id + "\n";
            break;
        }
        
        default:
        {
            assert(false);
        }
        }
        return s1 + s2 + s3;
    }
};

class RelExpAST_0 : public BaseAST
{
public:
    unique_ptr<BaseAST> add_exp;

    string IR_string(shared_ptr<string> id) const override
    {
        return add_exp->IR_string(id);
    }
};

class RelExpAST_1 : public BaseAST
{
public:
    unique_ptr<BaseAST> rel_exp;
    int op;
    unique_ptr<BaseAST> add_exp;

    string IR_string(shared_ptr<string> id) const override
    {
        auto rel_exp_id = make_shared<string>();
        auto add_exp_id = make_shared<string>();
        string s1 = rel_exp->IR_string(rel_exp_id);
        string s2 = add_exp->IR_string(add_exp_id);
        string s3;
        *id = "%" + to_string(val_cnt++);
        switch (op)
        {
        case 0:
        {
            s3 = *id + " = lt " + *rel_exp_id + ", " + *add_exp_id + "\n";
            break;
        }

        case 1:
        {
            s3 = *id + " = gt " + *rel_exp_id + ", " + *add_exp_id + "\n";
            break;
        }

        case 2:
        {
            s3 = *id + " = le " + *rel_exp_id + ", " + *add_exp_id + "\n";
            break;
        }

        case 3:
        {
            s3 = *id + " = ge " + *rel_exp_id + ", " + *add_exp_id + "\n";
            break;
        }
        
        default:
        {
            assert(false);
        }
        }
        return s1 + s2 + s3;
    }
};

class EqExpAST_0 : public BaseAST
{
public:
    unique_ptr<BaseAST> rel_exp;

    string IR_string(shared_ptr<string> id) const override
    {
        return rel_exp->IR_string(id);
    }
};

class EqExpAST_1 : public BaseAST
{
public:
    unique_ptr<BaseAST> eq_exp;
    int op;
    unique_ptr<BaseAST> rel_exp;

    string IR_string(shared_ptr<string> id) const override
    {
        auto eq_exp_id = make_shared<string>();
        auto rel_exp_id = make_shared<string>();
        string s1 = eq_exp->IR_string(eq_exp_id);
        string s2 = rel_exp->IR_string(rel_exp_id);
        string s3;
        *id = "%" + to_string(val_cnt++);
        switch (op)
        {
        case 0:
        {
            s3 = *id + " = eq " + *eq_exp_id + ", " + *rel_exp_id + "\n";
            break;
        }

        case 1:
        {
            s3 = *id + " = ne " + *eq_exp_id + ", " + *rel_exp_id + "\n";
            break;
        }
        
        default:
        {
            assert(false);
        }
        }
        return s1 + s2 + s3;
    }
};

class LAndExpAST_0 : public BaseAST
{
public:
    unique_ptr<BaseAST> eq_exp;

    string IR_string(shared_ptr<string> id) const override
    {
        return eq_exp->IR_string(id);
    }
};

class LAndExpAST_1 : public BaseAST
{
public:
    unique_ptr<BaseAST> land_exp;
    unique_ptr<BaseAST> eq_exp;

    string IR_string(shared_ptr<string> id) const override
    {
        auto land_exp_id = make_shared<string>();
        auto eq_exp_id = make_shared<string>();
        string s1 = land_exp->IR_string(land_exp_id);
        string s2 = eq_exp->IR_string(eq_exp_id);
        string s3, s4, s5;
        string id1 = "%" + to_string(val_cnt++);
        string id2 = "%" + to_string(val_cnt++);
        *id = "%" + to_string(val_cnt++);
        s3 = id1 + " = ne " + *land_exp_id + ", 0\n";
        s4 = id2 + " = ne " + *eq_exp_id + ", 0\n";
        s5 = *id + " = and " + id1 + ", " + id2 + "\n";
        return s1 + s2 + s3 + s4 + s5;
    }
};

class LOrExpAST_0 : public BaseAST
{
public:
    unique_ptr<BaseAST> land_exp;

    string IR_string(shared_ptr<string> id) const override
    {
        return land_exp->IR_string(id);
    }
};

class LOrExpAST_1 : public BaseAST
{
public:
    unique_ptr<BaseAST> lor_exp;
    unique_ptr<BaseAST> land_exp;

    string IR_string(shared_ptr<string> id) const override
    {
        auto lor_exp_id = make_shared<string>();
        auto land_exp_id = make_shared<string>();
        string s1 = lor_exp->IR_string(lor_exp_id);
        string s2 = land_exp->IR_string(land_exp_id);
        string s3, s4, s5;
        string id1 = "%" + to_string(val_cnt++);
        string id2 = "%" + to_string(val_cnt++);
        *id = "%" + to_string(val_cnt++);
        s3 = id1 + " = ne " + *lor_exp_id + ", 0\n";
        s4 = id2 + " = ne " + *land_exp_id + ", 0\n";
        s5 = *id + " = or " + id1 + ", " + id2 + "\n";
        return s1 + s2 + s3 + s4 + s5;
    }
};