%{
  #include <string>
  #include "codegen/operators.h"

  ast::BlockNode* rootBlock;
  bool hasSyntaxError = false;

  extern int yylex();
  extern void handleError(std::string& message, int lineNumber);

  void yyerror(const char* error) {
    std::string errorPrefix = "syntax error, ";
    auto parsedError = std::string(error).substr(errorPrefix.size());

    hasSyntaxError = true;
    handleError(parsedError, yylineno);
  }
%}

%union {
	ast::Node* node;
	ast::BlockNode* block;
	ast::ExpressionNode* expression;
	ast::StatementNode* statement;
	ast::IdentifierNode* identifier;
	ast::KickNode* kickExpression;
	ast::FunctionArgumentNode* functionDeclarationArgument;
	ast::TypeNode* type;
	std::vector<ast::ExpressionNode*>* functionCallArguments;
	std::vector<ast::FunctionArgumentNode*>* functionDeclarationArguments;
	std::vector<ast::KickNode*>* catchCallArguments;
	std::vector<ast::IdentifierNode*>* catchDestructuringIdentifiers;
	std::string* string;
	int token;
}

%token <string> IDENTIFIER FLOAT INTEGER STRING
%token <token> LBRACE RBRACE LPAREN RPAREN LBRACKET RBRACKET BRACKETS BLOCK SEMICOLON
%token <token> PLUS MINUS MULTIPLY DIVIDE MODULUS
%token <token> QUESTION COLON DOT COMMA RANGE
%token <token> EQUAL NOT_EQUAL GREATER GREATER_EQUAL LESS_THAN LESS_THAN_EQUAL
%token <token> AND OR TRUE FALSE BANG
%token <token> FUNCTION RETURN IF ELSE FOR WHILE KICK CATCH NEW IN
%token <token> ASSIGNMENT
  PLUS_ASSIGNMENT
  MINUS_ASSIGNMENT
  MULTIPLY_ASSIGNMENT
  DIVIDE_ASSIGNMENT
  MODULUS_ASSIGNMENT
  AND_ASSIGNMENT
  OR_ASSIGNMENT
%token <token> UNKNOWN_TOKEN

%type <block> program root_statements statements block
%type <statement> statement
  single_statement
  complex_statement
  expression_statement
  function_declaration_statement
  function_return_statement
  declaration_statement
  assignment_statement
  catch_statement
  indexer_assignment_statement
  compound_assignment_statement
  if_statement
  if_then_statement
  if_else_statement
  if_else_if_statement
  for_statement
  for_statement_default_increment
  for_statement_custom_increment
  while_statement
%type <expression> expression
  constant_expression
  float_expression
  integer_expression
  string_expression
  boolean_expression
  function_call_expression
  variable_expression
  arithmetic_expression
  parenthesis_expression
  create_array_expression
  indexer_expression
  property_expression
  initialize_array_expression
%type <functionCallArguments> function_call_arguments
%type <functionDeclarationArguments> function_declaration_arguments
%type <functionDeclarationArgument> function_declaration_argument
%type <catchCallArguments> catch_call_arguments
%type <catchDestructuringIdentifiers> catch_destructuring_identifiers
  catch_destructuring_identifier
  catch_destructuring_assignment
  catch_destructuring
%type <identifier> identifier
%type <kickExpression> kick_expression
%type <type> type_identifier primitive_type_identifier array_type_identifier

%left OR AND
%left EQUAL NOT_EQUAL GREATER GREATER_EQUAL LESS_THAN LESS_THAN_EQUAL
%left PLUS MINUS MULTIPLY MODULUS DIVIDE
%left DOT LBRACKET RBRACKET LPAREN RPAREN NEGATIVE

%start program

%%

program : root_statements { rootBlock = $1 };
root_statements : function_declaration_statement { $$ = new ast::BlockNode(); $$->statements.push_back($<statement>1) }
  | root_statements function_declaration_statement { $1->statements.push_back($<statement>2) };

block : BLOCK statements BLOCK { $$ = $2 };

statements : statement { $$ = new ast::BlockNode(); $$->statements.push_back($<statement>1) }
  | statements statement { $1->statements.push_back($<statement>2) };

statement : single_statement | complex_statement | error BLOCK { yyerrok };

single_statement : expression_statement SEMICOLON
  | function_return_statement SEMICOLON
  | assignment_statement SEMICOLON
  | declaration_statement SEMICOLON
  | catch_statement SEMICOLON
  | indexer_assignment_statement SEMICOLON
  | compound_assignment_statement SEMICOLON
  | IDENTIFIER error { YYABORT };

