#include <iostream>
#include <vector>
#include <string>
#include "validator.h"
#include "inst.h"
#include "cfg.h"
#include "z3++.h"

using namespace z3;

context c;

#define CURSRC sv->getCurRegVar(SRCREG(in))
#define CURDST sv->getCurRegVar(DSTREG(in))
#define NEWDST sv->updateRegVar(DSTREG(in))
#define IMM2 IMM2VAL(in)

#define OP_DST(i, regId) postRegVal[i][regId]
// #define OP_IMM1 IMM1VAL(instEndp)

/* class smtVar start */
smtVar::smtVar(unsigned int progId, unsigned int versId) {
	_name = std::to_string(progId) + "_" + std::to_string(versId);
	std::memset(regCurId, 0, NUM_REGS * sizeof(unsigned int));
	std::string namePrefix = "r_" + _name + "_";
	for (size_t i = 0; i < NUM_REGS; i++) {
		std::string name = namePrefix + std::to_string(i) + "_0";
		regVar.push_back(stringToExpr(name));
	}
}

smtVar::~smtVar() {
}

expr smtVar::updateRegVar(unsigned int regId) {
	regCurId[regId]++;
	std::string name = "r_" + _name + "_" + std::to_string(regId) \
	                   + "_" + std::to_string(regCurId[regId]);
	regVar[regId] = stringToExpr(name);
	return getCurRegVar(regId);
}

expr smtVar::getCurRegVar(unsigned int regId) {
	return regVar[regId];
}

expr smtVar::getinitRegVar(unsigned int regId) {
	std::string name = "r_" + _name + "_" + std::to_string(regId) + "_0";
	return stringToExpr(name);
}
/* class smtVar end */

/* class progSmt start */
progSmt::progSmt() {}

progSmt::~progSmt() {}

// assume program has no branch and is an ordered sequence of instructions
expr progSmt::smtProg(inst* program, int length, smtVar* sv) {
	inst* instLst = program;
	// length = 1
	if (length == 1) {
		return smtInst(sv, &instLst[0]);
	}
	int instLength = length;
	// length > 1, end with END or JMP instruction
	if (getInstType(instLst[length - 1]) == CFG_END) {
		instLength = length - 1;
	}
	expr p = smtInst(sv, &instLst[0]);
	for (size_t i = 1; i < instLength; i++) {
		p = p and smtInst(sv, &instLst[i]);
	}
	return p;
}

expr progSmt::smtInst(smtVar* sv, inst* in) {
	switch (in->_opcode) {
	case NOP: {
		return stringToExpr("true");
	}
	case ADDXY: {
		return (CURDST + CURSRC == NEWDST);
	}
	case MOVXC: {
		return (IMM2 == NEWDST);
	}
	case MAXC: {
		expr oldDst = CURDST;
		expr newDst = NEWDST;
		expr cond1 = (oldDst > IMM2) and (newDst == oldDst);
		expr cond2 = (oldDst <= IMM2) and (newDst == IMM2);
		return (cond1 or cond2);
	}
	case MAXX: {
		expr oldDst = CURDST;
		expr newDst = NEWDST;
		expr cond1 = (oldDst > CURSRC) and (newDst == oldDst);
		expr cond2 = (oldDst <= CURSRC) and (newDst == CURSRC);
		return (cond1 or cond2);
	}
	case RETX: {
		return stringToExpr("true");
	}
	case RETC: {
		return stringToExpr("true");
	}
	case JMPEQ: {
		return stringToExpr("true");
	}
	case JMPGT: {
		return stringToExpr("true");
	}
	case JMPGE: {
		return stringToExpr("true");
	}
	case JMPLT: {
		return stringToExpr("true");
	}
	case JMPLE: {
		return stringToExpr("true");
	}
	default: {
		return stringToExpr("false");
	}
	}
}

expr stringToExpr(string s) {
	if (s == "true") {
		return c.bool_val(true);
	}
	else if (s == "false") {
		return c.bool_val(false);
	}
	return c.int_const(s.c_str());
}

// init f, postRegVal, postPathCon
void progSmt::initVariables(graph& g) {
	size_t blockNum = g.nodes.size();
	f.resize(blockNum, stringToExpr("true"));

	postRegVal.resize(blockNum);
	for (size_t i = 0; i < blockNum; i++) {
		postRegVal[i].resize(NUM_REGS, stringToExpr("true"));
	}

	pathCon.resize(blockNum);
	for (size_t i = 0; i < blockNum; i++) {
		// Because of the corresponding relationship between pathCon and g.nodesIn,
		// the size of pathCon[i] is equal to that of g.nodesIn[i];
		pathCon[i].resize(g.nodesIn[i].size(), stringToExpr("true"));
	}
}

// topological sorting by DFS
void progSmt::topoSortDFS(size_t curBId, vector<unsigned int>& blocks, vector<bool>& finished, graph& g) {
	if (finished[curBId]) {
		return;
	}
	for (size_t i = 0; i < g.nodesOut[curBId].size(); i++) {
		topoSortDFS(g.nodesOut[curBId][i], blocks, finished, g);
	}
	finished[curBId] = true;
	blocks.push_back(curBId);
}

