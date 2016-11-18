#ifndef LSYSTEM_H
#define LSYSTEM_H

#include <unordered_map>

template <class actionSignature>
class LSystem {
public:
	struct Rule {
		string replace;
		actionSignature* action;
		void* actionArg;
	};
	typedef unordered_map<char, Rule> Grammar;
protected:
	Grammar grammar;
	string axiom;
	string value;
public:
	LSystem() {}
	LSystem(string axiom, Grammar grammar) : grammar(grammar), axiom(axiom), value(axiom) { }
	void iterate() {
		string newValue;
		for(string::iterator it = value.begin(); it != value.end(); ++it)
			newValue += grammar[*it].replace;
		value = newValue;
	}
	void iterate(int n) {
		for(int i = 0; i < n; ++i)
			iterate();
	}
	void run() {
		for(string::iterator it = value.begin(); it != value.end(); ++it)
			grammar[*it].action(grammar[*it].actionArg);
	}
	string getValue() { return value; }	
};

#endif