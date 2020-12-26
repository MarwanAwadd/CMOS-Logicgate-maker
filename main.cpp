#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <algorithm>
#include <vector>
#include <ctype.h>
using namespace std;

struct CMOS {
    string name = "M";
    string type;
    int source;
    int drain;
    int gate;
};
const int Ground = 0;
const int VDD = -1;
const int Output = -2;
int counter = 1;
string Vdd;

void SetPUNandPUDStrings (string function, string &PUN, string &PDN);
bool Validate(string s);
void NOT_Inputs(vector<CMOS> &T, char input, int& node);
void Setup (string function);
void InitInputs (string expression, vector<CMOS> &T, int &node);
void PUNGen(string PUN, vector<CMOS> &T, int &node);
void PDNGen(string PDN, vector<CMOS> &T, int &node);
void printCMOS(vector<CMOS> T);

int main() {
    string exp;
    vector<string> exps;

    cout << "Please enter your boolean expression: (|, &, !)" << endl;
    cout << "**Note that if you would like to enter multiple expressions, separate them by \";\"**" << endl;

    cin >> exp;
    exp = exp + ";";
    while(exp.length() > 0){
        int p = exp.find(';');
        exps.push_back(exp.substr(0, p));
        exp = exp.substr(p+1, exp.length());
    }
    
    for(int i = 0; i < exps.size(); i++)
        Setup(exps[i]);

    return 0;
}

bool Validate(string s) {
    bool flag = true;
    std::size_t i = 0;
    for (i = 0; i < s.size(); i++)
    {
        if ((int)(s[i]) < 65 || ((int)(s[i]) > 90 && (int)(s[i]) < 97) || (int)(s[i]) > 122) {
            if (s[i] != '=' && s[i] != '!' && s[i] != '&' && s[i] != '|' && s[i] != '(' && s[i] != ')') {
                flag = false;
                break;
            }
        }
        if (i != 0 && s[i] == s[0]) {
            flag = false;
            break;
        }
        if (s[i] == ' ') {
            flag = false;
            break;
        }
    }
    return flag;
}

void SetPUNandPUDStrings (string function, string &PUN, string &PDN){
    string s = function;
    cout << endl;
    for(int i = 0; i < s.size(); i++){
        if (isalpha(s[i])){
            if(s[i+1] != '!')
                s.insert(i + 1, "!");
            else
                s.erase(s.begin()+i+1);
        }
    }
    PUN = s;

    string t = function;
    for(int i = 0; i < t.size(); i++){
        if (isalpha(t[i])){
            if(t[i+1] != '!')
                t.insert(i + 1, "!");
            else
                t.erase(t.begin()+i+1);
        }
        else if (t[i] == '&'){
            t[i] = '|';
        }
        else if (t[i] == '|')
            t[i] = '&';
    }
    PDN = t;

    //cout << PUN << endl << PDN << endl;
}

void NOT_Inputs(vector<CMOS> &T, char input, int& node) {
    CMOS N1, P1;

    N1.type = "NMOS";
    P1.type = "PMOS";

    P1.gate = input;
    N1.gate = input;

    node++;
    P1.drain = node;
    N1.drain = node;

    P1.source = VDD;
    N1.source = Ground;

    T.push_back(N1);
    T.push_back(P1);
}

void InitInputs (string expression, vector<CMOS> &T, int &node){
    string s = expression;
    for(int i = 0; i < s.size(); i++){
        if (isalpha(s[i])){
            if(s[i+1] == '!')
                NOT_Inputs(T, s[i], node);
        }
    }
}

void Setup (string function){
    bool v = Validate(function);
    if(v){
        vector<CMOS> T;
        cout << function << endl;
        int node = 0;
        string PUN, PDN;
        SetPUNandPUDStrings(function, PUN, PDN);
        InitInputs(PUN, T, node);
        PUNGen(PUN, T, node);
        PDNGen(PDN, T, node);
        printCMOS(T);
    }
    else
        cout << "Error" << endl;
}

void PUNGen(string PUN, vector<CMOS> &T, int &node){
    vector<CMOS> PUNT;
    int inputCount = 0;
    for(int i = 0; i < PUN.length(); i++){
        if(isalpha(PUN[i])){
            CMOS P;
            P.type = "PMOS";
            if(PUN[i+1] == '!')
                P.gate = T[inputCount * 2].drain;
            else
                P.gate = PUN[i];
            if((PUN[i - 1] != '&') && ((inputCount == 0) || (((PUN[i + 1] == '|') || (PUN[i + 1] == '!' && PUN[i + 2] == '|'))) || (PUN[i - 1] == '|'))){
                P.source = VDD;
                if(PUN[i + 1] == '&' || (PUN[i + 1] == '!' && PUN[i + 2] == '&')){
                    node++;
                    P.drain = node;
                }
                else
                    P.drain = Output;
            }
            if(PUN[i - 1] == '&'){
                P.source = PUNT[inputCount - 1].drain;
                if(PUN[i + 1] == '&' || (PUN[i + 1] == '!' && PUN[i + 1] == '&')){
                    node++;
                    P.drain = node;
                }
                else
                    P.drain = Output;
            }
            PUNT.push_back(P);
            inputCount++;
        }
    }

    for(int i = 0; i < PUNT.size(); i++){
        T.push_back(PUNT[i]);
    }
}


void PDNGen(string PDN, vector<CMOS> &T, int &node){
    vector<CMOS> PDNT;
    int inputCount = 0;
    for(int i = 0; i < PDN.length(); i++){
        if(isalpha(PDN[i])){
            CMOS P;
            P.type = "NMOS";
            if(PDN[i+1] == '!')
                P.gate = T[inputCount * 2].drain;
            else
                P.gate = PDN[i];
            if((PDN[i - 1] != '&') && ((inputCount == 0) || (((PDN[i + 1] == '|') || (PDN[i + 1] == '!' && PDN[i + 2] == '|'))) || (PDN[i - 1] == '|'))){
                P.source = Ground;
                if(PDN[i + 1] == '&' || (PDN[i + 1] == '!' && PDN[i + 2] == '&')){
                    node++;
                    P.drain = node;
                }
                else
                    P.drain = Output;
            }
            if(PDN[i - 1] == '&'){
                P.source = PDNT[inputCount - 1].drain;
                if(PDN[i + 1] == '&' || (PDN[i + 1] == '!' && PDN[i + 1] == '&')){
                    node++;
                    P.drain = node;
                }
                else
                    P.drain = Output;
            }
            PDNT.push_back(P);
            inputCount++;
        }
    }
}

void printCMOS(vector<CMOS> T){
    for(int i = 0; i < T.size(); i++){
        cout << T[i].name << i + 1 << "     ";

        if(T[i].drain == Output)
            cout << "Output" << "     ";
        else
            cout << T[i].drain << "     ";

        if(T[i].gate >= 65 && T[i].gate <= 90)
            cout << static_cast<char>(T[i].gate) << "     ";
        else
            cout << T[i].gate << "     ";

        if(T[i].source == VDD)
            cout << "VDD" << "     " << "VDD" << "     ";
        else if(T[i].source == Ground)
            cout << "Ground" << "     " << "Ground" << "     ";
        else
            cout << T[i].source << "     " << T[i].source << "     ";

        cout << T[i].type << endl << endl;
    }
}

