//
//  test.cpp
//  
//
//  Created by B04705036 on 10/29/17.
//
#include "hmm.h"
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#define MAX_MNUM 10
static void viterbi(HMM* hmms, int total, string inStr, int& index);

int main(int argc, char **argv)
{
    if(argc!=4)
    {
        cout<<"Please follow the correct input form."<<endl;
        return 1;
    }
    
    ifstream inFile(argv[2]);
    ofstream oFile(argv[3]);
    string inStr;
    
    HMM hmms[MAX_MNUM];
    int total = load_models(argv[1], hmms, MAX_MNUM);
    
    while(getline(inFile,inStr))
    {
        int index;
        viterbi(hmms, total, inStr, index);
        oFile<<hmms[index].model_name<<"\n";
    }
    
    inFile.close();
    oFile.close();
    
    return 0;
}

static double Delta[2][MAX_STATE];
static void viterbi(HMM* hmms, int total, string inStr, int& index)
{
    int sequence = inStr.size();
    double mProb = 0.0;
    for(int mcnt=0; mcnt<total; mcnt++)
    {
        HMM* hmm = (hmms+mcnt);
        int states = hmm->state_num;
        
        int first = inStr[0]-'A';
        for(int i=0; i<states; i++)
        {
            Delta[0][i] = hmm->initial[i]*hmm->observation[first][i];
        }
        for(int t=1; t<sequence; t++)
        {
            int observ = inStr[t]-'A';
            for(int i=0; i<states; i++)
            {
                double maxRouteProb = 0.0;
                for(int j=0; j<states; j++)
                {
                    double comp = Delta[0][j]*hmm->transition[j][i];
                    if(comp > maxRouteProb)
                    {
                        maxRouteProb = comp;
                    }
                }
                Delta[1][i] = maxRouteProb*hmm->observation[observ][i];
            }
            for(int i=0; i<states; i++)
            {
                Delta[0][i] = Delta[1][i];
            }
        }
        double maxRouteProb = 0.0;
        for(int i=0; i<states; i++)
        {
            if(maxRouteProb < Delta[0][i])
            {
                maxRouteProb = Delta[0][i];
            }
        }
        
        if(mProb < maxRouteProb)
        {
            mProb = maxRouteProb;
            index = mcnt;
        }
    }
}
