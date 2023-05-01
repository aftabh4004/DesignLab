Mapper:
    The mapper will create key value pair for each log entry in which an image('.png' , '.jpg', '.gif', '.ico') is requested as (resources, 1)
    for example, if the log entry is like
    10.223.157.186 - - [15/Jul/2009:15:50:36 -0700] "GET /assets/img/search-button.gif HTTP/1.1" 200 168

    The key value pair will be generated as (/assets/img/search-button.gif, 1)
    The extensions are condsidered case-insensitive

Reducer:
    The result of mapper is first sort using linux sort function and then piped to reducer. 
    The reducer will count the occurance of unique image and will maintain a d[ctonary with key as image as value as its number of occurances.

    At any time in program, we make sure that the element in the dictonary is not greater than 10, we only keep the image whose count is in top 10, as seen so far.

    We will have the required output at the end.


How to run:
    $ make
