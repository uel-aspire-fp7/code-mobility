#!/bin/bash

set -o errexit
set -o pipefail

CDEFAULT='\e[39m'
CRED='\e[31m'
CYELLOW='\e[33m'
CGREEN='\e[32m'

BASE_DIRECTORY=$(dirname $0)

cd ${BASE_DIRECTORY}

echo -e "${CGREEN}Code Mobility Components build process STARTED...${CDEFAULT}"

echo -en "${CYELLOW}0) Cleaning previous build...${CDEFAULT}"

rm -f prebuilt/bin/x86/* prebuilt/obj/{android,linux}/*

echo -e " done\n"

### BINDER

echo -e "${CYELLOW}1) Binder${CDEFAULT}"

for PLATFORM in linux android; do
    cd src/binder

	echo -en "\tCompiling '${PLATFORM}' platform..."

    make -f Makefile.${PLATFORM} clean default > /dev/null

	echo -e " done"
    echo -en "\tCopying object files to prebuilt directory..."

    cd ../..
    mv src/binder/{binder,stub_downloader}.o prebuilt/obj/${PLATFORM}
    echo -e " done\n"
done

### DOWNLOADER

echo -e "${CYELLOW}1) Downloader${CDEFAULT}"

for PLATFORM in linux android; do
    cd src/downloader

	echo -en "\tCompiling '${PLATFORM}' platform..."

    make -f Makefile.${PLATFORM} clean all > /dev/null

	echo -e " done"
    echo -en "\tCopying object files to prebuilt directory..."

    cd ../..
    mv src/downloader/downloader.o prebuilt/obj/${PLATFORM}
    echo -e " done\n"
done

### CODE MOBILITY SERVER
echo -e "${CYELLOW}3) Code Mobility Server${CDEFAULT}"

cd src/mobility_server

echo -en "\tCompiling..."

make -f Makefile.linux_x86 clean all > /dev/null

echo -e " done"
echo -en "\tCopying application prebuilt directory..."

cd ../..
mv src/mobility_server/mobility_server prebuilt/bin/x86/
cp prebuilt/bin/x86/mobility_server /opt/online_backends/code_mobility/

echo -e " done\n"

echo -e "${CGREEN}Code Mobility Components build process COMPLETED.\n${CDEFAULT}"

[ $(which tree) != '' ] && 	tree -h prebuilt