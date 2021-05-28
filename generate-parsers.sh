#!/bin/sh

cd "$(dirname "$0")" || exit

sh 'generate-compiler-parser.sh'
sh 'generate-preprocessor-parser.sh'