expr progSmt::genBlockProgLogic(smtVar* sv, size_t curBId, inst* instLst, graph& g) {
	inst* start = &instLst[g.nodes[curBId]._start];
	int length = g.nodes[curBId]._end - g.nodes[curBId]._start + 1;
	return smtProg(start, length, sv);
}

void progSmt::storePostRegVal(smtVar* sv, size_t curBId) {
	for (size_t i = 0; i < NUM_REGS; i++) {
		postRegVal[curBId][i] = sv->getCurRegVar(i);
	}
}

void progSmt::smtJmpInst(smtVar* sv, vector<expr>& cInstEnd, inst& instEnd) {
	inst* in = &instEnd;
	// e is formula for Jmp
	expr e = stringToExpr("true");
	switch (instEnd._opcode) {
	case JMPEQ: {e = (CURDST == CURSRC); break;}
	case JMPGT: {e = (CURDST > CURSRC); break;}
	case JMPGE: {e = (CURDST >= CURSRC); break;}
	case JMPLT: {e = (CURDST < CURSRC); break;}
	case JMPLE: {e = (CURDST <= CURSRC); break;}
	}
	// keep order: insert no jmp first
	cInstEnd.push_back(!e); // no jmp
	cInstEnd.push_back(e);  // jmp
}

// update path condition "pCon" generated by curBId into the pathCon[nextBId]
void progSmt::addPathCond(expr pCon, size_t curBId, size_t nextBId, graph& g) {
	for (size_t j = 0; j < g.nodesIn[nextBId].size(); j++) {
		if (g.nodesIn[nextBId][j] == curBId) {
			pathCon[nextBId][j] = pCon;
		}
	}
}

// steps: 1. calculate c_in;
// 2. calculate post path condition "c" of current basic block curBId;
// 3. use c to update pathCon[nextBId]
// three cases: 1. no next blocks 2. one next block 3. two next blocks
void progSmt::genPostPathCon(smtVar* sv, size_t curBId, inst& instEnd, graph& g) {
	// case 1: no next blocks
	if (g.nodesOut[curBId].size() == 0) {
		return;
	}
	// step 1. calculate c_in;
	// When curBId is processed, pathCon[curBId] already has been
	// updated with the correct value because of topo sort
	// if current block(i.e., block 0) has no incoming edges, set c_in = true.
	expr cIn = stringToExpr("true");
	if (pathCon[curBId].size() > 0) { // calculate c_in by parents, that is, pathCon[curBId]
		cIn = pathCon[curBId][0];
		for (size_t i = 1; i < g.nodesIn[curBId].size(); i++) {
			// cIn = (cIn || pathCon[curBId][i]).simplify(); //TODO: imporve later
			cIn = (cIn || pathCon[curBId][i]);
		}
	}
	// case 2: one next block
	// In this case, the post path condition is same as the c_in
	if (g.nodesOut[curBId].size() == 1) {
		unsigned int nextBId = g.nodesOut[curBId][0];
		// update path condition to the pathCon[nextBId]
		addPathCond(cIn, curBId, nextBId, g);
		return;
	}
	// case 3: two next blocks
	// If keep order: cInstEnd[0]: no jmp path condition; cInstEnd[1]: jmp path condition,
	// then cInstEnd[i] -> g.nodesOut[curBId][i];
	// Why: according to the process of jmp path condition in function genAllEdgesGraph in cfg.cc
	// function smtJmpInst keep this order
	// case 3 step 2
	vector<expr> cInstEnd;
	smtJmpInst(sv, cInstEnd, instEnd);
	// case 3 step 3
	// push the cInstEnd[0] and cInstEnd[1] into nextBIds' pathCon
	// no jmp
	unsigned int nextBId = g.nodesOut[curBId][0];
	addPathCond(cInstEnd[0], curBId, nextBId, g);
	// jmp
	nextBId = g.nodesOut[curBId][1];
	addPathCond(cInstEnd[1], curBId, nextBId, g);
}

expr progSmt::getInitVal(smtVar* sv, size_t inBId) {
	expr e = (postRegVal[inBId][0] == sv->getinitRegVar(0));
	for (size_t i = 1; i < NUM_REGS; i++) {
		e = e && (postRegVal[inBId][i] == sv->getinitRegVar(i));
	}
	return e;
}

// TODO: needed to be generalized
// for each return value v, smt: v == output[progId]
expr progSmt::smtRetInst(size_t curBId, inst* instEnd, unsigned int progId) {
	expr e1 = stringToExpr("true");
	switch (instEnd->_opcode) {
	case RETX: {
		e1 = (OP_DST(curBId, DSTREG(instEnd)) == stringToExpr("output" + to_string(progId)));
		break;
	}
	case RETC: {
		e1 = (IMM1VAL(instEnd) == stringToExpr("output" + to_string(progId)));
		break;
	}
	}
	return e1;
}

