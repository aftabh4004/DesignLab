import re


dic = {}
ans = []
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
            ip = m.group(1)
            load = m.group(6)
            try:
                load = int(load.strip())
                ip = ip.strip()
            except ValueError:
                load = 0
            if ip in dic.keys():
                dic[ip] = [dic[ip][0] + 1, dic[ip][1] and (load < 1000)]
            else:
                dic[ip] = [1, (load < 1000)]
        except:
            continue
    
    for ip in dic.keys():
        if dic[ip][0] >= 10 and dic[ip][1]:
            ans.append(ip)
    print(len(ans))


