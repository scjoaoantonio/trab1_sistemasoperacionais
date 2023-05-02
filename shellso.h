#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <signal.h>

// Funcao para mostrar a mensagem inicial do shell
void msgInicial();

// Funcao para mostrar o diretorio atual
void mostrarDiretorio();

// Funcao para separar os argumentos de um comando
void separarArgumentos(char *comando, char **args);

// Funcao para separar os comandos de uma pipe
void separarComandosPipe(char *comando, char **comando1, char **comando2);

// Funcao para executar comandos em uma pipe
void execComandosPipe(char **comando1, char **comando2);

// Funcao para quebrar a string de entrada em argumentos
void quebraArgumentos(char **argumentos, int *numArgs, char *buffer, char *delimitador);

// Funcao para executar comandos simples
void execComandoSimples(char **args, int *background);

// Funcao para executar comandos com redirecionamento de entrada ou saida
void execComandoRedirecionamento(char **args, int *background);

// Funcao para executar comandos com pipes
void execComandoPipe(char **args, int *background);

// Funcao para executar comandos em background
void execComandoBackground(char **args);

// Funcao para lidar com o sinal SIGINT
void handlerSIGINT(int signal);

// Funcao para lidar com o sinal SIGTSTP
void handlerSIGTSTP(int signal);

#endif // SHELL_H