complex_statement : function_declaration_statement
  | if_statement
  | for_statement
  | while_statement;

assignment_statement : identifier ASSIGNMENT expression { $$ = new ast::AssignmentNode($1, $3) };
indexer_assignment_statement : indexer_expression ASSIGNMENT expression
  { $$ = new ast::SetterIndexerNode((ast::GetterIndexerNode*) $1, $3) };
compound_assignment_statement :
  identifier PLUS_ASSIGNMENT expression { $$ = new ast::CompoundAssignmentNode($1, $3, codegen::operators::Plus) }
  | identifier MINUS_ASSIGNMENT expression { $$ = new ast::CompoundAssignmentNode($1, $3, codegen::operators::Minus) }
  | identifier MULTIPLY_ASSIGNMENT expression { $$ = new ast::CompoundAssignmentNode($1, $3, codegen::operators::Multiply) }
  | identifier DIVIDE_ASSIGNMENT expression { $$ = new ast::CompoundAssignmentNode($1, $3, codegen::operators::Divide) }
  | identifier MODULUS_ASSIGNMENT expression { $$ = new ast::CompoundAssignmentNode($1, $3, codegen::operators::Modulus) }
  | identifier AND_ASSIGNMENT expression { $$ = new ast::CompoundAssignmentNode($1, $3, codegen::operators::And) }
  | identifier OR_ASSIGNMENT expression { $$ = new ast::CompoundAssignmentNode($1, $3, codegen::operators::Or) };

declaration_statement : type_identifier identifier ASSIGNMENT expression { $$ = new ast::DeclarationNode($1, $2, $4) };

expression_statement : expression { $$ = new ast::ExpressionStatementNode($1) }
  | kick_expression { $$ = new ast::ExpressionStatementNode($1) };

function_declaration_statement :
  FUNCTION identifier LPAREN function_declaration_arguments RPAREN COLON type_identifier block
    { $$ = new ast::FunctionDeclarationNode($2, $4, $7, $8) }
    | FUNCTION error { YYABORT }
    | FUNCTION identifier error { YYABORT };
function_return_statement : RETURN expression { $$ = new ast::FunctionReturnStatementNode($2) }
  | RETURN { $$ = new ast::FunctionReturnStatementNode() };
function_declaration_arguments : /*blank*/  { $$ = new std::vector<ast::FunctionArgumentNode*>() }
  | function_declaration_argument { $$ = new std::vector<ast::FunctionArgumentNode*>(); $$->push_back($1) }
  | function_declaration_arguments COMMA function_declaration_argument { $1->push_back($3) };
function_declaration_argument : identifier COLON type_identifier { $$ = new ast::FunctionArgumentNode($3, $1) };

catch_statement :
  catch_destructuring CATCH LBRACE catch_call_arguments RBRACE { $$ = new ast::CatchNode($1, $4) };
catch_call_arguments : kick_expression { $$ = new std::vector<ast::KickNode*>(); $$->push_back((ast::KickNode*) $1) }
  | catch_call_arguments COMMA kick_expression { $1->push_back((ast::KickNode*) $3) };
catch_destructuring : /*blank*/  { $$ = new std::vector<ast::IdentifierNode*>() }
  | catch_destructuring_assignment;
catch_destructuring_assignment : catch_destructuring_identifier ASSIGNMENT | catch_destructuring_identifiers ASSIGNMENT;
catch_destructuring_identifier : identifier COMMA { $$ = new std::vector<ast::IdentifierNode*>(); $$->push_back($1) };
catch_destructuring_identifiers :
  identifier COMMA identifier { $$ = new std::vector<ast::IdentifierNode*>(); $$->push_back($1); $$->push_back($3) }
    | catch_destructuring_identifiers COMMA identifier { $1->push_back($3) };

if_statement : if_then_statement
  | if_else_statement
  | if_else_if_statement
  | IF error { YYABORT };
if_then_statement : IF LPAREN expression RPAREN block { $$ = new ast::IfNode($3, $5) };
if_else_statement : IF LPAREN expression RPAREN block ELSE block { $$ = new ast::IfNode($3, $5, $7) };
if_else_if_statement :
  IF LPAREN expression RPAREN block ELSE if_statement { $$ = new ast::IfNode($3, $5, (ast::IfNode*) $7) };

for_statement : for_statement_default_increment
  | for_statement_custom_increment
  | FOR error { YYABORT };
for_statement_default_increment : FOR LPAREN identifier IN expression RANGE expression RPAREN block
  { $$ = new ast::ForNode($3, $5, $7, $9) };
