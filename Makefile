UDIS86DIR =	./udis86/out

NAME	=	another-libpe.dll
NAME_EXE=	a.exe

CXXFLAGS=	-Wall -Wextra -std=c++11 -I$(UDIS86DIR)/include
DEBUG=1
ifeq ($(DEBUG), 1)
  CXXFLAGS	+=	-g
endif
LDFLAGS	=	-L$(UDIS86DIR)/lib -ludis86 -shared
EXE_LDFLAGS =	-L. -lanother-libpe -Wl,--image-base,0x8000000

SRCS	=	AddrAwareObject.cpp \
		Module.cpp \
		Module_loader.cpp \
		Section.cpp \
		ExportTable.cpp \
		ImportTable.cpp \
		ResourceTable.cpp \
		RelocTable.cpp \
		Text.cpp

OBJS	=	$(SRCS:.cpp=.o)

$(NAME): $(OBJS)
	g++ $(OBJS) -o $(NAME) $(LDFLAGS)

$(NAME_EXE): main.o
	g++ main.o -o $(NAME_EXE) $(EXE_LDFLAGS)

all: $(NAME) $(NAME_EXE)

clean:
	rm -f $(OBJS) main.o 2>nul

fclean: clean
	rm -f $(NAME) $(NAME_EXE) 2>nul

re: fclean all
