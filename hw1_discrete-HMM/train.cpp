//
//  train_new.cpp
//  
//
//  Created by B04705036 on 10/29/17.
//
#include "hmm.h"
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
using namespace std;

float VSum(vector<float>& a);
static void init(HMM& hmm);
static void sample(HMM& hmm, string sample);
static void update(HMM& hmm, int total);

int main(int argc, char **argv)
{
    HMM hmm_initial;
    loadHMM(&hmm_initial, argv[2]);
    int state = 6;
    stringstream ss;
    int iter;
    ss << argv[1];
    ss >> iter;
    
    {
        string inStr;
        ifstream seqFile(argv[3]);
        for(int i=0; i<iter; i++)
        {
            int total = 0;
            seqFile.clear();
            seqFile.seekg(0, ios::beg);
            init(hmm_initial);
            while(getline(seqFile, inStr))
            {
                total++;
                sample(hmm_initial, inStr);
            }
            update(hmm_initial,total);
        }
        
        seqFile.close();
    }
    
    //output
    ofstream modelFile(argv[4]);
    modelFile<<"initial: 6"<<"\n";
    for(int i=0; i<state; i++)
    {
        modelFile<<fixed<<setprecision(5)<<setw(10)<<left<<hmm_initial.initial[i]<<' ';
    }
    modelFile<<"\n\n"<<"transition: 6"<<"\n";
    for(int i=0; i<state; i++)
    {
        for(int j=0; j<state; j++)
        {
            modelFile<<fixed<<setprecision(5)<<setw(10)<<left<<hmm_initial.transition[i][j]<<' ';
        }
        modelFile<<"\n";
    }
    modelFile<<"\n"<<"observation: 6"<<"\n";
    for(int k=0; k<6; k++)
    {
        for(int j=0; j<state; j++)
        {
            modelFile<<fixed<<setprecision(5)<<setw(10)<<left<<hmm_initial.observation[k][j]<<' ';
        }
        modelFile<<"\n";
    }
    modelFile<<"\n";
    
    modelFile.close();

    return 0;
}

//sample stage
static double Alpha[MAX_SEQ][MAX_STATE];
static double Beta[MAX_SEQ][MAX_STATE];
static double Gamma[MAX_SEQ][MAX_STATE];
static double Epsilon[MAX_SEQ][MAX_STATE][MAX_STATE];
//accumulate stage
static double pi[MAX_STATE];
static double ANUME[MAX_STATE][MAX_STATE];
static double ADENO[MAX_STATE];
static double gamma_t[MAX_STATE][MAX_OBSERV];

