#!/usr/bin/python3
# -*- coding: utf-8 -*-

import oursql

conn = oursql.connect(host='127.0.0.1', user='station', passwd='trollface',db='weather', port=3306)

curs = conn.cursor(oursql.DictCursor)


curs.execute('SELECT * FROM `data`')
