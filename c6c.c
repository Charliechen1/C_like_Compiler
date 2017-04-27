#include <stdio.h>
#include "calc3.h"
#include "y.tab.h"

extern int var_count, func_count;
extern int vType[200];
extern char* func[200];

/*
function to let nas know the number of global variables
*/
void printsp(){
    printf("\tpop\tsp\n"); 
    printf("\tpush\t%d\n", var_count); 
    printf("\tpop\tsp\n"); 
}

/* initialize the global variables */
void init(nodeType *p){
    if (!p) return;
    switch(p->type) {
	case typeFunc:
	    insertFUNC(p->func.name);
	    break;
	case typeOpr:
            switch(p->opr.oper) {
		case FOR:
		    init(p->opr.op[3]);
		    break;
		case WHILE:
		    init(p->opr.op[1]);
		    break;
		case IF:
		    if (p->opr.nops > 2) {
		    /* if else then */
			init(p->opr.op[1]);
			init(p->opr.op[2]);
		    } else {
		    /* if else */
			init(p->opr.op[1]);
		    }
		    break;
		case '=':
		    if(p->opr.op[0]->type == typeId) {
		    	/* insert into symbol table */
    		    	insertSYM(p->opr.op[0]->id.var_name);
		    }
		    break;
		default:
            	    init(p->opr.op[0]);
            	    init(p->opr.op[1]);
		    break;
	    }
	default:
	    break;
    }
}

/*
function to deal with printing for different type
*/
void printStackTop(int type){
    
    if(type == 1){
        printf("\tputi\n");
    }else if(type == 2){
        printf("\tputs\n");
    }else if(type == 3){
        printf("\tputc\n");
    }else{
        // unknown type
	yyerror("unknown type for printing");
    }
}

/*
function for checking existance of a variable in the symbol table
  var_name: variable name
*/
int inSYM(char * var_name)
{
  int i;
  for(i = 0; i < var_count; i++)
  {
    if(strcmp(sym[i], var_name) == 0)
    {
      return 1; // in sym
    }
  }
  return 0; // not in sym
}

/*
function for adding variables to symbol table
  var_name: variable name
*/
void insertSYM(char * var_name)
{
  if(var_count >= 200)
  {
    printf("\tNumber of variables exceeds the limit!\n");
    return;
  }
  if(inSYM(var_name) == 0)//not in sym
  {
    sym[var_count] = (char *) malloc (strlen(var_name));
    strcpy(sym[var_count], var_name);
    var_count ++;
  }
  else
  {
    // do nothing
    //printf("\tError: variable -%s- already declared!\n", var_name);
  }
}

/*
function for getting index of a variable in sym
*/
int getSYMIdx(char * var_name)
{
  int i;
  for(i = 0; i < var_count; i++)
  {
    if(strcmp(sym[i], var_name) == 0)
    {
      return i;
    }
  }
  return -1; // not found
}

/*
function for clearing the symbol table
*/
void emptySYM()
{
  int i;
  for(i = 0; i < var_count; i++)
  {
    free(sym[i]);
  }
  var_count = 0;
}

/*
function to get type for stored variable
*/
int checkType (int index)
{
    return vType[index];
}

/*
function for adding functions to func table
  func_name: function name
*/
void insertFUNC(char * func_name)
{
  if(func_count >= 200)
  {
    printf("\tNumber of variables exceeds the limit!\n");
    return;
  }
  if(getFUNCIdx(func_name) < 0)//not in sym
  {
    func[func_count] = (char *) malloc (strlen(func_name));
    strcpy(func[func_count], func_name);
    var_count ++;
  }
  else
  {
    // the function has already been defined
    yyerror("\tFunction has already been defined!\n");
    return;
  }
}

/*
function for getting index of a function in func
*/
int getFUNCIdx(char * func_name)
{
  int i;
  for(i = 0; i < func_count; i++)
  {
    if(strcmp(func[i], func_name) == 0)
    {
      return i;
    }
  }
  return -1; // not found
}

/*
function for clearing the function table
*/
void emptyFUNC()
{
  int i;
  for(i = 0; i < func_count; i++)
  {
    free(func[i]);
  }
  func_count = 0;
}


/*
function to set type for some variable
*/
void setType (int index, int t)
{
    vType[index] = t;
    return;
}

/*
function for checking type for an expression
*/
int checkExprType (nodeType* p){
    switch(p->type) {
        case typeId:{
            char *str = p->id.var_name;
            int index = getSYMIdx(str);
            int type = checkType(index);
            return type;
	}   
        case typeCon:
            return 1;
        case typeStrCon:
            return 2;
        case typeCharCon: 
            return 3;
        case typeOpr:
            switch(p->opr.oper) {
                case UMINUS:
                    return 1;
                default:
                    return 1;
            }
    }
}

