# silent echo calls
# MAKEFLAGS += --silent
# instead silencing individual calls with '@'


# compiler plus flags, -g: debug info, -Wall: all warnings
CC = @gcc -g -Wall

NAME      = doc
NAME_FULL = $(NAME).exe
OUT       = build\$(NAME_FULL)

SRC_DIR  = src
GLOBL_DIR = $(SRC_DIR)/global
APP_DIR   = $(SRC_DIR)/app

OBJ_DIR = bin

_OBJS =					\
	main.o				\
	file_io.o			\
	style.o				\
	doc.o					\
	def.o					\
	core_data.o

# put the OBJ_DIR in front of the obj names
OBJS = $(patsubst %,$(OBJ_DIR)/%,$(_OBJS))
OBJS_WIN = $(patsubst %,$(OBJ_DIR)\\%,$(_OBJS))

LIB_DIR = libs

_INC = 							  \
		external					\
		$(SRC_DIR)				\
		$(SRC_DIR)/global	\
		$(SRC_DIR)/app 
INC = $(patsubst %,-I %,$(_INC))


all: $(NAME)
	@echo ---- done ----

# compile all object files to .exe
$(NAME): $(OBJS) 
	$(CC) -o $(OUT) $(OBJS) $(LIB_LINK)

# compile all .c files to .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	echo $< -> $@	
	$(CC) -c $< -o $@ $(INC)

$(OBJ_DIR)/%.o: $(APP_DIR)/%.c
	echo $< -> $@	
	$(CC) -c $< -o $@ $(INC)


# prevents make from fideling with files named clean
.PHONY: clean

clean:
	del $(OUT)
	del $(OBJS_WIN) 
	@echo ---- clean done ---- 

# clean and them make all
cleanm: clean all

run: clean all
	@echo --- $(NAME) --- 
	@$(OUT)
