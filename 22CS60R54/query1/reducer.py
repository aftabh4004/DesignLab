def reducer():
    dic = {}
    for i in range(1, 13):
        try:
            fp = open('combiner_ms_access_log%s.txt' % i, 'r')
            lines = fp.readlines()
            for line in lines:
                try:
                    type, count, load = line.split('\t')
                    type = type.strip()
                    count = int(count.strip())
                    load = int(load.strip())

                    if type not in dic.keys():
                        dic[type] = [count, load]
                    else:
                        dic[type][0] += count
                        dic[type][1] += load

                except:
                    continue

            fp.close()
        except:
            continue
        
    
    with open('result1.txt', 'w') as res:
        for key in dic.keys():
            print('%s\t%.2f' % (key, dic[key][1]/dic[key][0]), file=res)