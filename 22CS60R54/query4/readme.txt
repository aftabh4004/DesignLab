mapper:
    The mapper will go through the access_log file and extract the resources and the corresponding requesting IP.
    Assumptions:
        1. Only GET requests are considered.
        2. files which have some extension such as .png, .php etc are only considered as resources.
        3. File names are case sensitive.

    mapper will write the key value pair as 
    (resources, ip)
    (ip, resources)
    in an intermediate file. mapper_access_log<i>.txt

We will sort the mapper's output using linux sort before passing it to the combiner.

Combiner:
    Combiner will read the sorted mapper output and construct an Adjacency List and write it
    in another intermediate file as

    resources1 ip1 ip2 ip3 ...
    ip1 resources1 resources2 resources3 ...


Reducer:
    Reducer will read the combiner's output files which have the individual's Adjacency list.
    It will megre all the Adjacency lists (in this case only two) to construct an Adjacency list of the overall graph.

    Once the Adjacency list is created for the overall network, we will apply DFS to find the connected components.
    Because the depth of recursion will go beyond 1000 which is the default limit for recursion depth in python, we
    have to overwrite it by writing  "sys.setrecursionlimit(<limit>)" 

    After getting the number of ips in each components, the total number of disjoint pair is calculate and written in 
    result4.txt 