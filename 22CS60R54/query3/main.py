from ast import arg
from mapper import mapper
from combiner import combiner
from reducer import reducer

import multiprocessing
import time
import os
# Mapper phase

start = time.time()
mapper_p = []
combiner_p = []
location = './access_logs/'
for i in range(1, 11):
    mapper_p.append(multiprocessing.Process(target=mapper, args=['access_log%s.txt'% i, location])) 

for p in mapper_p:
    p.start()

for p in mapper_p:
    p.join()

for i in range(1, 11):
    os.system('sort mapper_access_log%s.txt > ms_access_log%s.txt'% (i, i))

# Combiner phase

for i in range(1, 11):
    combiner_p.append(multiprocessing.Process(target=combiner, args=['ms_access_log%d.txt'% i])) 

for p in combiner_p:
    p.start()

for p in combiner_p:
    p.join()



# Reducer phase
reducer()

end = time.time()

print(end - start, 'sec')
