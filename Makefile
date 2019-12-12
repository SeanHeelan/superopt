SRC=src/
ISA=src/isa/toy-isa/
VERIFY=src/verify/toy-isa/
SEARCH=src/search/

all: main.out proposals_test.out inst_test.out cost_test.out prog_test.out mh_prog_test.out validator_test.out cfg_test.out inout_test.out smt_prog_test.out

main.out: main.cc main.h main_z3.o measure/common.cc measure/common.h measure/meas_mh_bhv.h measure/meas_mh_bhv.cc $(SEARCH)mh_prog.cc $(SEARCH)mh_prog.h $(SEARCH)proposals.cc $(SEARCH)proposals.h $(SRC)prog.cc $(SRC)prog.h $(SEARCH)cost.cc $(SEARCH)cost.h $(SRC)inout.cc $(SRC)inout.h $(ISA)inst.cc $(ISA)inst.h $(VERIFY)validator.cc $(VERIFY)validator.h $(VERIFY)cfg.cc $(VERIFY)cfg.h $(VERIFY)smt_prog.cc $(VERIFY)smt_prog.h $(SRC)utils.cc $(SRC)utils.h
	g++ -std=c++11 main_z3.o measure/common.cc measure/meas_mh_bhv.cc $(ISA)inst.cc $(SEARCH)mh_prog.cc $(SEARCH)proposals.cc $(SRC)prog.cc $(SEARCH)cost.cc $(SRC)inout.cc $(VERIFY)validator.cc $(VERIFY)cfg.cc $(VERIFY)smt_prog.cc $(SRC)utils.cc -o main.out ../z3/build/libz3.dylib -lpthread

main_z3.o: main.cc
	g++ -D_MP_INTERNAL -DNDEBUG -D_EXTERNAL_RELEASE -std=c++11 -fvisibility=hidden -c -mfpmath=sse -msse -msse2 -O3 -Wno-unknown-pragmas -Wno-overloaded-virtual -Wno-unused-value -fPIC -o main_z3.o  -I../z3/src/api -I../z3/src/api/c++ main.cc

proposals_test.out: $(ISA)inst.cc $(ISA)inst.h $(SEARCH)proposals.cc $(SEARCH)proposals_test.cc $(SRC)prog.cc $(SRC)prog.h $(SRC)utils.cc $(SRC)utils.h
	g++ -std=c++11 $(ISA)inst.cc $(SEARCH)proposals.cc $(SEARCH)proposals_test.cc $(SRC)prog.cc $(SRC)utils.cc -o $(SEARCH)proposals_test.out

inst_test.out: $(ISA)inst.cc $(ISA)inst.h $(ISA)inst_test.cc $(SRC)utils.cc $(SRC)utils.h
	g++ -std=c++11 $(ISA)inst.cc $(ISA)inst_test.cc $(SRC)utils.cc -o $(ISA)inst_test.out

cost_test.out: $(SEARCH)cost.cc cost_z3.o $(SEARCH)cost.h $(SRC)inout.h $(SRC)inout.cc $(ISA)inst.cc $(ISA)inst.h $(VERIFY)validator.cc $(VERIFY)validator.h $(VERIFY)cfg.cc $(VERIFY)cfg.h $(SRC)utils.cc $(SRC)utils.h $(VERIFY)smt_prog.cc $(VERIFY)smt_prog.h $(SRC)prog.cc $(SRC)prog.h
	g++ -std=c++11 $(SEARCH)cost.cc $(SEARCH)cost_z3.o $(SRC)inout.cc $(ISA)inst.cc $(VERIFY)validator.cc $(VERIFY)cfg.cc $(SRC)utils.cc $(VERIFY)smt_prog.cc $(SRC)prog.cc -o $(SEARCH)cost_test.out ../z3/build/libz3.dylib -lpthread

cost_z3.o: $(SEARCH)cost_test.cc
	g++ -D_MP_INTERNAL -DNDEBUG -D_EXTERNAL_RELEASE -std=c++11 -fvisibility=hidden -c -mfpmath=sse -msse -msse2 -O3 -Wno-unknown-pragmas -Wno-overloaded-virtual -Wno-unused-value -fPIC -o $(SEARCH)cost_z3.o  -I../z3/src/api -I../z3/src/api/c++ $(SEARCH)cost_test.cc

