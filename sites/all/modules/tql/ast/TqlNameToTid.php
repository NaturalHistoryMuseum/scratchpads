<?php
/*
 * Visits an AST and inserts every identifier into the terms array.
 * The key will be the term, the value how many time it occurs in the AST of the query.
 */

class TqlNameToTid extends TqlNodeVisitor
{

	public $terms = array();

	public $missingTerms = array();


	/*
	 * Compute term-ids for all terms occuring in the AST.
	 * The result is stored in $this->terns with the terms as keys and the term-ids as values.
	 */
	function computeTermIDs ($tqlAST, $vocabularyList)
	{
		$tqlAST->accept ($this);
		foreach ($this->terms as $term => &$ids)
		{
			$result = taxonomy_get_term_by_name($term);
			if (count ($result) > 0)
			{
				$ids = array();
				foreach ($result as $termid)
				{
					if (is_null($vocabularyList) || in_array($termid->vid, $vocabularyList))
					{
						array_push($ids,$termid->tid);
					}
				}
				if (count($ids) == 0)
				{
					// if $ids is set to null the code generator knows what to do.
					$ids = null;
					// add the not found term to the missing terms list (used for error reporting)
					$this->addToMissing($term);
				}
			}
			else
			{
				// the term was not found at all
				$this->addToMissing($term);
			}		
		}
	}

	function addToMissing($term)
	{
		if (!in_array($term, $this->missingTerms))
			array_push($this->missingTerms, $term);
	}

/*
 * Visitor functions
 */

	function visitIdentifier($tqlIdentifier)
	{
		if (!isset($this->terms[$tqlIdentifier->value]))
		{
			$this->terms[$tqlIdentifier->value] = null;
		}
	}

	function visitUnaryOperation($tqlUnaryOperation)
	{
		// check for null as operand can be null in error case
		if ($tqlUnaryOperation->operand)
		{
			$tqlUnaryOperation->operand->accept($this);
		}
	}

	function visitBinaryOperation($tqlBinaryOperation)
	{
		$tqlBinaryOperation->left->accept($this);
		$tqlBinaryOperation->right->accept($this);
	}
}
