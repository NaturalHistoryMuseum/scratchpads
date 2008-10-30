<?php
/*
 * Visits an AST and inserts every identifier into the terms array.
 * The key will be the term, the value how many time it occurs in the AST of the query.
 */

class TqlMySqlGenerator extends TqlASTVisitor
{
	public $nonExistentTags = array();

	private $terms = array();

	private $termTableName = "";

	private $tableNumber = 0;

	function generate($tqlAST, $terms, $termTableName)
	{
		$this->terms = $terms;
		$this->termTableName = $termTableName;
		$result = $tqlAST->accept ($this);
		if ($result)
			return "SELECT * FROM " .  $result . " AS finalTable";
		else
			return null;
	}

/*
 * Visitor functions
 */


	function visitIdentifier($tqlIdentifier)
	{
		$termIds = $this->terms[$tqlIdentifier->value];
		// in case the term has not been found it was set to 'null' by the 'TqlNameToTid' visitor.
		if (is_null($termIds))
		{
			// term is not in vocabulary
			return null;			
		}
		else
		{
			$termList = implode(',', $termIds);
			return "(SELECT DISTINCT nid FROM `$this->termTableName` WHERE tid in ($termList))";
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

		if (is_null($operand))
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
		if (is_null($left) || is_null($right))
			return null;

		//Original SQL: return "($left INTERSECT DISTINCT $right)";
		$this->tableNumber++;
		return "(SELECT DISTINCT left$this->tableNumber.nid FROM $left AS left$this->tableNumber, $right AS right$this->tableNumber WHERE left$this->tableNumber.nid  = right$this->tableNumber.nid)";
	}

	function visitOr($tqlOr)
	{
		$left = $tqlOr->left->accept($this);
		$right = $tqlOr->right->accept($this);

		// in this case it's impossible to have any result!
		if (is_null($left))
		{
			return $right; // might be null as well!
		}
		else if (is_null($right))
		{
			return $left; // might be null as well!
		}
		else
		{
			//Original SQL: return "($left UNION DISTINCT $right)";
			$this->tableNumber++;
			return "(SELECT * FROM $left AS left$this->tableNumber UNION SELECT * FROM $right AS right$this->tableNumber)";
		}		
	}

	function visitXor($tqlXor)
	{
		$left = $tqlXor->left->accept($this);
		$right = $tqlXor->right->accept($this);

		if (is_null($left))
		{
			return $right; // might be null as well!
		}
		else if (is_null($right))
		{
			return $left; // might be null as well!
		}
		else
		{
			//Original SQL: return "(($left EXCEPT DISTINCT $right) UNION ($right EXCEPT $left))";
			$this->tableNumber++;
			return "(SELECT * FROM (SELECT DISTINCT leftXor1.nid FROM (SELECT DISTINCT nid FROM `term_node` WHERE nid not in $left) AS leftXor1, $right AS rightXor1 WHERE leftXor1.nid  = rightXor1.nid) AS leftXor3 UNION SELECT * FROM (SELECT DISTINCT leftXor2.nid FROM (SELECT DISTINCT nid FROM `term_node` WHERE nid not in $right) AS leftXor2, $left AS rightXor2 WHERE leftXor2.nid  = rightXor2.nid) AS rightXor3)";
		}		
		
	}
}
