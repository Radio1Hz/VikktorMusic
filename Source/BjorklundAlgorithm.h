#include <iostream>
#include <cmath>
#include <stdexcept>
#include <vector>
using namespace std;

class Bjorklund {

public:

    Bjorklund() {};
    Bjorklund(int step, int pulse, int startPhase) : lengthOfSeq(step), pulseAmt(pulse), startPhase(startPhase) {
        if (lengthOfSeq <= 0 || lengthOfSeq < pulseAmt) lengthOfSeq = pulseAmt;
        init();
    };
    ~Bjorklund() {
        remainder.clear();
        count.clear();
        sequence.clear();
    };

    vector<int>remainder;
    vector<int>count;
    vector<bool>sequence; //accessing sequence directly is discouraged. use LoadSequence()

    int lengthOfSeq;
    int pulseAmt;
    int startPhase; // in units

    void init();
    void init(int step, int pulse);
    void iter();
    void buildSeq(int slot);
    void clear();
    void print();
    vector<bool> LoadSequence();
    
    int getSequence(int index) { return sequence.at(index); };
    int size() { return (int)sequence.size(); };

protected:
    vector<bool> LoadSequence(int phase);
};