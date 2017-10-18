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

### CODE MOBILITY SERVER
cd ${repo_dir}/src/mobility_server

make -f Makefile.linux_x86 clean all > /dev/null

mkdir -p ${build_dir}/mobility_server
mv ${repo_dir}/src/mobility_server/mobility_server ${build_dir}/mobility_server
mkdir -p /opt/online_backends/code_mobility/
cp ${build_dir}/mobility_server/mobility_server /opt/online_backends/code_mobility/
