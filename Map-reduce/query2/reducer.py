import sys
image = None
cur_image = None
cur_count = 0
min_count = 0
min_image = None
imagedict = {}
for line in sys.stdin:
    line = line.strip()

    try:
        image, count = line.split('\t', 1)
        image = image.strip()
        count = int(count.strip())
    except:
        continue

    if cur_image == image:
        cur_count += 1
    else:
        if cur_image is not None:
            if len(imagedict) == 0:
                imagedict[cur_image] = cur_count
                min_count = cur_count
                min_image = cur_image
            elif len(imagedict) < 10:
                imagedict[cur_image] = cur_count
                if cur_count <  min_count:
                    min_count = cur_count
                    min_image = cur_image
            else:
                if cur_count > min_count:
                    
                    try:
                        imagedict.pop(min_image)
                    except:
                        print(str(imagedict))
                        print(min_image, min_count)

                        print(cur_count)
                        break
                    imagedict[cur_image] = cur_count
                    min_count = cur_count
                    for key in imagedict.keys():
                        if imagedict[key] <= min_count:
                            min_count = imagedict[key]
                            min_image = key
                    
            # print('%s\t%s' % (cur_image, cur_count))
        
        cur_image = image
        cur_count = count
# print('%s\t%s' % (cur_image, cur_count))
if cur_count > min_count:
    imagedict.pop(min_image)
    imagedict[cur_image] = cur_count

for key in imagedict.keys():
    print('%s\t%d' % (key, imagedict[key]))