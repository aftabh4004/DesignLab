Mapper:
    The mapper will create key value pair for each log entry as (ip, 1)

Reducer:
    The result of mapper is first sort using linux sort function and then piped to reducer. The reducer will count
    identical successive ip entries and print the correspoding result as (ip_i, number of occurence of ip_i).
    At the end, this will count the number of request made from each unique IP.


How to run:
    $ make
