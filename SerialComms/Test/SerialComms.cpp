// Test.cpp : This file contains the 'main' function. Program execution begins and ends there.

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
//#include "pch.h"
#include <iostream>
#include <windows.h>
#include <WinBase.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <time.h>

bool selectTest(HANDLE Comms, char* testNum);
bool selectTestType(HANDLE Comms, char* testType);
void serialPort(HANDLE Comms);
int readPort(HANDLE Comms);
void parseFile();
int fileExist(char *file);
char* fileDir = NULL;
int readyToSend(HANDLE Comms);
bool nullTerm(HANDLE Comms);

int main(int argc, char* argv[])
{
	//OPENING SERIAL PORT
	char *commNum = argv[1];
	char *filePath = argv[2];
	char *testNum = argv[3];
	char *testType = argv[4];
	char preFix[10] = "\\\\.\\";


	strcat(preFix, commNum);
	//sprintf(Com, "%c", preFix);
	//LPCWSTR str1 = (LPCWSTR)preFix;
	int res = 0;        //variables to be passed to functinos
	int res1 = 0;
	int res2 = 0;
	int logNum = 0;
	HANDLE hComm;

	wchar_t wtext[20];
	mbstowcs(wtext, preFix, strlen(preFix) + 1);//Plus null
	LPWSTR ptr = wtext;
	
	hComm = CreateFile((LPCWSTR)ptr,                //port name
		GENERIC_READ | GENERIC_WRITE, //Read/Write
		0, NULL, OPEN_EXISTING, 0,
		NULL);

	if (hComm == INVALID_HANDLE_VALUE) { //print if COM port wont open
		printf("Error opening port");
		exit(42);
	}
	else {
		printf("Opening of port successful\n");   //Print to know serial port is open
	}

	//Calling necessary functions
	
	serialPort(hComm);

	readyToSend(hComm);
	res = fileExist(filePath);
	Sleep(10000);
		res1 = selectTest(hComm, testNum);
		Sleep(2000);
		res2 = selectTestType(hComm, testType);
		Sleep(2000);
		nullTerm(hComm);
		printf("%d", res1);
		printf("%d", res2);
	
	
	readPort(hComm);
	parseFile();
	CloseHandle(hComm);//Closing the Serial Port
	return 0;   //main returns 0
}

int readyToSend(HANDLE Comms) 
{

	//READING DATA FROM SERIAL PORT
	char word2[2] = "#";  //Text to look for
	char SerialBuffer[2];//Buffer for storing Rxed Data
	DWORD NoBytesRead;  //set type for NoBytesRead
	int done = 0;
	do
	{ //start do while loop
		ReadFile(Comms,           //Handle of the Serial port
			SerialBuffer,       //
			sizeof(SerialBuffer) - 1,//Size of Buffer
			&NoBytesRead,    //Number of bytes read
			NULL);

		SerialBuffer[NoBytesRead] = 0; //Specify null terminator to stop data rxing strangely
		printf("%s", SerialBuffer); //Print rx'd data to CMD

		char* fin2 = strstr(SerialBuffer, word2); //Find 'word' in SerialBuffer and assign to 'fin'

		if (fin2 != NULL)
		{
			printf("\n \r \n Ready to send data \n \r \n \r");
			done = 1;
		}

		memset(SerialBuffer, 0, 2);

	} while (done != 1);  //while, from do while loop

	return 0; //return testNum for use in another function

}

bool selectTest(HANDLE Comms, char *testChar) {
	DWORD NoBytesWritten = 0;  //set type for NoBytesRead
	
	//char test = *testChar;
	uint8_t test = (uint8_t)atoi(testChar);
	printf("\n\r\n Test Number: ");
	printf(testChar); //convert char to int


	WriteFile(Comms,           //Handle of the Serial port
		&test,       //
		sizeof(uint8_t),//Size of Buffer
		&NoBytesWritten,    //Number of bytes read
		NULL);

	return WriteFile;
}
bool selectTestType(HANDLE Comms, char* testType) {
	DWORD NoBytesWritten = 0;  //set type for NoBytesRead
	printf("\n\r\n  Test Type: ");
	printf(testType);
	printf("\n\r\n");

	uint8_t test1 = (uint8_t)atoi(testType);
	WriteFile(Comms,           //Handle of the Serial port
		&test1,       //
		sizeof(uint8_t),//Size of Buffer
		&NoBytesWritten,    //Number of bytes read
		NULL);
	return WriteFile;
}

bool nullTerm(HANDLE Comms) {
	DWORD NoBytesWritten = 0;  //set type for NoBytesRead

	char snd2 = '*';
	//uint8_t test = (uint8_t)atoi(snd2);
	WriteFile(Comms,           //Handle of the Serial port
		&snd2,       //
		sizeof(snd2),//Size of Buffer
		&NoBytesWritten,    //Number of bytes read
		NULL);
	return WriteFile;
}



