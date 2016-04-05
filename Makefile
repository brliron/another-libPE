UDIS86DIR =	/home/liron_b/self-compiled/udis86-1.7.2/out

NAME	=	a.exe

CXXFLAGS=	-Wall -Wextra -std=c++11 -I$(UDIS86DIR)/include

DEBUG=1
ifeq ($(DEBUG), 1)
  CXXFLAGS	+=	-g
endif
LDFLAGS	=	-L$(UDIS86DIR)/lib -ludis86 -Wl,--image-base,0x8000000

SRCS	=	main.cpp \
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

all: $(NAME)

clean:
	rm -f $(OBJS) 2>nul

fclean: clean
	rm -f $(NAME) 2>nul

re: fclean all
