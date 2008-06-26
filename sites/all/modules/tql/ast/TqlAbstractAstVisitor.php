<?php

// Basic structure: Identifiers, UnaryOperation and BinaryOperation
class TqlNodeVisitor
{
	// basic Visitor
	function visitIdentifier($tqlIdentifier)
	{}

	function visitUnaryOperation($tqlUnaryOperation)
	{}

	function visitBinaryOperation($tqlBinaryOperation)
	{}
}

// Operations: Not, And, Or, Xor
class TqlOperationVisitor extends TqlNodeVisitor
{
	function visitNot($tqlNot)
	{}

	function visitAnd($tqlAnd)
	{}

	function visitOr($tqlOr)
	{}

	function visitXor($tqlXor)
	{}	
}


// Basic AST Visitor
class TqlASTVisitor extends TqlOperationVisitor
{
}


// Nodes
class TqlNode
{
	// fields
	public $type;
	public $value;
	public $column;
	public $line;
	public $previous;
	public $next;
	
	function accept ($nodeVisitor)
	{}
}
class TqlIdentifier extends TqlNode
{
	function accept ($nodeVisitor)
	{
		return $nodeVisitor->visitIdentifier($this);
	}		
}

class TqlOperation extends TqlNode
{
	function acceptOperation ($operationVisitor)
	{}

}

class TqlUnaryOperation extends TqlOperation
{
	//child
	public $operand;

	// do not overwrite this function
	function accept ($nodeVisitor)
	{
		return $nodeVisitor->visitUnaryOperation($this);
	}
}

class TqlBinaryOperation extends TqlOperation
{
	// children
	public $left;
	public $right;

	// do not overwrite this function
	function accept ($nodeVisitor)
	{
		return $nodeVisitor->visitBinaryOperation($this);
	}
}


class TqlNot extends TqlUnaryOperation
{
	// do not overwrite this function
	function acceptOperation ($operationVisitor)
	{
		return $operationVisitor->visitNot($this);
	}
}

class TqlAnd extends TqlBinaryOperation
{
	// do not overwrite this function
	function acceptOperation ($operationVisitor)
	{
		return $operationVisitor->visitAnd($this);
	}
}

class TqlOr extends TqlBinaryOperation
{
	// do not overwrite this function
	function acceptOperation ($operationVisitor)
	{
		return $operationVisitor->visitOr($this);
	}
}

class TqlXor extends TqlBinaryOperation
{
	// do not overwrite this function
	function acceptOperation ($operationVisitor)
	{
		return $operationVisitor->visitXor($this);
	}
}
