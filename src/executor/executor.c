/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fbosch <fbosch@student.42barcelona.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/06 21:45:12 by fbosch            #+#    #+#             */
/*   Updated: 2023/09/24 15:28:17 by fbosch           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
	Executes commands, manages binaries and builtin-ins execution, pipes and forks.
	Each command has a pipe() and a fork(), therefore each command represents
	one process. To be able to connect processes through pipes, commands
	connect, with dup2(), their STDOUT to the write end of the pipe they created
	and the STDIN to the read end of the pipe created by the command that
	came before.

	If execute_commands() receives only one command and it is a built-in, it will
	be executed in the main process, no fork() will be used, this enables correct
	behaviour of commands like [cd], [env], [export], [unset], as we want to make
	the changes on the main process, not on the childs, just like bash.

	A default redirection is done to connect every process by pipes and later on,
	each process checks their own custom redirections like '<', '>', '<<' and '>>'.

	Before finishing it sets $? to the value of the last executed pipe. It can be
	retrieved using "echo $?".
*/
int	execute_commands(t_cmd *commands, t_env *envp, int *exit_s)
{
	t_pipe	data;
	int		i;

	if (init_data(&data, commands) == 1)
		return (1);
	dup_original_stds(&data.dup_stdin, &data.dup_stdout);
	if (data.n_cmds == 1 && is_builtin(commands))
		return (exec_one_builtin(commands, &data, envp, exit_s));
	i = 0;
	while (commands)
	{
		if (pipe(data.fd) == -1)
			return (perror_return(&data, EXIT_FAILURE, "Pipe"));
		data.pid[i] = fork();
		if (data.pid[i] == -1)
			return (perror_return(&data, EXIT_FAILURE, "Fork"));
		else if (data.pid[i] == 0)
			new_command(commands, &data, envp, exit_s);
		dup2(data.fd[0], STDIN_FILENO);
		close_pipe(data.fd[0], data.fd[1]);
		commands = commands->next;
		i++;
	}
	wait_childs(&data, exit_s);
	return (0);
}

void	wait_childs(t_pipe *data, int *exit_s)
{
	int	i;
	int	status;

	dup2(data->dup_stdin, STDIN_FILENO);
	dup2(data->dup_stdout, STDOUT_FILENO);
	i = 0;
	while (i < data->n_cmds)
	{
		waitpid(data->pid[i], &status, 0);
		i++;
	}
	free(data->pid);
	if (WIFEXITED(status))
		*exit_s = (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
	{
		if (WTERMSIG(status) == SIGINT)
			*exit_s = 130;
		else if (WTERMSIG(status) == SIGQUIT)
		{
			*exit_s = 131;
			ft_printf("Quit: 3\n");
		}
	}
}

void	new_command(t_cmd *commands, t_pipe *data, t_env *envp, int *exit_s)
{
	int		exit_code;
	char	*path;

	init_signals(CHILDS);
	manage_redirections(commands, data, FT_EXIT);
	if (is_builtin(commands))
		execute_builtins(commands->args, envp, exit_s, FT_EXIT);
	exit_code = search_path(commands->args[0], envp->env, &path);
	if (exit_code == CMD_NOT_FOUND)
		error_exit(data, CMD_NOT_FOUND, commands->args[0], MSSG_CMD_NOT_FOUND);
	else if (exit_code == NO_SUCH_FILE)
		error_exit(data, CMD_NOT_FOUND, commands->args[0], MSGG_NO_SUCH_FILE);
	else if (exit_code == IS_A_DIR)
		error_exit(data, CMD_NO_ACCESS, commands->args[0], MSGG_IS_A_DIR);
	else if (exit_code != 0)
		perror_exit(data, exit_code, commands->args[0]);
	if (execve(path, commands->args, envp->env) == -1)
		perror_exit(data, EXIT_FAILURE, "Execve");
}

int	exec_one_builtin(t_cmd *commands, t_pipe *data, t_env *envp, int *exit_s)
{
	if (manage_redirections(commands, data, FT_RETURN) == 1)
	{
		*exit_s = 1;
		dup2(data->dup_stdin, STDIN_FILENO);
		dup2(data->dup_stdout, STDOUT_FILENO);
		return (0);
	}
	free(data->pid);
	execute_builtins(commands->args, envp, exit_s, FT_RETURN);
	dup2(data->dup_stdin, STDIN_FILENO);
	dup2(data->dup_stdout, STDOUT_FILENO);
	return (0);
}
