import sys
ip = None
cur_ip = None
cur_count = 0
is_cur_suspected = None

for line in sys.stdin:
    line = line.strip()

    try:
        ip, load, count = line.split('\t')
        ip = ip.strip()
        count = int(count.strip())
        load = int(load.strip())
    except:
        continue

    if cur_ip == ip:
        cur_count += 1
        is_cur_suspected = is_cur_suspected and (load < 1000)
    else:
        if cur_ip is not None:
            if cur_count >= 10 and is_cur_suspected:
                print('%s' % (cur_ip))
        cur_ip = ip
        cur_count = count
        is_cur_suspected = (load < 1000)
if cur_count >= 10 and is_cur_suspected:
    print('%s' % (cur_ip))