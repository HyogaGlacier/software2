#include <stdio.h>
#include <stdlib.h>

int main(int argc, char**argv)
{
  // バイナリファイルを出力してみる
  // テキストファイルに出力した場合と比較

  int size = 1000000;
  double *d = (double*)malloc(sizeof(double)*size); // とりあえず100万確保

  for(int i = 0 ; i < size ; i++)
    d[i] = 0.5423 * rand();

  char *s = "data.txt";// ひとまずテキストでだしてみる
  FILE *fp;
  fp = fopen(s,"w");

  for(int i = 0 ; i < size ; i++)
    fprintf(fp,"%f\n",d[i]);
  fclose(fp);

  char *s2 = "data.dat";
  fp = fopen(s2, "wb"); // bはwindows は必須, linux/mac はなくてもよい
  fwrite(d,sizeof(double),size,fp);
  fclose(fp);
  return 0;
}
