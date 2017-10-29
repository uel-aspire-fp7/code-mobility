#!/bin/bash
set -o errexit
set -o pipefail
set -o nounset
#set -o xtrace

# Get the repo and build directories
repo_dir=$(dirname $0)
build_dir=$1
mkdir -p $build_dir

### CODE MOBILITY SERVER
cd ${repo_dir}/src/mobility_server
make -f Makefile.linux_x86 clean all > /dev/null
mv ${repo_dir}/src/mobility_server/mobility_server ${build_dir}
