#!/bin/sh

cd "$(dirname "$0")" || exit

if ! [ -d '.conan' ]; then
  mkdir '.conan'
fi

cd '.conan' || exit

conan install ..
