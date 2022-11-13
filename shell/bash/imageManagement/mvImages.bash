#!/bin/bash

# --------------------------------------------------------
# Move images and increment their name by a fixed
# number (${inc}). Parse all .JPG in current folder.
#
# Usage: 
# >> ./mvImages
#
# --------------------------------------------------------


inc=9
regex="DSC_0*([1-9][0-9]*)\.JPG"
newName="DSC_%04d.JPG"
newFolder="../113ETE17"

for i in *.JPG; do
    echo "--> ${i}"
    if [[ "${i}" =~ ${regex} ]]; then
	nb="${BASH_REMATCH[1]}"
	echo "${nb}"
	if (( "${nb}" >= 75 )); then
	    nb=$((${nb} + ${inc}))
	    nnb=$(printf "${newName}" "${nb}")
	    echo ": ${nnb}"
	    cp -n "${i}" "${newFolder}/${nnb}"
	fi
    else
	echo "File ${i} did not match regex."
    fi
done
