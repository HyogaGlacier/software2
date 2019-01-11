#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv){
  srand((unsigned)time(NULL));
  double q;
  q = atof(argv[1]);
  int step;
  step = atoi(argv[2]);
  int pos = 0;
  for(int i = 0; i < step; i++){
    if(pos % 2){
      pos += (rand() % 2 ? 1 : -1);
    } else {
      pos += ((double)rand() / RAND_MAX > q ? 0 : 1);
    }
  }
  printf("%d\n", pos);
  return 0;
}
