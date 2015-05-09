// UCLA CS 111 Lab 1 command reading

#include "command.h"
#include "command-internals.h"
#include <error.h>
#include "alloc.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

/* FIXME: You may need to add #include directives, macro definitions,
   static function definitions, etc.  */

/* FIXME: Define the type 'struct command_stream' here.  This should
   complete the incomplete type declaration in command.h.  */

struct token{
	node_type m_type;
	char* tokenString;
	int lineNumber;
	int newLines; // 0 for non-new line, 1 for single line, 2 or 2+ lines
};

typedef struct token Token;

struct stack_node{
	Token n_token;
	struct stack_node * next;
	struct stack_node * prev; 
};

typedef struct stack_node Stack_Node;

char * buffer;
Stack_Node * head_node = NULL;
Stack_Node * current_node = NULL;
Stack_Node * tail_node = NULL;
int line_number;
int multiLine;
//define stack for operators 
typedef struct operatorStackNode OperatorStackNode;

struct operatorStackNode{
	node_type n_type;
	OperatorStackNode * next;
	OperatorStackNode * prev;
};

typedef struct operatorStack OperatorStack; 

struct operatorStack{
	OperatorStackNode * head;
	OperatorStackNode * tail;
	OperatorStackNode * current;
	int numItemsInStack;
};

OperatorStackNode * getTopOfStack(OperatorStack * stack){
        if(stack->head != NULL)
                return stack->head;
        else
                return NULL; //should not ever hit this
}

// head is the top of the stack 
void push(OperatorStack * stack, node_type type){
	OperatorStackNode * node = (OperatorStackNode*) checked_malloc( sizeof( OperatorStackNode));
	node->n_type = type;
        if(stack->head != NULL){
                node->next = NULL;
                node->prev = stack->head;
                stack->head->next = node;
                stack->head=node;
                stack->current=node;
        }
        else{
                node->next = NULL;
                node->prev = NULL;
                stack->head=node;
                stack->current=node;
                stack->tail=node;
        }
        stack->numItemsInStack += 1;
}

//free memory? havent implemented
OperatorStackNode * pop(OperatorStack * stack ){
         OperatorStackNode * temp;
         if(stack->head != NULL){
		if(stack->head->prev == NULL)
                {
			temp = stack->head;
                        stack->head=NULL;
                        stack->current=NULL;
                        stack->tail=NULL;
                        stack->numItemsInStack -= 1;
		}
		else{
			temp = stack->head;
                        stack->head->prev->next = NULL;
                        stack->head=stack->head->prev;
                        stack->current=stack->head;
                        stack->numItemsInStack -= 1;
		}
        }
        else{
        	error(1, 0, "Tried to pop empty stack"); 
        }
        return temp;
};

int empty(OperatorStack * stack ){
        return ((stack->numItemsInStack == 0)? 1: 0);
}

//define stack for commands  when creating the command trees
typedef struct commandStackNode CommandStackNode;

struct commandStackNode{
	command_t command;
	CommandStackNode * next;
	CommandStackNode * prev;	
};

typedef struct commandStack CommandStack;

struct commandStack{
	CommandStackNode * head;
	CommandStackNode * tail;
	CommandStackNode * current;
	int numItemsInStack;
};


CommandStackNode * cgetTopOfStack(CommandStack * stack){
	if(stack->head != NULL)
		return stack->head;
	else
		return NULL; //should not ever hit this
}

// head is the top of the stack 
void cpush(CommandStack * stack, command_t m_command ){	
	CommandStackNode * node = (CommandStackNode*) checked_malloc( sizeof(CommandStackNode));
        node->command = m_command;
	node->command->status=-1;
	if(stack->head != NULL){
		node->next = NULL;
		node->prev = stack->head;
		stack->head->next = node;
		stack->head=node;
		stack->current=node;	
	}
	else{
		node->next = NULL;
		node->prev = NULL;
		stack->head=node;
		stack->current=node;
		stack->tail=node;
	}
	stack->numItemsInStack += 1;		
}

