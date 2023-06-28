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
//
// 
// - Labor 6 + 7
// - AsciiConverter
//   Der AsciiConverter nimmt als BMP gespeicherte Graustufen Bilddateien 
//   und wandelt diese in ein Bild das aus Asciizeichen besteht.
//   Durch eine Eingabe kann bestimmt werden wie groß das Bild ist, welches in der Ausgabe ausgegeben wird.
//   (Je nach Font invertiert)
// 
// - Namen: Cedric Frietsch,Luis Geissler, Simon Grohnert
// - Datum: 28.6.2023
//

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

//Ascii Patterns
const char PATTERN01[] = "@#%*+=-:. ";
const char PATTERN02[] = "@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\|()1{}[]?-_+~<>i!lI;:,\" ^'. ";
const int PATTERNSIZE02 = sizeof(PATTERN02) / sizeof(PATTERN02[0]);
const unsigned short VALID_FILETYPE = 0x4d42;
const unsigned short VALID_BITCOUNT = 8;
const int TARGET_RATIO = 2;    //Asciicharakters sind in etwa doppelt so hoch wie breit

struct BmpMetaData {
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

bool ReadFile(string fn, BmpMetaData& img) {
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

unsigned short Decode2Bytes(BmpMetaData& img, int pos) {
	//Addierung des zweiten Werts mithilfe des Shift Operators und einer Veroderung
	return (img.Data[pos] | (img.Data[pos + 1]<<8));
}

unsigned int Decode4Bytes(BmpMetaData& img, int pos) {
	//Addieren des zweiten, dritten und vierten Wertes mithilfe des Shift Operators und Veroderung
	return (img.Data[pos] |(img.Data[pos + 1]<<8) | (img.Data[pos + 2]<<16) | (img.Data[pos + 3]<<24));
}

bool DecodeHeader(BmpMetaData& img) {
	//Aufrufen der Position der Pixeldaten, der Höhe, der Breite und des PalettCounts
	img.PosPixelData = Decode4Bytes(img, 10);
	img.Width = Decode4Bytes(img, 18);
	img.Height = Decode4Bytes(img, 22);
	img.PaletteCount = Decode4Bytes(img, 46);
	
	img.FileType = Decode2Bytes(img, 0);
	img.BitCount = Decode2Bytes(img, 28);
	
	//Überprüfen des FileTypes und des Bitcount
	return (img.FileType == VALID_FILETYPE && img.BitCount == VALID_BITCOUNT);
}

unsigned char GetPixel(BmpMetaData& img, int x, int y) {
	//Aufrufen von Pixeldaten an Position x und y
	return img.Data[img.PosPixelData+x+img.Width*y];
}

int getAverage(int* eingabeMatrix[], int width, int height, int x, int y,int zoomWidth, int zoomHeight) {
	float pixelPerCharWidth = float(width) / zoomWidth;
	float pixelPerCharHeight = float(height) / zoomHeight;
	float zoomFactorWidth = float(zoomWidth) / float(width);
	float zoomFactorHeight = float(zoomHeight) / float(height);
	int sum = 0;
	int count = 0;
	//Ablaufen eines Feldes im Originalbildarray angepasst auf die eingegebene Größe
	for (float x0=x* pixelPerCharWidth; x0 < pixelPerCharWidth* x + pixelPerCharWidth;x0 += zoomFactorWidth) {
		for (float y0 = y * pixelPerCharHeight; y0 < pixelPerCharHeight* y + pixelPerCharHeight;y0 += zoomFactorHeight) {
			sum += eingabeMatrix[int(x0)][int(y0)];
				++count;
		}
	}
	//Durchschnittserrechnung der Addierten Werte
	return sum/count;
}

char IntToAscii(int value) {
	//Errechnen eines Characters aus einem Patterns in Abhängigkeit von eine Intwert
	return PATTERN02[value % (PATTERNSIZE02 - 1)];
	//Patternsize-1 da ein Wert mehr errechnet wurde und somit gab es sonst Lehrstallen im Bild
}

void AusgabeArray(int* array[], int width, int height) {
	//Ausgabe lauft Pixeldaten in anderer Reihenfolge ab, damit das Bild bei der Ausgabe richtig angezeigt wird 
	for (int iy = height - 1; iy >= 0; --iy) {
		for (int ix = 0; ix < width; ++ix) {
			cout << IntToAscii(array[ix][iy]);
		}
		cout << endl;
	}
}
void CompressStartImgAndPrint(int* eingabeMatrix[], int width, int height) {
	//Abfrage der breite des AsciiBildes
	cout << "Geben sie die gewuenschte Breite ein (in Asciizeichen): " << endl;
	int zoomWidth= 0;
	cin >> zoomWidth;
	

	//Errechnen der Asciibildgrößen aus Originalgrößen des Bildes
	float zoomFactor = float(zoomWidth)/float(width); 
	int zoomHeight = (height * zoomFactor)/ TARGET_RATIO;

	int** kompressMatrix = new int* [zoomWidth]; //Matrix für speicherung neuer Pixel werte

	for (int i = 0; i < zoomWidth; ++i) {
		kompressMatrix[i] = new int[zoomHeight];
	}
	//Ablaufen der Asciibildmatrix
	for (int x = 0; x < zoomWidth; x++) { 
		for (int y = 0; y <zoomHeight; y++) {

			kompressMatrix[x][y] = getAverage(eingabeMatrix,width,height,x,y,zoomWidth,zoomHeight);

		}
	}
	AusgabeArray(kompressMatrix, zoomWidth, zoomHeight);
}







int main()
{
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
	CompressStartImgAndPrint(array, sourceImg.Width, sourceImg.Height);

}
