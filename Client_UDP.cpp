// TS_Klient_UDP.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include "pch.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <cstdio>
#include <cstdlib>
#include <winsock2.h>
#include <string> 
#include <iostream> 
#include <bitset>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

// Zmienne globalne
std::bitset<2> poleOperacji;
std::bitset<16> poleLiczb_1;
std::bitset<16> poleLiczb_2;
std::bitset<3> poleStatusu;
std::bitset<8> poleID;
std::bitset<3> poleInstrukcji;
std::bitset<1> ACK;
std::bitset<7> poleDopelnienia;
char pakiet[7];

std::vector<int> wektor_liczb;

// Funkcje
int bit_to_int(const std::string &s) {
	int liczba = 0;
	for (int i = s.size() - 1, p = 1; i >= 0; i--, p *= 2)
	{
		if (s[i] == '1')
			liczba += p;
	}
	return liczba;
}

std::bitset<16> int_to_bit(int liczba) {
	std::bitset<16> bity;
	int i = 0;
	while (liczba > 0)
	{
		if (liczba == 1)
		{
			bity.set(i, true);
			break;
		}
		if (!(liczba % 2))
		{
			bity.set(i, false);
			liczba /= 2;
		}
		else
		{
			bity.set(i, true);
			liczba = liczba / 2;
		}
		i++;
	}
	return bity;
}

void czysc_pakiet()
{
	poleOperacji.reset();
	poleLiczb_1.reset();
	poleLiczb_2.reset();
	poleStatusu.reset();
	poleInstrukcji.reset();
	ACK.reset();
}

void zapakuj_pakiet() {
	std::string temp;
	
	temp = poleOperacji.to_string();
	temp += poleLiczb_1.to_string();
	temp += poleLiczb_2.to_string();
	temp += poleStatusu.to_string();
	temp += poleID.to_string();
	temp += poleInstrukcji.to_string();
	temp += ACK.to_string();
	temp += poleDopelnienia.to_string();

	std::string bajt_1 = temp.substr(0, 8);
	std::string bajt_2 = temp.substr(8, 8);
	std::string bajt_3 = temp.substr(16, 8);
	std::string bajt_4 = temp.substr(24, 8);
	std::string bajt_5 = temp.substr(32, 8);
	std::string bajt_6 = temp.substr(40, 8);
	std::string bajt_7 = temp.substr(48, 8);

	pakiet[0] = bit_to_int(bajt_1);
	pakiet[1] = bit_to_int(bajt_2);
	pakiet[2] = bit_to_int(bajt_3);
	pakiet[3] = bit_to_int(bajt_4);
	pakiet[4] = bit_to_int(bajt_5);
	pakiet[5] = bit_to_int(bajt_6);
	pakiet[6] = bit_to_int(bajt_7);
}

void odpakuj_pakiet() 
{
	std::string temp;
	std::bitset<8> bajt;

	for (int i = 0; i < 7; i++)
	{
		bajt = pakiet[i];
		temp += bajt.to_string();
	}

	poleOperacji = bit_to_int(temp.substr(0, 2));
	poleLiczb_1 = bit_to_int(temp.substr(2, 16));
	poleLiczb_2 = bit_to_int(temp.substr(18, 16));
	poleStatusu = bit_to_int(temp.substr(34, 3));
	poleID = bit_to_int(temp.substr(37, 8));
	poleInstrukcji = bit_to_int(temp.substr(45, 3));
	ACK = bit_to_int(temp.substr(48, 1));
	poleDopelnienia = bit_to_int(temp.substr(49, 7));
}

void wyslij_ACK(SOCKET &mainSocket, sockaddr_in &addr) {
	ACK.set(0, 1);
	zapakuj_pakiet();
	sendto(mainSocket, pakiet, sizeof(pakiet), 0, (SOCKADDR *)& addr, sizeof(struct sockaddr));
	std::cout << "WYSLANO ACK\n";
	ACK.reset(); // z powrotem na 0
}

