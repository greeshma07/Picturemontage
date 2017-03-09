
void loadFillerImages(){
	for(int i=0;i<file.size();i++){
		//if any files other than images or any gif image, just ignore them
		if(file[i].find(".jpeg") == string::npos && (file[i].find(".jpg") == string::npos)
			&& (file[i].find(".bmp") == string::npos) && (file[i].find(".png") == string::npos)
			&& file[i].find(".JPEG") == string::npos && (file[i].find(".JPG") == string::npos)
			&& (file[i].find(".BMP") == string::npos) && (file[i].find(".PNG") == string::npos)){
				continue;
		}
		Mat img = imread(file[i]);		
		if (img.empty()){
			cout << "Cannot load image!" <<file[i]<< endl;
			break;
		}
		imageList.push_back(img);		
	}	
}

void loadTargetImage(){
	targetImg=imread(targetImage);
	if (targetImg.empty()){
		cout << "Cannot load target image!" << endl;
		exit (1);
	}
	else
		cout<<"target image loaded"<<endl;
}

/*
to calculate each filler image red, green and blue average by
traversing the imagelist
*/
void findFillerImageAvg(){
	cout<<"Calculating filler image average ..."<<endl;
	for(int i=0;i<imageList.size();i++){
		float redAvg=0.0,greenAvg=0.0,blueAvg=0.0;
		for (int y = 0; y < imageList[i].rows; y++){
            for (int x = 0; x < imageList[i].cols; x++){
                blueAvg += imageList[i].ptr<uchar>(y)[(3 *x) + 0]; 
                greenAvg += imageList[i].ptr<uchar>(y)[(3*x) + 1];
                redAvg += imageList[i].ptr<uchar>(y)[(3*x) + 2];
            }
        }
        fi.blueAvg = blueAvg/(imageList[i].rows* imageList[i].cols);
        fi.greenAvg = greenAvg/(imageList[i].rows* imageList[i].cols);
        fi.redAvg = redAvg/(imageList[i].rows* imageList[i].cols);
		fi.timesUsed=0;
		fillerImgAvg.push_back(fi);
	}
}

/*
to find block red, green and blue average
*/
void findBlockAvg(Rect curBlock){
	blockRedAvg=0.0,blockGrAvg=0.0,blockBlueAvg=0.0;
	for (int y = curBlock.y; y < curBlock.y+block_size; y++){
	    for (int x = curBlock.x; x < curBlock.x+block_size; x++){
			if((y < targetImg.rows) && (x < targetImg.cols)){
				blockBlueAvg += targetImg.ptr<uchar>(y)[(3 *x) + 0];
				blockGrAvg += targetImg.ptr<uchar>(y)[(3*x) + 1];
				blockRedAvg += targetImg.ptr<uchar>(y)[(3*x) + 2];
			}
			else
				break;
	    }
	}	    
	blockBlueAvg = blockBlueAvg/(block_size* block_size);
	blockGrAvg = blockGrAvg/(block_size* block_size);
	blockRedAvg = blockRedAvg/(block_size* block_size);
}

/*
to find out which filler image is the best to replace block
using filleraverage,timesused and block average
*/
void findBestMatch(){
	match=0;
	float distance=0.0,smallest=0.0;
	for(int i=0;i<fillerImgAvg.size();i++){	
		distance=sqrt(pow(blockRedAvg - fillerImgAvg[i].redAvg,2) + pow(blockGrAvg - fillerImgAvg[i].greenAvg,2) + 
			pow(blockBlueAvg - fillerImgAvg[i].blueAvg,2)) * (fillerImgAvg[i].timesUsed+1);
		if(i==0)
			smallest = distance;
		if(smallest > distance){
			smallest = distance;
			match = i;
		}
		distance=0.0;
	}
}

/*
to replace a block in target image with the best filler image
*/
void blockReplacement(Rect curBlock){
	float red, green,blue;
	Mat img;
	imageList[match].copyTo(img);
	resize(img, img, Size(block_size,block_size));
	red = blockRedAvg - fillerImgAvg[match].redAvg;
	green = blockGrAvg - fillerImgAvg[match].greenAvg;
	blue = blockBlueAvg - fillerImgAvg[match].blueAvg;
	for (int y = 0; y < img.rows; y++){
        for (int x = 0; x < img.cols; x++){
			//Blue(if the value is out of range(<0 or >255) set it as 0 or 255 respectively)
            if(img.ptr<uchar>(y)[(3 *x) + 0]+blue < 0)
				img.ptr<uchar>(y)[(3 *x) + 0] = 0;
			else if(img.ptr<uchar>(y)[(3 *x) + 0]+blue > 255)
				img.ptr<uchar>(y)[(3 *x) + 0] = 255;
			else
				img.ptr<uchar>(y)[(3 *x) + 0] += blue;
			//green
			if(img.ptr<uchar>(y)[(3 *x) + 1]+green < 0)
				img.ptr<uchar>(y)[(3 *x) + 1] = 0;
			else if(img.ptr<uchar>(y)[(3 *x) + 1]+green > 255)
				img.ptr<uchar>(y)[(3 *x) + 1] = 255;
			else
				img.ptr<uchar>(y)[(3 *x) + 1] += green;
			//Red
			if(img.ptr<uchar>(y)[(3 *x) + 2]+red < 0)
				img.ptr<uchar>(y)[(3 *x) + 2] = 0;
			else if(img.ptr<uchar>(y)[(3 *x) + 2]+red > 255)
				img.ptr<uchar>(y)[(3 *x) + 2] = 255;
			else
				img.ptr<uchar>(y)[(3 *x) + 2] += red;
        }
    } 
	//copying each filler image to respected block in the output image
	if(curBlock.y < targetImg.rows-block_size && curBlock.x < targetImg.cols-block_size)
		img.copyTo(newImg(Range(curBlock.y,img.rows+curBlock.y),Range(curBlock.x,img.cols+curBlock.x)));
}

/*
to create mosaic for an image
*/
void createMosaic(){
	start1 = clock();
	findFillerImageAvg();
	start1 = clock() - start1;
	float tm=((float)start1)/CLOCKS_PER_SEC;
	for(int j=0;j<targetImg.rows;j+=block_size){
		for(int i=0;i<targetImg.cols;i+=block_size){
			Rect curBlock(i, j,block_size,block_size); //create block in targetimage
			start1 = clock();
			findBlockAvg(curBlock);			
			cout<<"Block average computed"<<endl;
			findBestMatch();
			cout<<"Found Match"<<endl;
			start1 = clock() - start1;
			searchTime+=((float)start1)/CLOCKS_PER_SEC;
			searchTime+=tm;
			blockReplacement(curBlock);
			blockCount++;
			cout<<"block replaced by filler image:"<<blockCount<<endl;
			fillerImgAvg[match].timesUsed+=1;
		}
	}
}