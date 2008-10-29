/* This is a lemon grammar for the Tag Query Language */
%name Tql_
%declare_class {class TqlParser}
%include_class
{
    // states whether the parse was successful or not
    public $successful = true;
    private $internalError = false;
    public $astParts = array();

    private function pushAST()
    {
        if ($this->_retvalue)
	    if (!in_array($this->_retvalue, $this->astParts))
	        array_push($this->astParts, $this->_retvalue);
    }
}

%token_prefix TK_

%parse_accept
{
    $this->ast = $this->_retvalue;
    $this->pushAst();
    $this->successful = !$this->internalError;
    $this->internalError = false;
}

%parse_failure
{
    $this->pushAst();
    $this->successful = !$this->internalError;
}

%syntax_error
{
    $this->internalError = true;
}

start(res) ::= expression(expr). { res = expr; }

primary_expression(res) ::= QUOTE(lquote) IDENTIFIER(ident) QUOTE(rquote). { res = ident; res->previous = lquote; res->next = rquote; }
primary_expression(res) ::= IDENTIFIER(ident). { res = ident; }
primary_expression(res) ::= LPAREN(lparen) expression(expr) RPAREN(rparen). { res = expr; res->previous = lparen; res->next = rparen; }

unary_expression(res) ::= primary_expression(expr) . { res = expr; }
unary_expression(res) ::= unary_operator(unaryOp) unary_expression(expr). { unaryOp->operand = expr;  res = unaryOp; }

unary_operator(res) ::= NOT(not). { res = not; }

and_expression(res) ::= unary_expression(expr). { res = expr; }
and_expression(res) ::= and_expression(leftExpr) unary_expression(rightExpr) . { res = new TqlAnd(); res->left = leftExpr; res->right = rightExpr; res->type = TqlParser::TK_AND; }
and_expression(res) ::= and_expression(leftExpr) AND(and) unary_expression(rightExpr). { res = and; and->left = leftExpr; and->right = rightExpr; }

exclusive_or_expression(res) ::= and_expression(expr). { res = expr; }
exclusive_or_expression(res) ::= exclusive_or_expression(leftExpr) XOR(xor) and_expression(rightExpr). { res = xor; xor->left = leftExpr; xor->right = rightExpr; }

logical_or_expression(res) ::= exclusive_or_expression(expr). { res = expr; }
logical_or_expression(res) ::= logical_or_expression(leftExpr) OR(or) exclusive_or_expression(rightExpr). { res = or; or->left = leftExpr; or->right = rightExpr; }

expression(res) ::= logical_or_expression(expr). { res = expr; }
