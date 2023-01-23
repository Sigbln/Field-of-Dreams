compile:
	gcc -o client.exe ./code/client.c
	gcc -o server.exe ./code/server.c
	gcc -o drop.exe ./code/drop_server.c

run_client:
	./client.exe 127.0.0.1 8080

run_server:
	./server.exe 127.0.0.1 8080

drop_server:
	./drop.exe && rm config.txt