prog_test.out: $(SRC)prog.cc $(ISA)inst.h $(ISA)inst.cc $(SRC)prog.h $(SRC)prog_test.cc $(SRC)utils.h $(SRC)utils.cc
	g++ -std=c++11 $(SRC)prog_test.cc $(SRC)prog.cc $(ISA)inst.cc $(SRC)utils.cc -o $(SRC)prog_test.out

mh_prog_test.out: $(SEARCH)mh_prog.cc $(SEARCH)mh_prog.h mh_prog_z3.o $(SEARCH)proposals.cc $(SEARCH)proposals.h $(SRC)prog.cc $(SRC)prog.h $(SEARCH)cost.cc $(SEARCH)cost.h $(SRC)inout.cc $(SRC)inout.h $(ISA)inst.cc $(ISA)inst.h $(VERIFY)validator.cc $(VERIFY)validator.h $(VERIFY)cfg.cc $(VERIFY)cfg.h $(VERIFY)smt_prog.cc $(VERIFY)smt_prog.h $(SRC)utils.cc $(SRC)utils.h measure/meas_mh_bhv.h measure/meas_mh_bhv.cc
	g++ -std=c++11 $(ISA)inst.cc $(SEARCH)mh_prog.cc $(SEARCH)proposals.cc $(SRC)prog.cc $(SEARCH)cost.cc $(SRC)inout.cc $(VERIFY)validator.cc $(VERIFY)cfg.cc $(SEARCH)mh_prog_z3.o $(VERIFY)smt_prog.cc $(SRC)utils.cc measure/meas_mh_bhv.cc -o $(SEARCH)mh_prog_test.out ../z3/build/libz3.dylib -lpthread

mh_prog_z3.o: $(SEARCH)mh_prog_test.cc
	g++ -D_MP_INTERNAL -DNDEBUG -D_EXTERNAL_RELEASE -std=c++11 -fvisibility=hidden -c -mfpmath=sse -msse -msse2 -O3 -Wno-unknown-pragmas -Wno-overloaded-virtual -Wno-unused-value -fPIC -o $(SEARCH)mh_prog_z3.o  -I../z3/src/api -I../z3/src/api/c++ $(SEARCH)mh_prog_test.cc

validator_test.out: validator_z3.o $(VERIFY)validator.cc $(VERIFY)validator.h $(ISA)inst.cc $(ISA)inst.h $(VERIFY)cfg.cc $(VERIFY)cfg.h $(SRC)inout.cc $(SRC)inout.h $(SRC)utils.cc $(SRC)utils.h $(VERIFY)smt_prog.cc $(VERIFY)smt_prog.h
	g++ -std=c++11 $(VERIFY)validator_z3.o $(VERIFY)validator.cc $(ISA)inst.cc $(VERIFY)cfg.cc $(SRC)inout.cc $(SRC)utils.cc $(VERIFY)smt_prog.cc -o $(VERIFY)validator_test.out ../z3/build/libz3.dylib -lpthread

validator_z3.o: $(VERIFY)validator_test.cc
	g++ -D_MP_INTERNAL -DNDEBUG -D_EXTERNAL_RELEASE -std=c++11 -fvisibility=hidden -c -mfpmath=sse -msse -msse2 -O3 -Wno-unknown-pragmas -Wno-overloaded-virtual -Wno-unused-value -fPIC -o $(VERIFY)validator_z3.o  -I../z3/src/api -I../z3/src/api/c++ $(VERIFY)validator_test.cc

smt_prog_test.out: smt_prog_z3.o $(VERIFY)smt_prog.cc $(VERIFY)smt_prog.h $(ISA)inst.cc $(ISA)inst.h $(VERIFY)cfg.cc $(VERIFY)cfg.h $(SRC)utils.cc $(SRC)utils.h
	g++ -std=c++11 $(VERIFY)smt_prog_z3.o $(VERIFY)smt_prog.cc $(ISA)inst.cc $(VERIFY)cfg.cc $(SRC)utils.cc -o $(VERIFY)smt_prog_test.out ../z3/build/libz3.dylib -lpthread

