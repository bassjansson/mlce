
CC      = g++
CFLAGS  = #-Wall
LDFLAGS = -lportmidi #-lporttime

EDITOR    = Editor.o     NoteMessages.o
SCHEDULAR = Schedular.o  NoteMessages.o

all: Editor Schedular

Editor: $(EDITOR)
	$(CC) -o $@ $(CFLAGS) $(EDITOR) $(LDFLAGS)

Schedular: $(SCHEDULAR)
	$(CC) -o $@ $(CFLAGS) $(SCHEDULAR) $(LDFLAGS)

.cpp.o:
	$(CC) -c $< $(CFLAGS)

clean:
	rm ./*.o
	rm ./Editor
	rm ./Schedular
	rm ./*.mlce
	#rm `find . -perm +111 -type f`

touch:
	touch ./*.cpp
	touch ./*.h

