#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define  NUM      1000
#define  SPACE    ' '
#define  SPACENUM  4

/* ヒストグラムを書く             */

typedef struct {
  int min;  //最小値
  int max;  //最大値
} min_max;

typedef struct {
  FILE *inputp;      			//data.txt(入力ファイル)のポインタ
  FILE *tmp_data;        	//data.bin(データ一時退避ファイル)のポインタ
  FILE *outputp;        	//output.txt(出力ファイル)のポインタ
} files ;

/*Prototype Declaration*/
void      histogram_main(void);
int       open_files(files *files);
void      create_files(int deci);
int       open_data_file( FILE *inputp);
min_max   read_data(FILE *inputp, FILE *tmp_data);
void      initialize_cnt(int i_max, int *cnt);
void      initialize_histogram(int i_max, int cnt_max, char histogram[][NUM]);
void      aggregate(FILE *tmp_data,int *cnt, int *cnt_max, int range, int redunce, int i_max);
void      make_histogram(int i_max, int cnt_max, int cnt[], char histogram[][NUM]);
void      writer(char chara, int num);
void      draw_histogram(FILE *outputp, int i_max, int cnt_max, int cnt[], char histogram[][NUM]);
void      axis_label(FILE *outputp, int datMin, int range, int i_max);
void      close_files(files *files);


/**** main program ***********************************************************************/
int main(void)
{
  histogramMain();
  return 0;
}

/******************************************************************************************************/
void histogram_main(void)
{
  int      open_flag                     //ファイルが開けたかの判断
  int      range;                        //データを区切る範囲
  int      indat;                        //データ読み込み
  int      redunce;                      //最小級を添字0に合わせる
  int      i_max;                        //級の数
  int      cnt[NUM];                     //階級ごとの頻度
  int      cnt_max;                      //最大頻度
  char     histogram[NUM][NUM];          //
  min_max  mainMM;                       //
  files    files;                        //

  //FILE *inputp;      //data.txt(入力ファイル)のポインタ
  //FILE *tmp_data;        //data.bin(データ一時退避ファイル)のポインタ
  //FILE *outputp;        //output.txt(出力ファイル)のポインタ
  //if (inputp = fopen("data.txt", "r") == NULL) {
  //  /*Failed open data.txt*/
  //  create_files(0);
  //}
  //else if ((tmp_data = fopen("data.bin", "wb")) == NULL) {
  //  /*Failed open data.bin*/
  //  fclose(inputp);
  //}
  //else if ((outputp = fopen("output.txt", "w")) == NULL) {
  //  /*Failed open output.txt*/
  //  fclose(inputp);
  //  fclose(tmp_data);
  //}
  //else {
  //  /*Successful open all files*/
  //  range = open_data_file(inputp);
  //  if (range == 0){
  //    create_files(1);
  //  }
  //  /*データの最大値・最小値を求める*/
  //  /*データを一時バイナリファイルに書き出し*/
  //  mainMM = read_data(inputp, tmp_data);
  //  fclose(inputp);
  //  fclose(tmp_data);
  //  fclose(outputp);
  //  remove("data.bin");
  //}
  //return;

  //3つのファイルを開く　なければ作る
  open_flag = Openfiles();

	if(open_flag){

	}
  /*データの最大値・最小値を求める*/
  /*データを一時バイナリファイルに書き出し*/
  mainMM = read_data(tmp_files.inputp, tmp_files.tmp_data);

  //級の数(cntの使用数)を求める(i_max)
  i_max = (mainMM.max / range * range - mainMM.max / range * range) / range + 1;

  //最小級を添字0に合わせる
  redunce = mainMM.min / range;

  //cntの初期化
  initialize_cnt(i_max, cnt);

  //データを一度閉じて最初から読み直す
  files.tmp_data = fopen("data.bin", "rb");
  if(files.tmp_data == NULL){
    MessageBox(NULL, TEXT("エラーが発生しました"),
        TEXT("ヒストグラム"), MB_OK);
  }

  //データの集計
  aggregate(files.tmp_data, cnt, &cnt_max, range, redunce, i_max);

  //ヒストグラムの作成
  make_histogram(i_max, cnt_max, cnt, histogram);

  //ヒストグラム描画処理
  draw_histogram(files.outputp, i_max, cnt_max, cnt, histogram);
  axis_label(files.outputp, mainMM.max, range, i_max);

	MessageBox(NULL, TEXT("[output.txt]に出力しました。"),
        TEXT("ヒストグラム"), MB_OK);

  close_files(files)
}

