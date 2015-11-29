#include <iostream>
extern int yyparse();
void evaluate(void);
void free_mem(void);
int main() {
	
  extern int yydebug;
  yydebug = 0;
  
  try {
    if ( yyparse() == 0 ) {	
      std::cout << "Syntactically correct program." << std::endl;
	  evaluate();
	  free_mem();
      return 0;
    }
    else {
      std::cout << "Oops!" << std::endl;
	  free_mem();
      return -1;
    }
  }
  catch( const std::string& msg) {
      std::cout << msg << std::endl;
  }
  catch( ... ) {
      std::cout << "Oops!" << std::endl;
  }
  return 0;
}
