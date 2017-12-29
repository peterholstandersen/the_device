#!/usr/bin/python

import glob
import os

data = []

def get_temperature(filename):

    with open(filename, "r") as f:
        if (os.path.getsize(filename) > 1024):
            f.seek(-1024, 2)

        buffer = f.read(1024)
        lines = buffer.split("\n")

        index = -1
        while (len(lines[index]) == 0):
            index = index -1
            
        last_line = lines[index]
        values = last_line.split("#")
        temperature = values[-1]
        return temperature

def printit(data):
    print "Content-type:text/html\r\n\r\n"
    print '<html>'

    print '<head>'
    print '<link rel="stylesheet" type="text/css" media="screen" href="/styles.css">'
    print '<meta http-equiv="refresh" content="1">'
    print '</head>'

    print '<body>'
    for dat in data:
        print '<div class="span">' + dat[1] + '</div>'
    print '</body>'

    print '</html>'

    
os.chdir("/var/www/log")
for filename in glob.glob("temperature_0x*.log"):
    name = os.path.splitext(os.path.basename(filename))[0]
    pair = (name, get_temperature(filename))
    data.append(pair)

printit(data)
