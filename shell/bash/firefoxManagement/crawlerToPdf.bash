#! /bin/bash

# --------------------------------------------------------
# Script to save as pdf an URL opened in Firefox.
# Try reading mode. Takes 20 sec per URL.
#
# Usage: 
# >> ./crawlerToPdf
#    $1 = the desktop number
#    $2 = the directory where to save pdf
#    $3 = title of the pdf
#    $4 = URL to save
#
# Assumed: 
#    firefox and xdotool installed
# --------------------------------------------------------

## Adapted from:
## https://askubuntu.com/questions/612485/save-firefox-webpage-by-xdotool/612510#612510

if [[ $# -ne 4 ]]; then
    echo "Invalid number of arguments (4 required)."
    echo "./crawlerToPdf <DESKTOP_ID> <SAVE_DIR> <TITLE> <URL>"
    exit 1
fi

DESKTOP=$1 #2
SAVE_DIR=$2 #"."
TITLE=$3  #"New File"
URL=$4  #"https://www.lemonde.fr/sport/article/2021/07/04/kevin-mayer-j-ai-toujours-pris-le-sport-comme-un-jeu_6086909_3242.html"

# check file first
if [ -f "${SAVE_DIR}/${TITLE}.pdf" ]; then
    echo "File already generated."
    exit 1
fi

# launch firefox in single window
# should be the only window in the desktop
firefox -new-window "${URL}"
FFWID=$(xdotool search --desktop=1 --name "Mozilla Firefox" | head -n1)
xdotool windowactivate "${FFWID}" --sync

# save page in read mode
sleep 5s
xdotool key "ctrl+alt+r"
sleep 3s
xdotool key "ctrl+p"
sleep 1s
xdotool key "Return"
sleep 1s
xdotool type "${TITLE}"
xdotool key "Return"
sleep 10s

# kill the window and check file
xdotool windowactivate "${FFWID}"
xdotool key "alt+F4"
mv "${HOME}/Bureau/${TITLE}.pdf" "${SAVE_DIR}"
if [ ! -f "${SAVE_DIR}/${TITLE}.pdf" ]; then
    echo "No file generated."
    exit 1
fi

