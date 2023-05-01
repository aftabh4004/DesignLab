import ply.yacc as yacc
from lextoken import tokens, lexer



file_obj= open('fifa_data.html','r',encoding="utf-8")
data=file_obj.read()
file_obj.close()

teams = []
links = []
teams_data = {}

lexer.input(data)
# while True:
#     tok = lexer.token()
#     if not tok:
#         break
#     print(tok)



def p_start(p):
    '''start : table'''
    p[0] = p[1]


def p_skiptag(p):
    '''skiptag : CONTENT skiptag
               | OPENHREF skiptag
               | CLOSEHREF skiptag
               | OPENSUP skiptag
               | CLOSESUP skiptag
               | '''

def p_handleli(p):
    '''handleli : OPENLI OPENHREF CONTENT CLOSEHREF skiptag CLOSELI skiptag handleli
                | OPENLI CONTENT CLOSELI handleli
                |'''
    if len(p) == 9:
        # print(p[3])
        teams.append(p[3])
        links.append(str(p[2]).split('"')[1])

def p_handleData(p):
    '''handleData : OPENDATA skiptag handleli CLOSEDATA handleData
                  |'''

def p_handlerow(p):
    '''handlerow : OPENROW handleData CLOSEROW'''


def p_table(p):
    '''table : BEGINTEAMNAME skiptag OPENTABLE handlerow CLOSETABLE'''




def p_error(p):
    pass
    

parser = yacc.yacc()  
parser.parse(data)


for i in range(len(teams)):
    teams_data[teams[i]] = links[i]