bool odbierz_ACK(SOCKET &mainSocket, sockaddr_in &addr) {
	int addr_len = sizeof(struct sockaddr);
	recvfrom(mainSocket, pakiet, sizeof(pakiet), 0, (SOCKADDR *)& addr, &addr_len);
	odpakuj_pakiet();
	if (bit_to_int(ACK.to_string()) == 1) {
		std::cout << "ODEBRANO ACK\n";
		//ACK.set(0, 0); // z powrotem na 0
		return true;
	}
	else
		return false;
}

void pokaz_pakiet() {
	std::cout << "========== PAKIET ==========" << std::endl;
	std::cout << "Pole operacji: " << poleOperacji << std::endl;
	std::cout << "Pole liczby 1: " << poleLiczb_1 << std::endl;
	std::cout << "Pole liczby 2: " << poleLiczb_2 << std::endl;
	std::cout << "Pole numeru ID: " << poleID << std::endl;
	std::cout << "Pole statusu: " << poleStatusu << std::endl;
	std::cout << "Pole instrukcji: " << poleInstrukcji << std::endl;
	std::cout << "Pole ACK: " << ACK << std::endl;
	std::cout << "Pole dopelnienia: " << poleDopelnienia << std::endl;
	std::cout << "============================" << std::endl;
}

bool czyPoprawne(const int &liczba1) {
	return !(liczba1 < 0 || liczba1 > 65535);
}

bool czyPoprawne(const int &liczba1, const int &liczba2) {
	return !(liczba1 < 0 || liczba1 > 65535 || liczba2 < 0 || liczba2 > 65535);
}

/*****************************************************/
// Operacje
void operacja1() {
	poleOperacji.reset(); // 00

	poleInstrukcji.reset();
	poleInstrukcji.set(0, 1); // 001

	int a, b;
	std::cout << "Podaj wartosci a oraz b:\n";
	std::cin >> a >> b;
	while (!czyPoprawne(a, b)) {
		std::cout << "Podano wartosci przekraczajace zakres 0-65535. Sprobuj ponownie.\n";
		std::cin.clear(); std::cin.ignore();
		std::cout << "Podaj wartosci a oraz b:\n";
		std::cin >> a >> b;
	}

	poleLiczb_1 = int_to_bit(a);
	poleLiczb_2 = int_to_bit(b); 
	zapakuj_pakiet();
	pokaz_pakiet(); 
}

void operacja2() {
	poleOperacji.reset();
	poleOperacji.set(0, 1); // 01

	poleInstrukcji.reset();
	poleInstrukcji.set(0, 1); // 001

	int a, b;
	std::cout << "Podaj wartosci a oraz b:\n";
	std::cin >> a >> b;
	while (!czyPoprawne(a, b)) {
		std::cout << "Podano wartosci przekraczajace zakres 0-65535. Sprobuj ponownie.\n";
		std::cin.clear(); std::cin.ignore();
		std::cout << "Podaj wartosci a oraz b:\n";
		std::cin >> a >> b;
	}

	poleLiczb_1 = int_to_bit(a);
	poleLiczb_2 = int_to_bit(b);
	zapakuj_pakiet();
	pokaz_pakiet();
}

void operacja3() {
	poleOperacji.reset();
	poleOperacji.set(1, 1); // 10

	poleInstrukcji.reset();
	poleInstrukcji.set(0, 1); // 001

	int a, b;
	std::cout << "Podaj wartosci a oraz b:\n";
	std::cin >> a >> b;
	while (!czyPoprawne(a, b)) {
		std::cout << "Podano wartosci przekraczajace zakres 0-65535. Sprobuj ponownie.\n";
		std::cin.clear(); std::cin.ignore();
		std::cout << "Podaj wartosci a oraz b:\n";
		std::cin >> a >> b;
	}

	poleLiczb_1 = int_to_bit(a);
	poleLiczb_2 = int_to_bit(b);
	zapakuj_pakiet();
	pokaz_pakiet();
}

