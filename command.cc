
/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */
#include<time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include<iostream>
#include <string>
#include <cstring>
#include <signal.h>
#include <fcntl.h>
#include "command.h"
void printPrompt(int) {
		printf("\n");
		Command::_currentCommand.prompt();
}
void putToLog(int) {
	time_t t;   // not a primitive datatype
    time(&t);
    FILE *fptr;
    fptr = fopen("log", "a");
    // exiting program 
    if (fptr == NULL) {
        printf("Error!");
        exit(1);
    }
    fprintf(fptr, "BabyChild\t%s\n", ctime(&t));
    fclose(fptr);
    
}

 
SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );
	_dirName = getenv("PWD");
	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_append =0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) 
	{
		free( _outFile );
	}

	if ( _inputFile ) {
		free( _inputFile );
	}

	if ( _errFile) {
		free( _errFile );
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	
}

void
Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf( "\n\n" );
	
}

void
Command::execute()
{	// Shell implementation
	 
	// printf("X1:%p\n", _errFile);
	 //printf("555555555555X:%s", *_outFile);
	// printf("Hello world");
	// Don't do anything if there are no simple commands
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}
 
	if(!strcmp( _simpleCommands[0]->_arguments[0], "exit" )) {
			printf("Good bye Baby Bunny\n");
			exit(0);
	}
		//======================cd=========================//
	if(!strcmp( _simpleCommands[0]->_arguments[0], "cd" ))
	  {
	    int ret;
	    if(_simpleCommands[0]->_numberOfArguments == 1)
		{
	      	ret = chdir(getenv("HOME"));
			_dirName = getenv("HOME");
		}
	    else if (_simpleCommands[0]->_numberOfArguments == 2)
		{	
	      	ret = chdir(_simpleCommands[0]->_arguments[1]);
			char *cstr = new char[_dirName.length() + 1];
			strcpy(cstr, _dirName.c_str());
			// do stuff
		  	getcwd(cstr, _dirName.size());
			_dirName = cstr;
			delete [] cstr;
		} else {
			//Error handling
		}
	    if(ret < 0)
		{
	      	perror("cd");
		} else {
			//Error Handling
		}
	    clear();
	    prompt();
	    return;
	  }
	  //==================cd=====================//

	// ================PIPE and Redirection ====================================//
	int defaultin = dup( 0 );
	int defaultout = dup( 1 );
	int defaulterr = dup( 2 );
	
	int fdout, fdin, fderr;	  
	if (_inputFile != 0)
	 		{
			fdin = creat(_inputFile, 0666);
			dup2(fdin, 0);
			}
			else
			{
				fdin = dup(defaultin);
			}
			
	if (_errFile != 0) {
			fderr = creat(_errFile, 0666);
			dup2(fderr, 2);
			}else{
				fderr = dup(defaulterr);
			}
			//Newly added code
			
			fdout = dup(defaultout);
			
			//Ended

 	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		char * commandName = _simpleCommands[i]->_arguments[ 0 ];
		char ** arguments = _simpleCommands[i]->_arguments;
		dup2(fdin,0);
		dup2(fdout,1);
	    dup2(fderr,2);
		if (i == _numberOfSimpleCommands - 1 )
		{
 
				int dontDoError = 0;
			if (_outFile != 0) {
				if(_append == 0){
				fdout = creat(_outFile, 0666);
				}else{
					printf("lol\n");
				fdout = open(_outFile, O_CREAT|O_WRONLY|O_APPEND, 0666);
				dontDoError = 1;				
				}
				dup2(fdout, 1);
			}else{
				fdout = dup(defaultout);
			}

			if (_errFile != 0) {
				if (dontDoError == 1 ) {
					dup2(fdout, 2);
				} else {
				fderr = creat(_errFile, 0666);
				dup2(fderr, 2);
				}
			}else{
				fderr = dup(defaulterr);
			}
			
			

		}
		else
		{
			int fdpipe[2];
			pipe(fdpipe);
			fdout=fdpipe[1];
			fdin=fdpipe[0];
		}
		dup2(fdout,1);
		close(fdout);
			int pid = fork();
			if ( pid == -1 ) {
			perror( "ls: fork\n");
			exit( 2 );
			}
		
			if (_background == 0) {
				//printf("xD lol");
			if (pid == 0) {
			//Child Body
		
			//Print To The File Descriptor 1
			int statusCode = execvp(commandName, arguments);
			if (statusCode == -1) {
			printf("%s: command not found\n", commandName);
			exit(1);
				}
			}
			//Parent body
			waitpid(pid,NULL,0);
			} else if (_background == 1)
			{
				
				if (pid == 0) {
					//Child Body
				int grandChild = fork();
				if (grandChild == 0) {
					//Grandchild Body
					//Execute the big job
				int statusCode = execvp(commandName, arguments);
					if (statusCode == -1) {
					printf("%s: command not found\n", commandName);
					exit(1);
							}
				}
				//waitpid(grandChild, NULL, 0);
				 //check
				 exit(1);
			}
			
			}
	}
		dup2( defaultin, 0 );
        dup2( defaultout, 1 );
        dup2( defaulterr, 2 );

		close(defaultin);
		close(defaultout);
		close(defaulterr);

	clear();
	prompt();
}



void
Command::prompt()
{
	std::cout << "myshell/" << _dirName << ">";
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);

int 
main()
{
	signal(SIGINT, printPrompt);
	signal (SIGCHLD, putToLog);
	Command::_currentCommand.prompt();
	yyparse();
	return 0;
}

