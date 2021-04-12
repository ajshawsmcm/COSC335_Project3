SRCS=main.c xList.c xProcess.c xSimulator.c xSystem.c
OBJS=${SRCS:.c=.o}
BINS=rr mlq sjf xList_test xSimulator_test xProcess_test
CFLAGS=-g -Wall
CC=gcc
DIFF=diff
DIFFFLAGS=-strip-trailing-cr

# Default target
all: clean unit_tests fcfs

# Utility targets
clean:
	- rm *.o
	- rm ${BINS}
	- rm *_output
	- rm -rf *.dSYM

clobber: clean
	- rm *~
	- rm Makefile.bak

depends:
	makedepend -Y *.c

todos:
	grep "TODO" *.c *.h

tags:
	etags *.c *.h

# Main targets
rr: ${OBJS} xScheduler_RR.c
	${CC} ${CFLAGS} -o rr ${OBJS} xScheduler_RR.c

mlq: ${OBJS} xScheduler_MLQ.c
	${CC} ${CFLAGS} -o mlq ${OBJS} xScheduler_MLQ.c

sjf: ${OBJS} xScheduler_SJF.c
	${CC} ${CFLAGS} -o sjf ${OBJS} xScheduler_SJF.c

# Main target tests
rr_test: rr
	@ echo "Running FCFS scheduler."
	- ./rr

mlq_test: mlq
	@ echo "Running multi-level queuing scheduler."
	- ./mlq

sjf_test: sjf
	@ echo "Running shortest job first scheduler."
	- ./sjf

# Unit test targets
unit_tests: xList_test xSimulator_test xProcess_test
	@ echo "Testing xList."
	- ./xList_test > xList_output 2>&1 
	- diff xList_output xList_exemplar
	@ echo "Testing xSimulator."
	- ./xSimulator_test > xSimulator_output 2>&1 
	- diff xSimulator_output xSimulator_exemplar
	@ echo "Testing xProcess."
	- ./xProcess_test > xProcess_output 2>&1 
	- diff xProcess_output xProcess_exemplar

xList_test: xList_test.c xList.c
	${CC} -o xList_test xList_test.c xList.c

xSimulator_test: xSimulator_test.c xSimulator.c xList.c
	${CC} -o xSimulator_test xSimulator_test.c xSimulator.c xList.c

xProcess_test: xProcess_test.c xProcess.c
	${CC} -o xProcess_test xProcess_test.c xProcess.c

# Dependencies created by makedepend

# DO NOT DELETE

main.o: xSimulator.h xSystem.h xProcess.h
xList.o: xList.h
xListTest.o: xList.h
xProcess.o: xProcess.h
xProcessTest.o: xProcess.h
xScheduler.o: xScheduler.h xProcess.h xList.h xSystem.h
xSimulator.o: xSimulator.h xList.h
xSimulatorTest.o: xSimulator.h
xSystem.o: xSystem.h xProcess.h
