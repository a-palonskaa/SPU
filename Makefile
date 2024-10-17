ASM_DIR = assembler
DISASM_DIR = disassembler
PROC_DIR = processor
SUBDIRS = $(ASM_DIR) $(PROC_DIR)

all: asm disasm proc

asm:
	@for dir in $(ASM_DIR); do \
		$(MAKE) -C $$dir all;  \
	done

disasm:
	@for dir in $(DISASM_DIR); do \
		$(MAKE) -C $$dir all;     \
	done

proc:
	@for dir in $(PROC_DIR); do \
		$(MAKE) -C $$dir all;   \
	done

clean:
	@for dir in $(SUBDIRS); do  \
		$(MAKE) -C $$dir clean; \
	done
