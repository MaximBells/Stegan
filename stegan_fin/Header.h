typedef unsigned __int16 WORD;

typedef struct {
	WORD   bfType;         
	int    bfSize;         
	int    bfReserved;     
	int    bfOffBits;      
						  
	int    biSize;        
						   
						   
	int    biWidth;        
	int    biHeight;       
	WORD   biPlanes;       
	WORD   biBitCount;     
	int    biCompression;  
						   
						   
	int    biSizeImage;    
						   
	int    biXPelsPerMeter;
	int    biYPelsPerMeter;
	int    biClrUsed;      
						   
	int    biClrImportant; 
						   
} BMPheader;