//free memory? havent implemented
CommandStackNode * cpop(CommandStack * stack ){
	CommandStackNode * temp;
	if(stack->head != NULL){
		if(stack->head->prev == NULL)
		{
			temp = stack->head;
			stack->head=NULL;
			stack->current=NULL;
			stack->tail=NULL;
			stack->numItemsInStack -= 1;
		}
		else{
			temp = stack->head;
                	stack->head->prev->next = NULL;
                	stack->head=stack->head->prev;
                	stack->current=stack->head;
			stack->numItemsInStack -= 1;
		}
        }
	else{
		error(1, 0, "Tried to pop empty stack"); 
	}
	return temp;
};

int cempty(CommandStack * stack ){
	return ((stack->numItemsInStack == 0)? 1: 0);
}

int isCharacterValid(char testChar){
	if(isalnum(testChar))
		return 1;
	else if(testChar == '!' || testChar == '%' ||
	        testChar == '+' || testChar == '-' ||
		testChar == '.' || testChar == '/' || 
		testChar == ':' || testChar == '@' ||
		testChar == '^' || testChar == '_' ||
		testChar == ',')
		return 1;
	else 
		return 0;
}
void loadFileIntoBuffer(int (*get_next_byte) (void *), void * get_next_byte_argument){
	size_t CurrentSize = 2048; //initialize the buffer size to some value
	size_t counter = 0; // to step through the buffer
	buffer = (char*) checked_malloc(CurrentSize);
	char currentChar = get_next_byte(get_next_byte_argument);
	while(currentChar != EOF)
	{
		buffer[counter]=currentChar;
		counter++;
		if(CurrentSize == counter-1){
			buffer = (char*) checked_grow_alloc(buffer, &CurrentSize);
		}
		currentChar = get_next_byte(get_next_byte_argument);
	}	
	//end buffer properly
	buffer[counter]='\0'; 	
}
void printBuffer(void){
	printf("The following is a print out of the buffer:\n%s", buffer);
}
void printSeperatedBuffer(void){
	Stack_Node * counter =  (Stack_Node*) checked_malloc(sizeof(Stack_Node));
	counter = head_node;
	printf("The following is the seperated buffer:\n");
	while(counter != NULL){
		if((counter->n_token).m_type == NEW_LINE )
		{
			printf("newline    no:%d\n", (counter->n_token).newLines);
			counter = counter->next;
			continue;
		}
		printf("%s\n", (counter->n_token).tokenString);
		counter = counter->next;
	}
	printf("Finished print seperated buffer\n");
}
void createNodeAndInsertIntoStack(node_type type, int numberOfCharacters, int startingPointInBuffer){
	//create new node if space
	Stack_Node * temp_node = (Stack_Node*) checked_malloc(sizeof(Stack_Node));
	(temp_node->n_token).m_type=type;
	temp_node->next = temp_node->prev = NULL;
	(temp_node->n_token).lineNumber=line_number;
	(temp_node->n_token).tokenString = (char*) checked_malloc((sizeof(char)*numberOfCharacters)+1); //extra one for terminating \0
	(temp_node->n_token).newLines = multiLine;  
	int j;
	for(j =0; j <= numberOfCharacters; j++){
		if(j==numberOfCharacters){
			(temp_node->n_token).tokenString[j]= '\0';
			break;	
		}
		(temp_node->n_token).tokenString[j]= buffer[startingPointInBuffer+j];
	}		
	//now insert into stack
	if(current_node != NULL){
		current_node->next= temp_node;
		temp_node->prev = current_node;
		current_node = temp_node;
		tail_node = current_node;
	}
	else{
		current_node=temp_node;
		tail_node=temp_node;
		head_node=temp_node;
	}
}


bool isOperator(Stack_Node * node)
{
	node_type type= (node->n_token).m_type;
  switch (type)
    {
    case PIPE:
    case AND:
    case SEMICOLON:
    case LESS_THAN:
    case GREATER_THAN:
      return true;
      break;
    default:
	break;
    }
  return false;
}

