import re
import sys
def mapper(input_file, location):
    with open (location + input_file, 'r', encoding='utf-8') as fp, open('mapper_' + input_file, 'w') as mfp:
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
                req = m.group(4)
                method = req.split()[0].strip()
                resource = req.split()[1].strip().split('?')[0].strip()
                

                if '.' in resource and method == 'GET':
                    print('%s\t%s' %(resource, ip), file=mfp)
                    print('%s\t%s' %(ip, resource), file=mfp)

            except:
                continue