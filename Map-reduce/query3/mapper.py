import re
import sys
from datetime import datetime
import calendar

input_file =  sys.argv[1]
with open(input_file, 'r')as fp:
    while True:
        try:
            line = fp.readline()
        except:
            continue
        
        if line == '':
            break
        
        m = re.match(r"(\d+\.\d+\.\d+\.\d+) (.*?) \[(.+)\] \"(.+)\" (\d+) (\d+|-)", line)

        try:
            try:
                timestr = m.group(3).split()
                packet_size = int(m.group(6))
            except ValueError:
                packet_size = 0

            offset = timestr[1]
            # print(offset)
            offestsec = int(offset[1:3]) * 60 * 60 + int(offset[3:5]) * 60
            if offset[0] == '-':
                offestsec *= -1


            epoch = int(datetime.strptime(timestr[0], "%d/%b/%Y:%H:%M:%S").timestamp()) + offestsec
            print('%s\t%s' %(epoch, packet_size))
        except:
            continue


