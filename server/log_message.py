#!/usr/bin/python

import cgi
import cgitb
import time

cgitb.enable()

print("Content-Type: text/html\r\n")

print "<html><head></head><body>"

arguments = cgi.FieldStorage()
device_id = arguments.getvalue('id')
msg = arguments.getvalue('msg')
ticks = time.time()
localtime = time.asctime( time.localtime(ticks) )

if not msg:
    # TODO: return some HTML code
    print "NOK"
else:
    f = open("/var/www/log/" + device_id + ".log", "a+")
    f.write(localtime)
    f.write(" # " + str(int(ticks)))
    f.write(" # " + msg)
    f.write("\n")
    f.close()
    print("")
    print("OK")
    
print "</body></html>"
