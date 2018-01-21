#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

bool check_calcu_operate(int t);

enum {
  TK_NOTYPE = 256, TK_EQ, 

  /* TODO: Add more token types */
  TK_DECIMAL, 
  TK_COMMA, 
  TK_OPEN_PAREN, TK_CLOSE_PAREN, 
  TK_NEGTIVE
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"-", '-'},         	// sub or neg
  {"\\*", '*'},         // multi
  {"/", '/'},         	// div
  {"[0-9]+", TK_DECIMAL},	// decimal numbers
  {",", TK_COMMA},		// comma
  {"\\(", TK_OPEN_PAREN},	// open paren
  {"\\)", TK_CLOSE_PAREN},// close paren
  {"==", TK_EQ},         // equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
		  	case TK_DECIMAL:
				strncpy(tokens[nr_token].str, substr_start, substr_len);
				tokens[nr_token].type = rules[i].token_type;
				break;
		  	case '-':
				if (position == 0 || check_calcu_operate(i-1))
					tokens[nr_token].type = TK_NEGTIVE;
				break;
          	default:
				tokens[nr_token].type = rules[i].token_type;
		   		continue;
        }

		nr_token++;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

// check open_paren & close_paren
bool check_parentheses(int p, int q){
	if (tokens[p].type == TK_OPEN_PAREN && tokens[q].type == TK_CLOSE_PAREN)
		return true;
	else
		return false;
}

// check add, sub, multi, div, neg
bool check_calcu_operate(int t){
	if (tokens[t].type == '+' || tokens[t].type == '-' || tokens[t].type == '*' || tokens[t].type == '/' || tokens[t].type == TK_NEGTIVE)   
		return true;
	else
		return false;
}

int get_dominant(int p, int q){
	int domi = -1;
	int min_level = 15;
	int level;
	int cur;
	for (cur = p; cur <= q; cur++){
		if (!check_calcu_operate(cur))
			continue;

		switch (tokens[cur].type){
			case TK_NEGTIVE:
				level = 9;
				break;
			case '+':
			case '-':
				level = 7;
				break;
			case '*':
			case '/':
				level = 8;
				break;
			default:
				assert(0);
		}

		if (level <= min_level){
			min_level = level;
			domi = cur;
		}
	}

	Log("hit level %d, %d - %d", min_level, p, q);
	Assert(domi != -1, "cannot hit dominant");
	return domi;
}

uint32_t eval(int p, int q){
	if (p > q){
		panic("Bad expression");
	}
	else if (p == q){
		uint32_t n;
		sscanf(tokens[p].str, "%d", &n);
		return n;
	}
	else if (check_parentheses(p, q)){
		return eval(p+1, q-1);
	}
	else{
		int domi = get_dominant(p, q);
		if (tokens[domi].type == TK_NEGTIVE)
			return -1 * eval(domi+1, q);

		int val1 = eval(p, domi-1);
		int val2 = eval(domi+1, q);
		switch (tokens[domi].type){
			case '+':
				return val1 + val2;
			case '-':
				return val1 - val2;
			case '*':
				return val1 * val2;
			case '/':
				return val1 / val2;
			default:
				assert(0);
		}
	}
}
			

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
	return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  int res = eval(0, nr_token-1);
  *success = true;

  return res;
}
