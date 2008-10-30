<?php

/*
 * Visits an AST and inserts every identifier into the terms array.
 * The key will be the term, the value how many time it occurs in the AST of the query.
 */

class TqlAstDumper extends TqlASTVisitor
{

	function dump($tqlAST)
	{
		$this->terms = $terms;
		return $tqlAST->accept ($this);
	}

/*
 * Visitor functions
 */


	function visitIdentifier($tqlIdentifier)
	{
		if (strpos ($tqlIdentifier->value," ") !== false){
			// we have at least one space. let's add quotes!
			return '"' . $tqlIdentifier->value . '"';
		} 
		else {
			return $tqlIdentifier->value;
		}
	}

	function putInBrackets($string)
	{
		return '(' . $string .')';
	}

	function visitUnaryOperation($tqlUnaryOperation)
	{
		// switch to operation visitor
		return $tqlUnaryOperation->acceptOperation($this);
	}

	function visitBinaryOperation($tqlBinaryOperation)
	{
		// switch to operation visitor
		return $tqlBinaryOperation->acceptOperation($this);
	}

	function visitNot($tqlNot)
	{
		$operand = $tqlNot->operand->accept($this);
		// proper bracketing
		if ($tqlNot->operand->type != TqlParser::TK_IDENTIFIER)
			$operand = $this->putInBrackets ($operand);

		return '<i>not</i> ' . $operand;
	}

	function visitAnd($tqlAnd)
	{
		$left = $tqlAnd->left->accept($this);
		$right = $tqlAnd->right->accept($this);
		// proper bracketing:
		if ($tqlAnd->left->type == TqlParser::TK_OR or $tqlAnd->left->type == TqlParser::TK_XOR)
			$left = $this->putInBrackets ($left);

		if ($tqlAnd->right->type == TqlParser::TK_OR or $tqlAnd->right->type == TqlParser::TK_XOR)
			$right = $this->putInBrackets ($right);

		return $left . ' <i>and</i> ' . $right;
	}

	function visitOr($tqlOr)
	{
		$left = $tqlOr->left->accept($this);
		$right = $tqlOr->right->accept($this);

		return $left . ' <i>or</i> ' . $right;
	}

	function visitXor($tqlXor)
	{
		$left = $tqlXor->left->accept($this);
		$right = $tqlXor->right->accept($this);
		// proper bracketing:
		if ($tqlXor->left->type == TqlParser::TK_OR or $tqlXor->left->type == TqlParser::TK_AND)
			$left = $this->putInBrackets ($left);

		if ($tqlXor->right->type == TqlParser::TK_OR or $tqlXor->right->type == TqlParser::TK_AND)
			$right = $this->putInBrackets ($right);


		return $left . ' <i>xor</i> ' . $right;		
	}
}
