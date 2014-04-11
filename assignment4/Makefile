all: retrieve.c
	gcc -pthread -g -o retrieve retrieve.c
test: retrieve
	./retrieve /home/dryke014/4061/assignment4/test_data1/clients.txt /home/dryke014/4061/assignment4/test_data1/output2
clean:
	rm retrieve
