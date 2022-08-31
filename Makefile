NAME	=	ft_ping
SRCS	=	srcs/arg.c \
		srcs/err_resp.c \
		srcs/ft_ping.c \
		srcs/ping_pong.c \
		srcs/pngs.c \
		srcs/signals.c \
		srcs/stats.c \
		srcs/utils.c
OBJS	=	$(SRCS:.c=.o)
CC	=	gcc
CFLAGS	=	-Wall -Wextra -Werror
RM	=	rm -rf

all	:	$(NAME)

$(NAME)	:	$(OBJS)
		$(CC) $(CFLAGS) $^ -o $@
		sudo setcap 'cap_net_admin,cap_net_raw+ep' $@
%.o	:	%.c
		$(CC) $(CFLAGS) -c $< -o $@

clean	:
		$(RM) $(OBJS)

fclean	:	clean
		$(RM) $(NAME)

re	:	fclean all
