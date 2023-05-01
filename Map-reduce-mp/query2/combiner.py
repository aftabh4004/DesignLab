import re
import sys
def combiner(input_file):
    current_resource = None
    current_ips = []

    with open(input_file, 'r') as fp, open('combiner_' + input_file, 'w') as cfp:
        lines =  fp.readlines()
        for line in lines:
            
            try:
                line = line.strip()

                if line == '':
                    break
                resource, ip = line.split('\t')
                
                resource = resource.strip()
                ip = ip.strip()

            except:
                continue
            
            if current_resource == resource:
                current_ips.append(ip)
            else:
                if current_resource:
                    ips = ''
                    for p in current_ips:
                        ips += '\t' + p
                    print('%s%s' % (current_resource, ips), file=cfp)
                current_resource = resource
                current_ips = [ip]

        ips = ''
        for p in current_ips:
            ips += '\t' + p
        print('%s%s' % (current_resource, ips), file=cfp)