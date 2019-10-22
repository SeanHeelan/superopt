#pragma once

#include <vector>
#include "inst.h"
#include "cfg.h"
#include "z3++.h"

using namespace z3;

// SMT Variable format
// [type]_[progId]_[nodeId]_[regId/memId]_[versionId]
// [type]: r means register; m means memory
class smtVar {
private:
	string _name;
	// store the curId
	unsigned int regCurId[NUM_REGS];
	std::vector<expr> regVar;
public:
	// 1. Convert progId and versId into _name, that is string([progId]_[versId])
	// 2. Initialize regVal[i] = r_[_name]_0, i = 0, ..., NUM_REGS
	smtVar(unsigned int progId, unsigned int versId);
	~smtVar();
	// inital value for [curId] is 0, and increases when updated
	expr updateRegVar(unsigned int regId);
	expr getCurRegVar(unsigned int regId);
	expr getinitRegVar(unsigned int regId);
};

// convert string s into expr e, the type of e is int_const
expr stringToExpr(string s);

class progSmt {
private:
	// //control flow graph
	// graph g;
	// f[i] is program logic FOL formula F of basic block i
	vector<expr> f;
	// postRegVal[i] is post register values of basic block i,
	// which are initial values for NEXT basic blocks
	vector<vector<expr> > postRegVal;
	// pathCon[i] stores pre path conditions of basic block i
	// There is a corresponding relationship between pathCon and g.nodesIn
	// more specifically, pathCon[i][j] stores the pre path condition from basic block g.nodesIn[i][j] to i
	vector<vector<expr> > pathCon;
	// program FOL formula
	expr smt = stringToExpr("true");
	// program output FOL formula
	expr smtOutput = stringToExpr("true");
	// return the SMT for the given program
	expr smtProg(inst* program, int length, smtVar* sv);
	// return SMT for the given instruction
	expr smtInst(smtVar* sv, inst* in);
	void initVariables(graph& g);
	void topoSortDFS(size_t curBId, vector<unsigned int>& blocks, vector<bool>& finished, graph& g);
	expr genBlockProgLogic(smtVar* sv, size_t curBId, inst* instLst, graph& g);
	void storePostRegVal(smtVar* sv, size_t curBId);
	void smtJmpInst(smtVar* sv, vector<expr>& cInstEnd, inst& instEnd);
	void addPathCond(expr pCon, size_t curBId, size_t nextBId, graph& g);
	void genPostPathCon(smtVar* sv, size_t curBId, inst& instEnd, graph& g);
	expr getInitVal(smtVar* sv, size_t inBId);
	expr smtRetInst(size_t curBId, inst* instEnd, unsigned int progId);
	void processOutput(graph& g, inst* instLst, unsigned int progId);
public:
	progSmt();
	~progSmt();
	expr genSmt(unsigned int progId, inst* instLst, int length);
};

// return the SMT for the pre condition
expr smtPre(smtVar* sv);
// return the SMT for the post condition check
expr smtPost();
bool equalCheck(inst* instLst1, int len1, inst* instLst2, int len2);

