// Informatik 1
// Laboraufgabe: Bildverarbeitung
// - Graustufenbild laden aus einfacher BMP-Datei
//   siehe: https://en.wikipedia.org/wiki/Lenna
//   siehe: https://solarianprogrammer.com/2018/11/19/cpp-reading-writing-bmp-images/
//   siehe: https://de.wikipedia.org/wiki/Windows_Bitmap
// - Sobeloperator anwenden
//   siehe: https://de.wikipedia.org/wiki/Sobel-Operator
//   siehe: https://www.geeksforgeeks.org/edge-detection-using-prewitt-scharr-and-sobel-operator/amp/
// - abspeichern

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

//Ascii Patterns
const char PATTERN01[] = "@#%*+=-:. ";
const char PATTERN02[] = "@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,\" ^'. ";
const int PATTERNSIZE02 = sizeof(PATTERN02) / sizeof(PATTERN02[0]);
const unsigned short VALID_FILETYPE = 0x4d42;
const unsigned short VALID_BITCOUNT = 8;
const int TARGET_RATIO = 2;


void AusgabeArray(int* array[], int width, int height);

struct HexCharStruct
{
	unsigned char c;
	HexCharStruct(unsigned char _c) : c(_c) { }
};

inline std::ostream& operator<<(std::ostream& o, const HexCharStruct& hs)
{
	return (o << std::hex << (int)hs.c);
}

inline HexCharStruct hex(unsigned char _c)
{
	return HexCharStruct(_c);
}

struct BmpMetaData
{
	// durch Datei-Lesen gesetzt
	unsigned char* Data;
	unsigned int FileLen = 0;

	// durch DecodeHeader zu setzen
	unsigned short FileType;		// 2 Bytes an Bytepos = 0, müssen 0x4d42 sein
	unsigned int PosPixelData;		// 4 Bytes an Bytepos = 10
	unsigned int Width;				// 4 Bytes an Bytepos = 18
	unsigned int Height;			// 4 Bytes an Bytepos = 22
	unsigned short BitCount;		// 2 Bytes an Bytepos = 28
	unsigned int PaletteCount;		// 4 Bytes an Bytepos = 46
};

bool ReadFile(string fn, BmpMetaData& img)
{
	// Datei im Stil "C" oeffnen
	FILE* file = fopen(fn.c_str(), "rb");
	if (file == NULL)
		return false;

	// Dateigroesse erfragen
	fseek(file, 0, SEEK_END);
	img.FileLen = ftell(file);
	fseek(file, 0, SEEK_SET);

	// Datenpuffer holen
	img.Data = new unsigned char[img.FileLen];

	// Daten lesen und Datei schliessen (!)
	fread(img.Data, img.FileLen, 1, file);
	fclose(file);

	// OK
	return true;
}

//Teständerung

BmpMetaData DuplicateImage(BmpMetaData& img)
{
	BmpMetaData res = img;
	res.Data = new unsigned char[res.FileLen];
	memcpy(res.Data, img.Data, img.FileLen);
	return res;
}

unsigned short Decode2Bytes(BmpMetaData& img, int pos)
{
	// Erstellen Sie hier ihren Code.
	return (img.Data[pos] | (img.Data[pos + 1]<<8));
}

unsigned int Decode4Bytes(BmpMetaData& img, int pos)
{
	// Erstellen Sie hier ihren Code.
	return (img.Data[pos] |(img.Data[pos + 1]<<8) | (img.Data[pos + 2]<<16) | (img.Data[pos + 3]<<24));
}

bool DecodeHeader(BmpMetaData& img)
{
	// Erstellen Sie hier ihren Code.
	img.PosPixelData = Decode4Bytes(img, 10);
	img.Width = Decode4Bytes(img, 18);
	img.Height = Decode4Bytes(img, 22);
	img.PaletteCount = Decode4Bytes(img, 46);
	img.FileType = Decode2Bytes(img, 0);
	img.BitCount = Decode2Bytes(img, 28);
	return (img.FileType == VALID_FILETYPE && img.BitCount == VALID_BITCOUNT);
}

unsigned char GetPixel(BmpMetaData& img, int x, int y){
	return img.Data[img.PosPixelData+x+img.Width*y];
}

