from mapper import mapper
from combiner import combiner
from reducer import reducer

import multiprocessing
import time
import os
# Mapper phase

start = time.time()
location = './access_logs/'


mapper_p1 = multiprocessing.Process(target=mapper, args=['access_log11.txt', location])
mapper_p2 = multiprocessing.Process(target=mapper, args=['access_log12.txt', location])

mapper_p1.start()
mapper_p2.start()

mapper_p1.join()
mapper_p2.join()

os.system('sort mapper_access_log11.txt > ms_access_log11.txt ')
os.system('sort mapper_access_log12.txt > ms_access_log12.txt ')


# Combiner phase

combiner_p1 = multiprocessing.Process(target=combiner, args=['ms_access_log11.txt'])
combiner_p2 = multiprocessing.Process(target=combiner, args=['ms_access_log12.txt'])

combiner_p1.start()
combiner_p2.start()

combiner_p1.join()
combiner_p2.join()


# Reducer phase
reducer()

end = time.time()

print(end - start, 'sec')
