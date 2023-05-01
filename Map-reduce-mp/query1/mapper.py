import re

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
                content = m.group(4).split()[1].strip()
                load = m.group(6)
                load = int(load.strip())
            except ValueError:
                load = 0
                    
            type = None
            if '.html' in content or '.php' in content or '.css' in content or '.js' in content:
                type = 'webpage'
            if '.png' in content or '.jpg' in content or '.gif' in content or '.ico' in content:
                type = 'image'
            elif '.mp4' in content or '.flv' in content:
                type = 'video'
            elif '.mp3' in content:
                type = 'audio'

            if type is not None:
                print('%s\t%s\t%s' %(type, 1, load), file=mfp)
        