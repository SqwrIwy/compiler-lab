#pragma once

#include <iostream>
#include <memory>
#include <cassert>
#include <tr1/unordered_map>
using namespace std;

static int func_cnt = 0;
static int val_cnt = 0;

static tr1::unordered_map<string, int> consts;
static tr1::unordered_map<string, string> vars;

class BaseAST
{
public:
    virtual ~BaseAST() = default;

    virtual string IR_string(shared_ptr<string> id) const
    {
        return "";
    }

    virtual int get_value() const
    {
        return 0;
    }

    virtual string get_ident() const
    {
        return "";
    }
};

class CompUnitAST : public BaseAST
{
public:
    unique_ptr<BaseAST> func_def;

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

    string IR_string(shared_ptr<string> id) const override
    {
        return "fun @" + ident + "(): " + func_type->IR_string(nullptr) + "\n{\n" + block->IR_string(nullptr) + "}\n";
    }
};

class FuncTypeAST : public BaseAST
{
public:
    string _int;

    string IR_string(shared_ptr<string> id) const override
    {
        assert(_int.compare(string("int")) == 0);
        return "i32";
    }
};

class BlockAST : public BaseAST
{
public:
    unique_ptr<BaseAST> block_items;

    string IR_string(shared_ptr<string> id) const override
    {
        return "\%entry:\n" + block_items->IR_string(nullptr);
    }
};

class BlockItemsAST : public BaseAST
{
public:
    unique_ptr<BaseAST> block_items;
    unique_ptr<BaseAST> block_item;

    string IR_string(shared_ptr<string> id) const override
    {
        string s;
        if (block_items != nullptr)
            s += block_items->IR_string(nullptr);
        s += block_item->IR_string(nullptr);
        return s;
    }
};

class BlockItemAST_0 : public BaseAST
{
public:
    unique_ptr<BaseAST> decl;

    string IR_string(shared_ptr<string> id) const override
    {
        return decl->IR_string(nullptr);
    }
};

class BlockItemAST_1 : public BaseAST
{
public:
    unique_ptr<BaseAST> stmt;

    string IR_string(shared_ptr<string> id) const override
    {
        return stmt->IR_string(nullptr);
    }
};

class StmtAST_0 : public BaseAST
{
public:
    string _return;
    unique_ptr<BaseAST> exp;

    string IR_string(shared_ptr<string> id) const override
    {
        assert(_return.compare(string("return")) == 0);
        auto exp_id = make_shared<string>();
        string s = exp->IR_string(exp_id);
        return s + "ret " + *exp_id + "\n";
    }
};

class StmtAST_1 : public BaseAST
{
public:
    unique_ptr<BaseAST> lval;
    unique_ptr<BaseAST> exp;

