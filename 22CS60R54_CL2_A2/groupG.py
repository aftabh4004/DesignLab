import ply.yacc as yacc
from lextoken import tokens, lexer


file_obj= open('fifa_data.html','r',encoding="utf-8")
data=file_obj.read()
file_obj.close()

goalf_c = []
match_data = []
single_data = {
    'date': '',
    'team1': '',
    'team2': '',
    'score': [],
    'stadium': '',
    'goal_scorer_t1': [],
    'goal_scorer_t2': [],
    'Attendence' : '',
    'Referee': ''
}

lexer.input(data)
# while True:
#     tok = lexer.token()
#     if not tok:
#         break
#     #print(tok)



def p_start(p):
    '''start : root BEGINGROUPH skipall'''

def p_skipall(p):
    '''skipall : BEGINGROUPB skipall         
               | BEGINGROUPD skipall         
               | BEGINGROUPE skipall         
               |    BEGINGROUPF skipall         
               |    BEGINGROUPC  skipall        
               |    BEGINGROUPH   skipall       
               |    BEGINKNOCKOUT  skipall
               |    BEGINROF16 skipall
               |    BEGINQF skipall
               |    BEGINSF skipall
               |    BEGINTPPO skipall
               |    BEGINFINALS skipall      
               |    BEGINTABLE    skipall       
               |    BEGINTEAMNAME  skipall      
               |    CLOSEDATA  skipall          
               |    CLOSEDIV    skipall         
               |    CLOSEHEADER        skipall  
               |    CLOSEHREF            skipall
               |    CLOSELI              skipall
               |    CLOSEROW             skipall
               |    CLOSESPAN           skipall 
               |    CLOSESTYLE           skipall
               |    CLOSESUP            skipall 
               |     CLOSETABLE          skipall 
               |         CLOSETIME           skipall 
               |      CODEDCHAR           skipall 
               |    CONTENT             skipall 
               | GARBAGE             skipall 
            |    OPENDATA            skipall 
            |            OPENDIV           skipall   
            |    OPENHEADER          skipall 
            |           OPENHREF            skipall 
            | OPENLI              skipall 
            |         OPENROW           skipall   
            |         OPENSPAN            skipall 
            |    OPENSTYLE           skipall 
            |    OPENSUP             skipall 
            |     OPENTABLE           skipall 
            |         OPENTIME skipall
            |'''

def p_skiptag(p):
    '''skiptag : CONTENT skiptag
               | OPENHREF skiptag
               | CLOSEHREF skiptag
               | OPENSUP skiptag
               | CLOSESUP skiptag
               | '''
def p_skiptagex(p):
    '''skiptagex : CONTENT skiptagex
               | OPENHREF skiptagex
               | CLOSEHREF skiptagex
               | OPENSUP skiptagex
               | CLOSESUP skiptagex
               | OPENHEADER skiptagex
               | CLOSEHEADER skiptagex
               | OPENLI skiptagex
               | CLOSELI skiptagex
               | '''

def p_handleinfo(p):
    '''handleinfo : OPENHREF CONTENT CLOSEHREF CONTENT OPENHREF CONTENT CLOSEHREF CONTENT CONTENT CONTENT CONTENT OPENHREF CONTENT'''
    #print("Stadium: " + p[2] + ', ' + p[6])
    #print("Attendence: " + p[9])
    #print("Referee " + p[13])
    single_data['stadium'] = p[2] + ', ' + p[6]
    single_data['Attendence'] = p[9]
    single_data['Referee'] = p[13]
    match_data.append(single_data.copy())
    single_data['team2'] = ''
    single_data["goal_scorer_t1"] = []
    single_data["goal_scorer_t2"] = []
    

