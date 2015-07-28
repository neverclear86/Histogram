#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define	DEBUG

#define	NUM			1000
#define SPACE		' '
#define SPACENUM	4

/* �q�X�g�O����������		*/

typedef struct {
	int min;	//�ŏ��l
	int max;	//�ő�l
} MinMax;

typedef struct {
	FILE *inputTxtp;			//data.txt(���̓t�@�C��)�̃|�C���^
	FILE *inputp;				//data.bin(�f�[�^�ꎞ�ޔ��t�@�C��)�̃|�C���^
	FILE *outputp;				//output.txt(�o�̓t�@�C��)�̃|�C���^
} Files ;


/******************************************************************************************************/
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
/******************************************************************************************************/
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
/******************************************************************************************************/
//�f�[�^���ꎞ�t�@�C���ɏ������݁E�f�[�^�̍ő�l�A�ŏ��l�����߂�
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
	
	// Y��
	for (j = cntMax ; j >= 0; j--){
		if (j % 5 == 0){
			fprintf(outputp, "%3d", j);
		}
		else{
			writer(SPACE, 3);
		}
		fprintf(outputp, "_|");

		flag = 0;
		// X��
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
	int		range;					//�f�[�^����؂�͈�
	int		indat;					//�f�[�^�ǂݍ���
//	int		datMax;					//�f�[�^�̍ő�l
//	int		datMin;					//�f�[�^�̍ŏ��l
	int		redunce;				//�ŏ�����Y��0�ɍ��킹��
	int		iMax;					//���̐�
	int		cnt[NUM];				//�K�����Ƃ̕p�x
	int		cntMax;					//�ő�p�x
	char	histogram[NUM][NUM];

	MinMax mainMM;
	Files files;
		
	//FILE *inputTxtp;			//data.txt(���̓t�@�C��)�̃|�C���^
	//FILE *inputp;				//data.bin(�f�[�^�ꎞ�ޔ��t�@�C��)�̃|�C���^
	//FILE *outputp;				//output.txt(�o�̓t�@�C��)�̃|�C���^
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
	//	/*�f�[�^�̍ő�l�E�ŏ��l�����߂�*/
	//	/*�f�[�^���ꎞ�o�C�i���t�@�C���ɏ����o��*/
	//	mainMM = readData(inputTxtp, inputp);
	//	fclose(inputTxtp);
	//	fclose(inputp);
	//	fclose(outputp);
	//	remove("data.bin");
	//}

	//return;

	files = OpenFiles(files);

	//���̐�(cnt�̎g�p��)�����߂�(iMax)
	iMax = (mainMM.max / range * range - mainMM.max / range * range) / range + 1;
	
	//�ŏ�����Y��0�ɍ��킹��
	redunce = mainMM.min / range;
	
	//cnt�̏�����
	initializeCnt(iMax, cnt);

	//�f�[�^����x���čŏ�����ǂݒ���								
	files.inputp = fopen("data.bin", "rb");
	if(files.inputp == NULL){
		MessageBox(NULL, TEXT("�G���[���������܂���"),
				TEXT("�q�X�g�O����"), MB_OK);
		return;
	}
	
	//�f�[�^�̏W�v
	aggregate(files.inputp, cnt, &cntMax, range, redunce, iMax);
	
	//�q�X�g�O�����̍쐬
	makeHistogram(iMax, cntMax, cnt, histogram);

	//�q�X�g�O�����`�揈��
	drawHistogram(files.outputp, iMax, cntMax, cnt, histogram);
	axisLabel(files.outputp, mainMM.max, range, iMax);

	CloseFiles(files)
	
	MessageBox(NULL, TEXT("[output.txt]�ɏo�͂��܂����B"),
				TEXT("�q�X�g�O����"), MB_OK);
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
		/*�f�[�^�̍ő�l�E�ŏ��l�����߂�*/
		/*�f�[�^���ꎞ�o�C�i���t�@�C���ɏ����o��*/
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