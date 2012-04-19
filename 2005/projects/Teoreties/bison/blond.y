%{
#include <stdio.h>
#include <ctype.h>

//#include <sym.h>
int i,j;
%}
%token NUMBER
%%

lines : exp { printf("%.3f\n", $1); }
        ;

exp : exp '+' term {$$=$1+$3;}
  | exp '-' term {$$=$1-$3;}
  | term {$$=$1;}
  
 
term : term '*' factor {$$=$1*$3;}
  | term '/' factor {$$=$1/$3;}
  | term '^' factor {$$=pow($1,$3);}
  | term '!' {j=1;for (i=$1;i>0;i--){j=j*i;}$$=j;}  
  | 's' 'i' 'n' '(' exp ')' {$$=sin($5);}  
  | factor {$$=$1;}
  

factor : NUMBER {$$=$1;} 
  | '('exp')' {$$=$2;}
  

%%
main()
{
yyparse();
}

int yylex()
{
  int c;
  while ((c=getchar())==' ');
  if (isdigit(c)) {
        ungetc(c, stdin);
        scanf("%d", &yylval);
        return(NUMBER);
  }
  if (c =='\n') return 0;
        return(c);
  }
 
  yyerror(char *s)
  { printf("%s\n", s);
  }
