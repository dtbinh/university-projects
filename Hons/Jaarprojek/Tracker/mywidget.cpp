#include <cstdlib>
#include <qimage.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <qaction.h>
#include <qpushbutton.h>
#include <qcolor.h>
#include <qmessagebox.h>
#include <qinputdialog.h>
#include <qfiledialog.h>
#include <qslider.h>
#include <qcombobox.h>
#include <qvector.h>
#include <math.h>
#include <iostream>

#include "include/mywidget.h"
#include "include/mpeg2dec.h"
#include "include/stats.h"
#include "include/tracker.h"
#include "include/Mpeg.h"

#define fps 100/25

char in[255],out[255];

double t_cur;

double skin_t,edge_t;

int w,h;

QImage img;

Tracker *tracker;
Stats *stats;

//do we need to recalculate anything
bool reload;


//Constructor for the GUI

MyWidget::MyWidget(){
	
	
	playing = false;
	frameIndex = 1;
	
	//setup the initial gui
	b_play = new QPushButton(tr("Play"), this);
	b_save = new QPushButton(tr("Save Output"), this);
	b_open = new QPushButton(tr("Open File"), this);
	
	l_skin = new QLabel(tr("Skin Threshold"),this);
	s_skin = new QSlider(Horizontal,this);
	s_skin->setMinValue(1);
	s_skin->setMaxValue(7);
	s_skin->setTickInterval(1);
	s_skin->setValue((int)(SKINTHRES*10));
	skin_t = SKINTHRES;
	
	
	l_thres = new QLabel(tr("Edge Threshold"),this);
	s_thres = new QSlider(Horizontal,this);
	s_thres->setMinValue(3);
	s_thres->setMaxValue(7);
	s_thres->setTickInterval(1);
	s_thres->setValue((int)(THRES*10));
	edge_t = THRES;
	
	s_frame = new QSlider(Horizontal,this);
	
	status = new QLabel(tr("Status"),this);
	
	l_algo = new QComboBox(this);
	l_algo->insertItem("Linear Equation");
	l_algo->insertItem("Motion Vector");
	l_algo->insertItem("Hybrid");
	
	//creates some objects and threads
	list = new QVector<ptrFrame>();
	tracker = new Tracker(list);
	stats = new Stats(list);
	stats->start();
	
	
	connect(qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()));
	connect(b_play, SIGNAL(clicked()), SLOT(play()));
	connect(b_save, SIGNAL(clicked()), SLOT(save()));
	connect(b_open, SIGNAL(clicked()), SLOT(open_file()));
	
	connect(s_thres, SIGNAL(valueChanged(int)), SLOT(thresChanged()));
	connect(s_skin, SIGNAL(valueChanged(int)), SLOT(skinthresChanged()));
	
	connect(l_algo, SIGNAL(highlighted(int)), SLOT(algorithmChange()));
}
	
	
//called when a paintEvent occurs
//in:the event

void MyWidget::paintEvent(QPaintEvent *event)
{
	loadFirst();
}

//adapt the window 

void MyWidget::init()
{	
	w = 300;
	h = 350;
	setFixedSize(w+160,h);
	move(0,0);
	
	b_play->setGeometry(w+10, 10, 75, 30);
	b_open->setGeometry(w+10, 50, 100, 30);
	b_save->setGeometry(w+10, 90, 100, 30);
	
	l_thres->setGeometry(w+10,130,100,30);
	s_thres->setGeometry(w+10,150,100,30);
	
	l_skin->setGeometry(w+10,170,100,30);
	s_skin->setGeometry(w+10,190,100,30);
	
	status->setGeometry(w+10,h-40,100,30);
	
	s_frame->setGeometry(10,h-15,w,30);
	
	l_algo->setGeometry(w+10,210,140,30);
	
	reload = true;
	
}


//changes a Qstring object into a char array pointer	
//in: the QString object
//out: the char* poiinter

char* c2c(QString input)
{
	return (char*)input.latin1();
}

//skin threshold slider changed

