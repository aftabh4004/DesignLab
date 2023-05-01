mapper:
    The mapper function will take two arguments, first the file to be mapped i.e accesslog and second the location of file.
    Mapper will parse the access log file and get the required data using regex. 
    We will look for different type of resources such as images, videos etc and label them appropriately.
    At the end, it will write the key value pair as (type of resource, 1, load of resource) in the file mapper_access_log<i>.txt


Before passing the output of mapper to combiner, We will sort the output of mapper using linux sort function as
os.system('sort file1 > file2')

Combiner:
    Combiner will read the sorted output of mapper. It will just simply count the occurence of a type of resource and
    sum of the overall load.
    The combiner will write its output in file combiner_ms_access_log<i>.txt as 

    (type,  total count of particular type, sum of loads of all resource of particular type)

Reducer:
    Reducer will read the intermediate file written by all the combiner, i.e reducer will read all 10 files
    and maintain a dictionary to aggregate the count and load sum of each type of resource.

    The average size of a particular type of resource has been asked in the problem which can be calculate as

                overall load        dictionary[type][load]
    average = ----------------- = --------------------------
                overall count        dictionary[type][count]
    



Sequential Computation:

- The program will call mapper for each of the 10 access log files, and the mapper will write its output in an intermediate file.
- sort the output
- combiner will be called for each sorted file sequentially. Output of combiner will be written in another intermediate file.
- Run reducer.
