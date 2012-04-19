#include <qimage.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qwidget.h>
#include <qaction.h>
#include <qpushbutton.h>
#include <qcolor.h>
#include <math.h>

#include "include/mywidget.h"
#include "include/mpeg2dec.h"



int spots,w,h,ml,mr,ml2,mh;
int hl,hr,ht,hb;
double ur,ug,ub,sr,sg,sb;
bool tlelbow,trelbow;
point *lelbow,*relbow, *left,*right;
QImage img;

	MyWidget::MyWidget(){
		b_play = new QPushButton(tr("Play"), this);
		b_hot = new QPushButton(tr("Find Hotspots"), this);
		
		left = NULL;
		
		connect(qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()));
 		connect(b_play, SIGNAL(clicked()), SLOT(play()));
 		connect(b_hot, SIGNAL(clicked()), SLOT(hotspots()));
	}
	
	void MyWidget::init(int argc, char * argv[]){
		framecount = 1;
		themain(argc,argv);
		printf("Frames = %d \n",framecount);
		w = first->w;
		h = first->h;
		img.create(w,h,32,0,QImage::IgnoreEndian);
		
		setFixedSize(w+150,h);
		move(0,0);
		b_play->setGeometry(w+10, 10, 75, 30);
		b_hot->setGeometry(w+10, 50, 100, 30);
	}
	
	void MyWidget::play(){
		QPainter p1;
	
		QImage pic;
		ptrFrame *temp;
		int count = 0;
		current = first;
		while (current->next != NULL){
			spots = 0;
			count++;
			printf("Showing frame %d\n",count);
			loadPic(pic,current);
			
			p1.begin(this);
				p1.drawPixmap(0,0,pic);
			p1.end();
			
			temp = current;
			current = current->next;
		
		}
	
	}
	
	void MyWidget::hotspots(){
		
		printf("Finding hotspots\n");
		
		QImage pic;
		QPainter p1;
		
		allocBody();
		ml = w/2;
		ml2 = h/2;
		mr = w/2;
		mh = h/2;
		
		int r,g,b,i,j,temp,temp2,r1,g1,b1,cou = 0;
		long dr,dg,db,points;
		double rcount,gcount,bcount;
		rcount = 0;
		gcount = 0;
		bcount = 0;
		points = 0;
		ptrFrame *current = first;
		pic.create(w,h,32,0,QImage::IgnoreEndian);
		
		while (current->next != NULL){
		for (i=0;i<w;i++){
			for (j=0;j<h;j++){
				r=0;g=0;b=0;
				r1=0;g1=0;b1=0;
				temp = (int)current->r + i*current->w+j;
				temp2 = (int)current->next->r + i*current->w+j;
				memcpy(&r,(void*)temp,1);
				memcpy(&r1,(void*)temp2,1);
				
				temp = (int)current->g + i*current->w+j;
				temp2 = (int)current->next->g + i*current->w+j;
				memcpy(&g,(void*)temp,1);
				memcpy(&g1,(void*)temp2,1);
				
				temp = (int)current->b + i*current->w+j;
				temp2 = (int)current->next->b + i*current->w+j;
				memcpy(&b,(void*)temp,1);
				memcpy(&b1,(void*)temp2,1);
				
				dr = abs(r1-r);
				dg = abs(g1-g);
				db = abs(b1-b);
				//printf(" %d %d %d |",r,g,b);
				if ((dr > 50) && (dg > 50) && (db > 50)){
					rcount+= r1;
					gcount+= g1;
					bcount+= b1;
					points++;
				}
			}
		}
		//printf("Frame %d\n",cou++);
		current = current->next;
		}
		ur = rcount / points;
		ug = gcount / points;
		ub = bcount / points;
		printf("Mean rgb = %f,%f,%f\n",ur,ug,ub);
		current = first;
		rcount = 0;
		gcount = 0;
		bcount = 0;
		while (current->next != NULL){
		for (i=0;i<w;i++){
			for (j=0;j<h;j++){
				r=0;g=0;b=0;
				r1=0;g1=0;b1=0;
				temp = (int)current->r + i*current->w+j;
				temp2 = (int)current->next->r + i*current->w+j;
				memcpy(&r,(void*)temp,1);
				memcpy(&r1,(void*)temp2,1);
				
				temp = (int)current->g + i*current->w+j;
				temp2 = (int)current->next->g + i*current->w+j;
				memcpy(&g,(void*)temp,1);
				memcpy(&g1,(void*)temp2,1);
				
				temp = (int)current->b + i*current->w+j;
				temp2 = (int)current->next->b + i*current->w+j;
				memcpy(&b,(void*)temp,1);
				memcpy(&b1,(void*)temp2,1);
				
				dr = abs(r1-r);
				dg = abs(g1-g);
				db = abs(b1-b);
				//printf(" %d %d %d |",r,g,b);
				if ((dr > 50) && (dg > 50) && (db > 50)){
					rcount+= pow(r1 - ur,2)/ (points-1);
					gcount+= pow(g1 - ug,2)/ (points-1);
					bcount+= pow(b1 - ub,2)/ (points-1);
					pic.setPixel(i,j,qRgb(r,g,b));
					if (i<ml){
						ml = i;
					}
					if (i>mr){
						mr = i;
					}
					if (j<mh){
						mh = j;
					}
					if (j>ml2){
						ml2 = j;
					}
				}else{
					pic.setPixel(i,j,qRgb(255,255,255));
					
				}
			}
		}
		//printf("Frame %d\n",cou++);
		current = current->next;
		}
		sr = sqrt(rcount) ;
		sg = sqrt(gcount) ;
		sb = sqrt(bcount) ;
		printf("Standard dev rgb = %f,%f,%f,\n",sr,sg,sb);
		Isolate(pic,first);
		p1.begin(this);
			p1.drawPixmap(0,0,pic);
		p1.end();
		
	}
	
	void MyWidget:: Isolate(QImage pic,ptrFrame* ptr){
		int r,g,b,i,j,temp,temp2,r1,g1,b1,setpixels;
		long dr,dg,db;
		
		QImage mask;
		mask.create(w,h,32,0,QImage::IgnoreEndian);
		
		pic.create(ptr->w,ptr->h,32,0,QImage::IgnoreEndian);
		
		for (i=0;i<w;i++){
			for (j=0;j<h;j++){
				r=0;g=0;b=0;
				r1=0;g1=0;b1=0;
				temp = (int)ptr->r + i*ptr->w+j;
				temp2 = (int)ptr->next->r + i*ptr->w+j;
				memcpy(&r,(void*)temp,1);
				memcpy(&r1,(void*)temp2,1);
				
				temp = (int)ptr->g + i*ptr->w+j;
				temp2 = (int)ptr->next->g + i*ptr->w+j;
				memcpy(&g,(void*)temp,1);
				memcpy(&g1,(void*)temp2,1);
				
				temp = (int)ptr->b + i*ptr->w+j;
				temp2 = (int)ptr->next->b + i*ptr->w+j;
				memcpy(&b,(void*)temp,1);
				memcpy(&b1,(void*)temp2,1);
				
				dr = abs(r1-r);
				dg = abs(g1-g);
				db = abs(b1-b);
				//printf(" %d %d %d |",r,g,b);
				
				double thres = 0.3;
				if ((i>=ml) && (i<=mr) && (j>=mh) && (j<=ml2)){
					if (((ur - thres*sr) < r) && ((ur + thres*sr) > r) && ((ug - thres*sg) < g) && ((ug + thres*sg) > g) && ((ub - thres*sb) < b) && ((ub + thres*sb) > b)){
						pic.setPixel(i,j,qRgb(255,0,0));
						mask.setPixel(i,j,qRgb(255,255,255));
						setpixels++;
					}else{
						pic.setPixel(i,j,qRgb(r,g,b));
					}
				}
				
			}
		}
		isolateHead(mask,pic);
		isolateElbows(mask,pic);
		isolateHands(mask,pic);
		skinData(mask,pic);
	}
	
	
	void MyWidget:: loadPic(QImage pic,ptrFrame* ptr){
		int r,g,b,i,j,temp,temp2,r1,g1,b1,setpixels;
		long dr,dg,db;
		
		pic.create(ptr->w,ptr->h,32,0,QImage::IgnoreEndian);
		
		for (i=0;i<w;i++){
			for (j=0;j<h;j++){
				r=0;g=0;b=0;
				r1=0;g1=0;b1=0;
				temp = (int)ptr->r + i*ptr->w+j;
				memcpy(&r,(void*)temp,1);
				
				temp = (int)ptr->g + i*ptr->w+j;
				memcpy(&g,(void*)temp,1);
				
				temp = (int)ptr->b + i*ptr->w+j;
				memcpy(&b,(void*)temp,1);
				
				float thres = 0.3;
				if ((i>=ml) && (i<=mr) && (j>=mh) && (j<=ml2)){
					if (((ur - thres*sr) < r) && ((ur + thres*sr) > r) && ((ug - thres*sg) < g) && ((ug + thres*sg) > g) && ((ub - thres*sb) < b) && ((ub + thres*sb) > b)){
						pic.setPixel(i,j,qRgb(255,0,0));
					}else{
						pic.setPixel(i,j,qRgb(r,g,b));
					}
				}
				
			}
		}
	}
	
	void MyWidget:: isolateHead(QImage mask,QImage pic){
		int i,j,l,r;
		l = w/3;
		r = 2*w/3;
		hl = w;
		hr = 0;
		ht = h;
		hb = 0;
		for (i=l;i<r;i++){
			for (j=0;j<h/2;j++){
				if (mask.pixel(i,j) == qRgb(255,255,255)){
					if (i < hl){
						hl = i;
					}
					if (i > hr){
						hr = i;
					}
					if (j < ht){
						ht = j;
					}
					if (j > hb){
						hb = j;
					}
				}
			}
		}
		for (i=hl;i<=hr;i++){
			for (j=ht;j<=hb;j++){
				pic.setPixel(i,j,qRgb(0,255,0));
			}
		}
	}
	
	void MyWidget:: isolateElbows(QImage mask,QImage pic){
		int i,j,center,mt,mb;
		mt = h;
		mb = 0;
		center = hl+(hr-hl)/2;
		for (i=0;i<w;i++){
			for (j=h/2;j<h;j++){
				if (mask.pixel(i,j) == qRgb(255,255,255)){
					if (j<mt){
						mt = j;
					}
					if (j>mb){
						mb = j;
					}
				}
			}
		}
		
		int lex,ley,rex,rey;
		ley = h;
		rey = h;
		for (i=0;i<center;i++){
			for (j=h/2;j<(mt + (mb-mt)/2);j++){
				if (mask.pixel(i,j) == qRgb(255,255,255)){
					if (j<ley){
						ley = j;
						lex = i;
					}
				}
			}
		}
		for (i=center;i<w;i++){
			for (j=h/2;j<(mt + (mb-mt)/2);j++){
				if (mask.pixel(i,j) == qRgb(255,255,255)){
					if (j<rey){
						rey = j;
						rex = i;
					}
				}
			}
		}
		//printf("%d,%d\n",lex,ley);
		for (i=lex-2;i<lex+3;i++){
			for (j=ley-2;j<ley+3;j++){
				pic.setPixel(i,j,qRgb(255,255,0));
			}
		}
		for (i=rex-2;i<rex+3;i++){
			for (j=rey-2;j<rey+3;j++){
				pic.setPixel(i,j,qRgb(0,255,0));
			}
		}
		lelbow->x = lex;
		lelbow->y = ley;
		relbow->x = rex;
		relbow->y = rey;
		//printf("%d,%d\n",lex,ley);
	}
	void MyWidget:: isolateHands(QImage mask,QImage pic){
		int i,j,center,mt,mb;
		mt = h;
		mb = 0;
		center = hl+(hr-hl)/2;
		for (i=0;i<w;i++){
			for (j=h/2;j<h;j++){
				if (mask.pixel(i,j) == qRgb(255,255,255)){
					if (j<mt){
						mt = j;
					}
					if (j>mb){
						mb = j;
					}
				}
			}
		}
		
		int lhx,lhy,rhx,rhy;
		lhy = 0;
		rhy = 0;
		for (i=0;i<center;i++){
			for (j=(mt + (mb-mt)/2);j<h;j++){
				if (mask.pixel(i,j) == qRgb(255,255,255)){
					if (j>lhy){
						lhy = j;
						lhx = i;
					}
				}
			}
		}
		for (i=center;i<w;i++){
			for (j=(mt + (mb-mt)/2);j<h;j++){
				if (mask.pixel(i,j) == qRgb(255,255,255)){
					if (j>rhy){
						rhy = j;
						rhx = i;
					}
				}
			}
		}
		for (i=lhx-2;i<lhx+3;i++){
			for (j=lhy-2;j<lhy+3;j++){
				pic.setPixel(i,j,qRgb(0,0,255));
			}
		}
		for (i=rhx-2;i<rhx+3;i++){
			for (j=rhy-2;j<rhy+3;j++){
				pic.setPixel(i,j,qRgb(0,0,255));
			}
		}
		left->x = lhx;
		left->y = lhy;
		right->x = rhx;
		right->y = rhy;
		
	}
	
	void MyWidget:: skinData(QImage mask,QImage pic){
		int i,j,r,g,b,points;
		double rc,gc,bc;
		float mel,mer,cl,cr;
		QColor col;
		
		mel = ((lelbow->y - left->y)) / (lelbow->x - left->x);
		cl = lelbow->y - (mel*lelbow->x);
		for (j=lelbow->y;j<=left->y;j++){
			//printf("%d\n",j);
			for (i=left->x;i<lelbow->x;i++){
				if ((int)(mel*i+cl) == j){
					mask.setPixel(i,j-1,qRgb(0,255,0));
					mask.setPixel(i,j,qRgb(0,255,0));
					mask.setPixel(i,j+1,qRgb(0,255,0));
				}
			}
			for (i=left->x;i>=lelbow->x;i--){
				if ((int)(mel*i+cl) == j){
					mask.setPixel(i,j-1,qRgb(0,255,0));
					mask.setPixel(i,j,qRgb(0,255,0));
					mask.setPixel(i,j+1,qRgb(0,255,0));
				}
			}
		}
		mer = (relbow->y-right->y) / (relbow->x - right->x);
		cr = right->y - (mer*right->x);
		
		for (j=relbow->y;j<=right->y;j++){
			
			for (i=right->x;i<relbow->x;i++){
				if ((int)(mer*i+cr) == j){
					mask.setPixel(i,j-1,qRgb(0,255,0));
					mask.setPixel(i,j,qRgb(0,255,0));
					mask.setPixel(i,j+1,qRgb(0,255,0));
				}
			}
			for (i=right->x;i>=relbow->x;i--){
				if ((int)(mer*i+cr) == j){
					mask.setPixel(i,j-1,qRgb(0,255,0));
					mask.setPixel(i,j,qRgb(0,255,0));
					mask.setPixel(i,j+1,qRgb(0,255,0));
				}
			}
		}
		
		//mean
		rc= 0;
		gc = 0;
		bc = 0;
		points = 0;
		printf("AAA\n");
		for (i=ml;i<=mr;i++){
			printf("AAA\n");
			for (j=mh;j<=ml2;j++){
				printf("AAA2\n");
				if (mask.pixel(i,j) == qRgb(0,255,0)){
					col = pic.pixel(i,j);
					r = col.red();
					g = col.green();
					b = col.blue();
					printf("AAA\n");
					rc += r;
					gc += g;
					bc += b;
					points++;
				}
			}
		}
		ur = rc / points;
		ug = gc / points;
		ub = bc / points;
		
		rc= 0;
		gc = 0;
		bc = 0;
		for (i=ml;i<=mr;i++){
			for (j=mh;j<=ml2;j++){
				if (mask.pixel(i,j) == qRgb(0,255,0)){
					col = pic.pixel(i,j);
					r = col.red();
					g = col.green();
					b = col.blue();
					rc += pow(r - ur,2)/ (points-1);
					gc += pow(g - ug,2)/ (points-1);
					bc += pow(b - ub,2)/ (points-1);
				}
			}
		}
		sr = sqrt(rc);
		sg = sqrt(gc);
		sb = sqrt(bc);
		
		printf("Mean rgb = %f,%f,%f\n",ur,ug,ub);
		printf("Standard dev rgb = %f,%f,%f,\n",sr,sg,sb);
	}
	
	
	void MyWidget:: allocBody(){
		if (!(lelbow = (point*)malloc(sizeof(struct point)))){
			printf("Malloc failed\n");
		}
		if (!(relbow = (point*)malloc(sizeof(struct point)))){
			printf("Malloc failed\n");
		}
		if (!(left = (point*)malloc(sizeof(struct point)))){
			printf("Malloc failed\n");
		}
		if (!(right = (point*)malloc(sizeof(struct point)))){
			printf("Malloc failed\n");
		}
	}
