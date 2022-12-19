CC = gcc
CFLAGS = -g

shell: pwd.o echo.o prompt.o cd.o main.o listDir.o history.o pinfo.o fg.o bg.o discover.o input.o jobs.o
	$(CC) $(CFLAGS) main.o echo.o cd.o pwd.o prompt.o history.o pinfo.o fg.o bg.o listDir.o discover.o input.o jobs.o -o shell

pwd.o: pwd.c pwd.h variables.h
	$(CC) $(CFLAGS) -c pwd.c pwd.h

echo.o: echo.c echo.h
	$(CC) $(CFLAGS) -c echo.c echo.h

prompt.o: pwd.h prompt.h prompt.c
	$(CC) $(CFLAGS) -c prompt.c

cd.o: variables.h prompt.h pwd.h cd.c cd.h
	$(CC) $(CFLAGS) -c cd.c 

listDir.o : listDir.h listDir.c
	$(CC) $(CFLAGS) -c listDir.c

history.o: history.c history.h
	$(CC) $(CFLAGS) -c history.c

main.o: main.c headers.h 
	$(CC) $(CFLAGS) -c main.c

pinfo.o: pinfo.c
	$(CC) $(CFLAGS) -c pinfo.c

fg.o: fg.c fg.h
	$(CC) $(CFLAGS) -c fg.c

bg.o : bg.c bg.h
	$(CC) $(CFLAGS) -c bg.c

discover.o: discover.c discover.h
	$(CC) $(CFLAGS) -c discover.c

input.o: input.c input.h
	$(CC) $(CFLAGS) -c input.c

jobs.o: jobs.c jobs.h
	$(CC) $(CFLAGS) -c jobs.c

clean:
	rm *.o *.gch