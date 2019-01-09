#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
// bitDP解法は正直つまらないので、DEBUG_ACOのコメントを外すとデバッグ文が強制出力されながら、nが小さい場合でも強制的にACOを適用します
// #define DEBUG_ACO

// const による定数定義
const int width = 70;
const int height = 40;
#define max_cities 1000
const double eps = 1e-9;
const double inf = pow(2, 32);
unsigned int xor32(){
  static unsigned int y = 2463534242;
  y = y ^ (y << 13);
  y = y ^ (y >> 17);
  return y = y ^ (y << 15);
}


// 町の構造体（今回は2次元座標）を定義
typedef struct
{
  int x;
  int y;
} City;

// 整数最大値をとる関数
int max(const int a, const int b)
{
  return (a > b) ? a : b;
}

// 以下は実装が長くなるのでプロトタイプ宣言
// draw_line: 町の間を線で結ぶ
// draw_route: routeでの巡回順を元に移動経路を線で結ぶ
// plot_cities: 描画する
// distance: 2地点間の距離を計算
// solove(): 巡回問題をといて距離を返す/ 引数route に巡回順を格納

void draw_line(char **map, City a, City b);
void draw_route(char **map, City *city, int n, const int *route);
void plot_cities(FILE* fp, char **map, City *city, int n, const int *route);
double distance( City a, City b);
double solve(City *city, int n, int *route);
double get_cost(int n, int *order, City *city);

int main(int argc, char**argv)
{
  FILE *fp = stdout; // とりあえず描画先は標準出力としておく
  if (argc != 2){
    fprintf(stderr, "Usage: %s <the number of cities>\n", argv[0]);
    exit(1);
  }
  const int n = atoi(argv[1]);
  assert( n > 1 && n <= max_cities); // さすがに都市数1000は厳しいので

  City *city = (City*) malloc(n * sizeof(City));
  char **map = (char**) malloc(width * sizeof(char*));
  char *tmp = (char*)malloc(width*height*sizeof(char));
  for (int i = 0 ; i < width ; i++)
    map[i] = tmp + i * height;

  /* 町の座標をランダムに割り当て */
  for (int i = 0 ; i < n ; i++){
    city[i].x = rand() % (width - 5);
    city[i].y = rand() % height;
  }
  // 町の初期配置を表示
  plot_cities(fp, map, city, n, NULL);
  sleep(1);

  // 訪れる順序を記録する配列を設定
  int *route = (int*)calloc(n, sizeof(int));

  const double d = solve(city, n, route);
  printf("total distance = %f\n", d);
  plot_cities(fp, map, city, n, route);

  // 動的確保した環境ではfreeをする
  free(route);
  free(map[0]);
  free(map);
  free(city);
  
  return 0;
}

// 繋がっている都市間に線を引く
void draw_line(char **map, City a, City b)
{
  const int n = max(abs(a.x - b.x), abs(a.y - b.y));
  for (int i = 1 ; i <= n ; i++){
    const int x = a.x + i * (b.x - a.x) / n;
    const int y = a.y + i * (b.y - a.y) / n;
    if (map[x][y] == ' ') map[x][y] = '*';
  }
}

void draw_route(char **map, City *city, int n, const int *route)
{
  if (route == NULL) return;

  for (int i = 0; i < n; i++) {
    const int c0 = route[i];
    const int c1 = route[(i+1)%n];// n は 0に戻る必要あり
    draw_line(map, city[c0], city[c1]);
  }
}

void plot_cities(FILE *fp, char **map, City *city, int n, const int *route)
{
  fprintf(fp, "----------\n");

  memset(map[0], ' ', width * height); // グローバルで定義されたconst を使用

  // 町のみ番号付きでプロットする
  for (int i = 0; i < n; i++) {
    char buf[100];
    sprintf(buf, "C_%d", i);
    for (int j = 0; j < strlen(buf); j++) {
      const int x = city[i].x + j;
      const int y = city[i].y;
      map[x][y] = buf[j];
    }
  }

  draw_route(map, city, n, route);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      const char c = map[x][y];
      fputc(c, fp);
    }
    fputc('\n', fp);
  }
  fflush(fp);
}

