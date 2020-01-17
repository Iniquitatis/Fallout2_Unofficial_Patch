#!/bin/bash

set -xeu -o pipefail

release_dir="$(realpath $release_dir)"
extra_dir="$(realpath $extra_dir)"
release_ini="$release_dir/ddraw.ini"
custom_ini="$extra_dir/publish/ddraw.ini"

sfall_url="https://sourceforge.net/projects/sfall/files/sfall/sfall_$sfall_version.7z/download"
wget -q "$sfall_url" -O sfall.7z
for f in ddraw.dll ddraw.ini sfall.dat; do
  7zr e sfall.7z "$f"
  mv "$f" "$release_dir/"
done

crudini --merge "$release_ini" < "$custom_ini"
crudini --set "$release_ini" "Misc" "VersionString" "FALLOUT II 1.02.31${uversion}"
unix2dos "$release_ini"
