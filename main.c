// Bibliotecas necessarias 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

// Cores usadas
#define COR_AZUL   "\x1b[34m"
#define COR_VERDE  "\x1b[32m"
#define NEGRITO  "\e[1m"
#define RESET_COR  "\x1b[0m"

// Constante de 'true' para tornar mais legivel o codigo
#define ON 1

/*Mensagem inicial que aparece no topo do shell*/
void
msgInicial ()
{
  printf ("\n===== BEM VINDO AO SHELL =====\n\n");
}

/*Funcao para mostrar o diretorio em 
que o shell esta sendo executado*/
void
mostrarDiretorio ()
{
  char cwd[1024], username[1024];

  getlogin_r (username, 1024);	//Pega o nome do usuario

  if (getcwd (cwd, sizeof (cwd)) != NULL)
    {
      printf (COR_VERDE NEGRITO "%s@SO" RESET_COR, username);
      printf (NEGRITO ":" RESET_COR);
      printf (COR_AZUL NEGRITO "%s" RESET_COR, cwd);
    }
  else{
  	perror ("falha ao carregar diretorio\n");
  }
  printf ("$ ");
}

// Funcao para separar os argumentos de um comando
void
separarArgumentos (char *comando, char **args)
{
  char *arg;
  int i = 0;
  arg = strtok (comando, " ");
  while (arg != NULL)
    {
      args[i] = arg;
      arg = strtok (NULL, " ");
      i++;
    }
  args[i] = NULL;
}

// Funcao para separar os comandos de uma pipe
void
separarComandosPipe (char *comando, char **comando1, char **comando2)
{
  char *antes_pipe = strtok (comando, "|");
  char *depois_pipe = strtok (NULL, "|");

  // Separa pelo espaco os argumentos do primeiro comando e salva em comando1
  int i = 0;
  char *arg = strtok (antes_pipe, " ");
  while (arg != NULL)
    {
      comando1[i] = arg;
      arg = strtok (NULL, " ");
      i++;
    }
  comando1[i] = NULL;

  // Separa pelo espaco os argumentos do segundo comando e salva em comando2
  i = 0;
  arg = strtok (depois_pipe, " ");
  while (arg != NULL)
    {
      comando2[i] = arg;
      arg = strtok (NULL, " ");
      i++;
    }
  comando2[i] = NULL;
}

void
executarComandosPipe (char **comando1, char **comando2)
{
  int pipe_fd[2];
  pid_t pid, pid2;

  // Criar o pipe
  pipe (pipe_fd);
  // Cria o primeiro processo filho
  pid = fork ();
  if (pid == 0)
    {
      //Fecha a extremidade de leitura do pipe e redireciona a saida do processo pai para a entrada do pipe, usando a funcao dup2
      dup2 (pipe_fd[1], STDOUT_FILENO);
      close (pipe_fd[0]);

      // Executa o primeiro comando, se nao executar, da erro
      if (execvp (comando1[0], comando1) == -1)
	{
	  perror ("Erro: primeiro comando.");
	  exit (EXIT_FAILURE);
	}
    }

  // Cria o segundo processo filho 
  pid2 = fork ();

  if (pid2 == 0)
    {
      //Fecha a extremidade de leitura do pipe e redireciona a saida do processo filho para a entrada do pipe, usando a funcao dup2
      dup2 (pipe_fd[0], STDIN_FILENO);
      close (pipe_fd[1]);

      // Executa o segundo comando, se nao executar, da erro
      if (execvp (comando2[0], comando2) == -1)
	{
	  perror ("Erro: segundo comando.");
	  exit (EXIT_FAILURE);
	}
    }

  // Fecha as extremidades do pipe no processo pai
  close (pipe_fd[0]);
  close (pipe_fd[1]);

  // Espera pelos processos filhos terminarem
  waitpid (pid, NULL, 0);
  waitpid (pid2, NULL, 0);
}

/*Funcao que quebra a string de entrada em diversos 
argumentos com base em um delimitador*/
void
quebraArgumentos (char **argumentos, int *numArgs, char *buffer,
		  char *delimitador)
{
  char *token;
  token = strtok (buffer, delimitador);

  int i = 0;

  while (token)
    {
      argumentos[i] = malloc (sizeof (token) + 1);
      strcpy (argumentos[i], token);
      argumentos[i][strcspn (argumentos[i], "\n")] = 0;
      token = strtok (NULL, delimitador);
      i++;
    }
  argumentos[i] = NULL;
  *numArgs = i;
}

/*Redireciona a saida do comando para um arquivo
se o arquivo nao existe ele eh criado*/
void
redirecionaSaida (char **buffer)
{
  int aux;
  char *argv[100];

  quebraArgumentos (argv, &aux, buffer[0], " ");

  if (fork () == 0)
    {
      int redirecionaFd = open (buffer[1], O_WRONLY, S_IRUSR | S_IWUSR);	//Tags para escrita e acesso ao arquivo

      //Caso o arquivo nao exista eh incluida a tag de criacao de arquivo
      if (redirecionaFd < 0)
	{
	  redirecionaFd =
	    open (buffer[1], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	}

      //Verifica novamente se o arquivo foi criado com sucesso
      if (redirecionaFd < 0)
	{
	  perror ("Erro ao criar arquivo!");
	}

      dup2 (redirecionaFd, 1);

      execvp (argv[0], argv);
      perror (argv[0]);
      exit (1);
    }

  wait (NULL);
}


/*Funcao para executar um comando basico
sem redirecionador ou pipes*/
void
comandoBasico (char **argv)
{
  if (fork () > 0)
    {
      wait (NULL);
    }
  else
    {
      execvp (argv[0], argv);
      perror (argv[0]);		//Caso o comando nao exista emite uma mensagem de erro
      exit (1);
    }
}

int
main (void)
{
  int numArgs = 0;
  char *args[64];
  char comando[128];
  char *comando1[10];
  char *comando2[10];

  msgInicial ();

  while (ON)
    {
      mostrarDiretorio ();
      fflush (stdout);
      if (fgets (comando, sizeof (comando), stdin) == NULL)
	{
	  break;
	}

      // Remove o caractere de nova linha
      comando[strcspn (comando, "\n")] = 0;

      if (strcmp (comando, "fim") == 0)
	{
	  printf ("\n");
	  exit (1);
	}

      // Verifica se eh um comando simples ou com pipe
      else if (strstr (comando, "|") != NULL)
	{
	  // Se eh um comando com pipe, separa os comandos
	  separarComandosPipe (comando, comando1, comando2);
	  // Executa os comandos na pipe
	  executarComandosPipe (comando1, comando2);
	}
      else if (strstr (comando, "<="))
	{
	}			//Redirecionar entrada para arquivo

      else if (strchr (comando, '&'))
	{
	}			//Execucao em background

      else if (strstr (comando, "=>"))
	{			//Redirecionar saida para arquivo
	  quebraArgumentos (args, &numArgs, comando, "=>");
	  redirecionaSaida (args);
	}
      else
	{
	  // Se eh um comando simples, separa os argumentos e executa

	  quebraArgumentos(args,&numArgs,comando," "); 
	  //separarArgumentos (comando, args);
	  comandoBasico (args);
	}
    }
  return 0;
}
