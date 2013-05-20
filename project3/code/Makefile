# mpicc.lam /usr/bin/mpicc.lam
# mpicc.openmpi hcc

# mpicc
# mpirun

all:trivial ring datagen sample_sort_mpi verify

open:trivial_open ring_open datagen sample_sort_open verify

lam:trivial_lam ring_open datagen sample_sort_lam verify

trivial_open:trivial.c
	mpicc -o trivial trivial.c

trivial_lam:trivial.c
	mpicc.lam -o trivial trivial.c

ring_open:ring.c
	mpicc -o ring ring.c

ring_lam:ring.c
	mpicc.lam -o ring ring.c

datagen:datagen.c
	gcc -o datagen datagen.c

sample_sort_open:sample_sort_mpi.c
	mpicc -o sample_sort_mpi sample_sort_mpi.c

sample_sort_lam:sample_sort_mpi.c
	mpicc.lam -o sample_sort_mpi sample_sort_mpi.c

verify:verify.c
	gcc -o verify verify.c

clean:
	rm trivial
	rm ring
	rm datagen
	rm sample_sort_mpi
	rm verify
