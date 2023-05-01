import sys
ip = None
cur_ip = None
cur_count = 0

for line in sys.stdin:
    line = line.strip()

    try:
        ip, count = line.split('\t')
        ip.strip()
        count = int(count.strip())
    except:
        continue

    if cur_ip == ip:
        cur_count += 1
    else:
        if cur_ip is not None:
            print('%s\t%s' % (cur_ip, cur_count))
        cur_ip = ip
        cur_count = count
print('%s\t%s' % (cur_ip, cur_count))