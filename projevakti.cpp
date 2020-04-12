#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std;

bool IsRecordInRankingRecord(int a, int* TenPlace);
void RankingRecord(Table* sortedTable, int a, int* TenPlace); // Bu fonksiyon en yuksek 10 degeri saklatiyor butun tablo siralatilacaksa bunun yerine yapilmali.
void RecordForNewItem(Table* sortedTable, int okunansatir, int* TenPlace);
int GetQuantity(string line);
string GetDescription(string line);
void NewItem(Table* sortedTable, int HashInfo, string StockInfo, string line, int okunansatir);
int SearchHashCode(Table* sortedTable, int okunansatir, int HashInfo, string StcokInfo);
string GetStockCode(string line);
int hf(std::string const& s);

struct Table {
	string StockCode;
	string Description;
	int TotalQuantity;
	int Hash;
};

int main() {
	clock_t start = clock(), stop;
	ifstream inFile;
	string line;
	string StockInfo;
	int HashInfo = 0;//anlık hash degeri.
	int ReadingLine = 0;//0 a esitlenecek.
	int FindingVar = 0;//bulunma degiskeni.
	int NewItemQuantity = 0;
	int TotalLine = 0;
	int TenPlace[10] = { 0,1,2,3,4,5,6,7,8,9 };
	struct Table* sortedTable = new Table[600000];// bu duzenlenecek.
	inFile.open("OnlineRetail.csv");
	if (!inFile) {
		//Dosya acilma kontrolu.
		cerr << "Unable to open file";
		exit(1);
	}
	else {
		while (!inFile.eof()) {// dosyada satir varken calisacak.
			getline(inFile, line);
			StockInfo = GetStockCode(line);
			HashInfo = hf(StockInfo);
			if (ReadingLine != 0) {
				FindingVar = SearchHashCode(sortedTable, ReadingLine, HashInfo,StockInfo);//stock code aranır varsa indis, yoksa -1 dondurur.
				if (FindingVar != -1) {//bi onceki veriler arandı ve hash esleti. quantity arttırılması yapilacak
					int newQuantity = GetQuantity(line); //tablodaki elemanın quantitiysini arttırmak icin arttirilacak deger cekilir.test edilmedi
					sortedTable[FindingVar].TotalQuantity = sortedTable[FindingVar].TotalQuantity + newQuantity; //arttirildi.test edilmedi
					//aynisi bulunan itemin quantity si arttirilir.
					RankingRecord(sortedTable, FindingVar, TenPlace);                                            //butun data icin switch fonksliyonuyla degisirebilirsin.
				}
				else { // bi onceki veriler arandı ve hash eslesmedi.
					NewItem(sortedTable, HashInfo, StockInfo, line, ReadingLine);//yeni kayidi struca ekleyen func.
					RecordForNewItem(sortedTable, ReadingLine, TenPlace);//kontrol yapıp tabloları degisiriyor.
					ReadingLine++;
					//yeni quantity degeri icin siralama yapilir.
				}

			}
			else { //else ilk kayıt buraya.
				NewItem(sortedTable, HashInfo, StockInfo, line, ReadingLine);
				ReadingLine++;
			}
		}
		bool swapped;
		for (int j = 0; j < 9; j++) { //https://www.geeksforgeeks.org/bubble-sort/ optimized bubble
			swapped = false;
			for (int i = 0; i < 9 - j; i++) {
				if (sortedTable[TenPlace[i]].TotalQuantity < sortedTable[TenPlace[i + 1]].TotalQuantity) {
					int temp = TenPlace[i];
					TenPlace[i] = TenPlace[i + 1];
					TenPlace[i + 1] = temp;
					swapped = true;
				}
			}
			if (swapped == false)
				break;
		}
		cout << "Product (StockCode)                          Description                                       #TotalQuantity" << endl;
		for (int i = 0; i < 10; i++) {
			cout << i + 1 <<"-  "<< sortedTable[TenPlace[i]].StockCode << "\t \t \t" << sortedTable[TenPlace[i]].Description << setw(70 - sortedTable[TenPlace[i]].Description.size()) << sortedTable[TenPlace[i]].TotalQuantity << endl;
		}
	}
	inFile.close();
	stop = clock();
	cout << "Total Elapsed Time : " <<(float)(stop - start) / CLOCKS_PER_SEC << " seconds." << endl;
	return 0;
}
bool IsRecordInRankingRecord(int a, int* TenPlace) {                                                 //RankingRecor icin yapilan fonskiyon. Gelen tablo indisinin rankta
	for (int i = 0; i < 10; i++) {                                                                   //olup olmadigini kontrol ediyor.
		if (a != TenPlace[i]) {
		}
		else {
			return true;
		}
	}
	return false;
}
void RankingRecord(Table* sortedTable, int a, int* TenPlace) {                                        //mainde ve RecordForNewItem fonkisyonlarinda calisiyor, Tablo suanki sira ve 
	bool k = IsRecordInRankingRecord(a, TenPlace);                                                    //sirakaydinin yapildigi degiskenler cagirilir. Tabloya yeni eleman eklenince ve
	for (int i = 0; i < 10; i++) {                                                                    //TotalQuantity guncellenince calisir. En yuksek 10 TotalQuantity degerlerini tasiyan
		if (k == false && sortedTable[TenPlace[i]].TotalQuantity < sortedTable[a].TotalQuantity) {    //tablo indislerini TenPlace degiskenine, her guncelleme sonrasi guncelleyerek kayit eder. 
			TenPlace[i] = a;
			k = IsRecordInRankingRecord(a, TenPlace);
		}
	}
}
void RecordForNewItem(Table* sortedTable, int ReadingLine, int* TenPlace) {                           //Yeni tablo kayidi yapar, yeni kayidin degerlerini kaydetmek uzere
	int temp = ReadingLine;                                                                           //RankingRecordu cagirir.
	RankingRecord(sortedTable, temp, TenPlace);                                                       //eger siralarken arama yapmak istersen Rankingi switch ile degisir.
}
void NewItem(Table* sortedTable, int HashInfo, string StockInfo, string line, int ReadingLine) {       // Yeni tablo ekleme fonksiyonu.
	int quantity = GetQuantity(line);
	string description = GetDescription(line);
	sortedTable[ReadingLine] = { StockInfo, description, quantity, HashInfo };
}
int SearchHashCode(Table* sortedTable, int ReadingLine, int HashInfo, string StockInfo) {
	while (ReadingLine != 0) {
		if (sortedTable[ReadingLine - 1].Hash == HashInfo) {
			//Hash eslesti StockCode kontrolu yapilacak;
			if (sortedTable[ReadingLine - 1].StockCode == StockInfo) {
				//StockCode eslesti eslesen deger dondurulecek.
				return ReadingLine - 1;
			}
			else {
				//Hashler esit fakat stockcodelar esit degil yukariya cikiliyor.
				ReadingLine--;
			}
		}
		else {
			ReadingLine--;
		}
	}
	return ReadingLine - 1;
}
int GetQuantity(string line) {//Quantity degerini okuyan fonksiyon.
	string Quantity;
	int TotalQuantity;
	int stringIndices = 0;
	int k = 0; // dosyadaki ayrac indisi.
	if (line.size() == 0) {
		Quantity = "0";
	}
	else {
		while (stringIndices != line.size() + 1 && k != 4) {// satirdaki karakterler boyunca gezinme.
			if (line[stringIndices] == ';') {//karakter taramasi sirasinda ";" karakteri kayitlari.
				// noktali virgul gorunca k arttirilir.
				k++;
			}
			else if (k == 3) {
				Quantity = Quantity + line[stringIndices];
			}
			stringIndices++;//stringindices her zaman arttirilir.
		}
	}
	if (Quantity == "Quantity") {
		Quantity = "0";
	}
	TotalQuantity = stoi(Quantity);// dosyadan okunan string int yapiliyor.
	return TotalQuantity;
}
string GetDescription(string line) { // Description degerini okuyan fonksiyon.
	string Description;
	int stringIndices = 0;
	int k = 0; // dosyadaki ayrac indisi.
	if (line.size() == 0) {
		return "None";
	}
	else {
		while (stringIndices != line.size() + 1 && k != 3) { // satirdaki karakterler boyunca gezinme.
			if (line[stringIndices] == ';') { //karakter taramasi sirasinda ";" karakteri kayitlari.
				// noktali virgul gorunca k arttirilir.
				k++;
			}
			else if (k == 2) {
				Description = Description + line[stringIndices];
			}
			stringIndices++;//stringindices her zaman arttirilir.
		}
	}
	return Description;
}

string GetStockCode(string line) {
	string StockCode;
	int stringIndices = 0;
	int k = 0; // dosyadaki ayrac indisi.
	if (line.size() == 0) {
		return "None";
	}
	else {
		while (stringIndices != line.size() + 1 && k != 2) { // satirdaki karakterler boyunca gezinme.
			if (line[stringIndices] == ';') { //karakter taramasi sirasinda ";" karakteri kayitlari.
				// noktali virgul gorunca k arttirilir.
				k++;
			}
			else if (k == 1) {
				StockCode = StockCode + line[stringIndices];
			}
			stringIndices++;//stringindices her zaman arttirilir.
		}
	}
	return StockCode;
}
int hf(std::string const& s) { // http://www.cse.yorku.ca/~oz/hash.html "Djb2 Hash function."
	unsigned long hash = 5381;
	for (auto c : s) {
		hash = (hash << 5) + hash + c; /* hash * 33 + c */
	}
	return hash;
}