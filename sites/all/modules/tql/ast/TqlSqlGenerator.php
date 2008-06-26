<?php

THIS CODE WAS ACTUALLY NEVER TESTED

TAKE IT AS A STARTING POINT AND HAVE A LOOK AT THE GENERATOR FOR MYSQL

/*
 * Visits an AST and inserts every identifier into the terms array.
 * The key will be the term, the value how many time it occurs in the AST of the query.
 */

class TqlSqlGenerator extends TqlASTVisitor
{
	public $nonExistentTags = array();

	private $terms = array();

	private $termTableName = "";

	function generate($tqlAST, $terms, $termTableName)
	{
		$this->terms = $terms;
		$this->termTableName = $termTableName;
		return $tqlAST->accept ($this);
	}

/*
 * Visitor functions
 */


	function visitIdentifier($tqlIdentifier)
	{
		$termId = $this->terms[$tqlIdentifier->value];
		if ($termId == -1)
		{
			// term is not in vocabulary
			return null;			
		}
		else
		{
			return "(SELECT DISTINCT nid FROM `$this->termTableName` WHERE tid=$termId)";
		}
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

		if ($operand == null)
		{
			// in this case we return the whole universe... :-)
			return "(SELECT DISTINCT nid FROM `$this->termTableName`)";
		}
		else
		{
			return "(SELECT DISTINCT nid FROM `$this->termTableName` WHERE nid not in $operand)";
		}
	}

	function visitAnd($tqlAnd)
	{
		$left = $tqlAnd->left->accept($this);
		$right = $tqlAnd->right->accept($this);

		// in this case it's impossible to have any result!
		if ($left == null || $right == null)
			return null;

		return "($left INTERSECT DISTINCT $right)";
	}

	function visitOr($tqlOr)
	{
		$left = $tqlOr->left->accept($this);
		$right = $tqlOr->right->accept($this);

		// in this case it's impossible to have any result!
		if ($left == null)
		{
			return $right; // might be null as well!
		}
		else if ($right == null)
		{
			return $left; // might be null as well!
		}
		else
		{
			return "($left UNION DISTINCT $right)";			
		}		
	}

	function visitXor($tqlXor)
	{
		$left = $tqlXor->left->accept($this);
		$right = $tqlXor->right->accept($this);

		if ($left == null)
		{
			return $right; // might be null as well!
		}
		else if ($right == null)
		{
			return $left; // might be null as well!
		}
		else
		{
			return "(($left EXCEPT DISTINCT $right) UNION ($right EXCEPT $left))";			
		}		
		
	}
}
