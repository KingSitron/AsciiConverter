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

//#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

//Ascii Patterns
const char PATTERN01[] = "@#%*+=-:. ";
const char PATTERN02[] = "@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,\" ^'. ";
const unsigned short VALID_FILETYPE = 0x4d42;
const unsigned short VALID_BITCOUNT = 8;
const string = "Hallo hier ist eine Veränderung";



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

void AppyOperator3x3(char* eingabeMatrix[], int width, int hight)
{
	width = width - width % 3; //Überschuss der eingabe MAtrix an dieser abziehen 
	hight = hight - hight % 6;

	char** kompressMatrix; //Matrix für speicherung neuer Pixel werte
	char kompressX = width / 3; //Matrix größe 
	char kompressY = hight / 6;

	kompressMatrix = new char* [kompressX];

	for (int i = 0; i < kompressX; ++i) {
		kompressMatrix[i] = new char[kompressY];
	}

	for (int i = 0; i <= width; i+3) { //Geht immer in dreierschitten durch in x Richtung 
		for (int k = 0; k <= hight; k+6) { //Geht immer 6 pixel nach unten in y Richtung

			int mittelwert = 0; //Speicherung der Mittelwerte

			for (int x = i; x < i + 4; x++) { //Geht immer die einzelnen pixel im 3x6 feld druch
				for (int y = k; y < k + 7; y++) {

					mittelwert = mittelwert + eingabeMatrix[x][y];

				}
			}

			kompressMatrix[i / 3][k / 6] = mittelwert / 18;

		}
	}
	AusgabeArray(kompressMatrix, kompressX, kompressY);
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
void AusgabeArray(char* array[],int width,int height) {
	for (int iy = height-1; iy >=0 ; --iy) {
		for (int ix = 0; ix < width; ++ix) {
			cout<< hex(array[ix][iy])<<" ";
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
	if (!ReadFile("ZumEinlesenUngleich.bmp", sourceImg))
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
	
	char** array = new char* [sourceImg.Width];

	for (int ix = 0; ix < sourceImg.Width; ++ix) {
		array[ix] = new char[sourceImg.Height];
		for (int iy = 0; iy < sourceImg.Height; ++iy) {
			array[ix][iy] = GetPixel(sourceImg, ix ,iy);
		}
	}

	AusgabeArray(array, sourceImg.Width, sourceImg.Height);
	//AppyOperator3x3(array, sourceImg.Width, sourceImg.Height);



	// convert
	GenerateAsciiArt(sourceImg, 120, 1);
}
