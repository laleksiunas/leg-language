#!/bin/sh

cd "$(dirname "$0")" || exit

compilation_dir='cmake-build-debug'

case "$2" in
  'Release')
    compilation_dir='cmake-build-release'
    ;;

  *)
    compilation_dir='cmake-build-debug'
    ;;
esac

"$compilation_dir/bundle/leg_cli" "examples/$1"
