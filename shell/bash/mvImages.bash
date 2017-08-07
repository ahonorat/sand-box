#!/bin/bash

regex="DSC_0*([1-9][0-9]*)\.JPG"

delay=9

for i in *.JPG; do
    echo "--> ${i}"
    if [[ "${i}" =~ ${regex} ]]; then
	nb="${BASH_REMATCH[1]}"
	echo "${nb}"
	if (( "${nb}" >= 75 )); then
	    nb=$((${nb} + ${delay}))
	    nnb=$(printf "DSC_%04d.JPG" "${nb}")
	    echo ": ${nnb}"
	    cp -n "${i}" "../113ETE17/${nnb}"
	fi
    else
	echo "pokpa"
    fi
done
