default: feeder
feeder: feeder.o semaphores.o shared_memory.o
	gcc -pthread feeder.o semaphores.o shared_memory.o -o feeder
feeder.o:
	gcc -c feeder.c -o feeder.o 
semaphores.o:
	gcc -c semaphores.c -o semaphores.o 
shared_memory.o:
	gcc -c shared_memory.c -o shared_memory.o
clean:
	rm feeder feeder.o semaphores.o shared_memory.o file*