#!/bin/bash
set -o errexit
set -o pipefail
set -o nounset
#set -o verbose
#set -o xtrace

# Parameters:
# 1: Path to a directory containing mobile blocks
# 2: Path to an object file from which to take data
# 3: Suffix for the update file we will create, containing the names of the updated mobile blocks (metadata)

# Check the number of parameters
if [ "$#" -ne 3 ]; then
  echo "Illegal number of parameters. Expected a directory containing mobile blocks, a path to an object file, and a suffix for the update file."
  exit -1
fi

blocks_dir=$1
obj=$2
update_file="updated_$3"

if [ ! -d $blocks_dir ]; then
  echo "The directory containing the mobile blocks doesn't exit."
  exit -2
fi

if [ ! -f $obj ]; then
  echo "The object file passed as an argument doesn't exist."
  exit -3
fi


# Find the metadata for the blocks that might contain data from the given object
cd $blocks_dir
obj_name=$(basename $obj)
md_files=$(grep -l $obj_name *.metadata)

# Empty the update file to be created
> $update_file

# Iterate over all these metadata files
for md_file in $md_files; do

  # Some initialization
  updated=false
  block=${md_file%.metadata}

  # Read the subsections one by one
  while read -r label1 subobj label2 subsec label3 offset; do
    # Check if it is the right subobject
    if [ $(basename $subobj) != $(basename $obj) ]; then
      continue
    fi

    # The block will be updated
    updated=true

    # Get the offset in decimal
    offset=$(printf '%d' $offset)

    # Create a temporary file to contain the new data and get it
    temp_file=$(mktemp)
    /opt/diablo-gcc-toolchain/bin/arm-diablo-linux-gnueabi-objcopy -O binary --only-section=$subsec $obj $temp_file

    # Write the new data at the requested location
    dd if=$temp_file of=$block obs=1 seek=$offset conv=notrunc &> /dev/null

    # Cleanup
    rm $temp_file

  done < $md_file

  # If the file was updated, append its name to the update file
  if [ "$updated" = true ]; then
    echo $block >> $update_file
  fi
done
