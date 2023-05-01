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
            req = m.group(4).split()[1]
            reqq = req.lower()[-4:]
            if reqq in ('.png' , '.jpg', '.gif', '.ico'):
                print('%s\t%s' %(req, 1))
        except:
            continue


