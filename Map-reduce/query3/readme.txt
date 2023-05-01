Mapper:
    The mapper will first read and convert the hit time and zone to the epochs, and the use epochs as the key.
    the value will be the corresponding loads.
    So for a log entry,
    10.223.157.186 - - [15/Jul/2009:14:58:59 -0700] "GET / HTTP/1.1" 403 202

    the key value pair will be (1247644739, 202)


Reducer:
    The result of mapper is first sort using linux sort function and then piped to reducer. The reducer will sum of all the load
    that are within a 15 min interval and compare with the maximum so far. if the current load is more then the value of max will
    be updated.

    This gives us the value of load which is maximum among all 15 min windows.  


How to run:
    $ make
