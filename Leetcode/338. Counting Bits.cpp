class Solution {
public:
    std::string decimalToBinary(int decimalNum) {
    if (decimalNum == 0) return "0";
    std::string binaryStr = "";
    while (decimalNum > 0) {
        char bit = (decimalNum % 2) + '0';
        binaryStr = bit + binaryStr;
        decimalNum /= 2;
    }
    return binaryStr;
    }
    int count(std::string a) {
        int co=0;
        int x= a.length();
        for (int i=0;i<x;i++) {
            if (a[i]=='1') {co+=1;}
        }
        return co;
    }
    vector<int> countBits(int n) {
        std::vector<int> res(n+1);
        for (int i=0;i<=n;i++) {
            std::string a=decimalToBinary(i);
            int x=count(a);
            res[i] = x;
        }
        return res;
    }
};
