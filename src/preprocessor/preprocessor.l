%{
#include <iostream>
#include <stack>
#include <vector>
#include <string>

#define register

int lineIndex = 0;
std::stack<int> indents;
std::vector<std::string> lines;
std::vector<int> blankLineIndexes;
bool isFirstStatement = true;

void processIndent(const std::string& line);
void processStatements();

void appendToLastLine(const std::string& sequence);
void addNewLine();
size_t appendLines(const std::string& multilineStatement);

%}

%option noyywrap

%%

\{[^}]*\}                                               { lineIndex += appendLines(std::string(yytext)); }
\([^)]*\)                                               { lineIndex += appendLines(std::string(yytext)); }

^[ ]*\n                                                 { blankLineIndexes.push_back(lineIndex); lineIndex++; }
^[ ]*"//"[^\n]*\n                                       { blankLineIndexes.push_back(lineIndex); lineIndex++; }
\n                                                      { lineIndex++; }
[ ]                                                     { appendToLastLine(" "); }
^[ ]*[^ \n]+                                            {
                                                          int lastIndex = yyleng - 1;
                                                          processIndent(std::string(yytext));

                                                          while (lastIndex >= 0) {
                                                            unput(yytext[lastIndex]);
                                                            lastIndex--;
                                                          }
                                                        }

"//"[^\n]*                                              ;
[^ \n]                                                  { appendToLastLine(std::string(yytext)); }

<<EOF>>                                                 { processIndent(""); return 0; }

%%
