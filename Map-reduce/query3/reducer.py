import sys
epoch = None
cur_epoch = 0
cur_load = 0
max_load = 0

for line in sys.stdin:
    line = line.strip()

    try:
        epoch, load = line.split('\t')
        epoch = int(epoch.strip())
        load = int(load.strip())
    except:
        continue

    if epoch - cur_epoch < 15 * 60  :
        cur_load += load
    else:
        if cur_epoch is not None:
            if max_load < cur_load:
                max_load = cur_load
        cur_epoch = epoch
        cur_load = load
print(max_load)