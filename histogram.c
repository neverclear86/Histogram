#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

/* ヒストグラムを書く		*/

FILE *inputTxtp;		//data.txt(入力ファイル)のポインタ
FILE *inputp;			//data.csv(データ一時退避ファイル)のポインタ
FILE *outputp;			//output.txt(出力ファイル)のポインタ


void newCreate(int deci)	//data.txtを作る、データが無いのを警告
{

	if (deci == 0)			//data.txtが存在しない
	{
		inputTxtp = fopen("data.txt", "w");
		fprintf(inputTxtp, "範囲:\n");
		MessageBox(NULL, TEXT("[data.txt]を新規作成しました。\n[書式]\n1行目:区切る範囲\n2行目以降:数値(1つごとに改行)\n※少数には未対応※\n"),
			TEXT("ヒストグラム"), MB_OK);
		fclose(inputTxtp);
	}
	else					//data.txtにデータがない
	{
		MessageBox(NULL, TEXT("[data.txt]にデータが無いか、範囲が0です。\n[書式]\n1行目:区切る範囲\n2行目以降:数値(1つごとに改行)\n※少数には未対応※\n"),
			TEXT("ヒストグラム"), MB_OK);
	}


	return;
}




int main(void)
{

	int range = 0;			//データを区切る範囲
	int indat;				//データを入れる
	int datMax;				//データの最大値
	int datMin;				//データの最小値
	int cnt[100];			//階級ごとの頻度
	int cntMax = 0;			//最大頻度
	char histogram[50][50];	//グラフ描画
	int iMax;				//級の数
	int redunce;			//添字計算用
	int i;
	int j;


	/*data.txtの読み込み*/
	inputTxtp = fopen("data.txt", "r");

	if (inputTxtp == NULL)
	{
		newCreate(0);
		return 1;
	}

	fscanf(inputTxtp, "範囲:%d", &range);
	if (range == 0)
	{
		newCreate(1);
		return 1;
	}


	/*データの最大値・最小値を求める*/
	/*データをバイナリファイルに書き出し*/


	inputp = fopen("data.csv", "wb");

	if (fscanf(inputTxtp, "%d", &indat) > 0)		//１つ目のデータ
	{
		datMax = indat;
		datMin = indat;
		fwrite(&indat, sizeof(int), 1, inputp);
	}
	else
	{
		newCreate(1);
		return 1;
	}



	while (fscanf(inputTxtp, "%d", &indat) > 0)		//２つ目以降
	{
		if (datMax < indat)
		{
			datMax = indat;
		}

		if (datMin > indat)
		{
			datMin = indat;
		}

		fwrite(&indat, sizeof(int), 1, inputp);

	}

	iMax = (datMax - datMin) / range + 1;			//ヒストグラムの横の数(cntの使用量)

	/*cnt, histogramの初期化*/

	for (i = 0; i < iMax + 1; i++)
	{
		cnt[i] = 0;
		for (j = 0; j < 20; j++)
		{
			histogram[i][j] = ' ';
		}
	}


	redunce = datMin / range;						//添字計算用

	fclose(inputp);									//データを一度閉じて最初から読み直す
	inputp = fopen("data.csv", "rb");

	/*データの集計*/

	while (fread(&indat, sizeof(int), 1, inputp) > 0)
	{
		cnt[indat / range - redunce] += 1;
		cntMax = (cntMax < cnt[indat / range - redunce]) ?	cnt[indat / range - redunce] :
															cntMax;
	}

	/*ヒストグラムの作成*/

	for (i = 0; i < iMax; i++)
	{
		for (j = 0; j < cntMax + 1; j++)
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


	/*ヒストグラム描画処理*/
	/*ifが雑　どうにかせねば*/
	outputp = fopen("output.txt", "w");

	for (j = cntMax ; j >= 0; j--)
	{
		if (j % 5 == 0)
		{
			fprintf(outputp, "%d", j);
		}
		else
		{
			fprintf(outputp, " ");
		}
		fprintf(outputp, "_| ");

		for (i = 0; i < iMax; i++)
		{
			if (histogram[i][j] != '_')
			{
				if (cnt[i] < cnt[i - 1])
				{
					fprintf(outputp, "    ");
				}
				else
				{
					fprintf(outputp, "%c", histogram[i][j]);
					fprintf(outputp, "    ");
					
				}

				if (cnt[i] > cnt[i + 1])
				{
					fprintf(outputp, "%c", histogram[i][j]);
				}

			}
			else if (cnt[i] == 0)
			{
				fprintf(outputp, "    ");
			}
			else if (cnt[i] < cnt[i - 1])
			{
				if (cnt[i] > cnt[i + 1])
				{
					fprintf(outputp, "____ ");
				}
				else
				{
					fprintf(outputp, "____");
				}
			}
			else if (cnt[i] > cnt[i + 1])
			{
				fprintf(outputp, " ____ ");
			}
			else
			{
				fprintf(outputp, " ____");
			}
		}
		fprintf(outputp, "\n");
	}

	fprintf(outputp, "   ");
	for (i = 0; i < iMax; i++)
	{
		fprintf(outputp, "￣￣￣");
	}
	fprintf(outputp, "\n    ");
	for (i = 0; i < iMax; i++)
	{
		fprintf(outputp, " %4d", datMin / range * range + range * i);
	}
	fprintf(outputp, "\n    ");
	for (i = 0; i < iMax; i++)
	{
		fprintf(outputp, "   | ", datMin / range * range + range * (i + 1) - 1);
	}
	fprintf(outputp, "\n    ");
	for (i = 0; i < iMax; i++)
	{
		fprintf(outputp, " %4d", datMin / range * range + range * (i + 1) - 1);
	}


	MessageBox(NULL, TEXT("[output.txt]に出力しました。"),
		TEXT("ヒストグラム"), MB_OK);

	/*CloseFiles*/
	fclose(inputTxtp);
	fclose(inputp);
	fclose(outputp);

	return 0;
}