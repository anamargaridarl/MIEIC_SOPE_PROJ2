CC=gcc
CFLAGS = -Wall -Wextra -pedantic
DEPS = constants.h types.h accounts.h sope.h userflag.h communication.h serverfifoaux.h answerfifoaux.h crypto.h argcheck.h 
COMN = log.o communication.o crypto.o argcheck.o
SRV = server.o accounts.o answerfifoaux.o linked_list.o
USR = user.o userflag.o serverfifoaux.o

all: user server

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

user: $(USR) $(COMN) 
	$(CC) -o $@ $^ $(CFLAGS)

server:  $(SRV) $(COMN) 
	$(CC) -o $@ $^ $(CFLAGS) -lpthread -D_REENTRANT

clean:
	rm -f *.o user server /tmp/secure* /tmp/pipe* ulog.txt slog.txt
	
clean-logs:
	rm *.txt
	
format:
	clang-format -i *.c *.h -style=WebKit


