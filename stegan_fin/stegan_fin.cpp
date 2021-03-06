#define _CRT_SECURE_NO_WARNINGS
#pragma pack(1)
#include "pch.h"
#include <iostream>
#include <fstream>
#include <string>
#include "Header.h"
#include <vector>
using namespace std;
#include <iostream>
#include <fstream>

class Stegan {
	vector<int> key_main;
	int times_in;
	int times_all;
	string text;
	int _mx, _my;
public:
	Stegan() {
		times_in = 0;
		times_all = 0;
	}
	int *loadBMP(const char *fname)
	{	
		
		int mx,my = -1;
		FILE *f = fopen(fname, "rb");
		if (!f) return NULL;
		BMPheader bh;    
		size_t res;

		// читаем заголовок
		res = fread(&bh, 1, sizeof(BMPheader), f);
		if (res != sizeof(BMPheader)) { fclose(f); return NULL; }

		// проверяем сигнатуру
		if (bh.bfType != 0x4d42 && bh.bfType != 0x4349 && bh.bfType != 0x5450) { fclose(f); return NULL; }

		// проверка размера файла
		fseek(f, 0, SEEK_END);
		int filesize = ftell(f);
		// восстановим указатель в файле:
		fseek(f, sizeof(BMPheader), SEEK_SET);
		// проверим условия
		if (bh.bfSize != filesize ||
			bh.bfReserved != 0 ||
			bh.biPlanes != 1 ||
			(bh.biSize != 40 && bh.biSize != 108 && bh.biSize != 124) ||
			bh.bfOffBits != 14 + bh.biSize ||

			bh.biWidth < 1 || bh.biWidth >10000 ||
			bh.biHeight < 1 || bh.biHeight>10000 ||
			bh.biBitCount != 24 ||             // пока рассматриваем только полноцветные изображения
			bh.biCompression != 0                // пока рассматриваем только несжатие изображения
			)
		{
			fclose(f);
			return NULL;
		}
		// Заголовок прочитан и проверен, тип - верный (BGR-24), размеры (mx,my) найдены
		mx = bh.biWidth;
		_mx = mx;
		my = bh.biHeight;
		_my = my;
		int mx3 = (3 * mx + 3) & (-4);    // Compute row width in file, including padding to 4-byte boundary
		unsigned char *tmp_buf = new unsigned  char[mx3*my];    // читаем данные
		res = fread(tmp_buf, 1, mx3*my, f);
		if ((int)res != mx3 * my) { delete[]tmp_buf; fclose(f); return NULL; }
		// данные прочитаны
		fclose(f);
		// выделим память для результата
		int * v = new int[mx*my];
		// Перенос данных (не забудем про BGR->RGB)
		unsigned char *ptr = (unsigned char *)v;
		for (int y = my - 1; y >= 0; y--) {
			unsigned char *pRow = tmp_buf + mx3 * y;
			for (int x = 0; x < mx; x++) {
				*ptr++ = *(pRow + 2);
				*ptr++ = *(pRow + 1);
				*ptr++ = *pRow;
				pRow += 3;
				ptr++;
			}
		}
		delete[]tmp_buf;
		return v;    // OK
	}
	
	int *crypt(int*v, int key, int rate) {
		
		int all_pixels = _mx * _my;
		int i = 0;
		unsigned char *ptr = (unsigned char*)v;
		int mx3 = (3 * _mx + 3) & (-4);
		unsigned char *tmp_buf = new unsigned  char[mx3*_my];
		for (int y = _my - 1; y >= 0; y--) { //идем по массиву
			unsigned char *pRow = tmp_buf + mx3 * y;
			for (int x = 0; x < _mx; x++) {
				
					if (i == rate && key>0) { //условие выполняется, если ключ больше 0 и момент рэйта
						
						
						*ptr++ = ReadBitToByte(key%10, *pRow);  //последний цифру ключа записываем в пиксель
						i = 0;
						key = key / 10;
						times_in++; //количество раз, которое зайдет алгоритм
					}
					i++;
					pRow += 3;
					ptr++;
			}
		}
		times_all = times_in; //такой же счетчик, как и times_in. Просто не обнуляется и нужен только для вывода количества обходов алгоритма
		delete[] tmp_buf;
		return v;

	}
	int *de_crypt(int*v, int rate) {
		int i = 0;
		unsigned char *ptr = (unsigned char*)v;
		int mx3 = (3 * _mx + 3) & (-4);
		unsigned char *tmp_buf = new unsigned  char[mx3*_my];
		for (int y = _my - 1; y >= 0; y--) {
			unsigned char *pRow = tmp_buf + mx3 * y;
			for (int x = 0; x < _mx; x++) {

				if (i == rate && times_in>0) { //выполнится столько раз, сколько заходил в предыдущие разы алгоритм
					int bit = GetBitValue(*ptr++, 8);  //смотрим 8 бит
					times_in--;  //количество раз отнимается
					i = 0;
					key_main.push_back(bit); //заносим бит в вектор ключа
				}
				i++;
				pRow += 3;
				ptr++;

			}
		}
		delete[] tmp_buf;
		return v;

	}