double distance(City a, City b)
{
  const double dx = a.x - b.x;
  const double dy = a.y - b.y;
  return sqrt(dx * dx + dy * dy);
}

double solve(City *city, int n , int *route)
{
  route[0] = 0; // 循環した結果を避けるため、常に0番目からスタート

  // 頂点数が20以下の際はbitDPを用いる。
  // dp[i][j]: 今まで訪れた頂点のビット列表現がiで、最後に訪れた頂点がjの時の今までの総移動距離の最小値
  // これを構成するのにO(n^2*2^n)かかる。丁度n=20位がそれなりの時間内に終わらせる解法としては妥当
  // その後、ルートを構成する必要があるので、dpテーブルから復元する。
  #ifdef DEBUG_ACO
  if (0){
  #else
  if (n <= 20){
  #endif
    int m = 1 << n;
    double **dp = (double**)malloc(sizeof(double*) * m);
    for (int i = 0 ; i < m ; i++){
      dp[i] = (double*)malloc(sizeof(double) * n);
      for (int j = 0 ; j < n ; j++)
        dp[i][j] = inf;
    }
    // 初期位置はcity[0]で固定->答えがdp[m-1][0]で固定できる
    for (int i = 1 ; i < n ; i++)
      dp[1 << i][i] = distance(city[0], city[i]);
    for (int i = 0 ; i < m ; i++){
      for (int j = 0 ; j < n ; j++){
        if (fabs(dp[i][j] - inf) < eps)
          continue;
        if (i & (1 << j) == 0)
          continue;
        for (int k = 0 ; k < n ; k++){
          if (i & (1 << k))
            continue;
          if (dp[i][j] + distance(city[j], city[k]) - dp[i | (1 << k)][k] < -eps){
            dp[i | (1 << k)][k] = dp[i][j] + distance(city[j], city[k]);
          }
        }
      }
    }
    int last_pos = 0;
    int state = m - 1;
    for (int i = 1 ; i < n ; i++){
      for (int j = 0 ; j < n ; j++){
        if (fabs(dp[state][last_pos] - dp[state ^ (1 << last_pos)][j] - distance(city[j], city[last_pos])) < eps){
          route[i] = j;
          state ^= (1 << last_pos);
          last_pos = j;
          break;
        }
      }
    }
    double sum_d = get_cost(n, route, city);
    return sum_d;
  }
  else{
    // 以降はn>20で、全頂点が平面上にあるという前提があるので、平面切除法などを用いても良かったが、それでも数時間かかるプログラムを乗せるのはあまりよくないと考え、気軽にACOを実装した。
    // 今回は「同時に排出するアリは一匹」という制約を付けた。
    // n=1000とかでも2000匹ごろには収束します。1000匹30秒程度（自分の環境で）なのでまぁいいほうではないかと思う。
    // 座標がすべて整数値であることから、上手く処理を短縮できないかと考えたが、結構難しそうだった。
    // 今回の問題では、「n=1000のケースで自分の解がどれくらいいいのか分からない（randのsseedは環境依存なので、他人と比較するのもみんなのコードをいじる必要がある）」という点があったので、ベンチマークを用意してくれると助かります（srand(1)とか入れるだけでも他人と比較できてよかったかもしれない）
    // 初期値
    double alpha = 1.0, beta = -1.0, Q=1.0, vanish = 0.95;
    int ant_num = 2000;
    // 答えを入れるrouteは取り合えず初期値0->1->2->...->n->0で
    for (int i = 0 ; i < n ; i++)
      route[i] = i;
    double sum_d = get_cost(n, route, city);
    // 評価値がweight**alpha * dist**beta の加重分配的なやつ
    double **weight = (double**)malloc(sizeof(double*) * n);
    // weightは適当に[0, 1]で作る
    for (int i = 0 ; i < n ; i++){
      weight[i] = (double*)malloc(sizeof(double) * n);
      for (int j = 0 ; j < n ; j++){
        if (i == j)
          continue;
        weight[i][j] = (double)xor32() / inf;
      }
    }
    double **pheromon = (double**)malloc(sizeof(double*) * n);
    for (int i = 0 ; i < n ; i++)
      pheromon[i] = (double*)malloc(sizeof(double) * n);
    short *ant_visit = (short*)malloc(sizeof(short) * n);
    int *ant_order = (int*)malloc(sizeof(int) * n);
    // 各アリについて、
    for (int i = 0 ; i < ant_num ; i++){
      // routeの初期化
      for (int j = 0 ; j < n ; j++)
        ant_visit[j] = 1;
      int current_pos = 0;
      ant_order[0] = current_pos;
      ant_visit[current_pos] = 0;
      // アリを移動させる
      for (int j = 1 ; j < n ; j++){
        // 行先の評価値のsumをとる
        double denomination = 0.0;
        for (int k = 0 ; k < n ; k++){
          if (ant_visit[k] == 0)
            continue;
          denomination += pow(weight[current_pos][k], alpha) * pow(distance(city[current_pos], city[k]), beta); 
        }
        double numerator = (double)xor32() / inf * denomination;
        int idx = -1;
        for (int k = 0 ; k < n ; k++){
          if (ant_visit[k] == 0)
            continue;
          double prob = pow(weight[current_pos][k], alpha) * pow(distance(city[current_pos], city[k]), beta);
          if (numerator - prob < eps){
            idx = k;
            break;
          }
          else {
            numerator -= prob;
          }
        }
        // ここでidxが未定義のままだとバグります（アルゴリズム的には問題ないけど）
        // 仕方がないので適当に割り当てる（実際にバグるか知らないけど）
        if (idx == -1){
          for (int k =  0 ; k < n ; k++){
            if (ant_visit[k] == 0)
              continue;
            idx = k;
            break;
          }
        }
        // idxへ進む
        current_pos = idx;
        ant_visit[idx] = 0;
        ant_order[j] = idx;
      }
      
      // アリの移動距離を計測
      double l = get_cost(n, ant_order, city);

#ifdef DEBUG_ACO
      printf("ant%d:\t", i);
      for (int j = 0 ; j < n ; j++){
        printf("%d ", ant_order[j]);
      }
      printf("%lf\n", l);
#endif
      // フェロモンを薄くする
      for (int j = 0 ; j < n ; j++){
        for (int k = 0 ; k < n ; k++){
          if (j == k)
            continue;
          weight[j][k] *= vanish;
        }
      }
      // フェロモンを追加
      for (int j = 0 ; j < n ; j++){
        weight[ant_order[j]][ant_order[(j + 1) % n]] += Q / l;
      }
      // 答えの更新
      if (sum_d - l > eps){
        sum_d = l;
        // ant_orderは各アリのループの外側で宣言している(大して意味はないですがmalloc->freeの連続を回避するため)注意
        for (int j = 0 ; j < n ; j++)
          route[j] = ant_order[j];
      } 
    }
    // ちょっとクロスが目立つので、ここで2-optを挟んでおく
    // （これすらも結構な計算量なので微妙ですが）
    short update = 1;
    while (update){
      update = 0;
      for (int i = 1 ; i < n ; i++){
        for (int j = i + 1 ; j < n ; j++){
          double d1 = distance(city[route[i - 1]], city[route[i]]) + distance(city[route[j]], city[route[(j + 1) % n]]);
          double d2 = distance(city[route[i]], city[route[(j + 1) % n]]) + distance(city[route[i - 1]], city[route[j]]);
          if (d1 - d2 > eps){
            // reverse [i, j]
            int left = i;
            int right = j;
            while (left < right){
              int tmp = route[left];
              route[left] = route[right];
              route[right] = tmp;
              left++;
              right--;
            }
            update = 1;
          }
        }
      }
    }
    sum_d = get_cost(n, route, city);
    return sum_d;
  }
}

double get_cost(int n, int *route, City *city){
  // トータルの巡回距離を計算する
  double sum_d = 0;
  for (int i = 0 ; i < n ; i++){
    const int c0 = route[i];
    const int c1 = route[(i+1)%n]; // nは0に戻る
    sum_d += distance(city[c0],city[c1]);
  }
  return sum_d;
}

