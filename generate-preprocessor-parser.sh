#!/bin/sh
set -u

cd "$(dirname "$0")" || exit

preprocessor_dir='src/preprocessor'
generated_dir="$preprocessor_dir/generated"

if ! [ -d "$generated_dir" ]; then
  mkdir "$generated_dir"
fi

lex -o "$generated_dir/preprocessor.cpp" "$preprocessor_dir/preprocessor.l"
