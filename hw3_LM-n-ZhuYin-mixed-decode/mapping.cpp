#include<map>
#include<set>
#include<vector>
#include<string>
#include<fstream>
#include<iostream>
#include<iterator>
#include<algorithm>

using namespace std;

int main(int argc, char** argv)
{
    ifstream in(argv[1]);   //read in Big5-ZhuYin.map
    ofstream op(argv[2]);   //output ZhuYin-Big5.map
    string inputStr;
    set<string> ZY;
    multimap<string, string> MAP;
    
    while(getline(in, inputStr))
    {
        vector<string> token;
        string del = "/";
        inputStr.erase(remove(inputStr.begin(), inputStr.end(), ' '), inputStr.end());
        string word = inputStr.substr(0,2);
        inputStr.erase(0,2);
        size_t pos = 0;
        while((pos=inputStr.find(del))!= string::npos)
        {
            token.push_back(inputStr.substr(0, pos));
            inputStr.erase(0, pos+del.length());
        }
        token.push_back(inputStr);
        
        for(int i=0; i<token.size(); i++)
        {
            string tmp = token[0].substr(0,2);
            if(i>0 && (token[i].substr(0,2)==tmp))
                continue;
            ZY.insert(token[i].substr(0,2));
            MAP.insert(make_pair(token[i].substr(0,2), word));
        }
        token.clear();
    }
    
    in.close();
    typedef multimap<string, string>::iterator MAPIterator;
    set<string>::iterator Sit2;
    for(Sit2 = ZY.begin(); Sit2!=ZY.end(); ++Sit2)
    {
        string root = *Sit2;
        vector<string> element;
        op<<root<<' ';
        pair<MAPIterator, MAPIterator> result = MAP.equal_range(root);
        for(MAPIterator Mit = result.first; Mit!=result.second; Mit++)
        {
            op<<Mit->second<<' ';
            element.push_back(Mit->second);
        }
        op<<endl;
        for(int i=0; i<element.size(); i++)
        {
            op<<element[i]<<' '<<element[i]<<endl;
        }
    }
    op.close();
    
}