void serialPort(HANDLE Comms) {
	//CONFIGURING SERIAL PORT
	BOOL Status;
	DCB dcbSerialParams = { 0 }; // Initializing DCB structure
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	dcbSerialParams.BaudRate = CBR_57600;  // Setting BaudRate = 9600
	dcbSerialParams.ByteSize = 8;         // Setting ByteSize = 8
	dcbSerialParams.StopBits = ONESTOPBIT;// Setting StopBits = 1
	dcbSerialParams.Parity = NOPARITY;  // Setting Parity = None
	dcbSerialParams.fDsrSensitivity = FALSE;
	dcbSerialParams.fRtsControl = RTS_CONTROL_ENABLE;
	dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;
	dcbSerialParams.fOutxCtsFlow = FALSE; // CTS output handshaking.
	dcbSerialParams.fOutxDsrFlow = FALSE; // DSR output handshaking.
	SetCommState(Comms, &dcbSerialParams); //Assignin serial settings to comm handle
	//Status = EscapeCommFunction(Comms, SETXOFF); 
	//Status = EscapeCommFunction(Comms, SETXON); 
	//Status = EscapeCommFunction(Comms, CLRRTS);
	//Status = EscapeCommFunction(Comms, CLRDTR);
	Status = EscapeCommFunction(Comms, SETRTS); //RTS Request to Send Signal to esentially reset Serial port (arduino)
	//Status = EscapeCommFunction(Comms, SETDTR); //DTR data terminal ready, PC side terminal ready to recieve data

}

int fileExist(char *file) {
	//DOES FILENAME ALREADY EXIST
	char testNum[10];
	char *fp = NULL;
	char testName[10] = "\\TestLog_";
	char fileEnd[5] = ".txt";
	int constant = 19;
	int fpLen = strlen(file);
	int dirLen = fpLen + constant;
	fileDir = (char*)malloc(dirLen * sizeof(char));


	if (fileDir == NULL) {
		printf(" \nFile Memory allocation Failed \n");
		return 1;
	}	

	for (int i = 1; i < 1000; i++) {

		sprintf(testNum, "%d", i); //converting testNum as int to string		   
							    //adding test num to file dir
		strcpy(fileDir, file);       //adding strings to make file name
		strcat(fileDir, testName);
		strcat(fileDir, testNum);
		strcat(fileDir, fileEnd);

		printf("%s", fileDir);

		/* Check for existence */
		if ((_access(fileDir, 0)) != -1) {           //Function to check if file exists
			printf("\nFile logs exists\n");
		}
		else { //if file doesnt exists 
			printf("\nFile doesnt exist\n");
			break;
		}
	}
	return 0;  //send file num to next function
}

int readPort(HANDLE Comms) 
{
	printf("Waiting for data");
	//READING DATA FROM SERIAL PORT
	char word[2] = "@";  //Text to look for
	char SerialBuffer[2];//Buffer for storing Rxed Data
	DWORD NoBytesRead;  //set type for NoBytesRead
	char *str = fileDir;
		
	//printf("%s", str);
	FILE* fp;    //create file pointer

	fp = fopen(str, "w");   //create file to write serial data to

	do
	{ //start do while loop
		ReadFile(Comms,           //Handle of the Serial port
			SerialBuffer,       //
			sizeof(SerialBuffer) - 1,//Size of Buffer
			&NoBytesRead,    //Number of bytes read
			NULL);

		SerialBuffer[NoBytesRead] = 0; //Specify null terminator to stop data rxing strangely
		printf("%s", SerialBuffer); //Print rx'd data to CMD
		fprintf(fp, "%s", SerialBuffer); //Save rx'd data to txt file


		char* fin = strstr(SerialBuffer, word); //Find 'word' in SerialBuffer and assign to 'fin'



			//if fin has data then close Serial and close/save file
		if (fin != NULL)
		{
			printf("\n \r Results Successfully logged \n \r \n \r");
			fclose(fp);         //close file
			fp = NULL;          //set file pointer to equal null to break from the loop
		}

	memset(SerialBuffer, 0, 2);

	} while (fp != NULL);  //while, from do while loop

return 0; //return testNum for use in another function
}

void parseFile()
{
	//FUNCTION TO PARSE DATA FROM LOG FILE TO FIND FAILED TESTS
	char* dir = fileDir;

	FILE* fp1;      //create file pointer
	char str[512] = { 0 };  //string to store data read from log file
	const char fail[5] = "FAIL";    //text to look for
	const char testnum[18] = "Test Iteration";     //text to look for
	char* ret;  //create char variables
	char* num;
	BOOL testFailed = FALSE;

	fp1 = fopen(dir, "r");  //Open file to read
	printf("%s", dir);

	if (fp1 == NULL)
	{ //if you cant open file, print 
		printf("\n \r File open fail");
	}

	else if (fp1 != NULL)  //if you cant open file, print
	{
		printf("\n \r File opened");
		//Loop to find test number and fail strings 
		while (fgets(str, 512, fp1) != NULL)
		{
			num = strstr(str, testnum); //find test num in string and assign to num
			ret = strstr(str, fail);    //find 'fail' in string and assign to ret

			if (ret != NULL)
			{    //if both variables have data then print num
				printf("\n \r %s", num);  //num has been given enough mem to include the fail that comes after it
				testFailed = TRUE;
			}
		}
	}

	if (!testFailed)
	{
		printf("\n\r\n All tests passed");
	}


	if (fp1 != NULL)
	{
		fclose(fp1);    //close/save file
		fp1 = NULL;
	}

	getchar();
	getchar();
}












// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
