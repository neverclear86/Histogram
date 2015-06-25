#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define	DEBUG

#define	NUM			1000
#define SPACE		' '
#define SPACENUM	4

/* �q�X�g�O����������		*/


FILE *inputTxtp;			//data.txt(���̓t�@�C��)�̃|�C���^
FILE *inputp;				//data.csv(�f�[�^�ꎞ�ޔ��t�@�C��)�̃|�C���^
FILE *outputp;				//output.txt(�o�̓t�@�C��)�̃|�C���^


//data.txt�����A�f�[�^�������̂��x��
void newCreate(int deci)
{
	FILE *inputTxtp;

	if (deci == 0){			//data.txt�����݂��Ȃ�
		inputTxtp = fopen("data.txt", "w");
		fprintf(inputTxtp, "�͈�:\n");
		MessageBox(NULL, TEXT("[data.txt]��V�K�쐬���܂����B\n[����]\n1�s��:��؂�͈�\n2�s�ڈȍ~:���l(1���Ƃɉ��s)\n�������ɂ͖��Ή���\n"),
			TEXT("�q�X�g�O����"), MB_OK);
		fclose(inputTxtp);
	}
	else{					//data.txt�Ƀf�[�^���Ȃ�
		MessageBox(NULL, TEXT("[data.txt]�Ƀf�[�^���������A�͈͂�0�ł��B\n[����]\n1�s��:��؂�͈�\n2�s�ڈȍ~:���l(1���Ƃɉ��s)\n�������ɂ͖��Ή���\n"),
			TEXT("�q�X�g�O����"), MB_OK);
	}
	
	return;
}

//data.txt�̓ǂݍ���
int openDataFile( FILE *inputTxtp)
{
	int range;

	if (inputTxtp == NULL){
		newCreate(0);
		return 0;
	}

	fscanf(inputTxtp, "�͈�:%d", &range);
	if (fscanf(inputTxtp, "�͈�:%d", &range) == EOF){
		newCreate(1);
		return 0;
	}

	return range;
}

//�f�[�^���ꎞ�t�@�C���ɏ������݁E�f�[�^�̍ő�l�A�ŏ��l�����߂�
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
	
	// Y��
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
		// X��
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
	
	MessageBox(NULL, TEXT("[output.txt]�ɏo�͂��܂����B"),
		TEXT("�q�X�g�O����"), MB_OK);
}

void axisLabel(FILE *outputp, int datMin, int range, int iMax)
{
	int i;
	
	while (datMin > 10000)
	{
		datMin = datMin / 1000;
		range = range / 1000;
	}
	fprintf(outputp, "�P");
	for (i = 0; i < iMax; i++)
	{
		fprintf(outputp, "�P�P�P");
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
	int range;					//�f�[�^����؂�͈�
	int indat;					//Put data
	int datMax;					//�f�[�^�̍ő�l
	int datMin;					//�f�[�^�̍ŏ��l
	int redunce;				//�ŏ�����Y��0�ɍ��킹��
	int iMax;					//���̐�
	int cnt[NUM];				//�K�����Ƃ̕p�x
	int cntMax;					//�ő�p�x
	char histogram[NUM][NUM];	//�O���t�`��
	int i;
	int j;

	//data.txt�̓ǂݍ���
	inputTxtp = fopen("data.txt", "r");
	range = openDataFile(inputTxtp);

	if (range == 0){
		return 1;
	}

	/*�f�[�^�̍ő�l�E�ŏ��l�����߂�*/
	/*�f�[�^���ꎞ�o�C�i���t�@�C���ɏ����o��*/
	inputp = fopen("data.csv", "wb");
	readData(inputTxtp, inputp, &datMax, &datMin);

	//���̐�(cnt�̎g�p��)�����߂�(iMax)
	iMax = (datMax / range * range - datMin / range * range) / range + 1;
	
	//�ŏ�����Y��0�ɍ��킹��
	redunce = datMin / range;
	
	//cnt�̏�����
	initializeCnt(iMax, cnt);

	//�f�[�^����x���čŏ�����ǂݒ���
	fclose(inputp);									
	inputp = fopen("data.csv", "rb");
	
	//�f�[�^�̏W�v
	aggregate(inputp, cnt, &cntMax, range, redunce, iMax);
	
	//histogram�̏�����
	initializeHistogram(iMax, cntMax, histogram);

	//�q�X�g�O�����̍쐬
	makeHistogram(iMax, cntMax, cnt, histogram);

	//�q�X�g�O�����`�揈��
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