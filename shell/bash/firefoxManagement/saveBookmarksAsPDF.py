#!/usr/bin/python3

# Load bookmark Firefox json file and print
# as pdf all bookmarks stored in given folder.
# Requires: './crawlerToPdf.bash' in same folder

import datetime
import json as js
from subprocess import Popen, PIPE

desktopId = 2
saveFolder = "/home/ahonorat/Bureau/printedBookmarks"
bmFolder = "presse/nécrologies"
bmFile = open("/home/ahonorat/Bureau/printedBookmarks/bookmarks-2022-11-11.json")
bmData = js.load(bmFile)

allBmks = bmData['children'][0]['children']
## worthy types:
# text/x-moz-place: pure bookmark
# text/x-moz-place-container: subfolder
# text/x-moz-place-separator: separator?

if bmFolder:
    # Retrieve bookmarks in given (sub)folder
    folderPath = bmFolder.split("/")
    while folderPath:
        folderBmks = None
        for bmkEntry in allBmks:
            if bmkEntry['title'] == folderPath[0] and \
               bmkEntry['type'] == 'text/x-moz-place-container':
                folderBmks = bmkEntry['children']
                allBmks = bmkEntry['children']
                folderPath.pop(0)
                break
        if folderBmks is None:
            print("No bookmark folder \"{}\" found.".format(bmFolder))
            exit(1)
    folderPureBmks = [bmk for bmk in folderBmks if bmk['type'] == 'text/x-moz-place']
    print("{} bookmarks found in folder.".format(len(folderPureBmks)))
else:
    # Retrieve all bookmarks
    folderPureBmks = []
    childrenToVisit = list(allBmks)
    while childrenToVisit:
        bmkEntry = childrenToVisit.pop(0)
        if bmkEntry['type'] == 'text/x-moz-place-container':
            childrenToVisit.extend(bmkEntry['children'])
        elif bmkEntry['type'] == 'text/x-moz-place':
            folderPureBmks.append(bmkEntry)
    print("{} bookmarks found in total.".format(len(folderPureBmks)))
    

# Function giving title of a bookmark
def getTitle(bmk):
    uri = bmk['uri']
    # remove params after '?' and trailing '/'
    uriWOparams = uri.split('?')[0]
    if uriWOparams.endswith('/'):
        uriWOparams = uriWOparams[:-1]
    uriParts = uriWOparams.split('/')
    # page name without trailing '.html'
    lastValueUri = uriParts[-1]
    lastValueOnly = lastValueUri.split('.')[0]
    # domain after http://
    websiteDomains = uriParts[2].split('.')
    if websiteDomains[0] == "www":
        websiteDomains = websiteDomains[1:]
    # domain without extension '.fr' etc...
    websiteMainDomain = '-'.join(websiteDomains[:-1])
    # get date, adapted from:
    # https://support.mozilla.org/en-US/questions/1152487
    truncEpochDate = bmk['dateAdded'] // 1e6
    date = datetime.datetime.fromtimestamp(truncEpochDate).strftime('%Y-%m-%d_')
    # concatenate date_domain_pageName as title
    title = date + websiteMainDomain + "_" + lastValueOnly
    return title


# Process the bookmarks
for bmk in folderPureBmks:
    uri = bmk['uri']
    if not uri.startswith("http"):
        print("Following bookmark seems not a valid URL.")
        print(uri)
    else: #elif not uri.startswith("https://www.lemonde.fr"):
        title = getTitle(bmk)
        print("++ Printing: {}".format(title))
        print(uri)
        # call the pdf printer script
        processPrint = Popen(["./crawlerToPdf.bash", str(desktopId), saveFolder, title, uri, stdout=PIPE)
        (output, err) = processPrint.communicate()
        exit_code = processPrint.wait()
        if exit_code:
            print("-- Error while printing: {}".format(title))
            print(output.decode('ascii').strip())

        

        