void MyWidget::skinthresChanged()
{
	skin_t = s_skin->value() / 10.0;
	reload = true;	
}

//edge threshold slider changed

void MyWidget::thresChanged()
{
	edge_t = s_thres->value() / 10.0;
	reload = true;
}

//when the algorithm change

void MyWidget::algorithmChange()
{
	//if needed in future
}


//free everything

void cleanup()
{
	uint i;
	ptrFrame *temp;
	for (i=0;i<list->size();i++)
	{
		temp = list->at(i);
		list->remove(i);
		
		free(temp->r);
		free(temp->g);
		free(temp->b);
		
		free(temp->left);
		free(temp->lelbow);
		free(temp->right);
		free(temp->relbow);
		
		free(temp);
	}
	list->clear();
}

//the open file dialog

void MyWidget::open_file()
{
	//pops up the dialog
	QString s = QFileDialog::getOpenFileName("./","Mpeg files (*.mpg *.mpeg)",this,"open file dialog","Choose a file" );
	
	//was a file selected
	if (s != NULL)
	{
		status->setText("Loading...");
	
		char *inp = c2c(s);
		sprintf(in,"%s",inp);
		
		//remove previous info
		cleanup();
		
		framecount = 1;
		//load the video
		
		Mpeg *loader = new Mpeg(list,in);
		loader->start();
		loader->load();
		//qApp->notify(this,&paintEvent());
		char count[120];
		
		while (loader->Processing())
		{
			qApp->processEvents();
			sprintf(count,"Loading... %d",framecount);
			status->setText(count);
		}

		ptrFrame* first = list->at(0);
		w = first->w;
		h = first->h;
		

		//adapt the gui

		img.create(w,h,32,0,QImage::IgnoreEndian);
		
		setFixedSize(w+160,h+50);
		move(0,0);
		b_play->setGeometry(w+10, 10, 75, 30);
		b_open->setGeometry(w+10, 50, 100, 30);
		b_save->setGeometry(w+10, 90, 125, 30);

		l_thres->setGeometry(w+10,130,100,30);
		s_thres->setGeometry(w+10,150,100,30);
		
		l_skin->setGeometry(w+10,170,100,30);
		s_skin->setGeometry(w+10,190,100,30);
		
		status->setGeometry(w+10,h-10,100,30);
		
		s_frame->setGeometry(10,h+10,w-10,30);
		s_frame->setMinValue(1);
		s_frame->setMaxValue(framecount-1);
		s_frame->setTickInterval(1);
		s_frame->setValue(1);
		
		l_algo->setGeometry(w+10,210,140,30);
		
		reload = true;
		status->setText("File Loaded");
	}
}

//this saves the output file

void MyWidget::save()
{
	//we can not save if something has changed
	if (reload)
	{
		return;
	}
	
	int frame = 1;
	bool ok;
	ptrFrame *loop = list->at(frame);
	//ask for the gesture
	QString text = QInputDialog::getText("SASL Gesture Detection", "Enter word associated with this gesture:", QLineEdit::Normal,QString::null, &ok, this );
	
	sprintf(out,"%s.txt",in);
	
	status->setText("Saving File...");
	
	//write the header and coordinates
	if ( ok && !text.isEmpty() )
	{
		FILE *f;
		f = fopen(out,"w");
		
		sprintf(out,"#File Name\n");
		fprintf(f,out);
		fprintf(f,in);
		
		sprintf(out,"\n#Gesture\n");
		fprintf(f,out);
		
		fprintf(f,c2c(text));
		
		sprintf(out,"\n#Width Height Position_of_chin(x,y) \n");
		fprintf(f,out);
		
		sprintf(out,"%d %d %d,%d\n",loop->w,loop->h,tracker->cx,tracker->cy);
		fprintf(f,out);
		
		sprintf(out,"#Left(x,y) Right(x,y) \n");
		fprintf(f,out);
		
		while (frame < list->size())
		{
			qApp->processEvents();
			loop = list->at(frame);
			sprintf(out,"%d,%d %d,%d\n",loop->lx,loop->ly,loop->rx,loop->ry);
			fprintf(f,out);
			frame++;
			
		}
		
		fclose(f);
		QMessageBox::information( this, "SASL Gesture Detection","Output file sucessfully saved.");
		status->setText("File Saved");
	}
}