char* getString(Stack_Node * node)
{
	return (node->n_token).tokenString;
}

int findLine(int currentPos)
{
  int i;
  int lineNo = 1;
  for (i = 0; i < currentPos; i++)
    {
      if (buffer[i] == '\n')
	lineNo++;
    }
  return lineNo;
}
// we could probably put this into the separateBuffer command
void validate()
{
	
	// parenthesis should be dealt with in separateBuffer, open parens must = closed parens per commmand tree

    // if input/output arrows, we must make sure there are valid commands on either side
    // additionally, we must make sure any additional input/out on the other sides are not facing inward
	// ie a>b<c, or the same way ie a>b>c/a<b<c (a<b>c is valid)
	// IF error, then output error(1, 0, "message here", [errant values, separated by commas]);
	Stack_Node * counter =  (Stack_Node*) checked_malloc(sizeof(Stack_Node));
	counter = head_node;
	node_type type;
	int parenBalance = 0; // balance of parenthesis must equal 0 at end
	int lastParen = 0;
	int lineNumber = 0;
	while (counter != NULL)
	{
		type = (counter->n_token).m_type; 
		Stack_Node * before = (Stack_Node*) checked_malloc(sizeof(Stack_Node));
		Stack_Node * after = (Stack_Node*) checked_malloc(sizeof(Stack_Node));		
		before = counter->prev;
		after = counter->next; 
		node_type before_type, after_type;
		if (before) before_type = (before->n_token).m_type;
		if (after) after_type = (after->n_token).m_type;
		lineNumber = (counter->n_token).lineNumber;
		switch (type)
		{
			// if semicolon, must confirm it is not first operator on line, and the following condition
			// need to deal with all subshell shit
			case (SEMICOLON):
				if (before)
					{
						if (before_type == WORD || before_type == RIGHT_PARENTHESIS) {}
						//else if (before_type == NEW_LINE)
						//	error(1, 0, "%d: syntax error: no valid operand before %s", lineNumber, getString(counter));	
						else
							error(1, 0, "%d: syntax error: %s operator before %s with no operand in between", 
											lineNumber, getString(before), getString(counter));
					}
						else 
							error(1, 0, "%d: syntax error: no valid operand before %s", lineNumber, getString(counter));
				break;
			// if pipe, before must be word or ) , after must be word or (
			case (PIPE):
			case (OR):
			case (AND):
				if (before)
				{
					if (before_type == WORD || before_type == RIGHT_PARENTHESIS) {}
					//else if (before_type == NEW_LINE)
					//	error(1, 0, "%d: syntax error: no valid operand before %s", lineNumber, getString(counter));	
					else
						error(1, 0, "%d: syntax error: %s operator before %s with no operand in between", 
										lineNumber, getString(before), getString(counter));
				}
					else 
						error(1, 0, "%d: syntax error: no valid operand before %s", lineNumber, getString(counter));
				if (after)
				{
					if (after_type == WORD || after_type == LEFT_PARENTHESIS || after_type == NEW_LINE ) {}
					//else if (after_type == NEW_LINE)
					//	error(1, 0, "%d: syntax error: no valid operand after %s", lineNumber, getString(counter));	
					else
						error(1, 0, "%d: syntax error: %s operator after %s with no operand in between", 
										lineNumber, getString(after), getString(counter));
				}
					else 
						error(1, 0, "%d: syntax error: no valid operand after %s", lineNumber, getString(counter));	
			
				break;
				// if input/output arrows, we must make sure there are valid commands on either side
				// additionally, we must make sure any additional input/out on the other sides are not facing inward
				// ie a>b<c, or the same way ie a>b>c/a<b<c (a<b>c is valid)
			
			
			// we ignored parenthesis for now
			// need to deal with incorrect files being passed in
			case (GREATER_THAN):
			case (LESS_THAN):
				if (before)
				{
					if (before_type == WORD || before_type == RIGHT_PARENTHESIS) {}
					//else if (before_type == NEW_LINE)
					//	error(1, 0, "%d: syntax error: no valid operand before %s", lineNumber, getString(counter));	
					else
						error(1, 0, "%d: syntax error: %s operator before %s with no operand in between", 
									lineNumber, getString(before), getString(counter));
				}
				else 
						error(1, 0, "%d: syntax error: no valid operand before %s", lineNumber, getString(counter));
				
				if (after)
				{
					if (after_type == WORD || after_type == LEFT_PARENTHESIS){}
					else if (after_type == NEW_LINE)
					 	error(1, 0, "%d: syntax error: no valid operand after %s", lineNumber, getString(counter));	
					else	
						error(1, 0, "%d: syntax error: %s operator after %s with no operand in between", 
									lineNumber, getString(after), getString(counter));
				}
				else 
					error(1, 0, "%d: syntax error: no valid operand after %s", lineNumber, getString(counter));	
				//FILE *file;
				//if ((file = fopen(getString(after), "r")))
				//	fclose(file);
				//else 
				//	error(1, 0, "%d : Input redirect needs an existing filename following it, file %s not found", 
				//	lineNumber, getString(after));
		
				break;
			case (COMMENT):
				if (before && before_type != NEW_LINE)
					error(1, 0, "%d: syntax error: %s operand/operator not allowed before %s", lineNumber, getString(before), getString(counter));	
				break;
			case (LEFT_PARENTHESIS):
				parenBalance++;
				lastParen = lineNumber;
				// before left paren, anything allowed except right paren
				if (before && (before_type == RIGHT_PARENTHESIS || before_type == WORD))
						error(1, 0, "%d: syntax error: %s operator before %s not valid", 
						lineNumber, getString(before), getString(counter));
				// after left paren, only word, another left paren, or new line allowed
				if (after)
				{
					if (after_type == WORD || after_type == LEFT_PARENTHESIS || after_type == NEW_LINE) {}
					else
						error(1, 0, "%d: syntax error: %s operator after %s not valid", 
						lineNumber, getString(after), getString(counter));
				}
				else	// next char is null
					error(1, 0, "%d: syntax error: no valid operand after %s", lineNumber, getString(counter));	
				break;
			case (RIGHT_PARENTHESIS):
				parenBalance--;
				lastParen = lineNumber;
				// before right paren, only word, another right paren, or new line allowed
				if (before)
				{
					if (before_type == WORD || before_type == RIGHT_PARENTHESIS || before_type == NEW_LINE) {}
					else
						error(1, 0, "%d: syntax error: %s operator before %s not valid", 
						lineNumber, getString(before), getString(counter));
				}
				else	// prev char is null
					error(1, 0, "%d: syntax error: no valid operand before %s", lineNumber, getString(counter));
					
				// after right paren, anything allowed except left paren
				if (after && ( after_type == LEFT_PARENTHESIS ||  after_type == WORD))
						error(1, 0, "%d: syntax error: %s operator after %s not valid", 
						lineNumber, getString(after), getString(counter));
				break;
				//if both sides of the word are redirects, they must be facing outward
				//ie a>b<c, or the same way ie a>b>c/a<b<c (a<b>c is valid, and will be checked for)
			case (WORD):
				if (before && after && (before_type == LESS_THAN 
					|| before_type == GREATER_THAN) && (after_type == LESS_THAN 
					|| after_type == GREATER_THAN))
				{
					if (before_type == LESS_THAN && after_type == GREATER_THAN){}
					else
						error(1, 0, "%d: syntax error: invalid redirects", lineNumber);
				}
				break;
			case (NEW_LINE):
				if (before)
				{
					if (before_type == GREATER_THAN || before_type == LESS_THAN)
						error(1, 0, "%d: syntax error: no valid operand after %s", lineNumber, getString(before));
					if(( before_type == AND || before_type == OR || before_type == PIPE) && (after_type != WORD && after_type != LEFT_PARENTHESIS) )
						error(1, 0, "%d: syntax error: no valid operand after %s", lineNumber, getString(before));
				}
					
				if (after)
				{ 	
					if (after_type == WORD || after_type == COMMENT
					|| after_type == LEFT_PARENTHESIS || after_type == RIGHT_PARENTHESIS)
					{}
					else
					error(1, 0, "%d: syntax error: no valid operand before %s", lineNumber, getString(after));
				}
					
				break;
			case (OTHER):
				//error(1, 0, "%d: syntax error: %s character not recognized by script", lineNumber, getString(counter));
				break;
			default:
				break;
					
		}
		counter = counter->next;
	}
	if (parenBalance != 0)
		error(1, 0, "%d: syntax error: open and close parenthesis do not match", lastParen);
}
void seperateBuffer(){
	node_type type;
	line_number = 1;
	int counter = 0;
	char currentChar, nextChar;
	int temp_counter2, temp_counter3, temp_counter4, temp_counter5, letter;
	//deal with case where  if (buffer[bufferIteratorT] == '\0')
	for(;(currentChar = buffer[counter]) != '\0'; counter++){
		multiLine = 0;
		nextChar = buffer[counter+1];
		//we have a word to process
		if(isCharacterValid(currentChar)){
			//find end of word
			int tempCounter = counter;
			int letterCounter = 0;
			while(isCharacterValid(buffer[tempCounter])){
				type = WORD;
				letterCounter++;
				tempCounter++;
			}
			//will increment by one when continuing so don't need to do it twice
			createNodeAndInsertIntoStack(type, letterCounter, counter);
			counter = tempCounter-1;
			continue;
		}
		// see if it's a special operator
		switch(currentChar){
		case SEQUENCE_CHAR:
			type = SEMICOLON;
			createNodeAndInsertIntoStack(type, 1, counter);
			break;
		//could be pipe or or operator deal with both
		case PIPE_CHAR:
			temp_counter2 = 1;
			if(nextChar == PIPE_CHAR){
				type=OR;
				temp_counter2++;
			}
			else{
				type=PIPE;
			}
			createNodeAndInsertIntoStack(type , temp_counter2, counter);
			counter+=(temp_counter2-1);
			break;
		case SUBSHELL_LEFT_CHAR: 
			type = LEFT_PARENTHESIS;
			createNodeAndInsertIntoStack(type , 1, counter);
			break;
		case SUBSHELL_RIGHT_CHAR:
			type = RIGHT_PARENTHESIS;
			createNodeAndInsertIntoStack(type , 1, counter);
			break;
		case INPUT_CHAR:
			type = LESS_THAN;
			createNodeAndInsertIntoStack(type , 1, counter);
			break;
		case OUTPUT_CHAR:
			type = GREATER_THAN;
			createNodeAndInsertIntoStack(type , 1, counter);
			break;
		case COMMENT_CHAR:
			type = COMMENT;
			temp_counter3 = 0;
			while(buffer[counter+temp_counter3] != '\n' && buffer[counter+temp_counter3] != '\0')
			{
				temp_counter3++;
			} 
			//counter will be incremented when continue, don't do twice
			createNodeAndInsertIntoStack(type , temp_counter3, counter);
			counter += temp_counter3-1;
			break; 
		case NEWLINE_CHAR:
			multiLine = 1;
			type = NEW_LINE;
			temp_counter4 = counter;
			while(buffer[temp_counter4] == '\n'){
				temp_counter4++;
				line_number++;
			}
			if (temp_counter4 - counter >= 2)
				multiLine = 2;
			//counter will be incremented once at the end
			createNodeAndInsertIntoStack(type, 1, counter);
			counter = temp_counter4-1;
			break;
		case AND_CHAR:
			//test if is in fact an and operator
			letter = 0;
			temp_counter5 = 0;
			if(nextChar == AND_CHAR)
			{
				type=AND;
				temp_counter5++;
				letter = 2;
			}	
			else{
				type=OTHER; //deal with it later
				error(1, 0, "%d: syntax error: %c character not recognized by script", line_number, currentChar);
				letter = 1;
			}
			createNodeAndInsertIntoStack(type, letter, counter);
			counter += temp_counter5;
			break;
		case WHITESPACE_CHAR:
		case TAB_CHAR:
			//don't need to store these
			break;
		default:
			type = OTHER; //deal with it later
			error(1, 0, "%d: syntax error: %c character not recognized by script", line_number, currentChar);
			createNodeAndInsertIntoStack(type, 1, counter);
			break;
		}
	}
}

