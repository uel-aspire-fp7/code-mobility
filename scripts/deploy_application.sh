#!/bin/bash

APPLICATION_ID=""
MOBILE_BLOCKS_PATH=$7

while getopts "a:p:i:" opt
do
	case $opt in

	a)
		APPLICATION_ID=$OPTARG
		;;
	\?)
		;;
	esac
done

echo "APPLICATION ID  = ${APPLICATION_ID}"

if [ -z "$APPLICATION_ID" ]; then
    echo "Please specify the application id"
    exit 1
fi

if [ -z "$MOBILE_BLOCKS_PATH" ]; then
    echo "Please specify the mobile blocks path"
    exit 1
fi

if [ ! -e "$MOBILE_BLOCKS_PATH" ]; then
    echo "Please specify a valid mobile blocks path"
    exit 1
fi

TARGET_PATH=/opt/online_backends/$APPLICATION_ID/code_mobility

[ -e $TARGET_PATH ] || mkdir -p $TARGET_PATH

rm -rf ${TARGET_PATH}/*

count=0
for f in $( ls $MOBILE_BLOCKS_PATH )
do
    target=${TARGET_PATH}/$( printf "%08x" ${count} )
    cp -r ${MOBILE_BLOCKS_PATH}/${f} ${target}
    echo $f > ${target}/source.txt
    count=$((count+1))
done
