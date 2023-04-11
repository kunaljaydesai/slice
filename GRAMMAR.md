# Slice Grammar

PROGRAM: [FUNCTION]*
FUNCTION: tok_def tok_identifier tok_lpar [FUNCTION_ARG]* tok_rpar tok_lbrak [BODY]* tok_rbrak
FUNCTION_ARG: tok_identifier | tok_identifier tok_comma
BODY: [CONDITIONAL | DEFINITION | RETURN_STATEMENT]

CONDITIONAL: IF_STATEMENT | IF_ELSE_STATEMENT
IF_STATEMENT: tok_if tok_lpar [EXPRESSION] tok_rpar tok_lbrak [BODY]* tok_rbrak 
IF_ELSE_STATEMENT: IF_STATEMENT tok_else tok_lbrak [BODY]* tok_rbrak 

DEFINITION: tok_identifier tok_equals EXPRESSION
RETURN_STATEMENT: tok_return EXPRESSION
EXPRESSION: [IDENTIFIER_EXPRESSION | PAREN_EXPRESSION | tok_number] [BIN_OP_EXPRESSION]?
BIN_OP_EXPRESSION: [tok_add | tok_sub] EXPRESSION | MULT_EXPRESSION
MULT_EXPRESSION: [tok_mult | tok_div] EXPRESSION | MOD_EXPRESSION
MOD_EXPRESSION: [tok_mod] EXPRESSION
IDENTIFIER_EXPRESSION = tok_identifier | tok_identifier tok_lpar EXPRESSION tok_rpar
PAREN_EXPRESSION = tok_lpar EXPRESSION tok_rpar
