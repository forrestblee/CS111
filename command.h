// UCLA CS 111 Lab 1 command interface

#include <stdbool.h>
#include <stdio.h>

typedef struct command_stream * command_stream_t;

typedef struct command * command_t;

typedef enum {
	NEW_LINE, COMMENT, OTHER,       
	WORD, SEMICOLON, PIPE, AND, OR,
	LEFT_PARENTHESIS, RIGHT_PARENTHESIS,
	GREATER_THAN, LESS_THAN
} node_type;

//setup shortcuts to main operators
#define SEQUENCE_CHAR       ';'
#define PIPE_CHAR           '|'
#define SUBSHELL_LEFT_CHAR  '('
#define SUBSHELL_RIGHT_CHAR ')'
#define INPUT_CHAR          '<'
#define OUTPUT_CHAR         '>'
#define COMMENT_CHAR        '#'
#define NEWLINE_CHAR        '\n'
#define AND_CHAR            '&'
#define OR_CHAR		    '|'
#define AND_COMMAND_STR     "&&"
#define OR_COMMAND_STR      "||"
#define WHITESPACE_CHAR     ' '
#define TAB_CHAR	    '\t'


//function prototypes for read-command.c
void loadFileIntoBuffer(int (*get_next_byte) (void *), void * get_next_byte_argument);
void printBuffer(void);
void seperateBuffer();
void setupBuffer(int (*get_next_byte) (void *), void * get_next_byte_argument);
int isCharacterValid(char testChar);
void createNodeAndInsertIntoStack(node_type type, int numberOfCharacters, int startingPointInBuffer);
void printSeperatedBuffer(void);
void validate();
/*
OperatorStackNode * getTopOfStack(OperatorStack * stack);
void push(OperatorStack * stack, node_type type);
OperatorStackNode * pop(OperatorStack * stack );
int empty(OperatorStack * stack );

CommandStackNode * cgetTopOfStack(CommandStack * stack);
void cpush(CommandStack * stack, command_t m_command );
CommandStackNode * cpop(CommandStack * stack );
int cempty(CommandStack * stack );

char* getString(Stack_Node * node);
bool isOperator(Stack_Node * node);
int findLine(int currentPos);
void emptyStackIntoCommandTree(CommandStack * cstack, 
OperatorStack * ostack, command_stream_t comStream);
void merge( CommandStack * cstack, OperatorStack * ostack);
int getPrecedence(node_type type);
command_t createCommand(void);
command_stream_t createCommandTrees(Stack_Node * head );
*/

/* Create a command stream from GETBYTE and ARG.  A reader of
   the command stream will invoke GETBYTE (ARG) to get the next byte.
   GETBYTE will return the next input byte, or a negative number
   (setting errno) on failure.  */
command_stream_t make_command_stream (int (*getbyte) (void *), void *arg);

/* Read a command from STREAM; return it, or NULL on EOF.  If there is
   an error, report the error and exit instead of returning.  */
command_t read_command_stream (command_stream_t stream);

/* Print a command to stdout, for debugging.  */
void print_command (command_t c);

/* Execute a command.  Use "time travel" if the flag is set.  */
void execute_command (command_t c, bool timetravel);

/* Return the exit status of a command, which must have previously
   been executed.  Wait for the command, if it is not already finished.  */
int command_status (command_t);
