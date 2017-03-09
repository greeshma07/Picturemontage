/*
Submitted by: Greeshma Sasidharan Nair 
Microsoft Visual Studio 2010 
purpose: To generate an image mosaic using number of different images.
*/

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <iostream>
#include <time.h>

using namespace cv;
using namespace std;

#include "variables.h"
#include "functions.h"

int main(){
	int userInput; //to take a value to exit the program
	glob(path,file); //to read files from the path given
	if(file.size()==0){
		cout<<"No images found"<<endl;
		exit(1);
	}
	cout<<"Filler image loading..."<<endl;
	loadFillerImages(); //loading filler images
	cout<<"Number of filler image loaded: "<<imageList.size()<<endl;
	if(imageList.size()==0){
		exit(1);
	}	
	loadTargetImage(); //load target image
	//creating a new image with the same pixels as target image
	newImg.create(targetImg.rows,targetImg.cols,CV_8UC(3));
	newImg=Scalar(0,0,0);
	cout<<"creating image mosaic starts.."<<endl;

	start = clock(); //performance evaluation
    cout<<"Calculating time performance..."<<endl;
	createMosaic(); //function to generate mosaic
	imwrite("mosaicResult.jpg",newImg);	
	start = clock() - start;
	createTime=((float)start)/CLOCKS_PER_SEC;	
	cout<<"Mosaic Image generated: mosaicResult.jpg"<<endl;
	cout<<"Time taken to generate mosaic "<<createTime<<" sec"<<endl;
	searchTime=searchTime/blockCount;
	cout<<"Average time taken to search the best match "<<searchTime<<" sec"<<endl;
	cout<<"Press 0(and enter) to exit"<<endl;
	cin>>userInput;
	while(userInput == 0)
		break;
	return 0;
}