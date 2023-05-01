from mapper import mapper
from combiner import combiner
from reducer import reducer

import multiprocessing
import time
import os
# Mapper phase

start = time.time()
location = './access_logs/'


mapper('access_log11.txt', location)
mapper('access_log12.txt', location)

os.system('sort mapper_access_log11.txt > ms_access_log11.txt ')
os.system('sort mapper_access_log12.txt > ms_access_log12.txt ')


# Combiner phase

combiner('ms_access_log11.txt')
combiner('ms_access_log12.txt')

# Reducer phase
reducer()

end = time.time()

print(end - start, 'sec')
