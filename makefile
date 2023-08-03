# CS 218 Summer 2023 Assignment #6
# Simple make file for asst #6

OBJS		= hw6.o
ASM		= yasm -g dwarf2 -f elf64
LD		= ld -g

all: hw6

hw6.o: hw6.asm 
	$(ASM) hw6.asm -l hw6.lst

hw6: hw6.o
	$(LD) -o hw6 $(OBJS)

# -----
# clean by removing object file.

clean:
	rm	$(OBJS)
	rm cypher_text.txt