void operacja4() {
	poleOperacji.reset();
	poleOperacji.set(); // 11

	poleInstrukcji.reset();
	poleInstrukcji.set(0, 1); // 001

	int a, b;
	std::cout << "Podaj wartosci a oraz b:\n";
	std::cin >> a >> b;
	while (!czyPoprawne(a, b)) {
		std::cout << "Podano wartosci przekraczajace zakres 0-65535. Sprobuj ponownie.\n";
		std::cin.clear(); std::cin.ignore();
		std::cout << "Podaj wartosci a oraz b:\n";
		std::cin >> a >> b;
	}

	poleLiczb_1 = int_to_bit(a);
	poleLiczb_2 = int_to_bit(b);
	zapakuj_pakiet();
	pokaz_pakiet();
}

void operacja5(SOCKET &mainSocket, sockaddr_in &addr) {
	int addr_len = sizeof(struct sockaddr);

	poleOperacji.reset(); // 00
	poleInstrukcji.reset(); // 000

	char odp;
	std::cout << "Sortowanie:\na) malejaco\nb) rosnaco\n";
	std::cin >> odp;
	while (odp != 'a' && odp != 'A' && odp != 'b' && odp != 'B') {
		std::cout << "Podano nieprawidlowa wartosc. Sprobuj ponownie.\n";
		std::cin.clear(); std::cin.ignore();
		std::cin >> odp;
	}

	//// Malejaco
	//if (odp == 'a' || odp == 'A') {
	//	poleInstrukcji.set(0, 0);
	//	poleInstrukcji.set(1, 0);
	//	poleInstrukcji.set(2, 0); // 000
	//}
	//// Rosnaco
	//if (odp == 'b' || odp == 'B') {
	//	poleInstrukcji.set(0, 1);
	//	poleInstrukcji.set(1, 1);
	//	poleInstrukcji.set(2, 1); // 111
	//}

	int count;
	std::cout << "Ile liczb chcesz przeslac?\n";
	std::cin >> count;
	while (!czyPoprawne(count)) {
		std::cout << "Podano nieprawidlowa wartosc. Sprobuj ponownie.\n";
		std::cin.clear(); std::cin.ignore();
		std::cout << "Ile liczb chcesz przeslac?\n";
		std::cin >> count;
	}

	for (int i = 0; i < count; i++) {
		if (odp == 'a' || odp == 'A') {
			poleInstrukcji.set(0, 0);
			poleInstrukcji.set(1, 1);
			poleInstrukcji.set(2, 0); // 010
		}
		// Rosnaco
		if (odp == 'b' || odp == 'B') {
			poleInstrukcji.set(0, 1);
			poleInstrukcji.set(1, 1);
			poleInstrukcji.set(2, 1); // 111
		}
		ACK.reset(); 
		int liczba;
		std::cout << "Podaj liczbe:\n";
		std::cin >> liczba;
		while (!czyPoprawne(liczba)) {
			std::cout << "Podano wartosc przekraczajaca zakres 0-65535. Sprobuj ponownie.\n";
			std::cin.clear(); std::cin.ignore();
			std::cout << "Podaj liczbe:\n";
			std::cin >> liczba;
		}
		poleLiczb_1 = int_to_bit(liczba);
		if (i == count - 1) {
			if (odp == 'a' || odp == 'A') {
				poleInstrukcji.set(0, 1);
				poleInstrukcji.set(1, 1);  
				poleInstrukcji.set(2, 0); // 011
			}
			if (odp == 'b' || odp == 'B') {
				poleInstrukcji.set(0, 0);
				poleInstrukcji.set(1, 1); 
				poleInstrukcji.set(2, 1); // 110
			}
		}
		zapakuj_pakiet();
		sendto(mainSocket, pakiet, sizeof(pakiet), 0, (SOCKADDR *)& addr, sizeof(struct sockaddr));
		odbierz_ACK(mainSocket, addr);
		czysc_pakiet();
	}

	for (int i = 0; i < count; i++) {
		czysc_pakiet();
		recvfrom(mainSocket, pakiet, sizeof(pakiet), 0, (SOCKADDR *)& addr, &addr_len);
		std::cout << "Odebrano pakiet nr " << i << std::endl;
		odpakuj_pakiet();
		pokaz_pakiet();
		wyslij_ACK(mainSocket, addr);
		wektor_liczb.push_back(bit_to_int(poleLiczb_1.to_string()));
	}
	std::cout << "Posortowane liczby:\n";
	std::cout << "[ ";
	for (auto &e : wektor_liczb) {
		std::cout << e << " ";
	}
	std::cout << "]";
}


