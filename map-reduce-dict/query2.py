import re
import sys
from collections import OrderedDict
import operator

dic = {}

input_file =  'access_log.txt'
with open(input_file, 'r')as fp:
    while True:
        try:
            line = fp.readline()
        except:
            continue
        
        if line == '':
            break
        
        m = re.match(r"(\d+\.\d+\.\d+\.\d+) (.*?) (\[.+\]) \"(.+)\" (\d+) (\d+|-)", line)

        try:
            req = m.group(4).split()[1]
            if '.png' in req or '.jpg' in req or '.gif' in req or '.ico' in req:
                if req in dic.keys():
                    dic[req] += 1
                else:
                    dic[req] = 1 
        except:
            continue
    
    dic = OrderedDict(sorted(dic.items(),key=operator.itemgetter(1), reverse=True))
    i = 1
    for key in dic.keys():
        print('%s\t%d' % (key, dic[key]))
        i += 1
        if i > 10:
            break


