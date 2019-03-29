
#include "stats.h"




stats::stats(PNG & im){

/* your code here */
    initSumHueX(im);
    initSumHueY(im);
    initSumSat(im);
    initSumLum(im);
    initHist(im);

}

void stats::initSumHueX(PNG & im){
    //temp table contains individual HueX value of each Pixels
    vector< vector<double>> oneHueXTable;
    int cc=0;

    //put HueX into each element of oneHueXTable column by column
    for(unsigned int i=0; i<im.width();i++){
        vector<double> column;
        for(unsigned int j=0; j<im.height();j++){
            column.push_back(cos(im.getPixel(i,j)->h*PI/180));
        }
        oneHueXTable.push_back(column);
    }
    //transfer from HueX table to sumHueX table
    for(unsigned int i=0;i<im.width();i++){
        //cc++;
        //cout<<cc<<endl;
        //cout<<"im.width= "<<im.width()<<endl;
        vector<double> column;

        for(unsigned int j=0; j<im.height();j++){

            double element=0;
            for(unsigned int x=0;x<=i;x++){
                for(unsigned int y=0; y<=j;y++){
                    element+=oneHueXTable[x][y];
                }
            }
            column.push_back(element);
        }

        sumHueX.push_back(column);     

    }


}

void stats::initSumHueY(PNG & im){
    int cc=0;
    //temp table contains individual HueY value of each Pixels
    vector< vector<double>> oneHueYTable;

    //put HueY into each element of oneHueYTable column by column
    for(unsigned int i=0; i<im.width();i++){
        vector<double> column;
        for(unsigned int j=0; j<im.height();j++){
            column.push_back(sin(im.getPixel(i,j)->h*PI/180));
        }
        oneHueYTable.push_back(column);
    }

    //transfer from HueY table to sumHueY table
    for(unsigned int i=0;i<im.width();i++){
        //cc++;
        //cout<<cc<<endl;
        //cout<<"Y-im.width= "<<im.width()<<endl;

        vector<double> column;

        for(unsigned int j=0; j<im.height();j++){

            double element=0;
            for(unsigned int x=0;x<=i;x++){
                for(unsigned int y=0; y<=j;y++){
                    element+=oneHueYTable[x][y];
                }
            }
            column.push_back(element);
        }

        sumHueY.push_back(column);     

    }


}

void stats::initSumSat(PNG & im){
    //cout<<"initSumSat"<<endl;
    //temp table contains individual Saturation value of each Pixels
    vector< vector<double>> oneSatTable;

    //put Sat into each element of oneSatTable column by column
    for(unsigned int i=0; i<im.width();i++){
        vector<double> column;
        for(unsigned int j=0; j<im.height();j++){
            column.push_back(im.getPixel(i,j)->s);
        }
        oneSatTable.push_back(column);
    }

    //transfer from Sat table to sumSat table
    for(unsigned int i=0;i<im.width();i++){

        vector<double> column;

        for(unsigned int j=0; j<im.height();j++){

            double element=0;
            for(unsigned int x=0;x<=i;x++){
                for(unsigned int y=0; y<=j;y++){
                    element+=oneSatTable[x][y];
                }
            }
            column.push_back(element);
        }

        sumSat.push_back(column);     

    }


}

void stats::initSumLum(PNG & im){
    //cout<<"initSumLum"<<endl;
    //temp table contains individual luminance value of each Pixels
    vector< vector<double>> oneLumTable;

    //put Lum into each element of oneLumTable column by column
    for(unsigned int i=0; i<im.width();i++){
        vector<double> column;
        for(unsigned int j=0; j<im.height();j++){
            column.push_back(im.getPixel(i,j)->l);
        }
        oneLumTable.push_back(column);
    }

    //transfer from Sat table to sumSat table
    for(unsigned int i=0;i<im.width();i++){

        vector<double> column;

        for(unsigned int j=0; j<im.height();j++){

            double element=0;
            for(unsigned int x=0;x<=i;x++){
                for(unsigned int y=0; y<=j;y++){
                    element+=oneLumTable[x][y];
                }
            }
            column.push_back(element);
        }

        sumLum.push_back(column);     

    }


}

void stats::initHist(PNG & im){
    //cout<<"initHist"<<endl;
    for(unsigned int i=0;i<im.width();i++){
        vector<vector<int>> col;
        for(unsigned int j=0; j<im.height();j++){
            //one pixel's histgram
            vector<int> bins(36,0);
            for(unsigned int x=0; x<=i;x++){
                for(unsigned int y=0;y<=j;y++){
                    int hue= im.getPixel(x,y)->h;
                    int index=hue/10;
                    bins[index]++;
                }
            }
            col.push_back(bins);
        }
        hist.push_back(col);
    }
}

