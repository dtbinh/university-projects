
#define UNKNOWNFILE 0x00
#define AUDIOFILE   0x01
#define VIDEOFILE   0x02
#define SYSTEMFILE  0X04

#define PACK_H_SIZE 8
#define BUFFSIZE 16384L
// 16kB buffer

typedef unsigned char marker;
typedef unsigned char Byte;
typedef long offset;




extern double starttime;
extern double timecorrect;



extern char basenamer[300];
extern int   chunknumber,chunkcount;
extern int confirmall;
extern Byte Buffer[BUFFSIZE];


extern float Byterate;
extern offset audioframelength;
extern int audiolayer;

extern int EoF;
extern offset VidSeqStart,VidSeqEnd;
extern double* startT;
extern offset* startO;
extern offset*stopO;
extern double* stopT;
extern double Tmax;

extern int demux;

extern FILE* InFd,*OutFd;
extern int splitnum;
extern int FileType;
extern offset FileSize;
extern offset Start,End; //offset of the buffer in file

extern offset MinimumCut,StuffingQuantum;

extern double FrameRate; 
