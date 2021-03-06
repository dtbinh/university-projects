/*typedef struct ptrFrame{
	
	ptrFrame *next;
}ptrFrame;
*/

//structure used to store data on the hands and elbows


class ptrFrame
{
	typedef struct point{
		int x,y;
		int vx,vy;
		int ax,ay;
	};
		//a linear y = mx +c equation used for the arms
	typedef struct equation{
		float m,c,length;
	};
		
	public:
		ptrFrame();
		
		
		void *r,*g,*b;
		int w,h;
		int rx,ry,lx,ly;
		//elbows and hands
		point *lelbow,*relbow, *left,*right;
		//arms
		equation *larm, *rarm;
		
	private:
		void allocBody();
		
};
