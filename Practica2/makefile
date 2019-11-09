compilador:=g++
opcionesc:= -std=c++11 -pthread -Wfatal-errors
hmonsrcs:= HoareMonitor.h HoareMonitor.cpp Semaphore.h Semaphore.cpp

all: fumadores_exe

x1: fumadores_exe
	./$<

fumadores_exe: fumadores.cpp $(hmonsrcs)
	$(compilador) $(opcionesc) -o $@ $< HoareMonitor.cpp Semaphore.cpp

clean:
	rm -f *_exe