void PutPixel(BmpMetaData& img, int x, int y, unsigned char value)
{
	// Erstellen Sie hier ihren Code.
	return;
}
int getAverage(int* eingabeMatrix[], int width, int height, int x, int y,int zoomWidth, int zoomHeight) {
	float pixelPerCharWidth = float(width) / zoomWidth;
	float pixelPerCharHeight = float(height) / zoomHeight;
	float zoomFactorWidth = float(zoomWidth) / float(width);
	float zoomFactorHeight = float(zoomHeight) / float(height);
	int sum = 0;
	int count = 0;
	for (float x0=x* pixelPerCharWidth; x0 < pixelPerCharWidth* x + pixelPerCharWidth;x0 += zoomFactorWidth) {
		for (float y0 = y * pixelPerCharHeight; y0 < pixelPerCharHeight* y + pixelPerCharHeight;y0 += zoomFactorHeight) {
			sum += eingabeMatrix[int(x0)][int(y0)];
				++count;
		}
	}
	return sum/count;
}
void AppyOperator3x3(int* eingabeMatrix[], int width, int height)
{
	//width = width - width % 3; //Überschuss der eingabe Matrix an dieser abziehen 
	//hight = hight - hight % 6;
	cout << "Geben sie die gewuenschte Breite ein (in Asciizeichen): " << endl;
	int zoomWidth= 0;
	cin >> zoomWidth;
	


	float zoomFactor = float(zoomWidth)/float(width);
	int zoomHeight = (height * zoomFactor)/ TARGET_RATIO;

	int** kompressMatrix = new int* [zoomWidth]; //Matrix für speicherung neuer Pixel werte

	for (int i = 0; i < zoomWidth; ++i) {
		kompressMatrix[i] = new int[zoomHeight];
	}
	for (int x = 0; x < zoomWidth; x++) { //Geht immer die einzelnen pixel im 3x6 feld druch
		for (int y = 0; y <zoomHeight; y++) {

			kompressMatrix[x][y] = getAverage(eingabeMatrix,width,height,x,y,zoomWidth,zoomHeight);

		}
	}
	/*int oldPixelToNewWidth = 1 / zoomFactor;
	int oldPixelToNewHeight = (1 / zoomFactor )* 2;
	for (int i = 0; i < zoomWidth* zoomFactor; i+= oldPixelToNewWidth) { //Geht immer in dreierschitten durch in x Richtung 
		for (int k = 0; k < zoomHeight* zoomFactor; k += oldPixelToNewHeight) { //Geht immer 6 pixel nach unten in y Richtung

			int mittelwert = 0; //Speicherung der Mittelwerte

			for (int x = i; x < i + oldPixelToNewWidth; x++) { //Geht immer die einzelnen pixel im 3x6 feld druch
				for (int y = k; y < oldPixelToNewHeight; y++) {

					mittelwert = mittelwert + eingabeMatrix[x][y];

				}
			}

			kompressMatrix[(i / oldPixelToNewWidth)][(k / oldPixelToNewHeight)] = mittelwert / (oldPixelToNewWidth * oldPixelToNewHeight);

		}
	}*/
	AusgabeArray(kompressMatrix, zoomWidth, zoomHeight);
}


//Ascii Ergänzung

char PixelValueToChar(int mode, int value)
{
	// Ihr Code hier ...
	return 0;
}

void GenerateAsciiArt(BmpMetaData& src, int numCols, int mode)
// mode == 1 -> PATTERN01[] wird genutzt.
//     sonst -> PATTERN02[] wird genutzt.
{
	// access
	if (src.Height < 1 || src.Width < 1 || numCols < 1)
		return;

	// Ihr Code hier ...

}

char IntToAscii(int value) {
	return PATTERN02[value % (PATTERNSIZE02-1)];
}

void AusgabeArray(int* array[],int width,int height) {
	for (int iy = height-1; iy >=0 ; --iy) {
		for (int ix = 0; ix < width; ++ix) {
			cout<< IntToAscii(array[ix][iy]);
		}
		cout << endl;
	}
}


int main()
{
	//Ascii Ergänzung
	// Bildstruktur
	BmpMetaData sourceImg = { 0 };

	// Datei lesen
	if (!ReadFile("Cat04_pretty.bmp", sourceImg))
	{
		cout << "Fehler beim Lesen der Eingangsdatei" << endl;
		return -1;
	}

	// Header-Daten dekodieren
	if (!DecodeHeader(sourceImg))
	{
		cout << "Fehler beim Dekodieren der Header-Daten" << endl;
		return -1;
	}
	//Array zur Bilderzeugung
	
	int** array = new int* [sourceImg.Width];

	for (int ix = 0; ix < sourceImg.Width; ++ix) {
		array[ix] = new int[sourceImg.Height];
		for (int iy = 0; iy < sourceImg.Height; ++iy) {
			array[ix][iy] = GetPixel(sourceImg, ix ,iy);
		}
	}

	//AusgabeArray(array, sourceImg.Width, sourceImg.Height);
	AppyOperator3x3(array, sourceImg.Width, sourceImg.Height);



	// convert
	GenerateAsciiArt(sourceImg, 120, 1);
}
