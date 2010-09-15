#pragma once
#include "main.h"

namespace Platform
{
	//==========================================================================================================================
	// Open a file to read data
	class CInputFile								// File input and output
	{
	public:	
		bool	Open(wstring fn);					// Open file
		long	Read();								// Read an integer
		ulong	Read(float *buff,int count);		// Read the specified number of floating-point numbers to the buffer;
													//		returns the actual number of bytes read.
		ulong	Read(int *buff,int count);			// || integers
		ulong	Read(char *buff,int count);			// || bytes
		bool	ReadFile(wstring fn);				// Read the entire file into the designated buffer
		void	Close();							// Close file
		ulong	GetSize();							// Get file length
		ulong	Seek(int dx,int wh){if(infile) infile.seekg(dx,wh);}
		//-----------------------------------------------------------------------------------------------------------------
		char *buff;
		char* GetBuffer(){return buff;}	
		CInputFile()
		{
			fSize=0;buff=0;
		}
		~CInputFile()
		{
			infile.close();
		}
	private:
		ulong fSize;								// File length
		ifstream infile;							// Input file stream
	};

	//==========================================================================================================================
	// Open a file to write data
	class COutputFile
	{
	public:
		bool	Open(wstring fn);					// Open file
		ulong	Write(char *data,int size);			// Write size bytes of data into the file
		ulong	Write(ulong Data);
		ulong	Write(long Data);
		ulong	Write(float Data);
		ulong	Write(unsigned int Data);
		ulong	Write(float *Data,int size);
		ulong   Write(ulong *data,int size);

		void	Close();							// Close file
		ulong	Tell();
		COutputFile()
		{}
		~COutputFile()
		{Close();}
	private:
		ofstream	outFile;						// Output file stream
	};

	//==========================================================================================================================
	// INI file access
	class CINIFile		
	{
	public:
		bool	Open(wstring fn);
		wstring ReadString(wstring sec,wstring key);			// Read string
		int		ReadInt(wstring sec,wstring key);				// Read integer
		void	Write(wstring sec,wstring key,wstring str);		// Write string
		void	Write(wstring sec,wstring key,int data);		// Write integer
	private:
		wstring FileName;
	public:
		CINIFile(wstring fn);
		CINIFile();
	};
	//==========================================================================================================================
	wstring GetCurPath();
	string  GetCurPathA();
	wstring GetSysDir();
};