	void get_key() { //вывести вектор ключа
		cout << "THE SECRET KEY IS - " << " ";
		for (int i = key_main.size()-1; i >= 0; i--) { //от последнего члена к первому, чтобы получилось также, как и ключ был изначально
			cout << key_main[i] << " ";
		}
		cout << endl;
	}

	void get_time() {
		cout << "THERE WERE TIMES - " << times_all << endl;
	}

	int GetBitValue(unsigned char &B, int N)
		/* Получает значение N-ого бита в байте B
		   Возвращает 1 или 0 (в зависимости от значения Бита)
		*/
	{
		int k = 256;

		for (int i = 0; i < N; i++) k /= 2;

		if ((B & k) != 0) return 1;
		else return 0;
	}
	unsigned char ReadBitToByte(int Bit, unsigned char &B)
		/* Записывает в байт B на последнюю позицию бит Bit
		*/
	{
		int A = 1;
		unsigned char Result = B;

		if (Bit == GetBitValue(B, 8)) return B;
		else if (Bit == 1) return Result = Result & A;
		else if (Bit == 0) return B ^ A;

		return NULL;
	}
	


	int saveBMP(const char *fname, int *v)	// В каждом элементе упаковано все три RGB-байта
	{
		BMPheader bh;	// Заголовок файла, sizeof(BMPheader) = 56
		memset(&bh, 0, sizeof(bh));
		bh.bfType = 0x4d42;	// 'BM'
		// Найдем длину строки в файле, включая округление вверх до кратного 4:
		int mx3 = (3 * _mx + 3) & (-4);
		int filesize = 54 + _my * mx3;
		bh.bfSize = filesize;
		bh.bfReserved = 0;
		bh.biPlanes = 1;
		bh.biSize = 40;
		bh.bfOffBits = 14 + bh.biSize;
		bh.biWidth = _mx;
		bh.biHeight = _my;
		bh.biBitCount = 24;
		bh.biCompression = 0;

		FILE *f = fopen(fname, "wb");
		if (!f) return -1;
		size_t res;

		// пишем заголовок
		res = fwrite(&bh, 1, sizeof(BMPheader), f);
		if (res != sizeof(BMPheader)) { fclose(f); return -1; }

		// приготовим временный буфер
		unsigned char *tmp_buf = new unsigned char[mx3*_my];
		// Перенос данных (не забудем про RGB->BGR)
		unsigned char *ptr = (unsigned char *)v;
		for (int y = _my - 1; y >= 0; y--) {
			unsigned char *pRow = tmp_buf + mx3 * y;
			for (int x = 0; x < _mx; x++) {
				*(pRow + 2) = *ptr++;
				*(pRow + 1) = *ptr++;
				*pRow = *ptr++;
				pRow += 3;
				ptr++;
			}
		}
		// сбросим в файл
		fwrite(tmp_buf, 1, mx3*_my, f);
		fclose(f);
		delete[]tmp_buf;
		return 0;	// OK
	}



};
int main() {
	int key = 100; //создали ключ
	int rate = 124; //создали рэйт
	Stegan st1;  //создали объект
	int * v = st1.loadBMP("C:/Users/РС/Desktop/ПРОГА/rainbow.bmp"); //загрузили bmp файл
	int *modified_v = st1.crypt(v, key, rate); //зашифровали bmp файл
	st1.de_crypt(modified_v, rate); //расшифровали bmp файл
	st1.get_key(); //вывели ключ
	st1.get_time(); //вывели количество раз
	st1.saveBMP("C:/Users/РС/Desktop/ПРОГА/rainbow2.bmp", modified_v); //сохранили зашифрованый bmp файл
	
	delete[]modified_v;
	
}
