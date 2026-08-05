class Solution {
public:
    int binaryToDecimal(const std::string& binaryStr) {
    int decimalNum = 0;
    for (char bit : binaryStr) {
        if (bit=='0')  {bit='1';}
        else {bit='0';}
        decimalNum = (decimalNum << 1) + (bit - '0');
    }
    return decimalNum;
    }
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

    int findComplement(int num) {
        std::string a=(decimalToBinary(num));
        int res=binaryToDecimal(a);
        return res;
        }
};
