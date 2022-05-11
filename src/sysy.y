%code requires {
  #include <memory>
  #include <string>
  #include "ast.h"
}

%{

#include <iostream>
#include <memory>
#include <string>
#include "ast.h"

int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);

using namespace std;

%}

%parse-param { std::unique_ptr<BaseAST> &ast }

%union {
  std::string *str_val;
  int int_val;
  BaseAST *ast_val;
}

%token INT RETURN LEQ GEQ EQ NEQ AND OR
%token <str_val> IDENT
%token <int_val> INT_CONST

%type <ast_val> FuncDef FuncType Block Stmt Exp PrimaryExp Number UnaryExp UnaryOp MulExp AddExp RelExp EqExp LAndExp LOrExp

%%

CompUnit
  : FuncDef {
    auto comp_unit = make_unique<CompUnitAST>();
    comp_unit->func_def = unique_ptr<BaseAST>($1);
    ast = move(comp_unit);
  }
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *$2;
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  ;

FuncType
  : INT {
    auto ast = new FuncTypeAST();
    ast->_int = string("int");
    $$ = ast;
  }
  ;

Block
  : '{' Stmt '}' {
    auto ast = new BlockAST();
    ast->stmt = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

Stmt
  : RETURN Exp ';' {
    auto ast = new StmtAST();
    ast->_return = string("return");
    ast->exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

Exp
  : LOrExp {
    auto ast = new ExpAST();
    ast->lor_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

PrimaryExp
  : '(' Exp ')' {
    auto ast = new PrimaryExpAST_0();
    ast->exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | Number {
    auto ast = new PrimaryExpAST_1();
    ast->number = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

Number
  : INT_CONST {
    auto ast = new NumberAST();
    ast->int_const = $1;
    $$ = ast;
  }
  ;

UnaryExp
  : PrimaryExp {
    auto ast = new UnaryExpAST_0();
    ast->primary_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | UnaryOp UnaryExp {
    auto ast = new UnaryExpAST_1();
    ast->unary_op = unique_ptr<BaseAST>($1);
    ast->unary_exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

UnaryOp
  : '+' {
    auto ast = new UnaryOpAST();
    ast->op = "+";
    $$ = ast;
  }
  | '-' {
    auto ast = new UnaryOpAST();
    ast->op = "-";
    $$ = ast;
  }
  | '!' {
    auto ast = new UnaryOpAST();
    ast->op = "!";
    $$ = ast;
  }
  ;

MulExp
  : UnaryExp {
    auto ast = new MulExpAST_0();
    ast->unary_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | MulExp '*' UnaryExp {
    auto ast = new MulExpAST_1();
    ast->mul_exp = unique_ptr<BaseAST>($1);
    ast->op = '*';
    ast->unary_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | MulExp '/' UnaryExp {
    auto ast = new MulExpAST_1();
    ast->mul_exp = unique_ptr<BaseAST>($1);
    ast->op = '/';
    ast->unary_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | MulExp '%' UnaryExp {
    auto ast = new MulExpAST_1();
    ast->mul_exp = unique_ptr<BaseAST>($1);
    ast->op = '%';
    ast->unary_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

AddExp
  : MulExp {
    auto ast = new AddExpAST_0();
    ast->mul_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | AddExp '+' MulExp {
    auto ast = new AddExpAST_1();
    ast->add_exp = unique_ptr<BaseAST>($1);
    ast->op = '+';
    ast->mul_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | AddExp '-' MulExp {
    auto ast = new AddExpAST_1();
    ast->add_exp = unique_ptr<BaseAST>($1);
    ast->op = '-';
    ast->mul_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

RelExp
  : AddExp {
    auto ast = new RelExpAST_0();
    ast->add_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | RelExp '<' AddExp {
    auto ast = new RelExpAST_1();
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->op = 0;
    ast->add_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | RelExp '>' AddExp {
    auto ast = new RelExpAST_1();
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->op = 1;
    ast->add_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | RelExp LEQ AddExp {
    auto ast = new RelExpAST_1();
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->op = 2;
    ast->add_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | RelExp GEQ AddExp {
    auto ast = new RelExpAST_1();
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->op = 3;
    ast->add_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

EqExp
  : RelExp {
    auto ast = new EqExpAST_0();
    ast->rel_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | EqExp EQ RelExp {
    auto ast = new EqExpAST_1();
    ast->eq_exp = unique_ptr<BaseAST>($1);
    ast->op = 0;
    ast->rel_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | EqExp NEQ RelExp {
    auto ast = new EqExpAST_1();
    ast->eq_exp = unique_ptr<BaseAST>($1);
    ast->op = 1;
    ast->rel_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

LAndExp
  : EqExp {
    auto ast = new LAndExpAST_0();
    ast->eq_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | LAndExp AND EqExp {
    auto ast = new LAndExpAST_1();
    ast->land_exp = unique_ptr<BaseAST>($1);
    ast->eq_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

LOrExp
  : LAndExp {
    auto ast = new LOrExpAST_0();
    ast->land_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | LOrExp OR LAndExp {
    auto ast = new LOrExpAST_1();
    ast->lor_exp = unique_ptr<BaseAST>($1);
    ast->land_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

%%

void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}
