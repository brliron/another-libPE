NAME	=	another-libpe.dll
NAME_EXE=	a.exe

CXXFLAGS=	-Wall -Wextra -std=c++11

# Compile with WITH_UDIS86=0 to disable disassembly support
WITH_UDIS86=1
ifeq ($(WITH_UDIS86), 1)
  UDIS86DIR =	./udis86/out
  CXXFLAGS  =	-DWITH_UDIS86 -I$(UDIS86DIR)/include
  LDFLAGS   =	-L$(UDIS86DIR)/lib -ludis86 -shared
endif

# Compile with WITH_EXECUTE=0 to disable exe file execution support
WITH_EXECUTE=1
ifeq ($(WITH_EXECUTE), 1)
  CXXFLAGS	+=	-DWITH_EXECUTE
endif

DEBUG=0
ifeq ($(DEBUG), 1)
  CXXFLAGS	+=	-g
endif
UNICODE=0
ifeq ($(UNICODE), 1)
  CXXFLAGS	+=	-DUNICODE -D_UNICODE
endif
LDFLAGS 	+=	-shared
EXE_LDFLAGS =	-L. -lanother-libpe -Wl,--image-base,0x8000000

SRCS	=	Module.cpp \
		Module_loader.cpp \
		Pointer.cpp \
		Section.cpp \
		ExportTable.cpp \
		ImportTable.cpp \
		ResourceTable.cpp \
		RelocTable.cpp \
		Text.cpp

OBJS	=	$(SRCS:.cpp=.o)

all: $(NAME) $(NAME_EXE)

$(NAME): $(OBJS)
	$(CXX) $(OBJS) -o $(NAME) $(LDFLAGS)

$(NAME_EXE): main.o
	$(CXX) main.o -o $(NAME_EXE) $(EXE_LDFLAGS)

clean:
	rm -f $(OBJS) main.o 2>nul

fclean: clean
	rm -f $(NAME) $(NAME_EXE) 2>nul

re: fclean all