void progSmt::processOutput(graph& g, inst* instLst, unsigned int progId) {
	expr e = stringToExpr("true");
	// search all basic blocks for the basic blocks without outgoing edges
	for (size_t i = 0; i < g.nodes.size(); i++) {
		if (g.nodesOut[i].size() != 0) continue;
		// process END instruction
		expr cIn = stringToExpr("true");
		// if current block has incoming edges
		if (g.nodesIn[i].size() > 0) {
			cIn = pathCon[i][0];
			for (size_t j = 1; j < g.nodesIn[i].size(); j++) {
				// cIn = (cIn || pathCon[i][j]).simplify();
				cIn = (cIn || pathCon[i][j]);
			}
		}
		expr e1 = smtRetInst(i, &instLst[g.nodes[i]._end], progId);
		e = e && implies(cIn, e1);
	}
	smtOutput = e;
}

expr progSmt::genSmt(unsigned int progId, inst* instLst, int length) {
	graph g;
	try {
		g.genGraph(instLst, length);
	}
	catch (const string errMsg) {
		cerr << errMsg << endl;
	}
	// std::cout << g;

	// init class variables
	initVariables(g);

	// blocks stores the block IDs in order after topological sorting
	vector<unsigned int> blocks;
	vector<bool> finished(g.nodes.size(), false);
	topoSortDFS(0, blocks, finished, g);
	std::reverse(blocks.begin(), blocks.end());
	// std::cout << "blocks is" << endl;
	// for (size_t i = 0; i < blocks.size(); i++) {
	// 	std::cout << blocks[i] << " ";
	// }
	// std::cout << std::endl;

	// process each basic block in order
	for (size_t i = 0; i < blocks.size(); i++) {
		unsigned int b = blocks[i];
		// std::cout << "process " << b << "......\n";
		smtVar sv(progId, b);
		// generate f_pl: the block program logic
		expr fPL = genBlockProgLogic(&sv, b, instLst, g);
		// std::cout << "f_pl is: " << fPL << "\n";
		if (b == 0) {
			f[0] = fPL;
		}
		else {
			for (size_t j = 0; j < g.nodesIn[b].size(); j++) {
				// generate f_iv: the logic that the initial values are fed by the last basic block
				expr fIV = getInitVal(&sv, g.nodesIn[b][j]);
				// std::cout << "f_iv is: " << fIV << "\n";
				// F_b = sum (c_j -> f_pl && f_iv_j)
				f[b] = f[b] && implies(pathCon[b][j], fPL && fIV);
			}
		}
		// std::cout << "F_" << b << " is: " << f[b] << "\n";

		// store post iv for current basic block b
		storePostRegVal(&sv, b);
		// std::cout << "postRegVal[" << b << "]: ";
		// for (size_t i = 0; i < NUM_REGS; i++) {
		// 	std::cout << postRegVal[b][i] << " ";
		// }
		// std::cout << "\n";

		// update post path condtions "pathCon" created by current basic block b
		genPostPathCon(&sv, b, instLst[g.nodes[b]._end], g);
	}

	for (size_t i = 0; i < f.size(); i++) {
		// cout << "curBId is " << i << "\n";
		// std::cout << f[i] << "\n";
		smt = smt && f[i];
	}

	processOutput(g, instLst, progId);
	return (smt && smtOutput).simplify();
}
/* class progSmt end */

// TODO: needed to be generalized
// assgin input r0 "pre0", other registers 0
expr smtPre(smtVar* sv) {
	expr p = (sv->getCurRegVar(0) == stringToExpr("pre0"));
	for (size_t i = 1; i < NUM_REGS; i++) {
		// expr e = stringToExpr("pre" + std::to_string(i));
		p = p and (sv->getCurRegVar(i) == 0);
	}
	return p;
}

// TODO: needed to be generalized
expr smtPost() {
	return stringToExpr("output1") == stringToExpr("output2");
}

bool equalCheck(inst* instLst1, int len1, inst* instLst2, int len2) {
	// smt = (pre1^pre2)^(p1^p2) => post
	smtVar svP1(1, 0);
	smtVar svP2(2, 0);
	expr pre1 = smtPre(&svP1);
	expr pre2 = smtPre(&svP2);
	// std::cout << "process progam 1 .......\n";
	progSmt ps1;
	expr p1 = ps1.genSmt(1, instLst1, len1);
	// std::cout << "process progam 2 .......\n";
	progSmt ps2;
	expr p2 = ps2.genSmt(2, instLst2, len2);

	// std::cout << "f1 is \n" << (p1 && pre1) << "\n";
	// std::cout << "f2 is \n" << (p2 && pre2) << "\n";

	expr post = smtPost();
	expr smt = implies(p1 && pre1 && p2 && pre2, post);

	solver s(c);
	s.add(!smt);
	switch (s.check()) {
	case unsat: return true;
	case sat: return false;
	case unknown: return false;
	}
	return false;
}
