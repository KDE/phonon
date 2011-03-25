#!/bin/bash

esc() {
    echo "$1" | sed -e 's/\//\\\//g' -e 's/\&/\\\&/g'
}

pwd_=`pwd`

cd @CMAKE_CURRENT_SOURCE_DIR@/..
if [ ! -e abi ]; then 
    mkdir abi
fi
cd abi

install_prefix="`esc "@CMAKE_INSTALL_PREFIX@"`"
target_dir_=`pwd`
target_dir="`esc $target_dir_`\/@PHONON_LIB_VERSION@\/${install_prefix}"

cd $pwd_

sed -i "s/$install_prefix/$target_dir/" @DESCRIPTOR_FILE_PATH@
rm -rf $target_dir_
make install DESTDIR=$target_dir

# cd ${target_dir_}
# args=""
# i=1
# for version in `ls | grep -P "\d\.\d\.\d"  | sort`; do
#     $args="${args} -d${i} ${version}/${version}.xml"
#     let "i=i+1"
# done
# 
# abi-compliance-checker -l Phonon $args