    string IR_string(shared_ptr<string> id) const override
    {
        auto lval_id = make_shared<string>();
        auto exp_id = make_shared<string>();
        assert(vars.count(lval->get_ident()) != 0);
        string s = exp->IR_string(exp_id);
        return s + "store " + *exp_id + ", " + vars[lval->get_ident()] + "\n";
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

    int get_value() const override
    {
        return lor_exp->get_value();
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

    int get_value() const override
    {
        return exp->get_value();
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

    int get_value() const override
    {
        return number->get_value();
    }
};

class PrimaryExpAST_2 : public BaseAST
{
public:
    unique_ptr<BaseAST> lval;

    string IR_string(shared_ptr<string> id) const override
    {
        return lval->IR_string(id);
    }

    int get_value() const override
    {
        return lval->get_value();
    }
};

class NumberAST : public BaseAST
{
public:
    int int_const;

    string IR_string(shared_ptr<string> id) const override
    {
        return to_string(int_const);
    }

    int get_value() const override
    {
        return int_const;
    }
};

class UnaryExpAST_0 : public BaseAST {
public:
    unique_ptr<BaseAST> primary_exp;

    string IR_string(shared_ptr<string> id) const override
    {
        return primary_exp->IR_string(id);
    }

    int get_value() const override
    {
        return primary_exp->get_value();
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
        switch (op)
        {
        case '+':
        {
            *id = *unary_exp_id;
            return s;
        }

        case '-':
        {
            *id = "\%" + to_string(val_cnt++);
            return s + *id + " = sub 0, " + *unary_exp_id + "\n";
        }

        case '|':
        {
            *id = "\%" + to_string(val_cnt++);
            return s + *id + " = eq 0, " + *unary_exp_id + "\n";
        }
        
        default:
        {
            assert(false);
        }
        }
    }

    int get_value() const override
    {
        int x = unary_exp->get_value();
        char op = unary_op->IR_string(nullptr)[0];
        switch (op)
        {
        case '+':
            return x;

        case '-':
            return -x;

        case '!':
            return !x;

        default:
            assert(false);
        }
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

    int get_value() const override
    {
        return unary_exp->get_value();
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

    int get_value() const override
    {
        int x = mul_exp->get_value();
        int y = unary_exp->get_value();
        switch (op)
        {
        case '*':
            return x * y;

        case '/':
            return x / y;

        case '%':
            return x % y;

        default:
            assert(false);
        }
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

    int get_value() const override
    {
        return mul_exp->get_value();
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

    int get_value() const override
    {
        int x = add_exp->get_value();
        int y = mul_exp->get_value();
        switch (op)
        {
        case '+':
            return x + y;

        case '-':
            return x - y;

        default:
            assert(false);
        }
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

    int get_value() const override
    {
        return add_exp->get_value();
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

    int get_value() const override
    {
        int x = rel_exp->get_value();
        int y = add_exp->get_value();
        switch (op)
        {
        case 0:
            return static_cast<int>(x < y);

        case 1:
            return static_cast<int>(x > y);

        case 2:
            return static_cast<int>(x <= y);

        case 3:
            return static_cast<int>(x >= y);

        default:
            assert(false);
        }
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

    int get_value() const override
    {
        return rel_exp->get_value();
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

    int get_value() const override
    {
        int x = eq_exp->get_value();
        int y = rel_exp->get_value();
        switch (op)
        {
        case 0:
            return static_cast<int>(x == y);

        case 1:
            return static_cast<int>(x != y);

        default:
            assert(false);
        }
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

    int get_value() const override
    {
        return eq_exp->get_value();
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

    int get_value() const override
    {
        int x = land_exp->get_value();
        int y = eq_exp->get_value();
        return static_cast<int>(x && y);
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

    int get_value() const override
    {
        return land_exp->get_value();
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

    int get_value() const override
    {
        int x = lor_exp->get_value();
        int y = land_exp->get_value();
        return static_cast<int>(x || y);
    }
};

class DeclAST_0 : public BaseAST
{
public:
    unique_ptr<BaseAST> const_decl;

    string IR_string(shared_ptr<string> id) const override
    {
        return const_decl->IR_string(nullptr);
    }
};

class DeclAST_1 : public BaseAST
{
public:
    unique_ptr<BaseAST> var_decl;

    string IR_string(shared_ptr<string> id) const override
    {
        return var_decl->IR_string(nullptr);
    }
};

class ConstDeclAST : public BaseAST
{
public:
    unique_ptr<BaseAST> btype;
    unique_ptr<BaseAST> const_defs;

    string IR_string(shared_ptr<string> id) const override
    {
        return const_defs->IR_string(nullptr);
    }
};

class ConstDefsAST : public BaseAST
{
public:
    unique_ptr<BaseAST> const_defs;
    unique_ptr<BaseAST> const_def;

    string IR_string(shared_ptr<string> id) const override
    {
        string s;
        if (const_defs != nullptr)
            s += const_defs->IR_string(nullptr);
        s += const_def->IR_string(nullptr);
        return s;
    }
};

class BTypeAST : public BaseAST
{
public:
    string btype;

    string IR_string(shared_ptr<string> id) const override
    {
        return btype;
    }
};

class ConstDefAST : public BaseAST
{
public:
    string ident;
    unique_ptr<BaseAST> const_init_val;

    string IR_string(shared_ptr<string> id) const override
    {
        assert(consts.count(ident) == 0);
        consts[ident] = const_init_val->get_value();
        return "";
    }
};

class ConstInitValAST : public BaseAST
{
public:
    unique_ptr<BaseAST> const_exp;

    int get_value() const override
    {
        return const_exp->get_value();
    }
};

class LValAST : public BaseAST
{
public:
    string ident;

    string IR_string(shared_ptr<string> id) const override
    {
        if (vars.count(ident) != 0)
        {
            *id = "\%" + to_string(val_cnt++);
            return *id + " = load " + vars[ident] + "\n";
        }
        if (consts.count(ident) != 0)
        {
            *id = to_string(this->get_value());
            return "";
        }
        assert(false);
    }

    int get_value() const override
    {
        assert(consts.count(ident) != 0);
        return consts[ident];
    }

    string get_ident() const override
    {
        return ident;
    }
};

class ConstExpAST : public BaseAST
{
public:
    unique_ptr<BaseAST> exp;

    int get_value() const override
    {
        return exp->get_value();
    }
};

class VarDeclAST : public BaseAST
{
public:
    unique_ptr<BaseAST> btype;
    unique_ptr<BaseAST> var_defs;

    string IR_string(shared_ptr<string> id) const override
    {
        assert(btype->IR_string(nullptr) == "int");
        return var_defs->IR_string(nullptr);
    }
};

class VarDefsAST : public BaseAST
{
public:
    unique_ptr<BaseAST> var_defs;
    unique_ptr<BaseAST> var_def;

    string IR_string(shared_ptr<string> id) const override
    {
        string s;
        if (var_defs != nullptr)
            s += var_defs->IR_string(nullptr);
        s += var_def->IR_string(nullptr);
        return s;
    }
};

class VarDefAST : public BaseAST
{
public:
    string ident;
    unique_ptr<BaseAST> init_val;

    string IR_string(shared_ptr<string> id) const override
    {
        assert(vars.count(ident) == 0);
        vars[ident] = "\%" + to_string(val_cnt++);
        if (init_val == nullptr)
        {
            return vars[ident] + " = alloc i32\n";
        } else
        {
            auto init_val_id = make_shared<string>();
            string s = init_val->IR_string(init_val_id);
            return s + vars[ident] + " = alloc i32\n" + "store " + *init_val_id + ", " + vars[ident] + "\n";
        }
    }
};

class InitValAST : public BaseAST
{
public:
    unique_ptr<BaseAST> exp;

    string IR_string(shared_ptr<string> id) const override
    {
        return exp->IR_string(id);
    }
};