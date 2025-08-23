CC      = gcc
PROF    = -O2 -g
NOCRYPT =
C_FLAGS = -Wall -D_GNU_SOURCE $(PROF) $(NOCRYPT)
L_FLAGS = $(PROF) -lm -lcrypt

ROT_FILES = act_comm.o act_enter.o act_info.o act_move.o act_obj.o act_wiz.o \
          alias.o ban.o clans.o comm.o const.o curse.o db.o db2.o effects.o \
          fight.o finger.o flags.o forget.o handler.o healer.o interp.o note.o \
          lookup.o magic.o magic2.o music.o recycle.o repent.o save.o scan.o \
          sign.o skills.o special.o tables.o update.o wizlist.o \
          mob_cmds.o mob_prog.o olc.o olc_act.o olc_save.o bit.o mem.o \
          string.o olc_mpcode.o math.o sedit.o skedit.o quest.o \
          asciimap.o

TARGET = rot

all: $(TARGET)

$(TARGET): $(ROT_FILES)
	$(CC) -o $(TARGET) $(ROT_FILES) $(L_FLAGS)

%.o: %.c merc.h
	$(CC) $(C_FLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)

move: 
	cp rot -f ../bin
	
start:
	../area/startup &