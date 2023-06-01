class Solution {
private:
    string digitCase(int n, char one, char five, char ten) {
        switch(n) {
        case 0: return {};
        case 1: return {one};
        case 2: return {one, one};
        case 3: return {one, one, one};
        case 4: return {one, five};
        case 5: return {five};
        case 6: return {five, one};
        case 7: return {five, one, one};
        case 8: return {five, one, one, one};
        case 9: return {one, ten};
        }
        return {}; // Just in case™
    }

public:
    string intToRoman(int num) {
        return string(num/1000, 'M')
             + digitCase(num/100 % 10, 'C', 'D', 'M')
             + digitCase(num/10  % 10, 'X', 'L', 'C')
             + digitCase(num/1   % 10, 'I', 'V', 'X');
    }
};