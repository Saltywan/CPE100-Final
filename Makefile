run:
	gcc -o main main.c -lcurl && ./main

clean:
	rm -rf main