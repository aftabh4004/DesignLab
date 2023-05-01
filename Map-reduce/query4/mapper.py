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
            load = m.group(6)
            try:
                load = int(load.strip())
            except ValueError:
                load = 0
            print('%s\t%s\t%s' %(ip, load, 1))
        except:
            continue


