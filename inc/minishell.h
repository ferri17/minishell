/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbosch <fbosch@student.42barcelona.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/14 13:55:12 by apriego-          #+#    #+#             */
/*   Updated: 2023/09/24 17:08:03 by fbosch           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

/*==============================	LIBRARIES	==============================*/

# include <errno.h>
# include <fcntl.h>
# include <libft.h>
# include <signal.h>
# include <stdio.h>
# include <sys/stat.h>
# include <readline.h>
# include <history.h>

/*=================================	MACROS	==================================*/

// Colours
# define GREENBASH "\033[1;38;2;180;235;31m"
# define NO_COL "\033[0m"
# define REDBASH "\033[1;38;2;255;0;0m"

// Signals
# define CTRL_C SIGINT
# define CTRL_SLASH SIGQUIT

// Mode
# define DEFAULT 0
# define CHILDS 1

// ASCII characters
# define C_VERTICAL_BAR 124
# define C_LESS 60
# define C_GREAT 62
# define C_DOLLAR 36
# define C_ONE_QUOTE 39
# define C_TWO_QUOTE 34

// General errors
# define MSSG_INVALID_ARGS "Invalid arguments: Usage [./minishell]\n"
# define MSSG_MEMORY_ERROR "Memory error, please free space and attempt again\n"
# define MSSG_EXECUTOR_ERROR "Error during command execution\n"
# define MSSG_CMD_NOT_FOUND "command not found"
# define MSGG_NO_SUCH_FILE "No such file or directory"
# define MSGG_IS_A_DIR "is a directory"
# define MSSG_EXPORT_ERR "minishell: export: `%s': not a valid identifier\n"
# define MSSG_SHLVL_ERR "minishell: warning: shell level (%d) too high, \
resetting to 1\n"
# define MSSG_EXIT_NUM_ERR "minishell: exit: %s: numeric argument required\n"
# define MSSG_EXIT_ERR "minishell: exit: too many arguments\n"

// Parser syntax errors
# define SYNTAX_ERR 42
# define SYNTAX_NL 0
# define SYNTAX_PIPE 1
# define SYNTAX_LESS 2
# define SYNTAX_GREAT 3
# define SYNTAX_LESS_LESS 4
# define SYNTAX_GREAT_GREAT 5

// Command errors
# define CMD_NO_ACCESS 126
# define CMD_NOT_FOUND 127
# define IS_A_DIR 400
# define NO_SUCH_FILE 401

// File modes
# define FILE_IN 0
# define FILE_OUT 1

// Exit flags
# define FT_EXIT 0
# define FT_RETURN 1

/*===============================	STRUCTURES	==============================*/

typedef enum s_redirect_type
{
	REDIRECT_NONE = 0,
	IN,
	HERE_DOC,
	OUT_TRUNC,
	OUT_APPEND,
}		t_type;

typedef enum s_token
{
	NONE = 0,
	PIPE,
	GREAT,
	GREAT_GREAT,
	LESS,
	LESS_LESS
}		t_token;

typedef struct s_quote
{
	int	two;
	int	one;
}		t_quote;

typedef struct s_lexer
{
	char			*word;
	t_token			token;
	struct s_lexer	*prev;
	struct s_lexer	*next;
}		t_lex;

typedef struct s_in_out
{
	t_type			type;
	char			*file;
	struct s_in_out	*next;
}		t_io;

typedef struct s_cmd
{
	char			**args;
	t_io			*redirect;
	struct s_cmd	*prev;
	struct s_cmd	*next;
}		t_cmd;

typedef struct s_pipe
{
	int		n_cmds;
	int		fd_in;
	int		fd_out;
	int		fd[2];
	int		dup_stdin;
	int		dup_stdout;
	pid_t	*pid;
	char	*path;
}		t_pipe;

typedef struct s_env
{
	char	**env;
	char	**export;
}		t_env;

/*==============================  FUNCTIONS  =============================*/

// Holds minishell loop: MiniShell$>....
/*------------------------------	ENTRY	-------------------------------*/
void	generate_terminal(t_env *env);
int		string_to_command(char *str, t_cmd **cmd, t_env *env, int *exit_s);
char	*generate_entry(t_env *env);
char	*ft_joincolors(char *array);

// Builtint shell commands
/*----------------------------		BUILTINS	-----------------------------*/
int		ft_export(char **command, t_env *env);
int		ft_unset(char **command, t_env *env);
int		ft_echo(char **command);
int		ft_pwd(void);
int		ft_env(char **env);
int		ft_cd(char **command, char ***env);
void	ft_exit(char **args, int *exit_s);
void	ft_generate_new_env(char **command, char **env, int j, int i);
int		ft_test_options_echo(char **command, int *i);
int		check_format_export(char *str);
char	**ft_add_env(char *str, char **env);
char	**ft_add_export(char *str, char **export);
t_env	*ft_add_env_export(char *str, t_env *env);
char	**ft_join_env(char *str, char **env);
char	**ft_join_export(char *str, char **export);
char	*ft_strjoin_env(char *env, char *str);
char	*ft_strjoin_export(char *export, char *str);
char	*ft_strdup_join(char *str);
char	*ft_strdup_export(char *envp);
int		ft_chdir(char *direct, char ***env);
char	*obtain_pwd(char **env);
char	*obtain_oldpwd(char **env);
t_env	*ft_join_env_export(char *str, t_env *env);
int		do_oldpwd(char ***env);

/*-----------------------------	HEREDOC --------------------------------*/
int		heredoc(t_cmd *commands);

