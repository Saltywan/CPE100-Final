run:
	gcc -o main main.c -lcurl -DDEBUG && ./main

clean:
	rm -rf main