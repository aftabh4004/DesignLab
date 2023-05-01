import ply.yacc as yacc
from lextoken import tokens, lexer


file_obj= open('fifa_data.html','r',encoding="utf-8")
data=file_obj.read()
file_obj.close()

award = []
awardee = []


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
               | OPENLI skiptag
               | CLOSELI skiptag
               | OPENSUP skiptag
               | CLOSESUP skiptag
               | '''
    

def p_handleData(p):
    '''handleData : OPENDATA OPENHREF CLOSEHREF CONTENT OPENHREF CONTENT CLOSEHREF CLOSEDATA handleData
                  | OPENDATA OPENHREF CONTENT CLOSEHREF CLOSEDATA handleData
                  | OPENDATA skiptag CLOSEDATA handleData
                  |'''
    if len(p) == 10:
        # print(p[6])
        awardee.append(p[6])
    if len(p) == 7:
        # print(p[3])
        awardee.append(p[3])
    
def p_handleHeader(p):
    '''handleHeader : OPENHEADER OPENHREF CONTENT CLOSEHREF CLOSEHEADER handleHeader
                    | OPENHEADER CONTENT CLOSEHEADER handleHeader
                    |'''
    if len(p) == 7:
        # print(p[3])
        award.append(p[3])
    if len(p) == 5:
        # print(p[2])
        award.append(p[2])

def p_handlerow(p):
    '''handlerow : OPENROW handleHeader CLOSEROW handlerow
                 | OPENROW handleData CLOSEROW handlerow
                 |'''

def p_table(p):
    '''table : BEGINAWARD skiptag OPENTABLE handlerow CLOSETABLE'''


def p_error(p):
    pass
    

parser = yacc.yacc()
parser.parse(data)

award_data = {}
for i in range(len(award)):
    award_data[award[i]] = awardee[i]
# print(award_data)
