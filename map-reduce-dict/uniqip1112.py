import re
ips = {}
for i in range(11, 13):

    try:
        fp = open('./access_logs/access_log%s.txt' % i, 'r')
        lines = fp.readlines()
        
        for line in lines:
            try:
                m = re.match(r"(\d+\.\d+\.\d+\.\d+) (.*?) (\[.+\]) \"(.+)\" (\d+) (\d+|-)", line)
                
                ip = m.group(1).strip()
                method = m.group(4).split()[0].strip()
                resource = m.group(4).split()[1].strip().split('?')[0].strip()
                if method == 'GET' and '.' in resource:
                    if ip not in ips.keys():
                        ips[ip] = 1
                    else:
                        ips[ip] += 1
            except:
                continue
        fp.close()
    except:
        continue
print(len(ips))
    