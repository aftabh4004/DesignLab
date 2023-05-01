import sys
import re
from dfs import DFT
def reducer():
    graph = {}
    for i in range(11, 13):
        try:
            fp = open('combiner_ms_access_log%s.txt' % i, 'r')
            lines = fp.readlines()
            for line in lines:
                try:
                    adjrow = line.split('\t')
                    
                    for i in range(len(adjrow)):
                        adjrow[i] = adjrow[i].strip()
                    parent = adjrow[0]
                    childs = adjrow[1:]
                    
                    if parent not in graph.keys():
                        graph[parent] = childs
                    else:
                        graph[parent] = list(set().union(graph[parent], childs))
                except:
                    continue

            fp.close()
        except:
            continue
    
    sys.setrecursionlimit(5000)
    visited = DFT(graph)
    components = {}
    for key in visited.keys():
        try:
            m = re.match(r"(\d+\.\d+\.\d+\.\d+)", key)
            m.group(1)
        except:
            continue
        if visited[key] not in components.keys():
            components[visited[key]] = 1
        else:
            components[visited[key]] += 1
    
    ans = 0
    for i in components.keys():
        for j in components.keys():
            ans += components[i] * components[j]
    with open('result4.txt', 'w') as res:
        print(components.values(), file=res)
        print(ans, file=res)

    