static int lbl;
int ex(nodeType *p, int blbl, int clbl) {
    int lblx, lbly, lblz;

    if (!p) return 0;
    switch(p->type) {
    case typeCon:
        printf("\tpush\t%d\n", p->con.value); 
        break;
    case typeCharCon:
	printf("\tpush\t%s\n", p->charCon.value);
        break;
    case typeStrCon:
	printf("\tpush\t\"%s\"\n", p->strCon.value);
        break;
    case typeId:{
        char *str = p->id.var_name;
        int index = getSYMIdx(str);

        if (index==-1)
        {
            printf("\tError: Variable \"%s\" undeclared\n", str);
            break;
        }

	printf("\tpush\tsb[%d]\n", index);

        break;
    }
    case typeOpr:
        switch(p->opr.oper) {
        case BREAK:
            printf("\tjmp\tL%03d\n", blbl);
            break;
        case CONTINUE:
            printf("\tjmp\tL%03d\n", clbl);
            break;
        case FOR:
            lblx = lbl++;
            lbly = lbl++;
            lblz = lbl++;
            ex(p->opr.op[0], blbl, clbl);
            printf("L%03d:\n", lblx);
            ex(p->opr.op[1], blbl, clbl);
            printf("\tj0\tL%03d\n", lbly);
            ex(p->opr.op[3], lbly, lblz);
            printf("L%03d:\n", lblz);
            ex(p->opr.op[2], blbl, clbl);
            printf("\tjmp\tL%03d\n", lblx);
            printf("L%03d:\n", lbly);
            break;
        case WHILE:
            lblx = lbl++;
            lbly = lbl++;
            printf("L%03d:\n", lblx);
            ex(p->opr.op[0], blbl, clbl);
            printf("\tj0\tL%03d\n", lbly);
            ex(p->opr.op[1], lbly, lblx);
            printf("\tjmp\tL%03d\n", lblx);
            printf("L%03d:\n", lbly);
            break;
        case IF:
            ex(p->opr.op[0], blbl, clbl);
            if (p->opr.nops > 2) {
                /* if else */
                printf("\tj0\tL%03d\n", lblx = lbl++);
                ex(p->opr.op[1], blbl, clbl);
                printf("\tjmp\tL%03d\n", lbly = lbl++);
                printf("L%03d:\n", lblx);
                ex(p->opr.op[2], blbl, clbl);
                printf("L%03d:\n", lbly);
            } else {
                /* if */
                printf("\tj0\tL%03d\n", lblx = lbl++);
                ex(p->opr.op[1], blbl, clbl);
                printf("L%03d:\n", lblx);
            }
            break;
        case READ:
            printf("\tread\n");
            if(p->opr.op[0]->type == typeId) {
                printf("\tpop\t%s\n", p->opr.op[0]->id.var_name);
            } else {
                printf("\tpopi\t%c\n", ex(p->opr.op[0], blbl, clbl));
            }
            break;
        case PRINT:
            ex(p->opr.op[0], blbl, clbl);
	    int type = checkExprType(p->opr.op[0]);
            printStackTop(type);
            break;
        case '=':
            ex(p->opr.op[1], blbl, clbl);
	    if(p->opr.op[0]->type == typeId) {
		/* insert into symbol table */
    		insertSYM(p->opr.op[0]->id.var_name);
                int ind = getSYMIdx(p->opr.op[0]->id.var_name);
		int thisT = checkExprType(p->opr.op[1]);
		setType(ind, thisT);

		printf("\tpop\tsb[%d]\n", getSYMIdx(p->opr.op[0]->id.var_name)); // for gb

	    } else {
		// array element or pointer, not in consideration yet
	
	    }
            break;
        case UMINUS:
            ex(p->opr.op[0], blbl, clbl);
            printf("\tneg\n");
            break;
        default:
            ex(p->opr.op[0], blbl, clbl);
            ex(p->opr.op[1], blbl, clbl);
            switch(p->opr.oper) {
                case '+':   printf("\tadd\n"); break;
                case '-':   printf("\tsub\n"); break;
                case '*':   printf("\tmul\n"); break;
                case '/':   printf("\tdiv\n"); break;
                case '%':   printf("\tmod\n"); break;
                case '<':   printf("\tcompLT\n"); break;
                case '>':   printf("\tcompGT\n"); break;
                case GE:    printf("\tcompGE\n"); break;
                case LE:    printf("\tcompLE\n"); break;
                case NE:    printf("\tcompNE\n"); break;
                case EQ:    printf("\tcompEQ\n"); break;
                case AND:   printf("\tand\n"); break;
                case OR:    printf("\tor\n"); break;
            }
        }
    }

    return 0;
}

void eop() {
    // add end of program label
    printf("\tjmp\tL999\n");
    printf("L998:\n");
    printf("\tpush\t999999\n");
    printf("\tprint\n");
    printf("\tpush\t999999\n");
    printf("\tprint\n");
    printf("\tpush\t999999\n");
    printf("\tprint\n");
    printf("L999:\n");
}
