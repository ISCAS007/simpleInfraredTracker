#ifndef TRACKSTATUS
#define TRACKSTATUS
#include <stdio.h>
#include <iostream>
#include <fstream>
//#include <vector>
#include <opencv2/opencv.hpp>
#include "GNN/Tracker.h"
#include "GNN/KalmanFilter.h"
using namespace std;
using namespace cv;

typedef Tracker<4, 2> GNNTracker;
class TrackStatus{
    public:
int id;
int lostCount;
int lifetime;
std::vector<int> frameNum;
std::vector<Point> path;
TrackStatus(){
    lostCount=0;
    lifetime=1;
    id=-1;
}
};

class TrackList{
public:
    vector<TrackStatus> list;
    void update(GNNTracker &tracker,ofstream &out,int frameNum){
        for(int i=0;i<list.size();i++){
            list[i].lostCount+=1;
        }

        for ( const auto & filter : tracker.filters() )
        {
            const GNNTracker::StateSpaceVector s = filter.state();
            int id=filter.id();
            Point p(s(0),s(1));

            bool matched=false;
            for(int j=0;j<list.size();j++){
                if(list[j].id==id){
                    matched=true;
                    list[j].lifetime+=1;
                    list[j].lostCount-=1;
                    list[j].frameNum.push_back(frameNum);
                    list[j].path.push_back(p);
                    break;
                }
            }

            if(!matched){
                TrackStatus ts;
                ts.id=id;
                ts.frameNum.push_back(frameNum);
                ts.path.push_back(p);
                list.push_back(ts);
            }
        }

        vector<TrackStatus>::iterator iter;
        iter=list.begin();
        for(int i=0;i<list.size();i++){
            if(list[i].lostCount>3){
                if(list[i].lifetime<3){
                    continue;
                }
//                ofstream out("out.txt");
                out<<"id= "<<list[i].id;
                out<<" path=";
                for(int j=0;j<list[i].path.size();j++){
                    out<<" "<<list[i].path[j]<<" frameNum="<<list[i].frameNum[j];
                }
                out<<endl;
//                out.close();
                list.erase(iter+i);
            }
        }
    }
};

#endif // TRACKSTATUS

