import ply.yacc as yacc
from lextoken import tokens, lexer


file_obj= open('fifa_data.html','r',encoding="utf-8")
data=file_obj.read()
file_obj.close()

match_data = []
single_data = {
    'date': '',
    'team1': '',
    'team2': '',
    'score': [],
    'stadium': '',
    'penalties_score': None,
    'goal_scorer_t1': [],
    'goal_scorer_t2': [],
    'pen_att_t1': [],
    'pen_att_t2': [],
    'Attendence' : '',
    'Referee': ''
}

lexer.input(data)
# while True:
#     tok = lexer.token()
#     if not tok:
#         break
#     print(tok)



def p_start(p):
    '''start : root BEGINQF skipall'''

def p_skipall(p):
    '''skipall : BEGINGROUPB skipall
               | BEGINGROUPC skipall         
               | BEGINGROUPD skipall         
               | BEGINGROUPE skipall         
               |    BEGINGROUPF skipall         
               |    BEGINGROUPG  skipall        
               |    BEGINGROUPH   skipall       
               |    BEGINKNOCKOUT  skipall   
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

def p_skipcontent(p):
    '''skipcontent : CONTENT skipcontent
                    |'''

def p_skiptag(p):
    '''skiptag : CONTENT skiptag
               | OPENHREF skiptag
               | CLOSEHREF skiptag
               | OPENSUP skiptag
               | CLOSESUP skiptag
               | '''

def p_handleinfo(p):
    '''handleinfo : OPENHREF CONTENT CLOSEHREF CONTENT OPENHREF CONTENT CLOSEHREF CONTENT CONTENT CONTENT CONTENT OPENHREF CONTENT'''
    # print("Stadium: " + p[2] + ', ' + p[6])
    # print("Attendence: " + p[9])
    # print("Referee " + p[13])
    single_data['stadium'] = p[2] + ', ' + p[6]
    single_data['Attendence'] = p[9]
    single_data['Referee'] = p[13]
    match_data.append(single_data.copy())
    single_data['team2'] = ''
    single_data["goal_scorer_t1"] = []
    single_data["goal_scorer_t2"] = []
    single_data['pen_att_t1'] = []
    single_data['pen_att_t2'] = []
    single_data['penalties_score'] = None
    

def p_handleHeader(p):
    '''handleHeader : OPENHEADER OPENHREF CONTENT CLOSEHREF CLOSEHEADER handleHeader
                    | OPENHEADER OPENHREF CONTENT CONTENT CLOSEHREF skiptag CLOSEHEADER handleHeader
                    | OPENHEADER skiptag CLOSEHEADER handleHeader
                    |'''
    if len(p) == 7:
        if str(p[3]) == "Penalties":
            single_data['penalties_score'] = ''
        elif single_data.get("team2", "") == "":
            single_data["team2"] = p[3]
            # print("team2 " + p[3])
        else:
            single_data["team1"] = p[3]
            # print("team1 " + p[3])
    if len(p) == 9:
        single_data["score"] = [int(p[3]), int(p[4])]
        # print("score " + p[3] + ' ' + p[4])
    

def p_handleliTeam2(p):
    '''handleliTeam2 : OPENLI skipcontent OPENHREF CONTENT CLOSEHREF skiptag CLOSELI handleliTeam2
                | '''
    
    if len(p) == 9:
        if single_data['penalties_score'] is not None:
            single_data['pen_att_t2'].append(p[4])
            # print('penalties attempt t2 ' + p[4])
        else: 
            single_data["goal_scorer_t2"].append(p[4])
            # print("goal scorer Team2 " + str(p[4]))
        


def p_handleliTeam1(p):
    '''handleliTeam1 : OPENLI skipcontent OPENHREF CONTENT CLOSEHREF skiptag CLOSELI handleliTeam1
                | '''
    
    if len(p) == 9:
        if single_data['penalties_score'] is not None:
            single_data['pen_att_t1'].append(p[4])
            # print('penalties attempt t1 ' + p[4])
        else: 
            single_data["goal_scorer_t1"].append(p[4])
            # print("goal scorer Team1 " + str(p[4]))
    


def p_handleData(p):
    '''handleData : OPENDATA skiptag CLOSEDATA 
                  | OPENHEADER CONTENT CONTENT CLOSEHEADER '''
    if len(p) == 5:
        single_data['penalties_score'] = p[2] + ' ' + p[3]
        # print('Penalties ' + p[2] + ' ' + p[3])


def p_handlerow(p):
    '''handlerow : OPENROW handleHeader CLOSEROW handlerow
                 | OPENROW OPENDATA skiptag handleliTeam1 CLOSEDATA handleData OPENDATA skiptag handleliTeam2 CLOSEDATA skiptag CLOSEROW handlerow
                 | '''

def p_handledatetime(p):
    '''handledatetime : OPENTIME CONTENT CONTENT CONTENT skiptag CLOSETIME'''
    if len(p) == 7:
        single_data["date"] = p[2] + ' ' + p[3] + ' ' + p[4]
        # print('================================================================')
        # print()
        # print('data: ' + p[2] + ' ' + p[3] + ' ' + p[4])

def p_root(p):
    '''root : BEGINGROUPA skipall BEGINROF16 skiptag handledatetime OPENTABLE handlerow CLOSETABLE handleinfo skiptag root
             | handledatetime OPENTABLE handlerow CLOSETABLE handleinfo skiptag root
             |'''
    

def p_error(p):
    pass
    

parser = yacc.yacc()  
parser.parse(data)
# print(match_data)
