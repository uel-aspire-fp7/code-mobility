#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Illegal number of arguments."
    cat README
	exit -1
fi

sed -e "s@TOOLCHAIN_PLACEHOLDER@$1@g" Makefile.template > Makefile.$2