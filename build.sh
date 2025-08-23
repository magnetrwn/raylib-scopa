#!/usr/bin/bash
set -e

if ! command -v gn &> /dev/null || ! command -v ninja &> /dev/null; then
  echo "GN and Ninja must be installed to build this project."
  exit 1
fi

BUILD_TYPE=${1:-Debug}
if [[ "$BUILD_TYPE" != "Debug" && "$BUILD_TYPE" != "Release" ]]; then
  echo "Usage: $0 <Debug [default] | Release>"
  exit 1
fi

gn gen out/"$BUILD_TYPE" --args="is_debug=$([ "$BUILD_TYPE" == "Debug" ] && echo true || echo false)"
ninja -C out/"$BUILD_TYPE" game
mkdir -p out/"$BUILD_TYPE"/obj/assets
rsync -a --delete assets/ out/"$BUILD_TYPE"/obj/assets/
out/"$BUILD_TYPE"/obj/game || { echo "Build failed."; exit 1; }