smt_prog_z3.o: $(VERIFY)smt_prog_test.cc
	g++ -D_MP_INTERNAL -DNDEBUG -D_EXTERNAL_RELEASE -std=c++11 -fvisibility=hidden -c -mfpmath=sse -msse -msse2 -O3 -Wno-unknown-pragmas -Wno-overloaded-virtual -Wno-unused-value -fPIC -o $(VERIFY)smt_prog_z3.o  -I../z3/src/api -I../z3/src/api/c++ $(VERIFY)smt_prog_test.cc

cfg_test.out: $(ISA)inst.h $(ISA)inst.cc $(VERIFY)cfg.h $(VERIFY)cfg.cc $(VERIFY)cfg_test.cc $(SRC)utils.cc $(SRC)utils.h
	g++ -std=c++11 $(ISA)inst.cc $(VERIFY)cfg.cc $(VERIFY)cfg_test.cc $(SRC)utils.cc -o $(VERIFY)cfg_test.out

inout_test.out: $(SRC)inout_test.cc $(SRC)inout.cc $(SRC)inout.h $(SRC)utils.cc $(SRC)utils.h
	g++ -std=c++11 $(SRC)inout_test.cc $(SRC)inout.cc $(SRC)utils.cc -o $(SRC)inout_test.out

clean:
	for i in */; do find . -name "*.o" -delete; done
	for i in */; do find . -name "*.out" -delete; done

all_measure: meas_time.out meas_mh_bhv_test.out

meas_time.out: measure/meas_time.cc measure/common.cc measure/common.h measure/meas_mh_bhv.h measure/meas_mh_bhv.cc meas_time_z3.o $(SEARCH)mh_prog.cc $(SEARCH)mh_prog.h $(SEARCH)proposals.cc $(SEARCH)proposals.h $(SRC)prog.cc $(SRC)prog.h $(SEARCH)cost.cc $(SEARCH)cost.h $(SRC)inout.cc $(SRC)inout.h $(ISA)inst.cc $(ISA)inst.h $(VERIFY)validator.cc $(VERIFY)validator.h $(VERIFY)cfg.cc $(VERIFY)cfg.h $(VERIFY)smt_prog.cc $(VERIFY)smt_prog.h $(SRC)utils.cc $(SRC)utils.h
	g++ -std=c++11 measure/meas_time_z3.o measure/common.cc measure/meas_mh_bhv.cc $(ISA)inst.cc $(SEARCH)mh_prog.cc $(SEARCH)proposals.cc $(SRC)prog.cc $(SEARCH)cost.cc $(SRC)inout.cc $(VERIFY)validator.cc $(VERIFY)cfg.cc $(VERIFY)smt_prog.cc $(SRC)utils.cc -o measure/meas_time.out ../z3/build/libz3.dylib -lpthread

meas_time_z3.o: measure/meas_time.cc
	g++ -D_MP_INTERNAL -DNDEBUG -D_EXTERNAL_RELEASE -std=c++11 -fvisibility=hidden -c -mfpmath=sse -msse -msse2 -O3 -Wno-unknown-pragmas -Wno-overloaded-virtual -Wno-unused-value -fPIC -o measure/meas_time_z3.o  -I../z3/src/api -I../z3/src/api/c++ measure/meas_time.cc

meas_mh_bhv_test.out: measure/meas_mh_bhv_test.cc measure/meas_mh_bhv.h measure/meas_mh_bhv.cc measure/common.h measure/common.cc $(SRC)prog.h $(SRC)prog.cc $(SRC)inout.h $(SRC)inout.cc $(ISA)inst.h $(ISA)inst.cc
	g++ -std=c++11 measure/meas_mh_bhv_test.cc measure/meas_mh_bhv.cc measure/common.cc $(SRC)prog.cc $(SRC)inout.cc $(ISA)inst.cc -o measure/meas_mh_bhv_test.out

path_measure=measure/
clean_measure:
	rm -f ${path_measure}meas_time.out ${path_measure}meas_time_z3.o ${path_measure}meas_mh_bhv_test.out
