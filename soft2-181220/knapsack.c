#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// 個数の上限はあらかじめ定めておく
const int max_items = 1000;
// キャパシティの上限も予め決めておく
const int max_weight = 10000;

// 以下は構造体の定義と関数のプロトタイプ宣言

// 構造体 itemset
// number分の価値value と 重さweight を格納しておく
// データをポインタで定義しており、mallocする必要あり
typedef struct itemset
{
  int number;
  double *value;
  int *weight;
} Itemset;

// 関数のプロトサイプ宣言

// Itemset *init_itemset(int, int);
//
// itemsetを初期化し、そのポインタを返す関数
// 引数:
//  品物の個数: number (int)
//  乱数シード: seed (int) // 品物の値をランダムにする
// 返り値:
//  確保されたItemset へのポインタ
Itemset *init_itemset(int number, int seed);

// void free_itemset();

// Itemset *load_itemset(char *filename)
//
// ファイルからItemset を設定し、確保された領域へのポインタを返す関数 [未実装]
// 引数:
//  Itemsetの必要パラメータが記述されたバイナリファイルのファイル名 filename (char*)
// 返り値:
//  Itemset へのポインタ
Itemset *load_itemset(char *filename);

// void print_itemset(const Itemset *list)
//
// Itemsetの内容を標準出力に表示する関数
void print_itemset(const Itemset *list);

// void save_itemset(char *filename, const Itemset *list)
//
// Itemsetのパラメータを記録したバイナリファイルを出力する関数
// 引数:
// Itemsetの必要パラメータを吐き出すファイルの名前 filename (char*)
// ファイルに書き込むItemset list (Itemset*)
// 返り値:
//  なし
void save_itemset(char *filename, const Itemset *list);

// double solve()
//
// ソルバー関数: 指定された設定でナップサック問題をとく [現状、未完成]
// 引数:
//   品物のリスト: Itemset *list
//   ナップサックの容量: capacity (double)
//   実際にナップサックに入れた品物を記録するフラグ: flags (int*)
// 返り値:
//   最適時の価値の総和を返す
//
// * 引数のflags の中身が書き換わり、最適なflags になる [ようにする]
double solve(const Itemset *list, int capacity, int *flags);

// double search()
//
// 探索関数: 指定されたindex以降の組み合わせで、最適な価値の総和を返す
//  再帰的に実行する
// 引数:
//  指定index : index (int)
//  品物リスト: list (Itemset*)
//  ナップサックの容量: capacity (double)
//  実際にナップサックに入れた品物を記録するフラグ: flags (int*)
//  途中までの価値と重さ (ポインタではない点に注意): sum_v, sum_w
// 返り値:
//   最適時の価値の総和を返す
double search(int index, const Itemset *list, int capacity, int *flags, double sum_v, int sum_w);

// main関数
// プログラム使用例: ./knapsack 10 20
//  10個の品物を設定し、キャパ20 でナップサック問題をとく
int main (int argc, char**argv)
{
  /* 引数処理: ユーザ入力が正しくない場合は使い方を標準エラーに表示して終了 */
  if (argc != 4){
    fprintf(stderr, "usage: %s <the number of items (int)> <max capacity (double)> <filename of Itemset (char*)>\n",argv[0]);
    exit(1);
  }

  const int n = atoi(argv[1]);
  assert( n <= max_items );
  
  const int W = atoi(argv[2]);
  assert( W <= max_weight );
  
  printf("max capacity: W = %d, # of items: %d\n",W, n);

  // ファイルからデータの読み込み
  char* filename = argv[3];
  Itemset *items = init_itemset(n, 1); // load_itemset(filename);
  print_itemset(items);

  // ファイルにデータの書き込み
  save_itemset("knapsack_data.dat", items);

  // 品物を入れたかどうかを記録するフラグ配列
  int *flags = (int*)calloc(n, sizeof(int));

  // ソルバーで解く
  double total = solve(items, W, flags);

  // 表示する
  printf("----\nbest solution:\n");
  printf("value: %4.1f\n",total);

  return 0;
}

// 構造体をポインタで確保するお作法を確認してみよう
Itemset *init_itemset(int number, int seed)
{
  Itemset *list = (Itemset*)malloc(sizeof(Itemset));
  list->number = number;

  list->value = (double*)malloc(sizeof(double)*number);
  list->weight = (int*)malloc(sizeof(int)*number);

  srand(seed);
  for (int i = 0 ; i < number ; i++){
    list->value[i] = 0.1 * (rand() % 200);
    list->weight[i] = 0.1 * (rand() % 200 + 1);
  }
  
  return list;
}

// ファイル読み込み
Itemset *load_itemset(char *filename)
{
  FILE *fp = fopen(filename, "rb");
  Itemset *list = (Itemset*)malloc(sizeof(Itemset));
  fread(&(list->number), sizeof(int), 1, fp);

  list->value = (double*)malloc(sizeof(double)*list->number);
  list->weight = (int*)malloc(sizeof(int)*list->number);
  fread(list->value, sizeof(double), list->number, fp);
  fread(list->weight, sizeof(int), list->number, fp);

  return list;
}

// itemset の free関数
void free_itemset(Itemset *list)
{
  free(list->value);
  free(list->weight);
  free(list);
}

// 表示関数
void print_itemset(const Itemset *list)
{
  int n = list->number;
  for(int i = 0 ; i < n ; i++){
    printf("v[%d] = %4.1f, w[%d] = %d\n", i, list->value[i], i, list->weight[i]);
  }
  printf("----\n");
}

// 書き込み関数
void save_itemset(char *filename, const Itemset *list)
{
  FILE *fp = fopen(filename, "wb");
  fwrite(&(list->number), sizeof(int), 1, fp);
  fwrite(list->value, sizeof(double), list->number, fp);
  fwrite(list->weight, sizeof(int), list->number, fp);
}

// ソルバー。dp[i]:ナップサックに入れる重量の合計がi以下の時の価値の最大値
// と置くと動的計画法を用いることが出来る。O(NW)(W=capacity)
double solve(const Itemset *list,  int capacity, int *flags)
{
  double *dp = (double*)malloc((capacity + 1) * sizeof(double));
  for (int i = 0 ; i <= capacity ; i++)
    dp[i] = 0.0;
}
