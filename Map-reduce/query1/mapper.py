import re
import sys

input_file =  sys.argv[1]
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
            ip = m.group(1)
            print('%s\t%s' %(ip, 1))
        except:
            continue


