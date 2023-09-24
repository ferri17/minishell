<p align="center">
	<img alt="GitHub Repo stars" src="https://img.shields.io/github/stars/ferri17/minishell?color=yellow" />
	<img alt="GitHub top language" src="https://img.shields.io/github/languages/top/ferri17/minishell" />
	<img alt="GitHub code size in bytes" src="https://img.shields.io/github/languages/code-size/ferri17/minishell?color=red" />
	<img alt="GitHub last commit (by committer)" src="https://img.shields.io/github/last-commit/ferri17/minishell" />
	<img alt="MacOS compatibility" src="https://img.shields.io/badge/macOS-compatible-brightgreen.svg" />
</p>

<h3 align="center">Minishell, recreating bash. (42 school project)</h3>

  <p align="center">
    The aim of this project is to create a shell using bash (bourne-again shell) as reference.
    <br/>
  </p>
</div>

<!-- ABOUT THE PROJECT -->
## About The Project
<img width="700" alt="minishell_screenshot" src="https://github.com/ferri17/minishell/assets/19575860/82977658-8af7-4e6e-98bf-af69bb83e507">

**Minishell is a Unix shell** really similar to bash but with some reduced functionalities. The project is written from scratch in C.
Some of the features are:
- Displays current directory when waiting for a new command.
- Working history of commands.
- Searches and launches the right executable (based on the PATH variable or using a
relative or an absolute path).
- Variable expansion of environment variables, tilde characater '~' and $?.
- Single and double quotes as bash would, single quotes prevent the shell from interpreting the metacharacters in the quoted sequence.
- Redirections:
	- '<' Redirects input.
	- '\>' Redirects output.
	- '\<<' + DELIMITER, reads the input until a line containing the
	delimiter is seen. It doesn’t update the history.
	- '\>>' Redirects output in append mode.

- Works with pipes '|'. The output of each command in the pipeline is connected to the input of the next command via a pipe.
- $? is updated with the exit code of the last executed command in the pipe.
- Handles signals ctrl-C, ctrl-D and ctrl-\ which behave like in bash.

<!-- GETTING STARTED -->
## Getting Started
In order to run the program first clone the repository:
```bash
git clone git@github.com:ferri17/minishell.git
```
Open the folder:
```bash
cd minishell/
```
Compile the program:
```bash
make
```
Run the program:
```bash
./minishell
```

<!-- Authors -->
## Authors
