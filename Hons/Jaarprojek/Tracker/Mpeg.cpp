#include <qvariant.h>
#include <qvector.h>
#include <iostream>
#include "include/ptrFrame.h"
#include "include/Mpeg.h"
#include "include/mpeg2dec.h"


//this the constructor of the thread object which is in charge of loading the
//video file into the vector list
//in: list of frames, input file

Mpeg:: Mpeg( QVector<ptrFrame> *vectorlist,char *inFile)
{
	list = vectorlist;
	active = true;
	in = inFile;
}

//The main body of the thread. If execution leaves this thread the thread is destroyed

void Mpeg:: run()
{
	while (active){
		if (processing)
		{
			//loads the Mpeg file
			themain(in);
			processing = false;
			active = false;
		}
	}
}

//tells the thread to load the mpeg file specified in the constructor

void Mpeg:: load()
{
	processing = true;
}

//checks the status of the thread

bool Mpeg::Processing()
{
	return processing;
}
