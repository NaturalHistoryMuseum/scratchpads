<?php

/*
 * Visits an AST and inserts every identifier into the terms array.
 * The key will be the term, the value is 'null' or a list of term objects holding information like the term-id and the vocabulary.
 */

class TqlErrorFormatter extends TqlASTVisitor
{
	/* 
	 * How it works:
	 *  For every part of the AST we have available we print it in green.
         *  For the missing parts we take query itself as a source, cut the part out and print it in red.
	 */

	// this is the index of how far we have processed the query. points to the first unconsumed char of the rest of the query.
	private $consumedIndex=0;

	private $result = "";

	private $query;

	private function dump($node)
	{
		$value = $node->value;
		if ($value)
		{
			$start = $node->column;
			$end = $start + strlen($value);

			if ($this->consumedIndex < $start)
			{
				$error = substr($this->query,$this->consumedIndex, $start - $this->consumedIndex);	
				if (strlen($error) > 0)
				{	// there is text not recognized by the parser.
					if (trim($error))
					{	// we have real content missing -> mark it red
						$this->result .= '<b style="color: red;">'.$error.'</b>';
					}
					else
					{	// let's just add a space
						$this->addSpace();
					}
				}
			}
			$this->consumedIndex = $end;
			$this->result .= '<b style="color: green;">'.$value.'</b>';
		}	
	}

	private function dumpWithSpace($node)
	{
		$this->result.=' ';
		$this->dump($node);
		$this->result.=' ';
	}

	private function dumpRemaining()
	{
		$error = substr($this->query, $this->consumedIndex);
		if ($error != "")
		{
			$this->result .= '<b style="color: red;">'.$error.'</b>';
		}
	}
	
	private function addSpace()
	{
		$this->result .= ' ';
	}

	function error($tqlASTParts, $originalQuery)
	{	// init the state
		$this->consumedIndex = 0;
		$this->result = "";
		$this->query = $originalQuery;
		foreach ($tqlASTParts as $ast)
		{
			$ast->accept ($this);
		}
		$this->dumpRemaining();

		return $this->result;
	}

/*
 * Visitor functions
 */
	function printAndAccept($node)
	{
		if ($node->previous)
			$this->dump($node->previous);

		$node->accept($this);

		if ($node->next)
			$this->dump($node->next);
	}


	function visitIdentifier($tqlIdentifier)
	{
		$this->dump ($tqlIdentifier);
	}

	function visitUnaryOperation($tqlUnaryOperation)
	{
		// switch to operation visitor
		// check for null as operand can be null in error case
		if ($tqlUnaryOperation->operand)
		{
			return $tqlUnaryOperation->acceptOperation($this);
		}
	}

	function visitBinaryOperation($tqlBinaryOperation)
	{
		// switch to operation visitor
		return $tqlBinaryOperation->acceptOperation($this);
	}

	function visitNot($tqlNot)
	{
		$this->dump ($tqlNot);
		$operand = $tqlNot->operand->accept($this);
	}

	function visitAnd($tqlAnd)
	{
		$left = $this->printAndAccept($tqlAnd->left);
		$this->dump($tqlAnd);
		$right =  $this->printAndAccept($tqlAnd->right);
	}

	function visitOr($tqlOr)
	{
		$left =  $this->printAndAccept($tqlOr->left);
		$this->dump($tqlOr);
		$right = $this->printAndAccept($tqlOr->right);
	}

	function visitXor($tqlXor)
	{
		$left = $tqlXor->left->accept($this);
		$this->dump($tqlXor);
		$right = $tqlXor->right->accept($this);
	}
}
