#!/bin/sh

create_compilation_dir() {
  if [ "$1" = 'Debug' ]; then
    compile_dir='cmake-build-debug'
  else
    compile_dir='cmake-build-release'
  fi

  mkdir -p "$compile_dir"

  cd "$compile_dir" || exit
}

compile_target() {
  compiler_dir="$1"

  mkdir -p "$compiler_dir"

  cd "$compiler_dir" || exit

  cmake -DCMAKE_BUILD_TYPE="$2" ../..
  make "leg_$1"

  cd ..
}

bundle() {
  bundle_dir='bundle'

  mkdir -p "$bundle_dir"

  cd "$bundle_dir" || exit

  for target in "$@"; do
    cp "../$target/bin/leg_$target" "leg_$target"
  done
}

case "$1" in
  'Release')
    env_type='Release'
    ;;

  *)
    env_type='Debug'
    ;;
esac

cd "$(dirname "$0")" || exit

sh 'generate-parsers.sh'

create_compilation_dir "$env_type"
compile_target compiler "$env_type"
compile_target preprocessor "$env_type"
compile_target cli "$env_type"
bundle compiler preprocessor cli
