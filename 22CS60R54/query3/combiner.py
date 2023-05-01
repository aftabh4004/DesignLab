def combiner(input_file):
    current_type = None
    current_count = 0
    current_load = 0
    load = 0


    # input comes from STDIN
    with open(input_file, 'r') as fp, open('combiner_' + input_file, 'w') as cfp:
        lines =  fp.readlines()
        for line in lines:
            
            try:
                line = line.strip()

                if line == '':
                    break
                type , count, load = line.split('\t')
                load = int(load.strip())
                count = int(count.strip())
                type = type.strip()
            except:
                continue
            
            if current_type == type:
                current_count += count
                current_load += load
            else:
                if current_type:
                    print ('%s\t%s\t%s' % (current_type, current_count, current_load), file=cfp)
                current_count = count
                current_type = type
                current_load = load

        
        print('%s\t%s\t%s' % (current_type, current_count, current_load), file=cfp)