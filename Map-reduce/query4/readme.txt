Mapper:
    The mapper will create key value pair for each log entry as (ip, load 1)
    the load is considered as zero if load in the entry is '-'
 
Reducer:
    The result of mapper is first sort using linux sort function and then piped to reducer.
    The reducer will count the request from unique IPs as well as it will maintain a bool variable
    for each IP which will set to False if any load >= 1000 is seen.

    In this way, whichever IP has request count >= 10 and a True flag will be considered as a bot.


How to run:
    $ make

