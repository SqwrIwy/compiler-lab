#pragma once

#include <iostream>
#include <memory>
#include <cassert>

class BaseAST
{
public:
    virtual ~BaseAST() = default;

    virtual void print_AST() const = 0;

    virtual std::string IR_string() const = 0;
};

class CompUnitAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> func_def;

    void print_AST() const override
    {
        std::cout << "CompUnitAST { ";
        func_def->print_AST();
        std::cout << " }";
    }

    std::string IR_string() const override
    {
        return func_def->IR_string();
    }
};

class FuncDefAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> func_type;
    std::string ident;
    std::unique_ptr<BaseAST> block;

    void print_AST() const override
    {
        std::cout << "FuncDefAST { ";
        func_type->print_AST();
        std::cout << ", " << ident << ", ";
        block->print_AST();
        std::cout << " }";
    }

    std::string IR_string() const override
    {
        return "fun @" + ident + "(): " + func_type->IR_string() + "\n{\n" + block->IR_string() + "}\n";
    }
};

class FuncTypeAST : public BaseAST
{
public:
    std::string _int;

    void print_AST() const override
    {
        std::cout << "FuncTypeAST { ";
        std::cout << _int;
        std::cout << " }";
    }

    std::string IR_string() const override
    {
        assert(_int.compare(std::string("int")) == 0);
        return "i32";
    }
};

class BlockAST : public BaseAST
{
public:
    std::unique_ptr<BaseAST> stmt;

    void print_AST() const override
    {
        std::cout << "BlockAST { ";
        stmt->print_AST();
        std::cout << " }";
    }

    std::string IR_string() const override
    {
        return "\%entry:\n" + stmt->IR_string();
    }
};

class StmtAST : public BaseAST
{
public:
    std::string _return;
    std::unique_ptr<BaseAST> number;

    void print_AST() const override
    {
        std::cout << "StmtAST { ";
        std::cout << _return << ", ";
        number->print_AST();
        std::cout << " }";
    }

    std::string IR_string() const override
    {
        assert(_return.compare(std::string("return")) == 0);
        return "ret " + number->IR_string() + "\n";
    }
};

class NumberAST : public BaseAST
{
public:
    int int_const;

    void print_AST() const override
    {
        std::cout << "NumberAST { ";
        std::cout << int_const;
        std::cout << " }";
    }

    std::string IR_string() const override
    {
        return std::to_string(int_const);
    }
};