//loads and draws the first frame on the screen

void MyWidget::loadFirst()
{
	QPainter p1;
	
	if (list->size() > 0)
	{
		loadPic(img,0);
		p1.begin(this);
			p1.drawPixmap(0,0,img);
		p1.end();
	}
}
	
	
//this does the actual playing and tracking of the file
	
void MyWidget::play(){
	
	//check if we are playing. This may mean we must pause
	if (playing)
	{
		playing = false;
		b_play->setText("Play");
		return;
	}
    //never play an empty file
	if (list->size() == 0)
	{
		return;
	}
	
	QPainter p1;
	QImage pic;
	
	
	int count = frameIndex-1;
	
	status->setText("Calculating");
	
	//find everything there is to know about the person in the video	
	hotspots();
	
	status->setText("Playing");
	
	//initial start time
	double cur = clock();
	
	
	playing = true;
	b_play->setText("Pause");
	//move through all the frames
	while (frameIndex < list->size() && playing)
	{
	
		count++;
		//load a picture
		loadPic(pic,frameIndex);
		//do all tracking
		tracker->trackFrame(pic,frameIndex,edge_t,skin_t,l_algo->currentText());
		//to make sure the frame rate is constant
		while ( (clock() - cur)/CLOCKS_PER_SEC*100 < fps){}
		//draw the frame
		p1.begin(this);
			p1.drawPixmap(0,0,pic);
		p1.end();	
		//get the time now
		cur = clock();
		
		//next frame
		frameIndex++;
		
		s_frame->setValue(count);
		
		qApp->processEvents();
	}
	//check how we stopped playing
	if (playing)
	{
		b_play->setText("Play");
		frameIndex = 1;
		status->setText("Stopped");
	}
	else
	{
		status->setText("Paused");
	}
	playing = false;
}
	
//this calls all the data extraction

void MyWidget::hotspots()
{
	
	//do we need to recalculate or only return to the original settings
	if (reload)
	{
		frameIndex = 1;
		
		//calculate the stats
		stats->hotspots(w,h,edge_t);
		
		while(stats->Processing())
		{
			qApp->processEvents();
		}
		
		//pass values to the tracker
		tracker->setAllStatsEdgeValues(stats->ur,stats->ug,stats->ub,stats->sr,stats->sg,stats->sb);
		tracker->setAllStatsSkinValues(stats->s_ur,stats->s_ug,stats->s_ub,stats->s_sr,stats->s_sg,stats->s_sb);
		tracker->setAllHeadValues(stats->hl,stats->hr,stats->ht,stats->hb);
		tracker->setAllBoxValues(stats->ml,stats->mr,stats->ml2,stats->mh,w,h);
		tracker->notifyArms();
	}
	//we do not need to reload at the moment
	reload = false;	
}
	
	
//load a frame and perform hand detection
//in:image to put frame, framenumber to load

void MyWidget:: loadPic(QImage pic,int frame)
{		
	ptrFrame *ptr;
	
	ptr = list->at(frame);
	
    long r,g,b,i,j,temp;
	
	pic.create(ptr->w,ptr->h,32,0,QImage::IgnoreEndian);
	
	for (i=0;i<w;i++)
	{
		for (j=0;j<h;j++)
		{
			r=0;g=0;b=0;
			//r1=0;g1=0;b1=0;
			temp = (long)ptr->r + i*ptr->w+j;
			memcpy(&r,(void*)temp,1);
			
			temp = (long)ptr->g + i*ptr->w+j;
			memcpy(&g,(void*)temp,1);
			
			temp = (long)ptr->b + i*ptr->w+j;
			memcpy(&b,(void*)temp,1);
			
			pic.setPixel(i,j,qRgb(r,g,b));	
			
			
		}
	}
}

