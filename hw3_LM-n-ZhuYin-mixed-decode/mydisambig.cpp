#include<map>
#include<set>
#include<cmath>
#include<vector>
#include<string>
#include<fstream>
#include<stdio.h>
#include<algorithm>
#include<iostream>
#include "Ngram.h"

using namespace std;

void print(ofstream&, vector<string>);
void setMapping(ifstream&, map<string, set<string> >&);
vector<string> processInline(string);
vector<set<string> > seqChoice(vector<string>, map<string, set<string> >&);
vector<string> Viterbi(Vocab&, Ngram&, vector<set<string> >);
//helper
pair<vector<string>, double> useBigram(Vocab&, Ngram&, string, vector<pair<vector<string>, double> >);
double getBigramProb(Vocab&, Ngram&, const char, const char);

int main(int argc, char** argv)
{
    ifstream testData(argv[2]); //testData
    ifstream MAP(argv[4]);
    ofstream output(argv[7]);
    
        int ngram_order = 2;
        Vocab voc;
        Ngram lm(voc, ngram_order);
        File lmFile(argv[6], "r");
        lm.read(lmFile);
        lmFile.close();
    
    map<string, set<string> > mapping;
    setMapping(MAP, mapping);
    
    string inputStr;
    while(getline(testData, inputStr))
    {
        vector<string> input = processInline(inputStr);
        vector<set<string> > seq = seqChoice(input, mapping);
        vector<string> outline = Viterbi(voc, lm, seq);
        print(output, outline);
    }
    
    MAP.close();
    testData.close();
    output.close();
}

void print(ofstream& out, vector<string> outline)
{
    out<<"<s> ";
    for(int i=0; i<outline.size(); i++)
        out<<outline[i]<<' ';
    out<<"</s>"<<endl;
}

// Get P(W2 | W1) -- bigram
double getBigramProb(Vocab& voc, Ngram& lm, const char *w1, const char *w2)
{
    VocabIndex wid1 = voc.getIndex(w1);
    VocabIndex wid2 = voc.getIndex(w2);
    
    if(wid1 == Vocab_None)  //OOV
        wid1 = voc.getIndex(Vocab_Unknown);
    if(wid2 == Vocab_None)  //OOV
        wid2 = voc.getIndex(Vocab_Unknown);
    
    VocabIndex context[] = { wid1, Vocab_None };
    return lm.wordProb( wid2, context);
}

// Get P(W3 | W1, W2) -- trigram
double getTrigramProb(Vocab& voc, Ngram& lm, const char *w1, const char *w2, const char *w3)
{
    VocabIndex wid1 = voc.getIndex(w1);
    VocabIndex wid2 = voc.getIndex(w2);
    VocabIndex wid3 = voc.getIndex(w3);
    
    if(wid1 == Vocab_None)  //OOV
        wid1 = voc.getIndex(Vocab_Unknown);
    if(wid2 == Vocab_None)  //OOV
        wid2 = voc.getIndex(Vocab_Unknown);
    if(wid3 == Vocab_None)  //OOV
        wid3 = voc.getIndex(Vocab_Unknown);
    
    VocabIndex context[] = { wid2, wid1, Vocab_None };
    return lm.wordProb( wid3, context );
}

void setMapping(ifstream& mapFile, map<string, set<string> >& mapping)
{
    string inputStr;
    while(getline(mapFile, inputStr))
    {
        string key = inputStr.substr(0,2);
        set<string> possibleW;
        inputStr.erase(0,3);
        size_t pos = 0;
        string del = " ";
        while((pos=inputStr.find(del))!= string::npos)
        {
            possibleW.insert(inputStr.substr(0, pos));
            inputStr.erase(0, pos+del.length());
        }
        possibleW.insert(inputStr);
        mapping[key] = possibleW;
    }
    /*
    map<string, set<string> >::iterator Mit;
    set<string>::iterator Sit;
    for(Mit = mapping.begin(); Mit!=mapping.end(); Mit++)
    {
        out<<"key:"<<Mit->first<<' ';
        for(Sit = Mit->second.begin(); Sit!=Mit->second.end(); Sit++)
            out<<"value:"<<*Sit<<' '<<(*Sit).size()<<' ';
        cout<<endl;
    }
     */
    
}

vector<string> processInline(string input)
{
    vector<string> result;
    input.erase(remove(input.begin(), input.end(), ' '), input.end());
    assert(input.size()%2 == 0);
    while(input.size()>0)
    {
        string element = input.substr(0,2);
        result.push_back(element);
        input.erase(0,2);
    }
    return result;
}

vector<set<string> > seqChoice(vector<string> input, map<string, set<string> >& mapping)
{
    vector<set<string> > seqChoice;
    map<string, set<string> >::iterator Mit;
    for(int i=0; i<input.size(); i++)
    {
        set<string> currChoice;
        Mit = mapping.find(input[i]);
        if(Mit!=mapping.end())
            currChoice = Mit->second;
        else
            currChoice.insert(input[i]);
        seqChoice.push_back(currChoice);
    }
    return seqChoice;
}

vector<string> Viterbi(Vocab& voc, Ngram& lm, vector<set<string> > seqchoice )
{
    vector<pair<vector<string>, double> > p;
    set<string>::iterator Sit = seqchoice.begin()->begin();
    double initProb = 0.0;
    for(Sit; Sit!=seqchoice.begin()->end(); ++Sit)
    {
        vector<string> possibleW;
        possibleW.push_back(*Sit);
        p.push_back(make_pair(possibleW, initProb));
    }
    for(int i=1; i<seqchoice.size(); i++)
    {
        vector<pair<vector<string>, double> > np;
        set<string> choiceSet = seqchoice[i];
        for(Sit = choiceSet.begin(); Sit!=choiceSet.end(); ++Sit)
        {
            string currW = *Sit;
            pair<vector<string>, double> optimal = useBigram(voc, lm, currW, p);
            np.push_back(optimal);
        }
        p.clear();
        p = np;
    }
    
    double Prob = double(-INT_MAX);
    int index = 0;
    for(int i=0; i<p.size(); i++)
    {
        if(p[i].second > Prob)
        {
            index = i;
            Prob = p[i].second;
        }
    }
    return p[index].first;
}

pair<vector<string>, double> useBigram(Vocab& voc, Ngram& lm, string currW, vector<pair<vector<string>, double> > p)
{
    vector<string>::iterator Wit;
    double maxProb = double(-INT_MAX);
    double prob;
    int index = 0;
    for(int i=0; i<p.size(); ++i)
    {
        Wit = p[i].first.end();
        string prev = *(Wit-1);
        double prevProb = p[i].second;
        double newProb = getBigramProb(voc, lm, prev.c_str(), currW.c_str());
        prob = prevProb+newProb;
        if(prob > maxProb)
        {
            index = i;
            maxProb = prob;
        }
    }
    pair<vector<string>, double> finp= p[index];
    finp.first.push_back(currW);
    finp.second = maxProb;
    return finp;
}