def p_handleHeader(p):
    '''handleHeader : OPENHEADER OPENHREF CONTENT CLOSEHREF CLOSEHEADER handleHeader
                    | OPENHEADER OPENHREF CONTENT CONTENT CLOSEHREF CLOSEHEADER handleHeader
                    | OPENHEADER skiptag CLOSEHEADER handleHeader
                    |'''
    if len(p) == 7:
        if single_data.get("team2", "") == "":
            single_data["team2"] = p[3]
            #print("team2 " + p[3])
        else:
            single_data["team1"] = p[3]
            #print("team1 " + p[3])
    if len(p) == 8:
        single_data["score"] = [int(p[3]), int(p[4])]
        #print("score " + p[3] + ' ' + p[4])
    

def p_handleliTeam2(p):
    '''handleliTeam2 : OPENLI OPENHREF CONTENT CLOSEHREF skiptag CLOSELI handleliTeam2
                | '''
    
    if len(p) == 8:
        single_data["goal_scorer_t2"].append(p[3])
        #print("goal scorer Team2 " + str(p[3]))
        


def p_handleliTeam1(p):
    '''handleliTeam1 : OPENLI OPENHREF CONTENT CLOSEHREF skiptag CLOSELI handleliTeam1
                | '''
    
    if len(p) == 8:
        single_data["goal_scorer_t1"].append(p[3])
        #print("goal scorer Team1 " + str(p[3]))

def p_handlerow(p):
    '''handlerow : OPENROW handleHeader CLOSEROW handlerow
                 | OPENROW OPENDATA skiptag handleliTeam1 CLOSEDATA OPENDATA skiptag CLOSEDATA OPENDATA skiptag handleliTeam2 CLOSEDATA skiptag CLOSEROW handlerow
                 | '''

def p_handledatetime(p):
    '''handledatetime : OPENTIME CONTENT CONTENT CONTENT skiptag CLOSETIME'''
    if len(p) == 7:
        single_data["date"] = p[2] + ' ' + p[3] + ' ' + p[4]
        #print('================================================================')
        #print()
        #print('data: ' + p[2] + ' ' + p[3] + ' ' + p[4])


def p_handlegoalData(p):
    '''handlegoalData : OPENDATA CONTENT CLOSEDATA OPENHEADER OPENHREF CONTENT CLOSEHREF CLOSEHEADER OPENDATA CONTENT CLOSEDATA OPENDATA CONTENT CLOSEDATA OPENDATA CONTENT CLOSEDATA OPENDATA CONTENT CLOSEDATA OPENDATA CONTENT CLOSEDATA OPENDATA CONTENT CLOSEDATA OPENDATA CONTENT CLOSEDATA OPENDATA CONTENT CLOSEDATA OPENDATA skiptag CLOSEDATA
                      | OPENDATA CONTENT CLOSEDATA OPENHEADER OPENHREF CONTENT CLOSEHREF CLOSEHEADER OPENDATA CONTENT CLOSEDATA OPENDATA CONTENT CLOSEDATA OPENDATA CONTENT CLOSEDATA OPENDATA CONTENT CLOSEDATA OPENDATA CONTENT CLOSEDATA OPENDATA CONTENT CLOSEDATA OPENDATA CONTENT CLOSEDATA OPENDATA CONTENT CLOSEDATA'''
    #print('===========================================================================')
    #print()
    #print('Goal forwarded & conceded')
    #print(p[6] + ' ' + p[22] + ' ' + p[25])
    goalf_c.append([p[6], p[22], p[25]])

def p_handlegoalrow(p):
    '''handlegoalrow : OPENROW skiptagex CLOSEROW handlegoalrow
                     | OPENROW handlegoalData CLOSEROW handlegoalrow
                     |'''


def p_root(p):
    '''root : BEGINGROUPA skipall BEGINGROUPG skiptag OPENTABLE handlegoalrow CLOSETABLE handledatetime OPENTABLE handlerow CLOSETABLE handleinfo skiptag root
             | handledatetime OPENTABLE handlerow CLOSETABLE handleinfo skiptag root
             |'''
    

def p_error(p):
    pass
    

parser = yacc.yacc()  
parser.parse(data)
#print(match_data)
#print(goalf_c)