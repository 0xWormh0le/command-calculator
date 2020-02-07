#include "Tree.h"

#include <string.h>

CharType getType(char ch)
{
	switch (ch)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return Digit;
		break;
	case '+':
	case '-':
		return PlusMinus;
		break;
	case '/':
	case '*':
		return MulDiv;
		break;
	case '(':
		return OpenBrac;
		break;
	case ')':
		return CloseBrac;
		break;
	default:
		return Etc;
		break;
	}
}

char* eatspace(char* src)
{
	int len = strlen(src);
	char* result = new char[len];
	int cnt = 0;
	for (int i = 0; i < len; i++)
	{
		if (src[i] != ' ')
		{
			result[cnt++] = src[i];
		}
	}
	result[cnt] = 0;
	return result;
}

Tree::Tree(char* expression, bool bracket_init)
{
	m_ptr = 0;
	m_root = 0;
	m_lastNode = 0;
	m_pendingOperator = 0;
	m_initByBracket = bracket_init;
	m_lastInputType = Etc;
	this->m_expression = eatspace(expression);
	this->Make();
}


Tree::~Tree()
{
	this->RemoveNode(m_root);
	m_root = 0;
}

int Tree::Evaluate()
{
	m_result = this->EvaluateNode(m_root);
	return m_errcode;
}

int Tree::EvaluateNode(Node* node)
{
	if (node == 0)
	{
		this->OnError(10);			// Malformed node
		return -1;
	}
	CharType ct = getType(node->m_value);
	if (ct == Digit)
	{
		return node->m_value - '0';
	}
	else
	{
		switch (node->m_value)
		{
		case '+':
			return EvaluateNode(node->m_leftChild) + EvaluateNode(node->m_rightChild);
			break;
		case '-':
			return EvaluateNode(node->m_leftChild) - EvaluateNode(node->m_rightChild);
			break;
		case '*':
			return EvaluateNode(node->m_leftChild) * EvaluateNode(node->m_rightChild);
			break;
		case '/':
			return EvaluateNode(node->m_leftChild) / EvaluateNode(node->m_rightChild);
			break;
		}
	}
}

void Tree::RemoveNode(Node* node)
{
	if (!node)
		return;
	if (node->m_leftChild != 0)
	{
		this->RemoveNode(node->m_leftChild);
		node->m_leftChild = 0;
	}
	if (node->m_rightChild != 0)
	{
		this->RemoveNode(node->m_rightChild);
		node->m_rightChild = 0;
	}
	delete node;
}

int Tree::Make()
{
	int len = strnlen_s(this->m_expression, 10000);
	while (m_ptr < len)
	{
		char ch = m_expression[m_ptr];
		CharType ct = getType(ch);
		switch (ct)
		{
		case Digit:
			if (!this->OnDigit(ch)) {
				return -1;
			}
			break;
		case PlusMinus:
			if (!this->OnPlusOrMinus(ch))
			{
				return -1;
			}
			break;
		case MulDiv:
			if (!this->OnMulOrDiv(ch))
			{
				return -1;
			}
			break;
		case OpenBrac:
			this->OnOpenBracket();
			break;
		case CloseBrac:
			if (!this->OnCloseBracket())
			{
				return -1;
			}
			m_ptr++;
			return 0;
		default:
			this->OnError(-1);			// wrong literal
			return -1;
			break;
		}
		m_ptr++;
	}
	
	return 0;
}

void Tree::MergeTree(Tree* tree) // merge according to pending operand
{
	this->m_ptr += tree->m_ptr;
	if (this->m_lastNode) // braced expression came after an operator, and root is the operator
	{
		this->m_lastNode->m_rightChild = tree->m_root;
		tree->m_root->m_parent = this->m_lastNode;
	}
	else // braced expression is the tree
	{
		m_root = tree->m_root;
		this->m_lastNode = m_root;
		this->m_pendingOperator = 0;				// this is just like a digit node
		m_lastInputType = Digit;
	}
}

Node* Tree::NewNode()
{
	Node* node = new Node;
	node->m_leftChild = 0;
	node->m_parent = 0;
	node->m_rightChild = 0;
	return node;
}

bool Tree::OnPlusOrMinus(char op)
{
	if (m_lastNode == 0)
	{
		this->OnError(2); // first literal should be digit.
		return false;
	}
	else
	{
		if (m_lastInputType == Digit) // we have operand, good to store operator
		{
			Node* node = Tree::NewNode();
			node->m_value = op;
			node->m_leftChild = m_root;
			m_root->m_parent = node;
			m_root = node;
			m_lastNode = node;
			m_pendingOperator = op;
			this->m_pendingOperator = '+';
		}
		else
		{
			this->OnError(3); // operator should come after operand
			return false;
		}
	}
	m_lastInputType = PlusMinus;
	return true;
}

bool Tree::OnMulOrDiv(char op)
{
	if (m_lastNode == 0)
	{
		this->OnError(2); // first literal should be digit.
		return false;
	}
	else
	{
		if (m_lastInputType == Digit) // we have operand, good to store operator
		{
			Node* node = Tree::NewNode();
			node->m_value = op;
			if (this->m_pendingOperator == '+') 
			{
				node->m_leftChild = m_lastNode;
				node->m_parent = m_lastNode->m_parent;
				if (node->m_parent) 
				{
					node->m_parent->m_rightChild = node;
				}
				m_lastNode->m_parent = node;
				
			}
			else if (this->m_pendingOperator == '*') 
			{
				Node* mNode = m_lastNode->m_parent;
				node->m_leftChild = mNode;
				node->m_parent = mNode->m_parent;
				if (node->m_parent) 
				{
					node->m_parent->m_rightChild = node;
				}
				mNode->m_parent = node;
			}
			else
			{
				node->m_leftChild = m_root;
				m_root->m_parent = node;
				m_root = node;
			}
			
			this->m_pendingOperator = '*';
			m_lastNode = node;
		}
		else
		{
			this->OnError(3); // operator should come after digit
			return false;
		}
	}
	m_lastInputType = MulDiv;
	return true;
}

bool Tree::OnOpenBracket()
{
	Tree* tree = new Tree(this->m_expression + m_ptr + 1, true);
	this->MergeTree(tree);
	//m_ptr++;
	return true;
}

bool Tree::OnCloseBracket()
{
	if (!this->m_initByBracket)
	{
		this->OnError(5); // closing bracket without opening
		return false;
	}
	return true;
}

bool Tree::OnDigit(char digit)
{
	if (m_lastNode != 0) // this is not first digit that met
	{
		if (m_lastInputType == Digit) { // digit after digit
			this->OnError(4); // only one digit is allowed
			return false;
		}
		else
		{
			Node* node = Tree::NewNode();
			node->m_value = digit;
			m_lastNode->m_rightChild = node;
			node->m_parent = m_lastNode;
			m_lastNode = node;
		}
	}
	else // this digit is very first input as an operand
	{
		Node* node = Tree::NewNode();
		node->m_value = digit;
		m_root = node;
		m_lastNode = node;
	}
	m_lastInputType = Digit;
	return true;
}

void Tree::OnError(int code)
{
	m_errcode = code;
}