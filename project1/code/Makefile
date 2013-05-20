
all : qsort_queue

qsort_queue_ : qsort_queue.o
	gcc qsort_queue.c -o qsort_queue

qsort_queue : qsort_queue.o queue.o
	gcc -o qsort_queue qsort_queue.o queue.o -lpthread

preprocessor : 
	gcc -E qsort_queue.c > qsort_queue.out

qsort_queue.o : qsort_queue.c
	gcc -DDEBUG -c qsort_queue.c -o qsort_queue.o

queue.o : queue.c
	gcc -DDEBUG -c queue.c -o queue.o

header :
	gcc -c qsort_queue.c -I /location/of/header/files -o qsort_queue

library :
	gcc -c qsort_queue.c -lpthread -L /usr/lib/libpthread

warning_error :
	gcc qsort_queue.c -o qsort_queue -Wall -Werror

debug :
	gcc -g qsort_queue.c -o qsort_queue

optimizations :
	gcc -O2 qsort_queue.c -o qsort_queue

clean :
	rm qsort_queue.o qsort_queue
