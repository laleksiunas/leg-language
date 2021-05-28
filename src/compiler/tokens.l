%{
  #include <string>
  #include "ast/Node.h"
  #include "ast/ExpressionNode.h"
  #include "ast/StatementNode.h"
  #include "ast/expressions/BlockNode.h"
  #include "ast/expressions/IntegerNode.h"
  #include "ast/expressions/FloatNode.h"
  #include "ast/expressions/StringNode.h"
  #include "ast/expressions/FunctionCallNode.h"
  #include "ast/expressions/VariableNode.h"
  #include "ast/expressions/KickNode.h"
  #include "ast/expressions/ArithmeticNode.h"
  #include "ast/expressions/BooleanNode.h"
  #include "ast/expressions/CreateArrayNode.h"
  #include "ast/expressions/GetterIndexerNode.h"
  #include "ast/expressions/GetterPropertyNode.h"
  #include "ast/expressions/InitializeArrayNode.h"
  #include "ast/statements/ExpressionStatementNode.h"
  #include "ast/statements/FunctionDeclarationNode.h"
  #include "ast/statements/FunctionReturnStatementNode.h"
  #include "ast/statements/AssignmentNode.h"
  #include "ast/statements/DeclarationNode.h"
  #include "ast/statements/CatchNode.h"
  #include "ast/statements/SetterIndexerNode.h"
  #include "ast/statements/CompoundAssignmentNode.h"
  #include "ast/statements/IfNode.h"
  #include "ast/statements/ForNode.h"
  #include "ast/statements/WhileNode.h"
  #include "ast/misc/IdentifierNode.h"
  #include "ast/misc/FunctionArgumentNode.h"
  #include "ast/misc/TypeNode.h"
  #include "ast/misc/PrimitiveTypeNode.h"
  #include "ast/misc/ArrayTypeNode.h"
  #include "parser.hpp"

  #define register
  #define YYERROR_VERBOSE 1
  #define SAVE_TOKEN yylval.string = new std::string(yytext, yyleng)
  #define TOKEN(t) (yylval.token = t)
%}

%option noyywrap
%option yylineno

%%

[ \t\r\n]                     ;
"//".*                        ;

"{"                           return TOKEN(LBRACE);
"}"                           return TOKEN(RBRACE);
"("                           return TOKEN(LPAREN);
")"                           return TOKEN(RPAREN);
"["                           return TOKEN(LBRACKET);
"]"                           return TOKEN(RBRACKET);
"[]"                          return TOKEN(BRACKETS);
"#"                           return TOKEN(BLOCK);
";"                           return TOKEN(SEMICOLON);

"+"                           return TOKEN(PLUS);
"-"                           return TOKEN(MINUS);
"*"                           return TOKEN(MULTIPLY);
"/"                           return TOKEN(DIVIDE);
"%"                           return TOKEN(MODULUS);
"!"                           return TOKEN(BANG);

"?"                           return TOKEN(QUESTION);
":"                           return TOKEN(COLON);
"."                           return TOKEN(DOT);
","                           return TOKEN(COMMA);
".."                          return TOKEN(RANGE);

"=="                          return TOKEN(EQUAL);
"!="                          return TOKEN(NOT_EQUAL);
">"                           return TOKEN(GREATER);
">="                          return TOKEN(GREATER_EQUAL);
"<"                           return TOKEN(LESS_THAN);
"<="                          return TOKEN(LESS_THAN_EQUAL);

"&&"                          return TOKEN(AND);
"||"                          return TOKEN(OR);

"="                           return TOKEN(ASSIGNMENT);
"+="                          return TOKEN(PLUS_ASSIGNMENT);
"-="                          return TOKEN(MINUS_ASSIGNMENT);
"*="                          return TOKEN(MULTIPLY_ASSIGNMENT);
"/="                          return TOKEN(DIVIDE_ASSIGNMENT);
"%="                          return TOKEN(MODULUS_ASSIGNMENT);
"&&="                         return TOKEN(AND_ASSIGNMENT);
"||="                         return TOKEN(OR_ASSIGNMENT);

"fn"                          return TOKEN(FUNCTION);
"if"                          return TOKEN(IF);
"else"                        return TOKEN(ELSE);
"for"                         return TOKEN(FOR);
"while"                       return TOKEN(WHILE);
"return"                      return TOKEN(RETURN);
"kick"                        return TOKEN(KICK);
"catch"                       return TOKEN(CATCH);
"true"                        return TOKEN(TRUE);
"false"                       return TOKEN(FALSE);
"new"                         return TOKEN(NEW);
"in"                          return TOKEN(IN);

[a-zA-Z_][a-zA-Z0-9_]*        SAVE_TOKEN; return IDENTIFIER;
\"([^\\\"]|\\.)*\"            SAVE_TOKEN; return STRING;
[0-9]+                        SAVE_TOKEN; return INTEGER;
[0-9]+\.[0-9]+			          SAVE_TOKEN; return FLOAT;

.                             return TOKEN(UNKNOWN_TOKEN);

%%
