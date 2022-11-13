#!/usr/bin/python

import json as js
import sortedcontainers as sc

tsmFile = open("/home/ahonorat/Bureau/TabSessionManager_temp-2022-02-27_19-53-13.json")
tsmData = js.load(tsmFile)

tabsInWindowList = tsmData[0]['windows']['11'].values()
sortedTabs = sc.SortedList(tabsInWindowList,key=lambda tabDict: tabDict["index"])
for tabDict in sortedTabs:
    print(tabDict['url'] + " " + tabDict['title'])
