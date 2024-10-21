ASM_DIR = assembler
DISASM_DIR = disassembler
PROC_DIR = processor
COMMON_DIR = common
STACK_DIR = stack
LIBS_DIR = $(COMMON_DIR) $(STACK_DIR)
SUBDIRS = $(ASM_DIR) $(PROC_DIR) $(DISASM_DIR) $(COMMON_DIR) $(STACK_DIR)

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

common:
	@for dir in $(COMMON_DIR); do \
		$(MAKE) -C $$dir all;     \
	done

stack:
	@for dir in $(STACK_DIR); do \
		$(MAKE) -C $$dir all;   \
	done

libs:
	@for dir in $(LIBS_DIR); do \
		$(MAKE) -C $$dir all;   \
	done

clean:
	@for dir in $(SUBDIRS); do  \
		$(MAKE) -C $$dir clean; \
	done