void setupBuffer(int (*get_next_byte) (void *), void * get_next_byte_argument){
	//load file 
	loadFileIntoBuffer(get_next_byte, get_next_byte_argument);
	//printBuffer();
	//seperate into tokens/ operators
	seperateBuffer();
	//printSeperatedBuffer();
	//check for invalid syntax
	validate();
}

void merge( CommandStack * cstack, OperatorStack * ostack)
{
	// switch statement for operators, pop an operator, pop two commands, second popped is commmand [0]
	
	OperatorStackNode * operator_n = pop(ostack);
	node_type opType = operator_n->n_type;
	if (cstack->numItemsInStack < 2)
		error(1, 0, "runtime error: not enough children in stack");
	command_t rightCommand = cpop(cstack)->command;
	command_t leftCommand = cpop(cstack)->command;
	command_t newCommand = (command_t) checked_malloc( sizeof(struct command) );
	
	//prep for op command
	enum command_type c_type;
	switch (opType)
	{
		case SEMICOLON:
			c_type = SEQUENCE_COMMAND;
			break;
		case PIPE:
			c_type = PIPE_COMMAND;
			break;
		case AND:
			c_type = AND_COMMAND;
			break;
		case OR:
			c_type = OR_COMMAND;
			break;
		default:
			// we should never reach this point
			break;	
	}
	newCommand->type = c_type;
	newCommand->status = -1;
	(newCommand->u).command[0] = leftCommand;
	(newCommand->u).command[1] = rightCommand;
	
	cpush( cstack, newCommand);
	
}

