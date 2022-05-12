#pragma once

#include <iostream>
#include <cassert>
#include <tr1/unordered_map>
#include "koopa.h"

using namespace std;

static tr1::unordered_map<uintptr_t, int> off;

void visit(const koopa_raw_program_t &program);
void visit(const koopa_raw_slice_t &slice);
void visit(const koopa_raw_function_t &func);
void visit(const koopa_raw_basic_block_t &bb);
void visit(const koopa_raw_value_t &value);
void visit(const koopa_raw_return_t &ret);
void visit(const koopa_raw_integer_t &integer);
void print_globl(const koopa_raw_slice_t &slice);
int calc_stack_frame_size(const koopa_raw_slice_t &slice);
bool has_return_value(const koopa_raw_value_t &value);
string offset(const koopa_raw_value_t &value);

void visit(const koopa_raw_program_t &program)
{
    cout << ".text" << endl;
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
    cout << func->name+1 << ":" << endl;
    visit(func->bbs);
}

void visit(const koopa_raw_basic_block_t &bb)
{
    int stack_frame_size = calc_stack_frame_size(bb->insts);
    cout << "addi sp, sp, -" + to_string(stack_frame_size * 4) << endl;
    auto slice = bb->insts;
    for (size_t i = 0; i < slice.len; ++i)
    {
        auto ptr = slice.buffer[i];
        assert(slice.kind == KOOPA_RSIK_VALUE);
        auto value = reinterpret_cast<koopa_raw_value_t>(ptr);
        switch (value->kind.tag)
        {
        case KOOPA_RVT_BINARY:
        {
            auto lhs = value->kind.data.binary.lhs;
            auto rhs = value->kind.data.binary.rhs;

            switch (lhs->kind.tag)
            {
            case KOOPA_RVT_INTEGER:
                cout << "li t0, " + to_string(lhs->kind.data.integer.value) << endl;
                break;

            case KOOPA_RVT_BINARY:
                cout << "lw t0, " + offset(lhs) << endl;
                break;

            case KOOPA_RVT_LOAD:
                cout << "lw t0, " + offset(lhs) << endl;
                break;

            default:
                assert(false);
            }

            switch (rhs->kind.tag)
            {
            case KOOPA_RVT_INTEGER:
                cout << "li t1, " + to_string(rhs->kind.data.integer.value) << endl;
                break;

            case KOOPA_RVT_BINARY:
                cout << "lw t1, " + offset(rhs) << endl;
                break;

            case KOOPA_RVT_LOAD:
                cout << "lw t1, " + offset(rhs) << endl;
                break;

            default:
                assert(false);
            }

            switch (value->kind.data.binary.op)
            {
            case 0: // ne
            {
                cout << "xor t2, t0, t1" << endl;
                cout << "snez t3, t2" << endl;
                break;
            }

            case 1: // eq
            {
                cout << "xor t2, t0, t1" << endl;
                cout << "seqz t3, t2" << endl;
                break;
            }

            case 2: // gt
            {
                cout << "sgt t3, t0, t1" << endl;
                break;
            }

            case 3: // lt
            {
                cout << "slt t3, t0, t1" << endl;
                break;
            }

            case 4: // ge
            {
                cout << "addi t0, t0, 1" << endl;
                cout << "sgt t3, t0, t1" << endl;
                break;
            }

            case 5: // le
            {
                cout << "addi t1, t1, 1" << endl;
                cout << "slt t3, t0, t1" << endl;
                break;
            }

            case 6: // add
            {
                cout << "add t3, t0, t1" << endl;
                break;
            }

            case 7: // sub
            {
                cout << "sub t3, t0, t1" << endl;
                break;
            }

            case 8: // mul
            {
                cout << "mul t3, t0, t1" << endl;
                break;
            }

            case 9: // div
            {
                cout << "div t3, t0, t1" << endl;
                break;
            }

            case 10: // mod
            {
                cout << "rem t3, t0, t1" << endl;
                break;
            }

            case 11: // and
            {
                cout << "and t3, t0, t1" << endl;
                break;
            }

            case 12: // or
            {
                cout << "or t3, t0, t1" << endl;
                break;
            }

            default:
            {
                break;
            }
            }
            cout << "sw t3, " + offset(value) << endl;
            break;
        }

        case KOOPA_RVT_RETURN:
        {
            auto ret = value->kind.data.ret.value;

            switch (ret->kind.tag)
            {
            case KOOPA_RVT_INTEGER:
                cout << "li a0, " + to_string(ret->kind.data.integer.value) << endl;
                break;

            case KOOPA_RVT_BINARY:
                cout << "lw a0, " + offset(ret) << endl;
                break;

            case KOOPA_RVT_LOAD:
                cout << "lw a0, " + offset(ret) << endl;
                break;

            default:
                assert(false);
            }

            cout << "addi sp, sp, " + to_string(stack_frame_size * 4) << endl;
            cout << "ret" << endl;
            break;
        }

        case KOOPA_RVT_ALLOC:
        {
            break;
        }

        case KOOPA_RVT_LOAD:
        {
            auto src = value->kind.data.load.src;
            cout << "lw t0, " + offset(src) + "\n";
            cout << "sw t0, " + offset(value) + "\n";
            break;
        }

        case KOOPA_RVT_STORE:
        {
            auto src = value->kind.data.store.value;
            auto dest = value->kind.data.store.dest;

            if (src->kind.tag == KOOPA_RVT_INTEGER)
                cout << "li t0, " + to_string(src->kind.data.integer.value) + "\n";
            else
                cout << "lw t0, " + offset(src) + "\n";

            cout << "sw t0, " + offset(dest) + "\n";
            break;
        }

        default:
        {
            assert(false);
        }
        }
    }
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
    cout << "li a0, " << int_val << endl;
    cout << "ret" << endl;
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
        cout << ".globl " << func->name+1 << endl;
    }
}

int calc_stack_frame_size(const koopa_raw_slice_t &slice)
{
    int stack_frame_size = 0;
    for (size_t i = 0; i < slice.len; ++i)
    {
        auto ptr = slice.buffer[i];
        assert(slice.kind == KOOPA_RSIK_VALUE);
        auto value = reinterpret_cast<koopa_raw_value_t>(ptr);
        if (value->kind.tag == KOOPA_RVT_ALLOC || has_return_value(value))
        {
            off[reinterpret_cast<uintptr_t>(value)] = stack_frame_size;
            stack_frame_size++;
        }
    }
    if ((stack_frame_size & 3) > 0)
    {
        stack_frame_size = ((stack_frame_size >> 2) + 1) << 2;
    }
    return stack_frame_size;
}

bool has_return_value(const koopa_raw_value_t &value)
{
    return value->ty->tag != KOOPA_RTT_UNIT;
}

string offset(const koopa_raw_value_t &value)
{
    return to_string(off[reinterpret_cast<uintptr_t>(value)] * 4) + "(sp)";
}