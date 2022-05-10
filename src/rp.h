#pragma once

#include <iostream>
#include "koopa.h"

void visit(const koopa_raw_program_t &program);
void visit(const koopa_raw_slice_t &slice);
void visit(const koopa_raw_function_t &func);
void visit(const koopa_raw_basic_block_t &bb);
void visit(const koopa_raw_value_t &value);
void visit(const koopa_raw_return_t &ret);
void visit(const koopa_raw_integer_t &integer);
void print_globl(const koopa_raw_slice_t &slice);

void visit(const koopa_raw_program_t &program)
{
    std::cout << ".text" << std::endl;
    print_globl(program.funcs);
    visit(program.values);
    visit(program.funcs);
}

void visit(const koopa_raw_slice_t &slice)
{
    for (size_t i = 0; i < slice.len; ++i)
    {
        auto ptr = slice.buffer[i];
        switch (slice.kind)
        {
        case KOOPA_RSIK_FUNCTION:
            visit(reinterpret_cast<koopa_raw_function_t>(ptr));
            break;
        case KOOPA_RSIK_BASIC_BLOCK:
            visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
            break;
        case KOOPA_RSIK_VALUE:
            visit(reinterpret_cast<koopa_raw_value_t>(ptr));
            break;
        default:
            assert(false);
        }
    }
}

void visit(const koopa_raw_function_t &func)
{
    std::cout << func->name+1 << ":" << std::endl;
    visit(func->bbs);
}

void visit(const koopa_raw_basic_block_t &bb)
{
    visit(bb->insts);
}

void visit(const koopa_raw_value_t &value)
{
    const auto &kind = value->kind;
    switch (kind.tag)
    {
    case KOOPA_RVT_RETURN:
        visit(kind.data.ret);
        break;
    case KOOPA_RVT_INTEGER:
        visit(kind.data.integer);
        break;
    default:
        assert(false);
    }
}

void visit(const koopa_raw_return_t &ret)
{
    auto ret_value = ret.value;
    assert(ret_value->kind.tag == KOOPA_RVT_INTEGER);
    int32_t int_val = ret_value->kind.data.integer.value;
    std::cout << "li a0, " << int_val << std::endl;
    std::cout << "ret" << std::endl;
}

void visit(const koopa_raw_integer_t &integer)
{
}

void print_globl(const koopa_raw_slice_t &slice)
{
    for (size_t i = 0; i < slice.len; ++i)
    {
        auto ptr = slice.buffer[i];
        assert(slice.kind == KOOPA_RSIK_FUNCTION);
        auto func = reinterpret_cast<koopa_raw_function_t>(ptr);
        std::cout << ".globl " << func->name+1 << std::endl;
    }
}