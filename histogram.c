#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define	DEBUG

#define	NUM			1000
#define SPACE		' '
#define SPACENUM	4

/* ヒストグラムを書く		*/

typedef struct {
	int min;	//最小値
	int max;	//最大値
} MinMax;

typedef struct {
	FILE *inputTxtp;			//data.txt(入力ファイル)のポインタ
	FILE *inputp;				//data.bin(データ一時退避ファイル)のポインタ
	FILE *outputp;				//output.txt(出力ファイル)のポインタ
} Files ;


/******************************************************************************************************/
//data.txtを作る、データが無いのを警告
void newCreate(int deci)
{
	FILE *inputTxtp;

	if (deci == 0){			//data.txtが存在しない
		inputTxtp = fopen("data.txt", "w");
		fprintf(inputTxtp, "範囲:\n");
		MessageBox(NULL, TEXT("[data.txt]を新規作成しました。\n[書式]\n1行目:区切る範囲\n2行目以降:数値(1つごとに改行)\n※少数には未対応※\n"),
			TEXT("ヒストグラム"), MB_OK);
		fclose(inputTxtp);
	}
	else{					//data.txtにデータがない
		MessageBox(NULL, TEXT("[data.txt]にデータが無いか、範囲が0です。\n[書式]\n1行目:区切る範囲\n2行目以降:数値(1つごとに改行)\n※少数には未対応※\n"),
			TEXT("ヒストグラム"), MB_OK);
	}
	
	return;
}
/******************************************************************************************************/
//data.txtの読み込み
int openDataFile( FILE *inputTxtp)
{
	int range;

	if (inputTxtp == NULL){
		newCreate(0);
		return 0;
	}

	fscanf(inputTxtp, "範囲:%d", &range);
	if (fscanf(inputTxtp, "範囲:%d", &range) == EOF){
		newCreate(1);
		return 0;
	}

	return range;
}
/******************************************************************************************************/
//データを一時ファイルに書き込み・データの最大値、最小値を求める
MinMax readData(FILE *inputTxtp, FILE *inputp)
{
	int indat;
	MinMax tmpMM;

	if (fscanf(inputTxtp, "%d", &indat) != EOF){		//FirstData
		tmpMM.min = indat;
		tmpMM.max = indat;
		fwrite(&indat, sizeof(int), 1, inputp);
	}
	else{
		newCreate(1);
		return;
	}

	while (fscanf(inputTxtp, "%d", &indat) != EOF){	//SecondData on and after
		if (tmpMM.max < indat){
			tmpMM.max = indat;
		}

		if (tmpMM.min > indat){
			tmpMM.min = indat;
		}

		fwrite(&indat, sizeof(int), 1, inputp);
	}

	return;
}
/******************************************************************************************************/
void initializeCnt(int iMax, int *cnt)
{
	int i;
	
	for (i = 0; i < iMax + 1; i++){
		cnt[i] = 0;
	}
	return;
}
/******************************************************************************************************/
void initializeHistogram(int iMax, int cntMax, char histogram[][NUM])
{
	int i;
	int j;
	
	for (i = 0; i < iMax + 1; i++){
		for (j = 0; j <= cntMax; j++){
			histogram[i][j] = SPACE;
		}
	}
}
/******************************************************************************************************/
void aggregate(FILE *inputp,int *cnt, int *cntMax, int range, int redunce, int iMax)
{
	int indat;
	int i;
	
	while (fread(&indat, sizeof(int), 1, inputp)){
		cnt[indat / range - redunce] += 1;
	}
	
	*cntMax = 0;
	for(i = 0; i < iMax; i++){
		if(*cntMax < cnt[i]){
			*cntMax = cnt[i];
		}
	}
}
/******************************************************************************************************/
void makeHistogram(int iMax, int cntMax, int cnt[], char histogram[][NUM])
{
	int i;
	int j;

	initializeHistogram(iMax, cntMax, histogram);
	for (i = 0; i < iMax; i++){
		for (j = 0; j <= cntMax; j++){
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
void drawHistogram(FILE *outputp, int iMax, int cntMax, int cnt[], char histogram[][NUM])
{
	int i, j;
	int flag;
	
	// Y軸
	for (j = cntMax ; j >= 0; j--){
		if (j % 5 == 0){
			fprintf(outputp, "%3d", j);
		}
		else{
			writer(SPACE, 3);
		}
		fprintf(outputp, "_|");

		flag = 0;
		// X軸
		for (i = 0; i < iMax; i++){
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
void axisLabel(FILE *outputp, int datMin, int range, int iMax)
{
	int i;
	
	while (datMin > 10000){
		datMin = datMin / 1000;
		range = range / 1000;
	}
	fprintf(outputp, "\n");
	writer(SPACE, 5);
	for (i = 0; i < iMax; i++){
		fprintf(outputp, "%5d", datMin / range * range + range * i);
	}
	fprintf(outputp, "\n");
	writer(SPACE, 5);
	for (i = 0; i < iMax; i++){
		fprintf(outputp, "   / ");
	}
	fprintf(outputp, "\n");
	writer(SPACE, 5);
	for (i = 0; i < iMax - 1; i++){
		fprintf(outputp, "%5d", datMin / range * range + range * (i + 1) - 1);
	}
	
}
/******************************************************************************************************/

/******************************************************************************************************/
void histogramMain(void)
{
	int		range;					//データを区切る範囲
	int		indat;					//データ読み込み
//	int		datMax;					//データの最大値
//	int		datMin;					//データの最小値
	int		redunce;				//最小級を添字0に合わせる
	int		iMax;					//級の数
	int		cnt[NUM];				//階級ごとの頻度
	int		cntMax;					//最大頻度
	char	histogram[NUM][NUM];

	MinMax mainMM;
	Files files;
		
	//FILE *inputTxtp;			//data.txt(入力ファイル)のポインタ
	//FILE *inputp;				//data.bin(データ一時退避ファイル)のポインタ
	//FILE *outputp;				//output.txt(出力ファイル)のポインタ
	//if (inputTxtp = fopen("data.txt", "r") == NULL) { 
	//	/*Failed open data.txt*/
	//	newCreate(0);
	//} 
	//else if ((inputp = fopen("data.bin", "wb")) == NULL) {
	//	/*Failed open data.bin*/
	//	fclose(inputTxtp);
	//} 
	//else if ((outputp = fopen("output.txt", "w")) == NULL) {
	//	/*Failed open output.txt*/
	//	fclose(inputTxtp);
	//	fclose(inputp);
	//}
	//else {
	//	/*Successful open all files*/
	//	range = openDataFile(inputTxtp);
	//	if (range == 0){
	//		newCreate(1);
	//	}
	//	/*データの最大値・最小値を求める*/
	//	/*データを一時バイナリファイルに書き出し*/
	//	mainMM = readData(inputTxtp, inputp);
	//	fclose(inputTxtp);
	//	fclose(inputp);
	//	fclose(outputp);
	//	remove("data.bin");
	//}

	//return;

	files = OpenFiles(files);

	//級の数(cntの使用数)を求める(iMax)
	iMax = (mainMM.max / range * range - mainMM.max / range * range) / range + 1;
	
	//最小級を添字0に合わせる
	redunce = mainMM.min / range;
	
	//cntの初期化
	initializeCnt(iMax, cnt);

	//データを一度閉じて最初から読み直す								
	files.inputp = fopen("data.bin", "rb");
	if(files.inputp == NULL){
		MessageBox(NULL, TEXT("エラーが発生しました"),
				TEXT("ヒストグラム"), MB_OK);
		return;
	}
	
	//データの集計
	aggregate(files.inputp, cnt, &cntMax, range, redunce, iMax);
	
	//ヒストグラムの作成
	makeHistogram(iMax, cntMax, cnt, histogram);

	//ヒストグラム描画処理
	drawHistogram(files.outputp, iMax, cntMax, cnt, histogram);
	axisLabel(files.outputp, mainMM.max, range, iMax);

	CloseFiles(files)
	
	MessageBox(NULL, TEXT("[output.txt]に出力しました。"),
				TEXT("ヒストグラム"), MB_OK);
}

/******************************************************************************************************/

Files OpenFiles(void)
{
	Files tmpFiles;

	if (tmpFiles.inputTxtp = fopen("data.txt", "r") == NULL) {
		/*Failed open data.txt*/
		newCreate(0);
	}
	else if ((tmpFiles.inputp = fopen("data.bin", "wb")) == NULL) {
		/*Failed open data.bin*/
		fclose(tmpFiles.inputTxtp);
	}
	else if ((tmpFiles.outputp = fopen("output.txt", "w")) == NULL) {
		/*Failed open output.txt*/
		fclose(tmpFiles.inputTxtp);
		fclose(tmpFiles.inputp);
	}
	else {
		/*Successful open all files*/
		range = openDataFile(tmpFiles.inputTxtp);
		if (range == 0){
			newCreate(1);
		}
		/*データの最大値・最小値を求める*/
		/*データを一時バイナリファイルに書き出し*/
		mainMM = readData(tmpFiles.inputTxtp, tmpFiles.inputp);
	}

	return;
}
/******************************************************************************************************/

void CloseFiles(Files *files){
	fclose(files->inputTxtp);
	fclose(files->inputp);
	fclose(files->outputp);
	remove("data.bin");
}

/**** main program ***********************************************************************/
int main(void)
{
	histogramMain();
	return 0;
}