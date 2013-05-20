all : prime_seq prime_omp qsort_seq qsort_omp qsort_queue

debug : 
	/opt/csw/bin/gcc -DDEBUG -c prime_seq.c -o prime_seq.o
	/opt/csw/bin/gcc -DDEBUG -fopenmp -c prime_omp.c -o prime_omp.o
	/opt/csw/bin/gcc -DDEBUG -c qsort_seq.c -o qsort_seq.o
	/opt/csw/bin/gcc -DDEBUG -fopenmp -c qsort_omp.c -o qsort_omp.o
	/opt/csw/bin/gcc -DDEBUG -c qsort_queue.c -o qsort_queue.o
	/opt/csw/bin/gcc -DDEBUG -c queue.c -o queue.o
	/opt/csw/bin/gcc prime_seq.o -lm -o prime_seq
	/opt/csw/bin/gcc prime_omp.o -lm -fopenmp -o prime_omp
	/opt/csw/bin/gcc qsort_seq.o -o qsort_seq
	/opt/csw/bin/gcc qsort_omp.o -fopenmp -o qsort_omp
	/opt/csw/bin/gcc -o qsort_queue qsort_queue.o queue.o -lpthread

prime_seq : prime_seq.o
	/opt/csw/bin/gcc prime_seq.o -lm -o prime_seq

prime_omp : prime_omp.o
	/opt/csw/bin/gcc prime_omp.o -lm -fopenmp -o prime_omp

qsort_seq : qsort_seq.o
	/opt/csw/bin/gcc qsort_seq.o -o qsort_seq

qsort_omp : qsort_omp.o
	/opt/csw/bin/gcc qsort_omp.o -fopenmp -o qsort_omp

qsort_queue : qsort_queue.o queue.o
	/opt/csw/bin/gcc -o qsort_queue qsort_queue.o queue.o -lpthread

prime_seq.o : prime_seq.c
	/opt/csw/bin/gcc -c prime_seq.c -o prime_seq.o

prime_omp.o : prime_omp.c
	/opt/csw/bin/gcc -c prime_omp.c -fopenmp -o prime_omp.o

qsort_seq.o : qsort_seq.c
	/opt/csw/bin/gcc -c qsort_seq.c -o qsort_seq.o

qsort_omp.o : qsort_omp.c
	/opt/csw/bin/gcc -c qsort_omp.c -fopenmp -o qsort_omp.o

qsort_queue.o : qsort_queue.c
	/opt/csw/bin/gcc -c qsort_queue.c -o qsort_queue.o

queue.o : queue.c
	/opt/csw/bin/gcc -c queue.c -o queue.o

summary : 
	(cd report; $(MAKE))

clean : 
	rm -f prime_seq.o
	rm -f prime_seq
	rm -f prime_omp.o
	rm -f prime_omp
	rm -f qsort_seq.o
	rm -f qsort_seq
	rm -f qsort_omp.o
	rm -f qsort_omp
	rm -f queue.o
	rm -f qsort_queue.o
	rm -f qsort_queue
	(cd report; $(MAKE) clean)