/******************************************************************************************************/
int open_files(files *files)
{
  int open_flag = 0;

  if (files->inputp = fopen("data.txt", "r") == NULL) {
    /*Failed open data.txt*/
    create_files(0);
  }
  else if ((files->tmp_data = fopen("data.bin", "wb")) == NULL) {
    /*Failed open data.bin*/
    fclose(files->inputp);
  }
  else if ((files->outputp = fopen("output.txt", "w")) == NULL) {
    /*Failed open output.txt*/
    fclose(files->inputp);
    fclose(files->tmp_data);
  }
  else {
    /*Successful open all files*/
    range = open_data_file(files->inputp);
    if (range == 0){
      create_files(1);
    }
		else{
			open_flag = 1;
		}
  }
  return open_flag;
}
/******************************************************************************************************/
//data.txtを作る、データが無いのを警告
void create_files(int deci)
{
  FILE *inputp;

  if (deci == 0){          //data.txtが存在しない
    inputp = fopen("data.txt", "w");
    fprintf(inputp, "範囲:\n");
    MessageBox(NULL, TEXT("[data.txt]を新規作成しました。\n[書式]\n1行目:区切る範囲\n2行目以降:数値(1つごとに改行)\n※少数には未対応※\n"),
      TEXT("ヒストグラム"), MB_OK);
    fclose(inputp);
  }
  else{                   //data.txtにデータがない
    MessageBox(NULL, TEXT("[data.txt]にデータが無いか、範囲が0です。\n[書式]\n1行目:区切る範囲\n2行目以降:数値(1つごとに改行)\n※少数には未対応※\n"),
      TEXT("ヒストグラム"), MB_OK);
  }
}
/******************************************************************************************************/
//data.txtの読み込み
int open_data_file( FILE *inputp)
{
  int range;

  if (inputp == NULL){
    create_files(0);
    return 0;
  }
  fscanf(inputp, "範囲:%d", &range);
  if (fscanf(inputp, "範囲:%d", &range) == EOF){
    create_files(1);
    return 0;
  }
  return range;
}
/******************************************************************************************************/
//データを一時ファイルに書き込み・データの最大値、最小値を求める
min_max read_data(FILE *inputp, FILE *tmp_data)
{
  int indat;
  min_max tmpMM;

  if (fscanf(inputp, "%d", &indat) != EOF){    //FirstData
    tmpMM.min = indat;
    tmpMM.max = indat;
    fwrite(&indat, sizeof(int), 1, tmp_data);
  }
  else{
    create_files(1);
    return;
  }

  while (fscanf(inputp, "%d", &indat) != EOF){  //SecondData on and after
    if (tmpMM.max < indat){
      tmpMM.max = indat;
    }

    if (tmpMM.min > indat){
      tmpMM.min = indat;
    }

    fwrite(&indat, sizeof(int), 1, tmp_data);
  }

  return;
}
/******************************************************************************************************/
void initialize_cnt(int i_max, int *cnt)
{
  int i;

  for (i = 0; i < i_max + 1; i++){
    cnt[i] = 0;
  }
  return;
}
/******************************************************************************************************/
void initialize_histogram(int i_max, int cnt_max, char histogram[][NUM])
{
  int i;
  int j;

  for (i = 0; i < i_max + 1; i++){
    for (j = 0; j <= cnt_max; j++){
      histogram[i][j] = SPACE;
    }
  }
}
/******************************************************************************************************/
void aggregate(FILE *tmp_data,int *cnt, int *cnt_max, int range, int redunce, int i_max)
{
  int indat;
  int i;

  while (fread(&indat, sizeof(int), 1, tmp_data)){
    cnt[indat / range - redunce] += 1;
  }

  *cnt_max = 0;
  for(i = 0; i < i_max; i++){
    if(*cnt_max < cnt[i]){
      *cnt_max = cnt[i];
    }
  }
}
/******************************************************************************************************/
void make_histogram(int i_max, int cnt_max, int cnt[], char histogram[][NUM])
{
  int i;
  int j;

  initialize_histogram(i_max, cnt_max, histogram);
  for (i = 0; i < i_max; i++){
    for (j = 0; j <= cnt_max; j++){
      if (cnt[i] > j){
        histogram[i][j] = '|';
      }
      else if (cnt[i] == j){
        histogram[i][j] = '_';
      }
    }
  }
}
/******************************************************************************************************/
void writer(char chara, int num)
{
  int i;

  if(chara == '_' && num <= 1){
    chara = SPACE;
  }

  for( i = 0; i < num; i++){
    fprintf(outputp, "%c", chara);
  }
}
/******************************************************************************************************/
void draw_histogram(FILE *outputp, int i_max, int cnt_max, int cnt[], char histogram[][NUM])
{
  int i, j;
  int flag;

  // Y軸
  for (j = cnt_max ; j >= 0; j--){
    if (j % 5 == 0){
      fprintf(outputp, "%3d", j);
    }
    else{
      writer(SPACE, 3);
    }
    fprintf(outputp, "_|");

    flag = 0;
    // X軸
    for (i = 0; i < i_max; i++){
      if(flag == 0){
        writer(histogram[i][j], 1);
      }

      if(histogram[i][j] != '_'){
        if (j != 0){
          writer(SPACE, SPACENUM);
        }
        else{
          writer('_', SPACENUM);
        }
      }
      else{
        writer('_', SPACENUM);
      }
      flag = 0;

      if(cnt[i] > cnt[i + 1]){
        writer(histogram[i][j], 1);
        flag = 1;
      }
    }
    fprintf(outputp, "\n");
  }
}
/******************************************************************************************************/
void axis_label(FILE *outputp, int datMin, int range, int i_max)
{
  int i;

  while (datMin > 10000){
    datMin = datMin / 1000;
    range = range / 1000;
  }
  fprintf(outputp, "\n");
  writer(SPACE, 5);
  for (i = 0; i < i_max; i++){
    fprintf(outputp, "%5d", datMin / range * range + range * i);
  }
  fprintf(outputp, "\n");
  writer(SPACE, 5);
  for (i = 0; i < i_max; i++){
    fprintf(outputp, "   / ");
  }
  fprintf(outputp, "\n");
  writer(SPACE, 5);
  for (i = 0; i < i_max - 1; i++){
    fprintf(outputp, "%5d", datMin / range * range + range * (i + 1) - 1);
  }

}
/******************************************************************************************************/

void close_files(files *files)
{
  fclose(files->inputp);
  fclose(files->tmp_data);
  fclose(files->outputp);
  remove("data.bin");
}