for_statement_custom_increment : FOR LPAREN identifier IN expression RANGE expression COLON expression RPAREN block
  { $$ = new ast::ForNode($3, $5, $7, $9, $11) };

while_statement : WHILE LPAREN expression RPAREN block { $$ = new ast::WhileNode($3, $5) }
  | WHILE error { YYABORT };

expression : constant_expression
  | function_call_expression
  | variable_expression
  | arithmetic_expression
  | parenthesis_expression
  | create_array_expression
  | indexer_expression
  | property_expression
  | initialize_array_expression;

function_call_expression :
  identifier LPAREN function_call_arguments RPAREN { $$ = new ast::FunctionCallNode($1, $3) };
function_call_arguments : /*blank*/  { $$ = new std::vector<ast::ExpressionNode*>() }
  | expression { $$ = new std::vector<ast::ExpressionNode*>(); $$->push_back($1) }
  | function_call_arguments COMMA expression { $1->push_back($3) };

kick_expression : KICK function_call_expression { $$ = new ast::KickNode((ast::FunctionCallNode*) $2) };

create_array_expression :
  NEW type_identifier LBRACKET integer_expression RBRACKET { $$ = new ast::CreateArrayNode($2, (ast::IntegerNode*) $4) };
initialize_array_expression :
  NEW array_type_identifier LBRACE function_call_arguments RBRACE { $$ = new ast::InitializeArrayNode($2, $4) };

indexer_expression :
  expression LBRACKET expression RBRACKET { $$ = new ast::GetterIndexerNode($1, $3) };

property_expression : expression DOT identifier { $$ = new ast::GetterPropertyNode($1, $3) };

arithmetic_expression : expression PLUS expression { $$ = new ast::ArithmeticNode($1, $3, codegen::operators::Plus) }
  | expression MINUS expression { $$ = new ast::ArithmeticNode($1, $3, codegen::operators::Minus) }
  | expression MULTIPLY expression { $$ = new ast::ArithmeticNode($1, $3, codegen::operators::Multiply) }
  | expression DIVIDE expression { $$ = new ast::ArithmeticNode($1, $3, codegen::operators::Divide) }
  | expression MODULUS expression { $$ = new ast::ArithmeticNode($1, $3, codegen::operators::Modulus) }
  | expression AND expression { $$ = new ast::ArithmeticNode($1, $3, codegen::operators::And) }
  | expression OR expression { $$ = new ast::ArithmeticNode($1, $3, codegen::operators::Or) }
  | expression EQUAL expression { $$ = new ast::ArithmeticNode($1, $3, codegen::operators::Equal) }
  | expression NOT_EQUAL expression { $$ = new ast::ArithmeticNode($1, $3, codegen::operators::NotEqual) }
  | expression GREATER expression { $$ = new ast::ArithmeticNode($1, $3, codegen::operators::Greater) }
  | expression GREATER_EQUAL expression { $$ = new ast::ArithmeticNode($1, $3, codegen::operators::GreaterEqual) }
  | expression LESS_THAN expression { $$ = new ast::ArithmeticNode($1, $3, codegen::operators::LessThan) }
  | expression LESS_THAN_EQUAL expression { $$ = new ast::ArithmeticNode($1, $3, codegen::operators::LessThanEqual) };

parenthesis_expression : LPAREN expression RPAREN { $$ = $2 };

variable_expression : identifier { $$ = new ast::VariableNode($1) };

constant_expression : float_expression | integer_expression | string_expression | boolean_expression;
float_expression : FLOAT { $$ = new ast::FloatNode(std::stof(*$1)); delete $1 }
  | MINUS FLOAT %prec NEGATIVE { $$ = new ast::FloatNode(std::stof(*$2) * -1); delete $2 };
integer_expression : INTEGER { $$ = new ast::IntegerNode(std::stoi(*$1)); delete $1 }
  | MINUS INTEGER %prec NEGATIVE { $$ = new ast::IntegerNode(std::stoi(*$2) * -1); delete $2 };
string_expression : STRING { $$ = new ast::StringNode($1->substr(1, $1->length() - 2)); delete $1 };
boolean_expression : TRUE { $$ = new ast::BooleanNode(true) } | FALSE { $$ = new ast::BooleanNode(false) };

identifier : IDENTIFIER { $$ = new ast::IdentifierNode(*$1); delete $1 };
type_identifier : primitive_type_identifier | array_type_identifier;
primitive_type_identifier : identifier { $$ = new ast::PrimitiveTypeNode($1) };
array_type_identifier : type_identifier BRACKETS { $$ = new ast::ArrayTypeNode($1) };

%%
