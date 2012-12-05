#!/usr/bin/python3
# -*- coding: utf-8 -*-

import _mysql
import sys


con = None

try:

    con = _mysql.connect('localhost', 'wth', 
        'station', 'main')
        
    con.query("SELECT VERSION()")
    result = con.use_result()
    
    print ("MySQL version: %s" % \
        result.fetch_row()[0])
    
except _mysql.Error as e:
  
    print ("Error %d: %s" % (e.args[0], e.args[1]))
    sys.exit(1)

finally:
    
    if con:
        con.close()