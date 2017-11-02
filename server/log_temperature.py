#!/usr/bin/python

import cgi
import cgitb
import time

cgitb.enable()

print("Content-Type: text/html\r\n")

print "<html><head></head><body>"

arguments = cgi.FieldStorage()
temperature = arguments.getvalue('temperature')
device_id = arguments.getvalue('id')
ticks = time.time()
localtime = time.asctime( time.localtime(ticks) )

if not temperature:
    # TODO: return some HTML code
    print "NOK"
else:
    f = open("/var/www/log/temperature_" + device_id + ".log", "a+")
    f.write(localtime)
    f.write(" # " + str(int(ticks)))
    f.write(" # " + temperature)
    f.write("\n")
    f.close()
    print("")
    print("OK")
    
print "</body></html>"