static void sampleInit(HMM& hmm_initial, string sample, int sequence)
{
    int states = hmm_initial.state_num;
    for(int t=0; t<sequence; t++)
    {
        for(int i=0; i<states; i++)
        {
            for(int j=0; j<states; j++)
            {
                Epsilon[t][i][j] = 0;
            }
            Alpha[t][i] = Beta[t][i] = Gamma[t][i] = 0;
        }
    }
}
static void sampleAlpha(HMM& hmm_initial, string sample, int sequence)
{
    int states = hmm_initial.state_num;
    int first = sample[0]-'A';
    for(int i=0; i<states; i++)
    {
        Alpha[0][i] = hmm_initial.initial[i]*hmm_initial.observation[first][i];
    }
    for(int t=1; t<sequence; t++)
    {
        int observ = sample[t]-'A';
        for(int i=0; i<states; i++)
        {
            double sum = 0.0;
            for(int j=0; j<states; j++)
            {
                sum+=Alpha[t-1][j]*hmm_initial.transition[j][i];
            }
            Alpha[t][i]=sum*hmm_initial.observation[observ][i];
        }
    }
    
}
static void sampleBeta(HMM& hmm_initial, string sample, int sequence)
{
    int states = hmm_initial.state_num;
    for(int i=0; i<states; i++)
    {
        Beta[sequence-1][i] = 1;
    }
    for(int t=sequence-2; t>=0; t--)
    {
        int observ = sample[t+1]-'A';
        for(int i=0; i<states; i++)
        {
            double sum = 0.0;
            for(int j=0; j<states; j++)
            {
                sum+=hmm_initial.transition[i][j]*hmm_initial.observation[observ][j]*Beta[t+1][j];
            }
            Beta[t][i] = sum;
        }
    }
}
static void sampleGammaEpsilon(HMM& hmm_initial, string sample, int sequence)
{
    int states = hmm_initial.state_num;
    for(int t=0; t<sequence; t++)
    {
        double sum = 0.0;
        for(int i=0; i<states; i++)
        {
            sum+=Alpha[t][i]*Beta[t][i];
        }
        for(int ii=0; ii<states; ii++)
        {
            Gamma[t][ii] = (Alpha[t][ii]*Beta[t][ii])/sum;
        }
    }
    
    for(int t=0; t<sequence-1; t++)
    {
        int observ = sample[t+1]-'A';
        double sum = 0.0;
        for(int i=0; i<states; i++)
        {
            for(int j=0; j<states; j++)
            {
                Epsilon[t][i][j] = Alpha[t][i]*hmm_initial.transition[i][j]*hmm_initial.observation[observ][j]*Beta[t+1][j];
                sum+=Epsilon[t][i][j];
            }
        }
        for(int i=0; i<states; i++)
        {
            for(int j=0; j<states; j++)
            {
                Epsilon[t][i][j] /=sum;
            }
        }
    }

    for(int i=0; i<states; i++)
    {
        pi[i] += Gamma[0][i];
    }
    
    
    for(int i=0; i<states; i++)
    {
        for(int t=0; t<sequence; t++)
        {
            int observ = sample[t]-'A';
            gamma_t[i][observ]+=Gamma[t][i];
        }
        ADENO[i]+=Gamma[sequence-1][i];
    }
    
    for(int i=0; i<states; i++)
    {
        for(int j=0; j<states; j++)
        {
            double sum = 0.0;
            for(int t=0; t<sequence-1; t++)
            {
                sum+=Epsilon[t][i][j];
            }
            ANUME[i][j] += sum;
        }
    }
}
static void init(HMM& hmm_initial)
{
    int states = hmm_initial.state_num;
    int observations = hmm_initial.observ_num;
    for(int i=0; i<states; i++)
    {
        pi[i] = ADENO[i] = 0;
        for(int j=0; j<states; j++)
        {
            ANUME[i][j] = 0;
        }
        for(int k=0; k<observations; k++)
        {
            gamma_t[i][k] = 0;
        }
    }
}
static void sample(HMM& hmm_initial, string sample)
{
    int sequence = sample.size();
    sampleInit(hmm_initial,sample,sequence);
    sampleAlpha(hmm_initial,sample,sequence);
    sampleBeta(hmm_initial,sample,sequence);
    sampleGammaEpsilon(hmm_initial,sample,sequence);
}
static void update(HMM& hmm_initial, int total)
{
    int states = hmm_initial.state_num;
    int observations = hmm_initial.observ_num;
    for(int i=0; i<states; i++)
    {
        hmm_initial.initial[i] = pi[i]/total;
    }
    //print
    for(int i=0; i<states; i++)
    {
        cout<<hmm_initial.initial[i]<<' ';
    }
    cout<<endl;
    
    for(int i=0; i<states; i++)
    {
        double sum = 0.0;
        for(int k=0; k<observations; k++)
        {
            sum+=gamma_t[i][k];
        }
        sum-=ADENO[i];
        for(int j=0; j<states; j++)
        {
            hmm_initial.transition[i][j] = ANUME[i][j]/sum;
        }
    }

    for(int i=0; i<states; i++)
    {
        double sum = 0.0;
        for(int k=0; k<observations; k++)
        {
            sum+=gamma_t[i][k];
        }
        for(int j=0; j<observations; j++)
        {
            hmm_initial.observation[j][i] = gamma_t[i][j] / sum;
        }
    }
    
}
