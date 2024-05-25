NAME			=	minishell

CC				=	gcc
CFLAGS			=	-Wall -Wextra -Werror -lreadline
RM				=	rm -rf

SRCS			=	minishell.c \
					free1.c \
					free2.c \
					env1.c \
					env2.c \
					env3.c \
					utils.c \
					special_expressions.c \
					cmd1.c \
					cmd2.c \
					path.c \
					heredoc.c \
					export.c \
					echo.c \
					signal.c \
					exit.c \
					cd.c \
					pwd.c \
					init.c \
					token_control.c \
					redirection.c \
					exec.c

LIBFT_PATH		=	./libft
LIBFT			=	$(LIBFT_PATH)/libft.a

all:				libft $(NAME)

$(NAME): $(SRCS)
					@$(CC) $(CFLAGS) -o $(NAME) $(SRCS) $(LIBFT)


run:
	@make re
	@./minishell
	@make fclean

v:
	@make re
	@valgrind --leak-check=full ./minishell
	@make fclean

libft:
	@make -C $(LIBFT_PATH)

clean:
	@make -C $(LIBFT_PATH) clean
	@$(RM) minishell.dSYM

f:	fclean

fclean: clean
	@make -C $(LIBFT_PATH) fclean
	@$(RM) $(NAME)

n:
	norminette $(SRCS) minishell.h

re:					fclean all

.PHONY:				all clean fclean re libft
