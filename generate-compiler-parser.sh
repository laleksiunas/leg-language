#!/bin/sh
set -u

cd "$(dirname "$0")" || exit

compiler_dir='src/compiler'
generated_dir="$compiler_dir/generated"

if ! [ -d "$generated_dir" ]; then
  mkdir "$generated_dir"
fi

lex -o "$generated_dir/tokens.cpp" "$compiler_dir/tokens.l"
bison -d -o "$generated_dir/parser.cpp" "$compiler_dir/parser.y"