command_stream_t emptyStackIntoCommandTree(CommandStack * cstack, OperatorStack * ostack, command_stream_t comStream){
	while(!empty(ostack))
                merge(cstack, ostack);
	//check sizes of stacks, op should be 0, com should be 1
        if ( cstack->numItemsInStack != 1)
        	error( 1, 0, "runtime error: too many children in stack");
        Command_Stream_Node * newTree = (Command_Stream_Node*) checked_malloc( sizeof(Command_Stream_Node));
	newTree->n_command = cgetTopOfStack(cstack)->command;
        //error( 1, 0, "y");
	if (comStream->head == NULL)
        {
        	comStream->head = newTree;
        	comStream->current = newTree;
        }
        else
        {
        	comStream->current->next = newTree;
                comStream->current = newTree;
        }
	cpop(cstack);
	return comStream;
}
      

int getPrecedence(node_type type){
	switch(type){
		case NEW_LINE :
		case SEMICOLON:
			return 0;
			break;
		case AND:
		case OR:
			return 1;
                        break;
		case PIPE:
			return 2;
                        break;
		case GREATER_THAN:
		case LESS_THAN:
			return 3;
                        break;
		default:
			return -1;
			break;
	}
}; 

command_t createCommand(void){
	command_t currCommand = (command_t) checked_malloc( sizeof(struct command) );
	return currCommand;
}

