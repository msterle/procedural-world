#ifndef PLS_H
#define PLS_H

#include <unordered_map>
#include <vector>
#include <list>
#include <functional>

class PLS {
protected:
	// note: compiler problems with variadic, so maximum of 5 arguments
	typedef std::function<float(float, float, float, float, float)> ffloat_t;
	typedef std::function<bool(float, float, float, float, float)> fbool_t;
	typedef std::function<void(float, float, float, float, float)> fvoid_t;
public:
	// argument class for either float or a float(float...) bind result
	class Param {
		ffloat_t func;
		float val;
		bool isFunc;
	public:
		Param(float val) : val(val), isFunc(false) { }
		Param(ffloat_t func) : func(func), isFunc(true) { }
		float value() {
			if(isFunc)
				return func(0, 0, 0, 0, 0);
			return val;
		}
		float value(std::vector<float> inParams) {
			inParams.resize(5); // make sure it's always 5 elements
			if(isFunc){
				return func(inParams[0], inParams[1], 
					inParams[2], inParams[3], inParams[4]);
			}
			return val;
		}
	};

	// class for module
	class Module {
	public:
		char symbol;
		std::vector<Param> params;
		// no parameters
		Module(char symbol) : symbol(symbol), params() { }
		// single parameter
		Module(char symbol, Param param) : symbol(symbol), params(std::vector<Param>(1, param)) { }
		// vector of parameters
		Module(char symbol, std::vector<Param> params) : symbol(symbol), params(params) { }
		// return copy with evaluated functional parameters
		Module eval(std::vector<float> inParams) {
			Module copy(*this);
			for(Param& p : copy.params)
				p = Param(p.value(inParams));
			return copy;
		}
	};
	class Production {
	public:
		fbool_t condition;
		std::list<Module> successor;
		Production(fbool_t condition, std::list<Module> successor) : 
			condition(condition), successor(successor) { } 
	};
protected:
	std::list<Module> axiom;
	std::unordered_map<char, Production> productions;
	std::unordered_map<char, fvoid_t> actions;
	std::list<Module> value;
	unsigned int generation;
public:
	PLS(std::list<Module> axiom, std::unordered_map<char, Production> productions, 
		std::unordered_map<char, fvoid_t> actions)
		: axiom(axiom), productions(productions), actions(actions), 
		value(axiom), generation(0) { }
	PLS() { }
	void iterate() {
		std::list<Module> newValue;
		for(Module module : value) {
			// TODO add contition
			std::unordered_map<char, Production>::iterator itProduction = productions.find(module.symbol);
			if(itProduction == productions.end()) {
				// no production rule for module, copy module
				newValue.push_back(module);
			}
			else {
				// production rule found
				// convert input params to vector<float>
				std::vector<float> inParams;
				for(Param p : module.params)
					inParams.push_back(p.value());
				// for each successor module, add to new value after evaluating parameters
				for(Module succ : itProduction->second.successor) {
					newValue.push_back(succ.eval(inParams));
				}
			}
		}
		value = newValue;
	}
	void iterate(int n) {
		for(int i = 0; i < n; ++i)
			iterate();
	}
	void run() {
		for(Module m : value) {
			std::unordered_map<char, fvoid_t>::iterator itAction = actions.find(m.symbol);
			// check if action exists
			if(itAction != actions.end()) {
				// convert params to vector<float> of size 5
				std::vector<float> paramsFloat;
				for(Param p : m.params)
					paramsFloat.push_back(p.value());
				paramsFloat.resize(5);
				itAction->second(paramsFloat[0], paramsFloat[1], 
					paramsFloat[2], paramsFloat[3], paramsFloat[4]);
			}
		}
	}
	std::list<Module> getValue() { return value; }
	unsigned int getGeneration() { return generation; }
	// static common functions for use by parameters
	static float pass(float a) { return a; } // pass placeholder
	static float mult(float a, float b) {return a * b; }
	static bool gteq(float a, float b) { return a >= b; }
};

#endif