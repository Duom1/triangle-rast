CFLAGS += -Wall -Wextra -O3 -march=native
LDFLAGS += -lraylib -lm -lpthread -lGL -ldl -lrt -lX11
SOURCES = main.c
OBJS = $(SOURCES:.c=.o)
NAME = game
RL_HOME = /home/user/code/foss/raylib-5.0/

ifeq ($(OS),Windows_NT)
	LDFLAGS += -mwindows
endif

.PHONY: default def clean release debug web host itchio

default: release
def: release

release: CFLAGS += -DNDEBUG -O3
release: $(NAME)

debug: CFLAGS += -g
debug: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f $(NAME) $(OBJS) $(NAME).html $(NAME).js $(NAME).wasm $(NAME).mem $(NAME).data index.html

web: CFLAGS = -Os -Wall
# Here are the instructions to compile raylib for the web.
# https://github.com/raysan5/raylib/wiki/Working-for-Web-(HTML5)#21-command-line-compilation
web: WEB_RL = $(RL_HOME)/src/libraylib.a # raylib for the web
# Raylib has minshell.html and shell.html if one of those is not 
# selected it uses the default emscripten one.
web: WEB_SHELL = --shell-file $(RL_HOME)/src/shell.html
web: CC = emcc
web:
	$(CC) -o $(NAME).html $(SOURCES) $(CFLAGS) $(WEB_RL) \
	-I$(RL_HOME)/src/ \
	-L$(WEB_RL) \
	-s USE_GLFW=3 \
	$(WEB_SHELL) \
	-DPLATFORM_WEB \
	-DNDEBUG
	cp $(NAME).html index.html

host: web
	python3 -m http.server 7777

itchio: web
	zip $(NAME).zip index.html $(NAME).js $(NAME).data $(NAME).wasm $(NAME).mem
