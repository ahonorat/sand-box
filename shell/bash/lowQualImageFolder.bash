#!/bin/bash

# --------------------------------------------------------
# Usage: 
# >> ./lowQualImageFolder
#    $1 = folder containing input images
#
# Assumed: 
#    convert installed
# --------------------------------------------------------

if [ $# -lt 1 -o -z "$1" -o ! -d "$1" ]; then
    echo ""
    echo "ERROR, Syntax: $0 <image folder>"
    echo ""
    exit 1
fi

## parameters
imageExtension="JPG"
qualityPrct="40%"
newNameSuffix="_lowQual"

cd "${1}"
## not so safe (if image name has newline)
newFolder="../$(basename ${1})${newNameSuffix}"
mkdir -p "${newFolder}"

if [ ! -d "${newFolder}" ]; then
    echo "New folder ${1}/${newFolder} could not be created!"
    exit 1
else 
    echo "Succesfully created new folder ${1}/${newFolder}"
fi

## not so safe (if image name has newline)
for img in *.${imageExtension}; do
    echo "Converting ${1}/${img} with quality reduction of ${qualityPrct} ..."
    newImageName="$(basename ${img} .${imageExtension})${newNameSuffix}.${imageExtension}"
    convert "${img}" -quality "${qualityPrct}" "${newFolder}/${newImageName}"
done