/*****************************************************/

int main()
{
	// Inicjacja Winsock
	WSADATA wsaData;

	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != NO_ERROR)
		printf("Initialization error.\n");

	// Utworzenie deskryptora gniazda - socket(AF_INET - rodzina adresow IPv4, typ gniazda, protokol)
	SOCKET mainSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (mainSocket == INVALID_SOCKET)
	{
		printf("Error creating socket: %ld\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	// Struktura sockaddr_in - reprezentuje adres IP
	sockaddr_in my_addr;
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	my_addr.sin_port = htons(27016);

	sockaddr_in their_addr;
	memset(&their_addr, 0, sizeof(their_addr)); // Wyzeruj reszte struktury
	their_addr.sin_family = AF_INET; // host byte order
	their_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	their_addr.sin_port = htons(27016); // short, network byte order

	int addr_len = sizeof(struct sockaddr);

	// Klient wysyla chec nawiazania polaczenia i zadanie podania ID
	char odp;
	std::cout << "Czy chcesz polaczyc sie z serwerem? (y/n)\n";
	std::cin >> odp;
	while (odp != 'n' && odp != 'N' && odp != 'y' && odp != 'Y') {
		std::cout << "Wprowadzono nieprawidlowa wartosc. Sprobuj ponownie.\n";
		std::cin.clear(); std::cin.ignore();
		std::cout << "Czy chcesz polaczyc sie z serwerem? (y/n)\n";
		std::cin >> odp;
	}
	if (odp == 'n' || odp == 'N') {
		std::cout << "Wyjscie z programu.\n";
		return 0;
	}
	if (odp == 'y' || odp == 'Y') {
		czysc_pakiet();
		poleInstrukcji.set(2, 1); // 100
		zapakuj_pakiet();
		pokaz_pakiet();
		sendto(mainSocket, pakiet, sizeof(pakiet), 0, (SOCKADDR *)& their_addr, sizeof(struct sockaddr));
		std::cout << "Wyslano zadanie podania ID\n";
	}
	
	// Klient otrzymuje potwierdzenie otrzymania zadania podania ID
	//recvfrom(mainSocket, pakiet, sizeof(pakiet), 0, (SOCKADDR *)& their_addr, &addr_len);
	//std::cout << "Odebrano pakiet" << std::endl;
	if (odbierz_ACK(mainSocket, their_addr)) {
		pokaz_pakiet();
	}
	else {
		std::cout << "Nie otrzymano ACK\n";
	}

	czysc_pakiet();
	recvfrom(mainSocket, pakiet, sizeof(pakiet), 0, (SOCKADDR *)& their_addr, &addr_len);
	std::cout << "Odebrano ID\n";
	odpakuj_pakiet();
	pokaz_pakiet();
	wyslij_ACK(mainSocket, their_addr);
	//odpakuj_pakiet();
	//std::cout << "Rozpakowano pakiet" << std::endl;

	while (1) {
		std::cout << "\n";
		std::cout << "Wybierz operacje:\n" <<
			"1. Wylosowanie wartosci z przedzialu (a, b>\n" <<
			"2. Operacja a % b\n" <<
			"3. Operacja a + b\n" <<
			"4. Operacja a - b\n" <<
			"5. Sortowanie wielu liczb\n" <<
			"6. Zakoncz dzialanie programu\n";
		int ans;
		std::cin >> ans;
		switch (ans) {
		case 1: {
			operacja1();
			sendto(mainSocket, pakiet, sizeof(pakiet), 0, (SOCKADDR *)& their_addr, sizeof(struct sockaddr));
			odbierz_ACK(mainSocket, their_addr);
			czysc_pakiet();
			recvfrom(mainSocket, pakiet, sizeof(pakiet), 0, (SOCKADDR *)& their_addr, &addr_len);
			odpakuj_pakiet();
			pokaz_pakiet();
			wyslij_ACK(mainSocket, their_addr);
			if (bit_to_int(poleStatusu.to_string()) == 3) { // 011
				std::cout << "Blad: a > b\n";
			}
			else {
				std::cout << "Losowa liczba z przedzialu (a, b> wynosi: " << bit_to_int(poleLiczb_1.to_string());
			}
			break;
		}
		case 2: {
			operacja2();
			sendto(mainSocket, pakiet, sizeof(pakiet), 0, (SOCKADDR *)& their_addr, sizeof(struct sockaddr));
			odbierz_ACK(mainSocket, their_addr);
			czysc_pakiet();
			recvfrom(mainSocket, pakiet, sizeof(pakiet), 0, (SOCKADDR *)& their_addr, &addr_len);
			odpakuj_pakiet();
			pokaz_pakiet();
			wyslij_ACK(mainSocket, their_addr);
			if (bit_to_int(poleStatusu.to_string()) == 3) { // 011
				std::cout << "Blad: b = 0\n";
			}
			else {
				std::cout << "a % b = " << bit_to_int(poleLiczb_1.to_string());
			}
			break;
		}
		case 3: {
			operacja3();
			sendto(mainSocket, pakiet, sizeof(pakiet), 0, (SOCKADDR *)& their_addr, sizeof(struct sockaddr));
			odbierz_ACK(mainSocket, their_addr);
			czysc_pakiet();
			recvfrom(mainSocket, pakiet, sizeof(pakiet), 0, (SOCKADDR *)& their_addr, &addr_len);
			odpakuj_pakiet();
			pokaz_pakiet();
			wyslij_ACK(mainSocket, their_addr);
			if (bit_to_int(poleStatusu.to_string()) == 3) { // 011
				std::cout << "Blad: a + b > 65535\n";
			}
			else {
				std::cout << "a + b = " << bit_to_int(poleLiczb_1.to_string());
			}
			break;
		}
		case 4: {
			operacja4();
			sendto(mainSocket, pakiet, sizeof(pakiet), 0, (SOCKADDR *)& their_addr, sizeof(struct sockaddr));
			odbierz_ACK(mainSocket, their_addr);
			czysc_pakiet();
			recvfrom(mainSocket, pakiet, sizeof(pakiet), 0, (SOCKADDR *)& their_addr, &addr_len);
			odpakuj_pakiet();
			pokaz_pakiet();
			wyslij_ACK(mainSocket, their_addr);
			if (bit_to_int(poleStatusu.to_string()) == 3) { // 011
				std::cout << "Blad: a - b < 0\n";
			}
			else {
				std::cout << "a - b = " << bit_to_int(poleLiczb_1.to_string());
			}
			break;
		}
		case 5: {
			operacja5(mainSocket, their_addr);
			wektor_liczb.clear();
			break;
		}
		case 6: {
			std::cout << "Konczenie dzialania programu.\n";
			czysc_pakiet();

			poleInstrukcji.set(0, 1);
			poleInstrukcji.set(1, 0);
			poleInstrukcji.set(2, 1); // 101

			zapakuj_pakiet();
			sendto(mainSocket, pakiet, sizeof(pakiet), 0, (SOCKADDR *)& their_addr, sizeof(struct sockaddr));
			odbierz_ACK(mainSocket, their_addr);

			closesocket(mainSocket);
			WSACleanup();
			system("pause");
			return 0;
			break;
		}
		default: {
			std::cout << "Wprowadzono niepoprawny numer operacji. Sprobuj ponownie.\n";
			std::cin.clear(); std::cin.ignore();
			break;
		}
		}
	}
}
