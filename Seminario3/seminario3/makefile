.SUFFIXES:
.PHONY: x1, x2, x3,x4,x5,x6,x7, clean

comp:= mpicxx -std=c++11 -Wall

x1: holamundo_exe
	mpirun -hostfile hostfile -np 5 ./$<

x2: sendrecv1_exe
	mpirun -hostfile hostfile -np 2 ./$<

x3: sendrecv2_exe
	mpirun -hostfile hostfile -np 5 ./$<

x4: ejemplo_probe_exe
	mpirun -hostfile hostfile -np 10 ./$<

x5: ejemplo_iprobe_exe
	mpirun -hostfile hostfile -np 10 ./$<

x6: intercambio_sincrono_exe
	mpirun -hostfile hostfile -np 8 ./$<

x7: intercambio_nobloq_exe
	mpirun -hostfile hostfile -np 8 ./$<



%_exe: %.cpp
	$(comp) $< -o $@

clean:
	rm -f *_exe
