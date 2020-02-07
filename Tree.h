#pragma once

struct Node {
	char m_value;
	Node* m_parent;
	Node* m_leftChild;
	Node* m_rightChild;
};

enum CharType
{
	Digit = 0,
	PlusMinus = 1,
	MulDiv = 2,
	OpenBrac = 3,
	CloseBrac = 4,
	Etc = -1
};

class Tree
{
public:
	Tree(char* expression, bool bracket_init = false);
	~Tree();

	int Evaluate();
	int GetResult() { return m_result; }

private:
	Node* m_root;
	Node* m_lastNode;
	int m_ptr;
	char* m_expression;

	void RemoveNode(Node* node);
	int Make();

	void MergeTree(Tree* tree);
	static Node* NewNode();

	int m_result;
	int m_errcode;
	char m_pendingOperator;
	bool m_initByBracket;

	CharType m_lastInputType;

	int EvaluateNode(Node* node);

protected:
	bool OnPlusOrMinus(char op);
	bool OnMulOrDiv(char op);
	bool OnOpenBracket();
	bool OnCloseBracket();
	bool OnDigit(char digit);
	void OnError(int errorCode);
};