// Converts input string to tokens for minishell to interpret
/*-----------------------------		TOKENIZER	----------------------------*/
int		tokenizer(char *str, t_lex **lexer);
int		initialize_lexer_node(t_lex *new, char *str, int i);
int		create_word(char *str, int i, t_lex *new);
int		create_token(char *str, int i, t_lex *new);
int		ft_isspace(int c);
int		ft_isquote(int c);
int		ft_isreserved(int c);
void	print_tokens(t_lex *lexer, char *str);
t_lex	*lexer_lstnew(void);
void	lexer_lstadd_back(t_lex **lst, t_lex *new);
void	lexer_lstclear(t_lex **lst);
int		lexer_lstsize(t_lex *lst);
t_lex	*lexer_lstlast(t_lex *lst);

// Reads from parser structure and expands variables
/*------------------------------  EXPANSOR  ------------------------------*/

int		expansor(t_cmd *def, char **env, int status);
char	*expand(char *str, char **env);
int		ft_omit_var(char *var);
int		calc_len_expan(char *str, char **env, int status);
char	*obtain_var(char *str);
void	init_quote(t_quote *quote);
void	check_expand(char *word, int exit, char *str, char **env);
void	find_quote(t_quote *quote, int i, char *str);
int		expansor_files(t_cmd *commands, char **env, int status);
int		expansor_files_aux(t_cmd *commands, char **env, int i);
int		expand_file(char *path, char **env, int exit);
int		calc_len_file(char *path);
int		fill_aux(char *path, char **file);
int		expand_vars_file(char **file, char **aux, char **env, int exit);
int		rewrite_file(char *path, char **aux);
int		calc_len_value_expan(char *str, char **env, int status, int *len);
void	expand_var(char *word, char *value, char *str, int len);
void	check_expand_file(char *word, int exit, char *str, char **env);
int		calc_len_expan_file(char *str, char **env, int status);
char	*create_value(char *word, int i, char **env, int exit);
int		check_tilde(char *w, int i);
int		check_dollar(char *word, int i);
int		ft_change_redirection(t_io *redirect, char *str);
int		expand_redirections(t_io *redir, char **env, int ex_s);

// Converts token list (lexer) into a simple commands list
/*---------------------------- 	PARSER	-------------------------------*/
int		parser(t_cmd **commands, t_lex **lexer, int *exit_s);
int		create_simple_command(t_lex **head, t_cmd *cmd);
int		fill_command(t_lex **head, t_io **redirect, t_cmd *cmd);
int		add_redirection(t_io **redirect, t_lex **head);
int		count_arguments(t_lex *lexer);
void	parser_error(int error);
int		check_duplicate_tokens(t_lex *lexer);
int		check_pipe_error(t_lex *lexer);
int		check_syntax_error(t_lex *lexer, int *exit_s);
t_cmd	*parser_lstnew(void);
void	parser_lstadd_back(t_cmd **lst, t_cmd *new);
void	parser_lstclear(t_cmd **lst);
int		parser_lstsize(t_cmd *lst);
t_cmd	*parser_lstlast(t_cmd *lst);
t_io	*redirect_lstnew(void);
void	redirect_lstadd_back(t_io **lst, t_io *new);
void	redirect_lstclear(t_io **lst);
int		redirect_lstsize(t_io *lst);
t_io	*redirect_lstlast(t_io *lst);

// Receives clean arguments in a t_cmd* linked list and manages execution
/*/*---------------------------	EXECUTOR ---------------------------*/

int		execute_commands(t_cmd *commands, t_env *envp, int *exit_s);
void	new_command(t_cmd *commands, t_pipe *data, t_env *envp, int *exit_s);
int		exec_one_builtin(t_cmd *cmd, t_pipe *data, t_env *envp, int *exit_s);
void	wait_childs(t_pipe *data, int *exit_s);
void	execute_builtins(char **args, t_env *envp, int *exit, int exit_flag);
int		is_builtin(t_cmd *commands);
int		search_path(char *cmd, char **envp, char **path);
int		try_paths(char **full_path, char *cmd, char **path);
int		try_local_path(char *cmd, char **path);
int		try_absolute_path(char *cmd, char **path);
int		search_extra_path(char *cmd, char *envp, char **path, int exit_status);
int		dup_custom_redirections(t_pipe *data, t_io *temp);
int		manage_redirections(t_cmd *commands, t_pipe *data, int out);
void	unlink_heredocs(t_io *redirection);
int		init_data(t_pipe *data, t_cmd *commands);
void	dup_original_stds(int *in, int *out);
void	close_pipe(int in, int out);
void	perror_exit(t_pipe *data, int exit_code, char *error);
void	error_exit(t_pipe *data, int exit_code, char *name, char *error);
int		perror_return(t_pipe *data, int exit_code, char *error);

// Handle signals
/*---------------------------  	SIGNALS   	------------------------------*/

int		init_signals(int mode);

// General utility functions
/*--------------------------      UTILS     -------------------------------*/
char	*find_home(char **env);
int		valid_command(char *command);
int		ft_strcmp(const char *s1, const char *s2);
int		ft_strlen_chr(char *str, char c);
void	ft_print_matrix(char **matrix, int i);
t_env	*ft_dup_matrix_env(char **envp);
void	ft_matrix_free(char **matrix);
void	ft_print_export(char **matrix);
int		contain_env(char **env, char *str);
int		ft_strcmp_env(char *env, char *str);
char	**ft_free_matrix(char **matrix, int j);
char	*ft_strdup_export(char *envp);
int		ft_strcmp_export(char *export, char *str);
int		ft_strcmp_nocase(char *s1, char *s2);
char	*ft_correct_shlvl(char *env);

#endif
