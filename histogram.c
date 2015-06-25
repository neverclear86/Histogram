#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define	DEBUG

#define	NUM			1000
#define SPACE		' '
#define SPACENUM	4

/* ヒストグラムを書く		*/


FILE *inputTxtp;			//data.txt(入力ファイル)のポインタ
FILE *inputp;				//data.csv(データ一時退避ファイル)のポインタ
FILE *outputp;				//output.txt(出力ファイル)のポインタ


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

//データを一時ファイルに書き込み・データの最大値、最小値を求める
void readData(FILE *inputTxtp, FILE *inputp, int *datMax, int *datMin)
{
	int indat;

	if (fscanf(inputTxtp, "%d", &indat) != EOF){		//FirstData
		*datMax = indat;
		*datMin = indat;
		fwrite(&indat, sizeof(int), 1, inputp);
	}
	else{
		newCreate(1);
		return;
	}

	while (fscanf(inputTxtp, "%d", &indat) != EOF){	//SecondData on and after
		if (*datMax < indat){
			*datMax = indat;
		}

		if (*datMin > indat){
			*datMin = indat;
		}

		fwrite(&indat, sizeof(int), 1, inputp);
	}

	return;
}

void initializeCnt(int iMax, int *cnt)
{
	int i;
	
	for (i = 0; i < iMax + 1; i++){
		cnt[i] = 0;
	}
	return;
}

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

void aggregate(FILE *inputp,int *cnt, int *cntMax, int range, int redunce, int iMax)
{
	int indat;
	int i;
	
	while (fread(&indat, sizeof(int), 1, inputp))
	{
		cnt[indat / range - redunce] += 1;
	}
	
	*cntMax = 0;
	for(i = 0; i < iMax; i++)
	{
		if(*cntMax < cnt[i])
		{
			*cntMax = cnt[i];
		}
	}
}

void makeHistogram(int iMax, int cntMax, int cnt[],char histogram[][NUM])
{
	int i;
	int j;
	
	for (i = 0; i < iMax; i++)
	{
		for (j = 0; j <= cntMax; j++)
		{
			if (cnt[i] > j)
			{
				histogram[i][j] = '|';
			}
			else if (cnt[i] == j)
			{
				histogram[i][j] = '_';
			}
		}
	}
}

void writer(char chara, int num)
{
	int i;
	
	if(chara == '_')
	{
		chara = ' ';
	}
	
	for( i = 0; i < num; i++)
	{
		fprintf(outputp, "%c", chara);
	}
}

void drawHistogram(FILE *outputp, int iMax, int cntMax, int cnt[], char histogram[][NUM])
{
	int i, j;
	int flag;
	
	// Y軸
	for (j = cntMax ; j >= 0; j--)
	{
		if (j % 5 == 0)
		{
			fprintf(outputp, "%3d", j);
		}
		else
		{
			writer(SPACE, 3);
		}
		fprintf(outputp, "_|");

		flag = 0;
		// X軸
		for (i = 0; i < iMax; i++)
		{
			if(flag == 0)
			{
				writer(histogram[i][j], 1);
			}
			
			if(histogram[i][j] != '_')
			{
				writer(SPACE, SPACENUM);
			}
			else
			{
				fprintf(outputp, "____");
			}
			flag = 0;
			
			if(cnt[i] > cnt[i + 1])
			{
				writer(histogram[i][j], 1);
				flag = 1;
			}
		}
		fprintf(outputp, "\n");
	}
	
	MessageBox(NULL, TEXT("[output.txt]に出力しました。"),
		TEXT("ヒストグラム"), MB_OK);
}

void axisLabel(FILE *outputp, int datMin, int range, int iMax)
{
	int i;
	
	while (datMin > 10000)
	{
		datMin = datMin / 1000;
		range = range / 1000;
	}
	fprintf(outputp, "￣");
	for (i = 0; i < iMax; i++)
	{
		fprintf(outputp, "￣￣￣");
	}
	fprintf(outputp, "\n     ");
	for (i = 0; i < iMax; i++)
	{
		fprintf(outputp, " %4d", datMin / range * range + range * i);
	}
	fprintf(outputp, "\n     ");
	for (i = 0; i < iMax; i++)
	{
		fprintf(outputp, "   | ", datMin / range * range + range * (i + 1) - 1);
	}
	fprintf(outputp, "\n     ");
	for (i = 0; i < iMax - 1; i++)
	{
		fprintf(outputp, " %4d", datMin / range * range + range * (i + 1) - 1);
	}
	
}

/**** main program ***********************************************************************/
int main(void)
{
	int range;					//データを区切る範囲
	int indat;					//Put data
	int datMax;					//データの最大値
	int datMin;					//データの最小値
	int redunce;				//最小級を添字0に合わせる
	int iMax;					//級の数
	int cnt[NUM];				//階級ごとの頻度
	int cntMax;					//最大頻度
	char histogram[NUM][NUM];	//グラフ描画
	int i;
	int j;

	//data.txtの読み込み
	inputTxtp = fopen("data.txt", "r");
	range = openDataFile(inputTxtp);

	if (range == 0){
		return 1;
	}

	/*データの最大値・最小値を求める*/
	/*データを一時バイナリファイルに書き出し*/
	inputp = fopen("data.csv", "wb");
	readData(inputTxtp, inputp, &datMax, &datMin);

	//級の数(cntの使用数)を求める(iMax)
	iMax = (datMax / range * range - datMin / range * range) / range + 1;
	
	//最小級を添字0に合わせる
	redunce = datMin / range;
	
	//cntの初期化
	initializeCnt(iMax, cnt);

	//データを一度閉じて最初から読み直す
	fclose(inputp);									
	inputp = fopen("data.csv", "rb");
	
	//データの集計
	aggregate(inputp, cnt, &cntMax, range, redunce, iMax);
	
	//histogramの初期化
	initializeHistogram(iMax, cntMax, histogram);

	//ヒストグラムの作成
	makeHistogram(iMax, cntMax, cnt, histogram);

	//ヒストグラム描画処理
	outputp = fopen("output.txt", "w");
	drawHistogram(outputp, iMax, cntMax, cnt, histogram);
	axisLabel(outputp, datMin, range, iMax);

	//CloseFiles
	fclose(inputTxtp);
	fclose(inputp);
	fclose(outputp);

	remove("data.csv");

	return 0;
}