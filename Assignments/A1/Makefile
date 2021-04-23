CC=g++
FILES1=netMonitor.cpp
FILES2=intfMonitor.cpp
CFLAGS=-I
CFLAGS+=-Wall

networkMonitor: ${FILES1}
		${CC} ${CFLAGS} ${FILES1} -o netMonitor

interfaceMonitor: ${FILES2}
		${CC} ${CFLAGS} ${FILES2} -o intfMonitor

clean: 
	rm -f *.o netMonitor intfMonitor

all: networkMonitor interfaceMonitor 