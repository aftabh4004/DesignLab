import ply.yacc as yacc
from lextoken import tokens, lexer



file_obj= open('fifa_data.html','r',encoding="utf-8")
data=file_obj.read()
file_obj.close()

stadiums = []
capacity = []
venue_details = []

lexer.input(data)
# while True:
#     tok = lexer.token()
#     if not tok:
#         break
#     print(tok)


def p_start(p):
    '''start : table'''

def p_skiptag(p):
    '''skiptag : CONTENT skiptag
               | OPENHREF skiptag
               | CLOSEHREF skiptag
               | OPENSUP skiptag
               | CLOSESUP skiptag
               | '''
    

def p_handleData(p):
    '''handleData : OPENDATA OPENHREF CONTENT CLOSEHREF CLOSEDATA handleData
                  | OPENDATA CONTENT  skiptag CLOSEDATA handleData
                  | '''
    if len(p) == 7:
        # print(p[3])
        stadiums.append(p[3])
    if len(p) == 6:
        # print(p[2])
        capacity.append(p[2])

def p_handlerow(p):
    '''handlerow : OPENROW OPENHEADER CONTENT CLOSEHEADER OPENHEADER CONTENT CLOSEHEADER OPENHEADER CONTENT CLOSEHEADER CLOSEROW handlerow
                 | OPENROW OPENHEADER OPENHREF CONTENT CLOSEHREF CLOSEHEADER handleData CLOSEROW handlerow
                 | OPENROW handleData CLOSEROW handlerow
                 | '''

def p_table(p):
    '''table : BEGINTABLE skiptag OPENTABLE handlerow CLOSETABLE'''


def p_error(p):
    pass
    

parser = yacc.yacc()
parser.parse(data)

for i in range(len(stadiums)):
    venue_details.append({'stadium': stadiums[i], 'capacity': capacity[i]})

# print(venue_details)
    