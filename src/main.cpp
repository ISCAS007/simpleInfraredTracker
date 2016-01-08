
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <iostream>
//#include "src/pl/SuBSENSE.h"
#include "src/pl/BackgroundSubtractorSuBSENSE.h"
#include <fstream>
#include <QString>
#include <QCoreApplication>
#include <QFileInfo>
#include <QSettings>
#include <QDir>
#include "src/GNN/KalmanFilter.h"
#include "src/GNN/Tracker.h"
#include "trackstatus.h"

using namespace std;
using namespace cv;
//enum status{LOST,MERGE,SPLIT,EMERGE};
// 4-dimensional state, 2-dimensional measurements
typedef Tracker<4, 2> GNNTracker;
GNNTracker tracker;

typedef GNNTracker::MeasurementSpaceVector Measurement;
typedef GNNTracker::Measurements Measurements;

int minObjectSize;
double speedUp;
int thresh;
TrackList tracklist;
int track(string videopath);
int main(int argc,char *argv[])
{
    /////////////////////////1. get config file path
    string configFilePath="";
    if(argc<2){
        string root="config.ini";
        QFileInfo info(QString::fromStdString(root));

        if(info.exists()){
            configFilePath=root;
        }
        else{
            root="../config.ini";
            info.setFile(QString::fromStdString(root));
            if(info.exists()){
                configFilePath=root;
            }
        }

        if(configFilePath.empty()){
            cout<<"cannot find config.ini"<<endl;
            return 0;
        }
        else{
            cout<<"find config.ini "<<configFilePath<<endl;
        }
    }
    else{
        stringstream ss;
        ss<<argv[1];
        ss>>configFilePath;
        QFileInfo info(QString::fromStdString(configFilePath));
        if(info.exists()&&info.isFile()){
            cout<<"find config file "<<configFilePath<<endl;
        }
        else{
            cout<<"please give the right path of config.ini"<<endl;
        }
    }

    ///////////////////////2. read config
    QSettings *configIniRead = new QSettings(QString::fromStdString(configFilePath), QSettings::IniFormat);
    //将读取到的ini文件保存在QString中，先取值，然后通过toString()函数转换成QString类型
    QString qroot = configIniRead->value("/path/root").toString();
    string root=qroot.toStdString();

    //global variable
    minObjectSize = configIniRead->value("/tracker/minObjectSize").toInt();
    speedUp=configIniRead->value("/tracker/speedUp").toDouble();
    thresh=configIniRead->value("/tracker/threshold").toInt();

    //打印得到的结果
    //    qDebug() << ipResult;
    //    qDebug() << portResult;
    cout<<"read from config file ..."<<endl;
    cout<<"root is "<<qroot.toStdString()<<endl;
    cout<<"minObjectSize is "<<minObjectSize<<endl;
    cout<<"speedUp is "<<speedUp<<endl;
    delete configIniRead;

    ///////////////////////3. read video

    QFileInfo info(QString::fromStdString(root));
    if(info.isFile()){
        track(root);
    }
    else if(info.isDir()){
        QDir *dir=new QDir(QString::fromStdString(root));
        QStringList nameFilter;
        nameFilter<<"*.avi"<<"*.mp4"<<"*.mov";
        QList<QFileInfo> fileinfolist=dir->entryInfoList(nameFilter);
        for(int i=0;i<fileinfolist.size();i++){
            string rootfile=fileinfolist[i].absoluteFilePath().toStdString();
            track(rootfile);
        }
    }
    else{
        cout<<"invalid root "<<root<<endl;
        return -1;
    }

    return 0;
}

