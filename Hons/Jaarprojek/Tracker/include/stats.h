#include <qthread.h>
class Stats : public QThread {
	public:
		Stats(QVector<ptrFrame> *vectorlist);
		void run();
		bool Processing();
		void hotspots(int width, int height,double edge_thres);
		
		//width height, movement area
		int ml,mr,ml2,mh;
		//head coordinates
		int hl,hr,ht,hb;
		
		double ur,ug,ub,sr,sg,sb;
		//mean and standard deviation for skin tone
		double s_ur,s_ug,s_ub,s_sr,s_sg,s_sb;
		
	private:
		void Isolate();
		void calc_hotspots();
		void isolateHead(QImage mask);
		void isolateElbows(QImage mask);
		void isolateHands(QImage mask);
		void skinData(QImage mask,QImage pic);
		
		int w,h,frames;
		
		
		double edge_t;
		bool processing,active;
		QVector<ptrFrame> *list;
	};
