/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   entry.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbosch <fbosch@student.42barcelona.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/22 12:13:18 by apriego-          #+#    #+#             */
/*   Updated: 2023/09/24 15:28:11 by fbosch           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <minishell.h>

/*
	Holds Minishell loop, it's the backbone of the program. From this function
	commands written by the user are tokenized, parsed, expanded and executed.
	In case of error it doesn't close Minishell, instead it gives control back 
	to the user to write a new command.

	Process of a commands:

	(string)Input -> Tokenizer -> Parser -> Expansor -> Executor

	Example:

	"echo $USER | wc -c" -> [echo] [$USER] [PIPE] [wc] [-c] ->
	-> [args[][] = "echo", "$USER", "NULL"] - [args[][] = "wc", "-c", NULL] ->
	-> [args[][] = "echo", "username", "NULL"] - [args[][] = "wc", "-c", NULL] ->
	-> STDOUT: 9
*/
void	generate_terminal(t_env *env)
{
	int		exit_s;
	t_cmd	*cmd;
	char	*str;

	exit_s = 0;
	str = generate_entry(env);
	while (str)
	{
		add_history(str);
		if (string_to_command(str, &cmd, env, &exit_s) == 0 && cmd != NULL)
		{
			init_signals(CHILDS);
			if (execute_commands(cmd, env, &exit_s) == 1)
				ft_printf_fd(STDERR_FILENO, MSSG_EXECUTOR_ERROR);
			init_signals(DEFAULT);
		}
		parser_lstclear(&cmd);
		free(str);
		str = generate_entry(env);
	}
	ft_matrix_free(env->env);
	ft_matrix_free(env->export);
	exit(exit_s);
}

char	*ft_joincolors(char *array)
{
	char	**aux;
	char	*str;

	aux = malloc(2 * sizeof(char *));
	if (!aux)
		return (NULL);
	aux[0] = ft_strjoin(GREENBASH, array);
	if (!aux)
		return ((char *)ft_free_matrix(aux, ft_array_len(aux)));
	aux[1] = ft_strjoin(aux[0], "$>");
	if (!aux)
		return ((char *)ft_free_matrix(aux, ft_array_len(aux)));
	str = ft_strjoin(aux[1], NO_COL);
	if (!str)
		return ((char *)ft_free_matrix(aux, ft_array_len(aux)));
	ft_free_matrix(aux, 2);
	return (str);
}

char	*generate_entry(t_env *env)
{
	char	*entry;
	char	*aux;
	char	*split;
	char	*str;

	aux = getcwd(NULL, 0);
	if (!aux)
		entry = ft_strdup("Oooops where is home :( :");
	else
	{
		if (find_home(env->env) != NULL && ft_strcmp(find_home(env->env),
				aux) == 0)
			entry = ft_joincolors("~");
		else if (ft_strcmp(aux, "/") == 0)
			entry = ft_joincolors(aux);
		else
		{
			split = ft_strrchr(aux, '/');
			entry = ft_joincolors(split + 1);
		}
		if (!entry)
			return (free(aux), NULL);
	}
	str = readline(entry);
	return (free(aux), free(entry), str);
}

int	string_to_command(char *str, t_cmd **commands, t_env *env, int *exit_s)
{
	int		status;
	t_lex	*lexer;

	lexer = NULL;
	*commands = NULL;
	status = tokenizer(str, &lexer);
	if (status == 0)
		status = parser(commands, &lexer, exit_s);
	if (status == 0)
		status = expansor(*commands, env->env, *exit_s);
	if (status == 0)
		status = heredoc(*commands);
	if (status == 0)
		status = expansor_files(*commands, env->env, *exit_s);
	if (status == 1)
		ft_printf_fd(STDERR_FILENO, MSSG_MEMORY_ERROR);
	lexer_lstclear(&lexer);
	return (status);
}