long stats::rectArea(pair<int,int> ul, pair<int,int> lr){

    /* your code here */
    return (lr.first-ul.first+1)*(lr.second-ul.second+1);

}

HSLAPixel stats::getAvg(pair<int,int> ul, pair<int,int> lr){
    /* your code here */
    double sat=getAvgSat(ul,lr);
    double lum=getAvgLum(ul,lr);
    double hueX=getAvgHueX(ul,lr);
    double hueY=getAvgHueY(ul,lr);

    double hue= atan2(hueY, hueX)*180/PI;
    hue= (hue>=0)? hue: (hue+360);

    return HSLAPixel(hue, sat, lum, 1.0);

}

double stats::getAvgSat(pair<int,int> ul, pair<int,int> lr){
    double big= sumSat[lr.first][lr.second];
    double top= (ul.second==0)?0:(sumSat[lr.first][ul.second-1]);
    double left= (ul.first==0)?0:(sumSat[ul.first-1][lr.second]);
    double add= (ul.first==0||ul.second==0)?0:(sumSat[ul.first-1][ul.second-1]);
    double sum= big-top-left+add;
    int dim= (lr.first-ul.first+1)*(lr.second-ul.second+1);

    return (double)sum/dim;
}

double stats::getAvgLum(pair<int,int> ul, pair<int,int> lr){
    double big= sumLum[lr.first][lr.second];
    double top= (ul.second==0)?0:(sumLum[lr.first][ul.second-1]);
    double left= (ul.first==0)?0:(sumLum[ul.first-1][lr.second]);
    double add= (ul.first==0||ul.second==0)?0:(sumLum[ul.first-1][ul.second-1]);
    double sum= big-top-left+add;
    int dim= (lr.first-ul.first+1)*(lr.second-ul.second+1);

    return (double)sum/dim;
}

double stats::getAvgHueX(pair<int,int> ul, pair<int,int> lr){
    double big= sumHueX[lr.first][lr.second];
    double top= (ul.second==0)?0:(sumHueX[lr.first][ul.second-1]);
    double left= (ul.first==0)?0:(sumHueX[ul.first-1][lr.second]);
    double add= (ul.first==0||ul.second==0)?0:(sumHueX[ul.first-1][ul.second-1]);
    double sum= big-top-left+add;
    int dim= (lr.first-ul.first+1)*(lr.second-ul.second+1);

    return (double)sum/dim;
}

double stats::getAvgHueY(pair<int,int> ul, pair<int,int> lr){
    double big= sumHueY[lr.first][lr.second];
    double top= (ul.second==0)?0:(sumHueY[lr.first][ul.second-1]);
    double left= (ul.first==0)?0:(sumHueY[ul.first-1][lr.second]);
    double add= (ul.first==0||ul.second==0)?0:(sumHueY[ul.first-1][ul.second-1]);
    double sum= big-top-left+add;
    int dim= (lr.first-ul.first+1)*(lr.second-ul.second+1);

    return (double)sum/dim;
}

vector<int> stats::buildHist(pair<int,int> ul, pair<int,int> lr){

    /* your code here */
    vector<int> big= hist[lr.first][lr.second];
    //big-top
    if(ul.second!=0){
        for (int i = 0; i< 36; i++){
            big[i] -= hist[lr.first][ul.second-1][i];
        }
    }
    //big-left
    if(ul.first!=0){
        for (int i = 0; i< 36; i++){
            big[i] -= hist[ul.first-1][lr.second][i];
        }
    }
    //big+add
    if((ul.first!=0)&&(ul.second!=0)){
        for (int i = 0; i< 36; i++){
            big[i] += hist[ul.first-1][ul.second-1][i];
        }
    }

    return big;

}

// takes a distribution and returns entropy
// partially implemented so as to avoid rounding issues.
double stats::entropy(vector<int> & distn,int area){

    double entropy = 0.;

/* your code here */

    for (int i = 0; i < 36; i++) {
        if (distn[i] > 0 ) 
            entropy += ((double) distn[i]/(double) area) 
                                    * log2((double) distn[i]/(double) area);
    }

    return  -1 * entropy;

}

double stats::entropy(pair<int,int> ul, pair<int,int> lr){

    /* your code here */
    vector<int> distn=buildHist(ul,lr);

    return entropy(distn,rectArea(ul,lr));

}
