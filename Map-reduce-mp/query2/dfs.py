

def DFS(node, graph, visited, count, temp):
    print('temp', file=temp)
    visited[node] = count

    for child in graph[node]:
        if visited[child] == 0:
            DFS(child, graph, visited, count, temp)

def DFT(graph):
    count = 1
    visited = {}
    for node in graph.keys():
        visited[node] = 0

    
    with open('temp.txt', 'w') as temp:
        for node in graph.keys():
            if visited[node] == 0:
                DFS(node, graph, visited, count, temp)
                count += 1
    return visited
    
    