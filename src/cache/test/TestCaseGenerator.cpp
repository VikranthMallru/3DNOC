#include "TestCaseGenerator.hpp"
int N = 32768; // Total number of requests generated from all cores.
int reqcount[8];
string dest[8];
string src[8];
string offset = "000000";

int convert(int dec) {
    if (dec == 0)
        return 0;
    else
        return (dec % 2) + 10 * convert(dec / 2);
}

void computedest() {
    for (int i = 0; i < 8; i++) {
        string temp = to_string(convert(i));
        dest[i] = "";
        for (int k = 0; k < 4 - temp.length(); k++)
            dest[i] += "0";
        dest[i] += (temp + offset);
    }
}


void c_TestCaseGenerator::f_testCaseGenerator(int c, int s[]) {
    computedest();
    for (int i = 0; i < c; ++i) {
        src[i] = dest[s[i]];
    }
    std::ofstream file("src/output/testcase.txt");
    if (!file) {
        throw std::invalid_argument("Configuration file not found.");
    }
    srand(time(0));
    int cc = 1;
    for (int i = 0; i < 8; i++)
        reqcount[i] = N / 8;
    int srcreqcnt[c];
    for (int z = 0; z < c; z++)
        srcreqcnt[z] = N / c;
    // To ensure equal amount of requests are being generated.
    while (N > 0) {
        // At clock cycle = cc , r requests are being generated.
        int r = (N > 10) ? rand() % 10 : N; // Assuming no more than 10 requests per clock cycle being generated.

        // count of clock cycle
        file << cc << " " << r << endl;
        for (int i = 0; i < r; i++) {
            int d;
            while (d = rand() % 8) {
                if (reqcount[d] > 0)
                    break;
            }
            int h;
            while (h = rand() % c) {
                if (srcreqcnt[h] > 0)
                    break;
            }
            file << src[h] << " " << dest[d] << endl;
            reqcount[d]--;
            srcreqcnt[h]--;
            N--;
        }
        cc++;
    }
    file << "-1 -1\n";
    file.close();
}