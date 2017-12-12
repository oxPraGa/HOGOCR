
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
using namespace cv;
using namespace std;
const String DB1 = "Database/BD1/";
const String DB2 = "Database/BD2_TAHOMA/";
const String DB3 = "Database/BD3_TIMES/";
const String DB4 = "Database/BD4_VAGAOM/";
const String DB5 = "Database/BD5_VERDANA/";
const String DB6 = "Database/BD6_HUMASFM/";
// function for read file diroctry
int getdir (string dir, vector<string> &files)
{
    DIR *dp;
    struct dirent *dirp;
    if((dp  = opendir(dir.c_str())) == NULL  ) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL )  {
        if(string(dirp->d_name) != "." && string(dirp->d_name) != "..")
        files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}


// first step crop char and resize to 64 * 128
void SegementOneResize(Mat src, Mat &dst) {
    dst = Mat(0, 0, CV_32F);
    dst = src;
    Mat src_gray, thres;
    vector<vector<Point>> contours;
    vector<Vec4i> hirechy;
    Rect boundRect;
    cvtColor(src, src_gray, CV_BGR2GRAY);
    threshold(src_gray, thres, 1, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
    // bounding rectangle
    findContours(thres, contours, hirechy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    boundRect = boundingRect(Mat(contours[0]));
    dst = src_gray(boundRect);
    resize(dst, dst, Size(64, 128));
}

// step 2 get canny contour

void CannyCont(Mat src, Mat &dst) {
    dst.create(src.size(),src.type());
    Mat dstCanny;
    vector<vector<Point>> contours;
    vector<Vec4i> hirechy;
    Canny(src, dstCanny, 15, 30);
    dst = dstCanny;

}


// function for compare descriptor
void Compare(std::vector<float> ho1, std::vector<float> ho2 , double &distance) {
    distance = 0;
    for (unsigned int i = 0; i < ho1.size(); i++) {
        distance += abs(ho1[i] - ho2[i]);
    }
}

// function fo compute HOG descriptor

void HogDes(Mat src,std::vector<float> &descriptor){
    hog.compute(src,descriptor);
    cv::HOGDescriptor hog;
}


void Cmpare2DB(vector<string> DB1files ,String DirBD1 ,vector<string> DB2files , String DirBD2 ,float &matches ){
    std::sort(std::begin(DB1files), std::end(DB1files));
    std::sort(std::begin(DB2files), std::end(DB2files));
    float MatchesNow = 0;
    for (unsigned int i=0; i < DB1files.size();i++) {

        Mat nowIm = imread(String(DirBD1+DB1files[i]));
        Mat dst1,dst2;
        SegementOneResize(nowIm,dst1);
        CannyCont(dst1,dst2);
        vector<float> hog1;
        HogDes(dst2,hog1);
        int min = -1;
        double minDist = 0;
        for (unsigned int j=0; j < DB2files.size();j++) {
            Mat nowIm2 = imread(String(DirBD2+DB2files[j]));
            Mat dst3,dst4;
            double distance;
            SegementOneResize(nowIm2,dst3);
            CannyCont(dst3,dst4);
            vector<float> hog2;
            HogDes(dst4,hog2);
            Compare(hog1,hog2,distance);
            if(min == -1 ){
                min = j;
                minDist = distance;
             }else{
                if(distance < minDist ){
                    min = j;
                    minDist = distance;
                }
             }
        }

        if( min == i ) MatchesNow++;
    }
    matches =(float) MatchesNow / (float) DB2files.size();
    matches *= 100;
}


int main()
{
   // read db1 files
   vector<string> db1 = vector<string>();
   getdir(DB1,db1);
   // read db2 files
   vector<string> db2 = vector<string>();
   getdir(DB6,db2);
   float match;
   // compare the two db 
   Cmpare2DB(db1,DB1,db2,DB6,match);
   cout << "match =>" << match <<"%";
   return 0;
}
