 # Yacc example
 
import ply.yacc as yacc

# Get the token map from the lexer.  This is required.
from lex import tokens

precedence = (
    ('left', 'PLUS', 'MINUS'),
    ('left', 'TIMES', 'DIVIDE', 'MOD'),
    ('right', 'EXP'),
    ('right', 'UMINUS'),            # Unary minus operator
)

variables = {}

def p_start_prefix(p):
    'start : PREFIX LPAREN prefix RPAREN'
    p[0] = p[3]

def p_start_assinment(p):
    'start : ID ASSIGN expression'
    variables[p[1]] = p[3]
    p[0] = p[3]

def p_start_expression(p):
    'start : expression'
    p[0] = p[1]


def p_prefix_num(p):
    'prefix : NUMBER PLUS NUMBER'
    p[0] = p[2] + str(p[1]) + str(p[3])
    
def p_start_equals(p):
    'expression : subexpression EQUALS subexpression'
    if p[1] == p[3]:
        p[0] = "Yes"
    else:
        p[0] = "No"


def p_exp_subexp(p):
    'expression : subexpression'
    p[0] = p[1]
    
def p_expression_plus(p):
    'subexpression : subexpression PLUS term'
    if p[1] is None or p[3] is None:
        p[0] = None
    else:
        p[0] = p[1] + p[3]

def p_expression_minus(p):
    'subexpression : subexpression MINUS term'

    if p[1] is None or p[3] is None:
        p[0] = None
    else:
        p[0] = p[1] - p[3]


def p_expression_term(p):
    'subexpression : term'
    p[0] = p[1]

def p_term_times(p):
    'term : term TIMES factor'
    p[0] = p[1] * p[3]

def p_term_div(p):
    'term : term DIVIDE factor'
    if p[1] is None or p[3] is None:
        p[0] = None
    elif p[3] == 0:
        p[0] = None
        print("Divide by zero error")
    else:
        p[0] = p[1] / p[3]

def p_term_mod(p):
    'term : term MOD factor'
    if p[1] is None or p[3] is None:
        p[0] = None
    elif p[3] == 0:
        p[0] = None
        print("Divide by zero error")
    else:
        p[0] = p[1] % p[3]


def p_term_factor(p):
    'term : factor'
    p[0] = p[1]

def p_pow_factor(p):
    'factor : pow EXP factor'
    p[0] = p[1] ** p[3]

def p_factor_pow(p):
    'factor : pow'
    p[0] = p[1]

def p_pow_num(p):
    'pow : NUMBER'
    p[0] = p[1]

def p_pow_id(p):
    'pow : ID'
    if p[1] not in variables:
        print(p[1] + " not defined")
        p[0] = None
    else:
        p[0] = variables[p[1]]

def p_expr_uminus(p):
    'pow : MINUS pow %prec UMINUS'
    p[0] = -p[2]


def p_pow_expr(p):
    'pow : LPAREN expression RPAREN'
    p[0] = p[2]

# Error rule for syntax errors
def p_error(p):
    print("Syntax error in input!")

# Build the parser
parser = yacc.yacc()

while True:
   try:
       s = input('calc > ')
   except EOFError:
       break
   if not s: continue
   result = parser.parse(s)
   print(result)