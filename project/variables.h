
int block_size=60;

string targetImage = "tarImg2.jpg";
string path = "images/*";

struct fillerImg{
	float redAvg;
	float greenAvg;
	float blueAvg;
	int timesUsed;
}fi;

clock_t start,start1;
float createTime,searchTime=0.0;
vector<Mat> imageList; //to store filler images
vector<fillerImg> fillerImgAvg;
Mat targetImg,newImg;

vector<string> file; 

float blockRedAvg,blockGrAvg,blockBlueAvg;
int match,blockCount=0;