/*
void printCommandStack(CommandStack * cstack){
	cstack->current=cstack->head;
	while(cstack->current != NULL){
		cstack->current->command->type;
		cstack->current = cstack->current->next;
	}
}
*/

command_stream_t createCommandTrees(Stack_Node * head ){
	CommandStack * commands = NULL;
	commands = (CommandStack*) checked_malloc(sizeof(CommandStack));
	commands->head=NULL;
	commands->tail=NULL;
	commands->current=NULL;
	commands->numItemsInStack=0;

	OperatorStack * operators = NULL;
	operators = (OperatorStack*) checked_malloc(sizeof(OperatorStack));
	operators->head=NULL;
	operators->tail=NULL;
	operators->current=NULL;
	operators->numItemsInStack=0;

	node_type n_type;
	command_stream_t final = (Command_Stream*) checked_malloc(sizeof(OperatorStack));
	final->head = NULL;
	final->current = NULL;
	final->counter = 0;
	//for iterating through token stack
	Stack_Node * currNode = head;
	for(;currNode != NULL; currNode = currNode->next){
		n_type = (currNode->n_token).m_type;
		// initialize variables needed in switch here
		command_t currCommand = NULL;
		Stack_Node * tempNode = NULL;	
		int i = 0 , wordCount = 1;
		node_type newline, semicolon;
		int j;
			
		switch(n_type){
			case COMMENT:
				//if(currNode->prev == NULL)
				//	break;
				// wrap up current command tree and move to next one only if you've accumalted a command since the last comment
				if(commands->numItemsInStack != 0){
					emptyStackIntoCommandTree(commands, operators, final);
				}
				break;
			case NEW_LINE:
				if(currNode->prev != NULL && ( (currNode->prev->n_token).m_type == AND || 
				(currNode->prev->n_token).m_type == OR || (currNode->prev->n_token).m_type == PIPE || 
				(currNode->prev->n_token).m_type == COMMENT))
				{
					break;
				}
				if(currNode->prev != NULL && ( (currNode->prev->n_token).m_type == NEW_LINE &&
				 (currNode->n_token).m_type == NEW_LINE))
				{	// only happens at very end of file where an extra new line is appende
					break;
				}
				// check prev token, if semicolon, end command tree, start new one              //ending command tree is taken care of in semicolon
				else if(currNode->prev != NULL && (currNode->prev->n_token).m_type != SEMICOLON ){ //&& ( (currNode->prev->n_token).m_type == WORD || 
					// (currNode->prev->n_token).m_type == RIGHT_PARENTHESIS ) ){
					// honestly don't need the and it should be handled in validate
					if((currNode->n_token).newLines >= 2){
						if(commands->numItemsInStack != 0) // don' want to empty stack twich
							emptyStackIntoCommandTree(commands, operators, final);
					}
					else{
						//create semicolon cammand and push on operator stack
						if(currNode->next != NULL && (currNode->next->n_token).m_type != COMMENT )
						{
							semicolon = SEMICOLON;
							if(empty(operators)){
                                push(operators, semicolon);
                            }
							else{
								while(getPrecedence(getTopOfStack(operators)->n_type)
                                    	>= getPrecedence(semicolon)){
                                    merge(commands, operators);
									if(empty(operators))
                                        break;
								}
								push(operators, semicolon);
							}		 
						}
					}
				}
			        break;
			case WORD: //strange error fix by making first statement not stack node
				tempNode = currNode->next;
				currCommand = createCommand();
				currCommand->type = SIMPLE_COMMAND;
				//count how many words in a row there are
				while (tempNode != NULL && (tempNode->n_token).m_type == WORD)
				{	
					wordCount++;
					tempNode = tempNode->next;
				}
				//allocate command string size accordingly
				(currCommand->u).word = (char**) checked_malloc((wordCount+1) * sizeof(char*));
				//insert words into command
				
				for (; currNode != tempNode;)
				{
					(currCommand->u).word[i] = (currNode->n_token).tokenString;
					if(currNode->next == tempNode){
						i++;
						break;
					}
					else{
						currNode = currNode->next;
					}
					i++;
				}	
				//last word pointer is null;
				
				// create commandstacknode thing
				cpush(commands, currCommand);	
        			break;
			case SEMICOLON:
				if(currNode->next == NULL)
					break;
				newline = NEW_LINE; // another strange error fix
				if((currNode->next->n_token).m_type == newline){
					if((currNode->next->n_token).newLines >=2){
						if(!cempty(commands)){
							emptyStackIntoCommandTree(commands, operators, final);
							break;
						}
					}
					else if((currNode->next->n_token).newLines == 1 &&	
						currNode->next->next != NULL &&
						 (currNode->next->next->n_token).m_type == newline){ // protects against that one 
						break;
					}
					 else if((currNode->next->n_token).newLines == 1 &&
                                                currNode->next->next == NULL){ // protects against that one
						if(!cempty(commands)){
                            emptyStackIntoCommandTree(commands, operators, final);
                            break;
                        }
                    }
				}
				if(empty(operators)){
                    push(operators, n_type);
                }
				else{
                    while(getPrecedence(getTopOfStack(operators)->n_type)
                                                                 >= getPrecedence(n_type)){
                        merge(commands, operators);
                        if(empty(operators))
							break;
                    }
                    push(operators, n_type);
                }
				break;
			case PIPE:
			case AND:
			case OR:		
				// if operator stack is empty push op onto stack
				if(empty(operators)){
					push(operators, n_type);
				}
				///else while popped op has >= precedence
				//pop op from op stack, pop two commands off command stack
				//combine into command, push on command stack
				//push current op onto op stack
				else{
					while(getPrecedence(getTopOfStack(operators)->n_type)
								 >= getPrecedence(n_type)){
						merge(commands, operators);
						if(empty(operators))
							break;
					}
					push(operators, n_type);
				}
			break;
			case LEFT_PARENTHESIS:
      			// push onto operator stack
      				push(operators, n_type);
			break;
			case RIGHT_PARENTHESIS:
			// while (havent found (
			//		pop operator off op stack
			//		pop 2 commands off command stack
			//		create command, push onto command stack
			//	create subshell command, push onto command stack
				while(getTopOfStack(operators)->n_type != LEFT_PARENTHESIS){
					merge(commands, operators);
				}
				currCommand = createCommand();	
				currCommand->type = SUBSHELL_COMMAND;
				(currCommand->u).subshell_command = cpop(commands)->command;	
				pop(operators);
				cpush(commands, currCommand);
			break;
			// doesnt handle subshell case 
			case GREATER_THAN: //for now asuume only one subsequent token is input or output
				currCommand = cgetTopOfStack(commands)->command;
				currCommand->output = (currNode->next->n_token).tokenString;
				currNode = currNode->next; // skip next token
				while((currNode->next->n_token).m_type == WORD){
					j = 0;
					while((currCommand->u).word[j] != NULL){
						j++;
					}	
					(currCommand->u).word = (char**)
					 checked_realloc( (currCommand->u).word , (j+2) * sizeof(char*));
					(currCommand->u).word[j]= (currNode->next->n_token).tokenString;
					(currCommand->u).word[j+1]=NULL;
					currNode = currNode->next;	
				}
			break;
			case LESS_THAN:
				currCommand = cgetTopOfStack(commands)->command;
                                currCommand->input = (currNode->next->n_token).tokenString;
				currNode = currNode->next;
				while((currNode->next->n_token).m_type == WORD){
                    j = 0;
                    while((currCommand->u).word[j] != NULL){
                        j++;
                    }
                    (currCommand->u).word = (char**)
                    checked_realloc( (currCommand->u).word , (j+2) * sizeof(char*));
                    (currCommand->u).word[j]= (currNode->next->n_token).tokenString;
                    (currCommand->u).word[j+1]=NULL;
                    currNode = currNode->next;
                }
			break;
			default:
				// empty the stack?
			break;
		}
	}
	// do one final emptying of stacki
	if(!cempty(commands))
		emptyStackIntoCommandTree(commands, operators, final);
	return final;
}

command_stream_t 
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */

  //first set up things by copying the file into buffer first
  setupBuffer(get_next_byte, get_next_byte_argument);
  //convert this into command stream
  command_stream_t done = createCommandTrees(head_node);  
  //error (1, 0, "command reading not yet implemented");
  return done;
}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  //error (1, 0, "command reading not yet implemented");
	if (s == NULL || s->head == NULL)
		return NULL;
//	if(s->head == NULL)
//		error(1, 0, "no head");
	Command_Stream_Node * current = s->head;
	int i = 0;
	for(; i < s->counter; i++)
	{
		current = current->next;
		if(current == NULL)
			return NULL;
	}
	(s->counter) += 1;
	// grab the current command
	command_t c = current->n_command;
	return (c);
}
