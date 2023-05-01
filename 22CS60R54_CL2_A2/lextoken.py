import ply.lex as lex


###DEFINING TOKENS###

tokens = ('BEGINTABLE', 'BEGINTEAMNAME', 'BEGINGROUPA', 'BEGINGROUPB','BEGINGROUPC',
'BEGINGROUPD','BEGINGROUPE','BEGINGROUPF','BEGINGROUPG','BEGINGROUPH', 'BEGINKNOCKOUT', 
'BEGINROF16', 'BEGINQF', 'BEGINSF', 'BEGINTPPO', 'BEGINFINALS','BEGINAWARD', 'BEGINCURSQUAD',
'OPENTABLE', 'CLOSETABLE', 'OPENROW', 'CLOSEROW',
'OPENHEADER', 'CLOSEHEADER', 'OPENHREF', 'CLOSEHREF',
'CONTENT', 'OPENDATA', 'CLOSEDATA' ,'OPENSPAN',
'CLOSESPAN', 'OPENDIV', 'CLOSEDIV', 'OPENSTYLE', 'CLOSESTYLE', 'OPENSUP', 'CLOSESUP', 
'OPENLI', 'CLOSELI', 'CODEDCHAR' , 'OPENTIME', 'CLOSETIME','GARBAGE')
t_ignore = '\t'


###############Tokenizer Rules################

def t_BEGINTABLE(t):
    '''<h3><span\sclass="mw-headline"\sid="Stadiums">Stadiums</span></h3>'''
    return t

def t_BEGINTEAMNAME(t):
    '''<h2><span\sclass="mw-headline"\sid="Teams">Teams</span></h2>'''
    return t

def t_BEGINGROUPA(t):
    '''<h3><span\sclass="mw-headline"\sid="Group_A">Group\sA</span></h3>'''
    return t

def t_BEGINGROUPB(t):
    '''<h3><span\sclass="mw-headline"\sid="Group_B">Group\sB</span></h3>'''
    return t

def t_BEGINGROUPC(t):
    '''<h3><span\sclass="mw-headline"\sid="Group_C">Group\sC</span></h3>'''
    return t

def t_BEGINGROUPD(t):
    '''<h3><span\sclass="mw-headline"\sid="Group_D">Group\sD</span></h3>'''
    return t

def t_BEGINGROUPE(t):
    '''<h3><span\sclass="mw-headline"\sid="Group_E">Group\sE</span></h3>'''
    return t

def t_BEGINGROUPF(t):
    '''<h3><span\sclass="mw-headline"\sid="Group_F">Group\sF</span></h3>'''
    return t

def t_BEGINGROUPG(t):
    '''<h3><span\sclass="mw-headline"\sid="Group_G">Group\sG</span></h3>'''
    return t

def t_BEGINGROUPH(t):
    '''<h3><span\sclass="mw-headline"\sid="Group_H">Group\sH</span></h3>'''
    return t

def t_BEGINKNOCKOUT(t):
    '''<h2><span\sclass="mw-headline"\sid="Knockout_stage">Knockout\sstage</span></h2>'''
    return t

def t_BEGINROF16(t):
    '''<h3><span\sclass="mw-headline"\sid="Round_of_16">Round\sof\s16</span></h3>'''
    return t

def t_BEGINQF(t):
    '''<h3><span\sclass="mw-headline"\sid="Quarter-finals">Quarter-finals</span></h3>'''
    return t

def t_BEGINSF(t):
    '''<h3><span\sclass="mw-headline"\sid="Semi-finals">Semi-finals</span></h3>'''
    return t

def t_BEGINTPPO(t):
    '''<h3><span\sclass="mw-headline"\sid="Third_place_play-off">Third\splace\splay-off</span></h3>'''
    return t

def t_BEGINFINALS(t):
    '''<h3><span\sclass="mw-headline"\sid="Final">Final</span></h3>'''
    return t

def t_BEGINAWARD(t):
    '''<h2><span\sclass="mw-headline"\sid="Awards">Awards</span></h2>'''
    return t

def t_BEGINCURSQUAD(t):
    '''<span\sclass="mw-headline"\sid="Current_squad">Current\ssquad</span>'''
    return t

def t_OPENTABLE(t):
    '''<tbody[^>]*>'''
    return t

def t_CLOSETABLE(t):
    '''</tbody[^>]*>'''
    return t

def t_OPENROW(t):
    '''<tr[^>]*>'''
    return t

def t_CLOSEROW(t):
    '''</tr[^>]*>'''
    return t

def t_OPENHEADER(t):
    '''<th[^>]*>'''
    return t

def t_CLOSEHEADER(t):
    '''</th[^>]*>'''
    return t

def t_OPENHREF(t):
    '''<a[^>]*>'''
    return t

def t_CLOSEHREF(t):
    '''</a[^>]*>'''
    return t

def t_OPENDATA(t):
    '''<td[^>]*>'''
    return t

def t_CLOSEDATA(t):
    '''</td[^>]*>'''
    return t

def t_CONTENT(t):
    '''[A-Za-z0-9,éúčćáãíÁńšüōğï\-. ]+'''
    # àâçéèêëîïôûùüÿñæœ
    return t


def t_OPENSUP(t):
    '''<sup[^>]*>'''
    return t

def t_CLOSESUP(t):
    '''</sup>'''
    return t

def t_OPENLI(t):
    '''<li[^n>]*>'''
    return t

def t_CLOSELI(t):
    '''</li>'''
    return t

def t_OPENTIME(t):
    '''<time>'''
    return t

def t_CLOSETIME(t):
    '''</time>'''
    return t

def t_CODEDCHAR(t):
    '''\&\#[0-9]+;'''

def t_OPENDIV(t):
    '''<div[^>]*>'''

def t_CLOSEDIV(t):
    '''</div[^>]*>'''

def t_OPENSTYLE(t):
    '''<style[^>]*>'''

def t_CLOSESTYLE(t):
    '''</style[^>]*>'''

def t_OPENSPAN(t):
    '''<span[^>]*>'''

def t_CLOSESPAN(t):
    '''</span[^>]*>'''


def t_GARBAGE(t):
    r'<[^>]*>'

def t_error(t):
    t.lexer.skip(1)


#########################################################################################
#Fill with parsing rules
#########################################################################################
#########DRIVER FUNCTION#######
lexer = lex.lex()