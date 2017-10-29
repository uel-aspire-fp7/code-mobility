#!/bin/bash
set -o errexit
set -o pipefail
set -o nounset
#set -o xtrace

# Get the repo and build directories, go to the build directory
repo_dir=$(dirname $0)
build_dir=$1
mkdir -p $build_dir
cd $build_dir

# Create extra symlinks
ln -s $repo_dir/scripts/deploy_application.sh $build_dir

# Create directory on online_backends
mkdir -p /opt/online_backends/code_mobility/

### BINDER
for PLATFORM in linux android; do
    cd ${repo_dir}/src/binder
    obj_dir=$build_dir/binder/obj/${PLATFORM}

    make -f Makefile.${PLATFORM} clean default > /dev/null

    mkdir -p ${obj_dir}
    mv ${repo_dir}/src/binder/{binder,stub_downloader}.o ${obj_dir}
done

### DOWNLOADER
for PLATFORM in linux android; do
    cd ${repo_dir}/src/downloader
    obj_dir=$build_dir/downloader/obj/${PLATFORM}

    make -f Makefile.${PLATFORM} clean all > /dev/null

    mkdir -p ${obj_dir}
    mv ${repo_dir}/src/downloader/downloader.o ${obj_dir}
done
