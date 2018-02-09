# 1 "src/main.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "src/main.c"
int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {

  int is_batch_mode = init_monitor(argc, argv);


  ui_mainloop(is_batch_mode);

  return 0;
}
