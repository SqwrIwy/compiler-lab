#include <cassert>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include "ast.h"
#include "koopa.h"
#include "rp.h"

using namespace std;

extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

int main(int argc, const char *argv[])
{
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  yyin = fopen(input, "r");
  assert(yyin);

  unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  assert(!ret);

  freopen(output, "w", stdout);

  if (string(mode).compare(string("-koopa")) == 0)
  {
    cout << ast->IR_string();
  }

  if (string(mode).compare(string("-riscv")) == 0)
  {
    koopa_program_t program;
    koopa_error_code_t ret = koopa_parse_from_string(ast->IR_string().c_str(), &program);
    assert(ret == KOOPA_EC_SUCCESS);
    koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
    koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
    koopa_delete_program(program);

    visit(raw);

    koopa_delete_raw_program_builder(builder);
  }

  return 0;
}
