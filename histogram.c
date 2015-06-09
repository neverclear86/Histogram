#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

/* �q�X�g�O����������		*/

FILE *inputTxtp;		//data.txt(���̓t�@�C��)�̃|�C���^
FILE *inputp;			//data.csv(�f�[�^�ꎞ�ޔ��t�@�C��)�̃|�C���^
FILE *outputp;			//output.txt(�o�̓t�@�C��)�̃|�C���^


void newCreate(int deci)	//data.txt�����A�f�[�^�������̂��x��
{

	if (deci == 0)			//data.txt�����݂��Ȃ�
	{
		inputTxtp = fopen("data.txt", "w");
		fprintf(inputTxtp, "�͈�:\n");
		MessageBox(NULL, TEXT("[data.txt]��V�K�쐬���܂����B\n[����]\n1�s��:��؂�͈�\n2�s�ڈȍ~:���l(1���Ƃɉ��s)\n�������ɂ͖��Ή���\n"),
			TEXT("�q�X�g�O����"), MB_OK);
		fclose(inputTxtp);
	}
	else					//data.txt�Ƀf�[�^���Ȃ�
	{
		MessageBox(NULL, TEXT("[data.txt]�Ƀf�[�^���������A�͈͂�0�ł��B\n[����]\n1�s��:��؂�͈�\n2�s�ڈȍ~:���l(1���Ƃɉ��s)\n�������ɂ͖��Ή���\n"),
			TEXT("�q�X�g�O����"), MB_OK);
	}


	return;
}




int main(void)
{

	int range = 0;			//�f�[�^����؂�͈�
	int indat;				//�f�[�^������
	int datMax;				//�f�[�^�̍ő�l
	int datMin;				//�f�[�^�̍ŏ��l
	int cnt[100];			//�K�����Ƃ̕p�x
	int cntMax = 0;			//�ő�p�x
	char histogram[50][50];	//�O���t�`��
	int iMax;				//���̐�
	int redunce;			//�Y���v�Z�p
	int i;
	int j;


	/*data.txt�̓ǂݍ���*/
	inputTxtp = fopen("data.txt", "r");

	if (inputTxtp == NULL)
	{
		newCreate(0);
		return 1;
	}

	fscanf(inputTxtp, "�͈�:%d", &range);
	if (range == 0)
	{
		newCreate(1);
		return 1;
	}


	/*�f�[�^�̍ő�l�E�ŏ��l�����߂�*/
	/*�f�[�^���o�C�i���t�@�C���ɏ����o��*/


	inputp = fopen("data.csv", "wb");

	if (fscanf(inputTxtp, "%d", &indat) > 0)		//�P�ڂ̃f�[�^
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



	while (fscanf(inputTxtp, "%d", &indat) > 0)		//�Q�ڈȍ~
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

	iMax = (datMax - datMin) / range + 1;			//�q�X�g�O�����̉��̐�(cnt�̎g�p��)

	/*cnt, histogram�̏�����*/

	for (i = 0; i < iMax + 1; i++)
	{
		cnt[i] = 0;
		for (j = 0; j < 20; j++)
		{
			histogram[i][j] = ' ';
		}
	}


	redunce = datMin / range;						//�Y���v�Z�p

	fclose(inputp);									//�f�[�^����x���čŏ�����ǂݒ���
	inputp = fopen("data.csv", "rb");

	/*�f�[�^�̏W�v*/

	while (fread(&indat, sizeof(int), 1, inputp) > 0)
	{
		cnt[indat / range - redunce] += 1;
		cntMax = (cntMax < cnt[indat / range - redunce]) ?	cnt[indat / range - redunce] :
															cntMax;
	}

	/*�q�X�g�O�����̍쐬*/

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


	/*�q�X�g�O�����`�揈��*/
	/*if���G�@�ǂ��ɂ����˂�*/
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
		fprintf(outputp, "�P�P�P");
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


	MessageBox(NULL, TEXT("[output.txt]�ɏo�͂��܂����B"),
		TEXT("�q�X�g�O����"), MB_OK);

	/*CloseFiles*/
	fclose(inputTxtp);
	fclose(inputp);
	fclose(outputp);

	return 0;
}