int track(string videopath){
    cout<<"start track "<<videopath<<endl;
    QString outpath=QString::fromStdString(videopath);
    int dotpos=outpath.indexOf('.');
    int strlen=outpath.length();
    outpath=outpath.replace(dotpos,strlen-dotpos,".infrared.track");
    cout<<"outpath is "<<outpath.toStdString()<<endl;

    ofstream out(outpath.toStdString());
    VideoCapture cap(videopath);
    if(!cap.isOpened())  // check if we succeeded
        return -1;

    Mat roi,fg,gray,bin;
    namedWindow("video input",CV_WINDOW_NORMAL);
    namedWindow("foreground mask",CV_WINDOW_NORMAL);
    int frameNum=0;

    BackgroundSubtractorSuBSENSE bgs;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    RNG rng(12345);
    for(;;frameNum++)
    {
        Mat frame;
        cap >> frame; // get a new frame from camera
        if(frame.empty()){
            break;
        }
        resize(frame,frame,Size(0,0),1.0/sqrt(speedUp),1.0/sqrt(speedUp));
        //init
        if(frameNum==0){
            fg.create(frame.size(),CV_8UC1);
            roi.create(frame.size(),CV_8UC1);
            roi.setTo(255);
            bgs.initialize(frame,roi);
            bin.create(frame.size(),CV_8UC1);
            cvtColor(frame,gray,CV_BGR2GRAY);
            threshold(gray,bin,thresh,255,THRESH_BINARY);
        }//detect
        else{
//            bgs(frame,fg,double(frameNum<=100));
            cvtColor(frame,gray,CV_BGR2GRAY);
            threshold(gray,bin,thresh,255,THRESH_BINARY);
            cout<<"thresh="<<thresh<<endl;
//            fg=fg|bin;
            fg=bin;

            //            vector<vector<Point> > contours0;
            Mat img=fg.clone();
            Mat kernel=cv::getStructuringElement(cv::MORPH_ELLIPSE,cv::Size(minObjectSize,minObjectSize));
            cv::morphologyEx(img,img,MORPH_OPEN,kernel);
            contours.clear();
            findContours( img, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

            //            contours.resize(contours0.size());
            //            for( size_t k = 0; k < contours0.size(); k++ )
            //                approxPolyDP(Mat(contours0[k]), contours[k], 3, true);

            /// Get the moments
            vector<Moments> mu(contours.size() );
            for( int i = 0; i < contours.size(); i++ )
            { mu[i] = moments( contours[i], false ); }

            ///  Get the mass centers:
            vector<Point2f> mc( contours.size() );
            Measurements m;
            for( int i = 0; i < contours.size(); i++ )
            {
                mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );
//                mc[i][0]=mc[i][0]/img.rows;
//                mc[i][1]=mc[i][1]/img.cols;
                m.emplace_back(Measurement(mc[i].x,mc[i].y));
            }

            /// Draw contours
            Mat drawing = frame;
            for( int i = 0; i< contours.size(); i++ )
            {
                Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
                drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
                circle( drawing, mc[i], 4, color, -1, 8, 0 );
            }

            for ( const auto & x : m )
            {
                const int r = 5;
                // draw measurement
                circle(drawing, Point(x(0), x(1)), 2*r, Scalar(0, 0, 0));
                line(drawing, Point(x(0) - r, x(1) - r), Point(x(0) + r, x(1) + r), Scalar(0, 0, 0) );
                line(drawing, Point(x(0) - r, x(1) + r), Point(x(0) + r, x(1) - r), Scalar(0, 0, 0) );
            }

            tracker.track(m);

            tracklist.update(tracker,out,frameNum);
            for ( const auto & filter : tracker.filters() )
            {

                const GNNTracker::StateSpaceVector s = filter.state();
                const GNNTracker::StateSpaceVector p = filter.prediction();

                // draw filter position
                circle(drawing, Point(s(0), s(1)), 5, Scalar(0, 0, 255));

                // draw filter prediction
                circle(drawing, Point(p(0), p(1)), 5, Scalar(255, 0, 0));

                // draw filter velocity (scaled)
                line(drawing, Point(s(0), s(1)), Point(s(0) + 5 * s(2), s(1) + 5 * s(3)), Scalar(0, 255, 0));

                stringstream id;

                id << filter.id();

                putText(drawing, id.str(), cv::Point2i(s(0) + 10, s(1)), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar(255, 0, 255));

            }

            imshow("video input", drawing);
            imshow("foreground mask",fg);
            imshow("bin",bin);

        }

        if(frameNum<=100){
            cout<<"init "<<videopath<<" frameNum="<<frameNum<<endl;
        }
        else{
            cout<<"track "<<outpath.toStdString()<<" frameNum="<<frameNum<<endl;
        }

        if(waitKey(30) >= 0) break;
    }

    out.close();
    return 0;
}
