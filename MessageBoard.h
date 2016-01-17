#pragma once
#include <string>
using namespace std;

class CMessageBoard
{
public:
	string message, fromLabel, toLabel;

	CMessageBoard() { Reset(); }
	void SetMessage(string _message){ message = _message; }
	void SetFromLabel(string label){ fromLabel = label; }
	void SetToLabel(string label){ toLabel = label; }
	string GetMessage() { return message; }
	string GetFromLabel(){ return fromLabel; }
	string GetToLabel(){ return toLabel; }
	void Reset(){ message = fromLabel = toLabel = "Nil"; }
};

