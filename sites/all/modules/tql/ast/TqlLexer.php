<?php
class TqlLexer
{
    private $data;
    private $N;
    public $token;
    public $value;
    public $node;
    private $line;
    private $state = 1;

    function __construct($data)
    {
        $this->data = $data;
        $this->N = 0;
        $this->line = 1;
    }

    function init(&$node)
    {
        $node->value = $this->value;
        $node->type = $this->token;
        $node->column = $this->N;
        $node->line = $this->line;
        return $node;
    }


    private $_yy_state = 1;
    private $_yy_stack = array();

    function yylex()
    {
        return $this->{'yylex' . $this->_yy_state}();
    }

    function yypushstate($state)
    {
        array_push($this->_yy_stack, $this->_yy_state);
        $this->_yy_state = $state;
    }

    function yypopstate()
    {
        $this->_yy_state = array_pop($this->_yy_stack);
    }

    function yybegin($state)
    {
        $this->_yy_state = $state;
    }



    function yylex1()
    {
        $tokenMap = array (
              1 => 0,
              2 => 0,
              3 => 0,
              4 => 0,
              5 => 0,
              6 => 0,
              7 => 0,
              8 => 0,
              9 => 0,
              10 => 0,
            );
        if ($this->N >= strlen($this->data)) {
            return false; // end of input
        }
        $yy_global_pattern = "/^(\\b[nN][oO][tT]\\b|-)|^(\\b[aA][nN][dD]\\b)|^(\\b[oO][rR]\\b)|^(\\b[xX][oO][rR]\\b)|^(\\()|^(\\))|^(\")|^([^\"\s()]+)|^(\\s)|^(.)/";

        do {
            if (preg_match($yy_global_pattern, substr($this->data, $this->N), $yymatches)) {
                $yysubmatches = $yymatches;
                $yymatches = array_filter($yymatches, 'strlen'); // remove empty sub-patterns
                if (!count($yymatches)) {
                    throw new Exception('Error: lexing failed because a rule matched' .
                        'an empty string.  Input "' . substr($this->data,
                        $this->N, 5) . '... state START');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
                if ($tokenMap[$this->token]) {
                    // extract sub-patterns for passing to lex function
                    $yysubmatches = array_slice($yysubmatches, $this->token + 1,
                        $tokenMap[$this->token]);
                } else {
                    $yysubmatches = array();
                }
                $this->value = current($yymatches); // token value
                $r = $this->{'yy_r1_' . $this->token}($yysubmatches);
                if ($r === null) {
                    $this->N += strlen($this->value);
                    $this->line += substr_count("\n", $this->value);
                    // accept this token
                    return true;
                } elseif ($r === true) {
                    // we have changed state
                    // process this token in the new state
                    return $this->yylex();
                } elseif ($r === false) {
                    $this->N += strlen($this->value);
                    $this->line += substr_count("\n", $this->value);
                    if ($this->N >= strlen($this->data)) {
                        return false; // end of input
                    }
                    // skip this token
                    continue;
                } else {                    $yy_yymore_patterns = array(
        1 => "^(\\b[aA][nN][dD]\\b)|^(\\b[oO][rR]\\b)|^(\\b[xX][oO][rR]\\b)|^(\\()|^(\\))|^(\")|^([^\"\s()]+)|^(\\s)|^(.)",
        2 => "^(\\b[oO][rR]\\b)|^(\\b[xX][oO][rR]\\b)|^(\\()|^(\\))|^(\")|^([^\"\s()]+)|^(\\s)|^(.)",
        3 => "^(\\b[xX][oO][rR]\\b)|^(\\()|^(\\))|^(\")|^([^\"\s()]+)|^(\\s)|^(.)",
        4 => "^(\\()|^(\\))|^(\")|^([^\"\s()]+)|^(\\s)|^(.)",
        5 => "^(\\))|^(\")|^([^\"\s()]+)|^(\\s)|^(.)",
        6 => "^(\")|^([^\"\s()]+)|^(\\s)|^(.)",
        7 => "^([^\"\s()]+)|^(\\s)|^(.)",
        8 => "^(\\s)|^(.)",
        9 => "^(.)",
        10 => "",
    );

                    // yymore is needed
                    do {
                        if (!strlen($yy_yymore_patterns[$this->token])) {
                            throw new Exception('cannot do yymore for the last token');
                        }
                        if (preg_match($yy_yymore_patterns[$this->token],
                              substr($this->data, $this->N), $yymatches)) {
                            $yymatches = array_filter($yymatches, 'strlen'); // remove empty sub-patterns
                            next($yymatches); // skip global match
                            $this->token = key($yymatches); // token number
                            $this->value = current($yymatches); // token value
                            $this->line = substr_count("\n", $this->value);
                        }
                    	$r = $this->{'yy_r1_' . $this->token}();
                    } while ($r !== null || !$r);
			        if ($r === true) {
			            // we have changed state
			            // process this token in the new state
			            return $this->yylex();
			        } else {
	                    // accept
	                    $this->N += strlen($this->value);
	                    $this->line += substr_count("\n", $this->value);
	                    return true;
			        }
                }
            } else {
                throw new Exception('Unexpected input at line' . $this->line .
                    ': ' . $this->data[$this->N]);
            }
            break;
        } while (true);

    } // end function


    const START = 1;
    function yy_r1_1($yy_subpatterns)
    {

  $this->token = TqlParser::TK_NOT;
  $this->node = $this->init(new TqlNot());
    }
    function yy_r1_2($yy_subpatterns)
    {

  $this->token = TqlParser::TK_AND;
  $this->node = $this->init(new TqlAnd());
    }
    function yy_r1_3($yy_subpatterns)
    {

  $this->token = TqlParser::TK_OR;
  $this->node = $this->init(new TqlOr());
    }
    function yy_r1_4($yy_subpatterns)
    {

  $this->token = TqlParser::TK_XOR;
  $this->node = $this->init(new TqlXor());
    }
    function yy_r1_5($yy_subpatterns)
    {

  $this->token = TqlParser::TK_LPAREN;
  $this->node = $this->init(new TqlNode());
    }
    function yy_r1_6($yy_subpatterns)
    {

  $this->token = TqlParser::TK_RPAREN;
  $this->node = $this->init(new TqlNode());
    }
    function yy_r1_7($yy_subpatterns)
    {

  $this->token = TqlParser::TK_QUOTE;
  $this->node = $this->init(new TqlNode());
  $this->yybegin(self::IN_QUOTE);
    }
    function yy_r1_8($yy_subpatterns)
    {

  $this->token = TqlParser::TK_IDENTIFIER;
  $this->node = $this->init(new TqlIdentifier());
    }
    function yy_r1_9($yy_subpatterns)
    {

  return false;
    }
    function yy_r1_10($yy_subpatterns)
    {

  return false;
    }


    function yylex2()
    {
        $tokenMap = array (
              1 => 0,
              2 => 0,
            );
        if ($this->N >= strlen($this->data)) {
            return false; // end of input
        }
        $yy_global_pattern = "/^(\")|^([^\"]+)/";

        do {
            if (preg_match($yy_global_pattern, substr($this->data, $this->N), $yymatches)) {
                $yysubmatches = $yymatches;
                $yymatches = array_filter($yymatches, 'strlen'); // remove empty sub-patterns
                if (!count($yymatches)) {
                    throw new Exception('Error: lexing failed because a rule matched' .
                        'an empty string.  Input "' . substr($this->data,
                        $this->N, 5) . '... state IN_QUOTE');
                }
                next($yymatches); // skip global match
                $this->token = key($yymatches); // token number
                if ($tokenMap[$this->token]) {
                    // extract sub-patterns for passing to lex function
                    $yysubmatches = array_slice($yysubmatches, $this->token + 1,
                        $tokenMap[$this->token]);
                } else {
                    $yysubmatches = array();
                }
                $this->value = current($yymatches); // token value
                $r = $this->{'yy_r2_' . $this->token}($yysubmatches);
                if ($r === null) {
                    $this->N += strlen($this->value);
                    $this->line += substr_count("\n", $this->value);
                    // accept this token
                    return true;
                } elseif ($r === true) {
                    // we have changed state
                    // process this token in the new state
                    return $this->yylex();
                } elseif ($r === false) {
                    $this->N += strlen($this->value);
                    $this->line += substr_count("\n", $this->value);
                    if ($this->N >= strlen($this->data)) {
                        return false; // end of input
                    }
                    // skip this token
                    continue;
                } else {                    $yy_yymore_patterns = array(
        1 => "^([^\"]+)",
        2 => "",
    );

                    // yymore is needed
                    do {
                        if (!strlen($yy_yymore_patterns[$this->token])) {
                            throw new Exception('cannot do yymore for the last token');
                        }
                        if (preg_match($yy_yymore_patterns[$this->token],
                              substr($this->data, $this->N), $yymatches)) {
                            $yymatches = array_filter($yymatches, 'strlen'); // remove empty sub-patterns
                            next($yymatches); // skip global match
                            $this->token = key($yymatches); // token number
                            $this->value = current($yymatches); // token value
                            $this->line = substr_count("\n", $this->value);
                        }
                    	$r = $this->{'yy_r2_' . $this->token}();
                    } while ($r !== null || !$r);
			        if ($r === true) {
			            // we have changed state
			            // process this token in the new state
			            return $this->yylex();
			        } else {
	                    // accept
	                    $this->N += strlen($this->value);
	                    $this->line += substr_count("\n", $this->value);
	                    return true;
			        }
                }
            } else {
                throw new Exception('Unexpected input at line' . $this->line .
                    ': ' . $this->data[$this->N]);
            }
            break;
        } while (true);

    } // end function


    const IN_QUOTE = 2;
    function yy_r2_1($yy_subpatterns)
    {

  $this->token = TqlParser::TK_QUOTE;
  $this->node = $this->init(new TqlNode());
  $this->yybegin(self::START);
    }
    function yy_r2_2($yy_subpatterns)
    {

  $this->token = TqlParser::TK_IDENTIFIER;
  $this->node = $this->init(new TqlIdentifier());
    }

}
