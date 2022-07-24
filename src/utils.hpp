#ifndef MG_UTILS
#define MG_UTILS

#include <string>
#include <cmath>

namespace mg {
  inline double sign(double v) {
    if (v >= 0) return 1;
    else return -1;
  }


  inline std::string fastDTOS(double v) {
    if(v == 0) return "0";
    std::string res = "";
    bool sign = false;
    if(v < 0) {
      sign = true;
      v = -v;
    }
    if(v > 0.99) {
      int integerPart = (int)round(v*10);
      char t = (char)('0' + (integerPart % 10));
      integerPart /= 10;
      res += std::to_string(integerPart);
      if(t > '0') {
        res += '.';
        res += t;
      }
    } else {
      int i;
      for (i = -2; i < 24 && v < 9.99; i++) {
        v *= 10;
      }
      v = round(v);
      if(i <= 5) {
        res = "0.";
        for (int j = 0; j < i; j++) {
          res += '0';
        }
        char t = (char)('0' + ((int)v % 10));
        v = (int)v / 10;
        res += (char)('0' + ((int)v % 10));
        if(t > '0') res += t;
      } else {
        char t = (char)('0' + ((int)v % 10));
        v = (int)v / 10;
        res += (char)('0' + ((int)v % 10));
        if(t > '0'){
          res += '.';
          res += t;
        }
        res += 'e';
        res += '-';
        res += std::to_string(i + 1);
      }
    }
    if(sign) return '-' + res;
    else return res;
  }

  inline std::string doubleToString(double v) {
    if(v == 0) return "0";
    std::string res = "";
    bool sign = false;
    if(v < 0) {
      sign = true;
      v = -v;
    }
    unsigned long long integerPart = (unsigned long long)v;
    v = v - integerPart;
    if(integerPart == 0) {
      if(v > 1e-9) {
        res = "0.";
        int ns = 0;
        int ip;
        for(int i = 0; i < 10; i++) {
          v *= 10;
          ip = (int)v;
          v = v - ip;
          if(ip) {
            for (int j = 0; j < ns; j++) {
              res += '0';
            }
            ns = 0;
            res += (char)('0' + ip);
          } else {
            ns++;
          }
        }
      } else {
        int i;
        for(i = 0; i < 24 && v < 1; i++) {
          v *= 10;
        }
        res += (char)('0' + (int)v);
        res += '.';
        for(int j = 0; j < 6; j++)  {
          v -= (int)v;
          v *= 10;
          res += (char)('0' + (int)v);
        }
        res += 'e';
        res += '-';
        res += std::to_string(i);
      }
    } else {
      while(integerPart) {
        res = (char)('0' + (integerPart % 10)) + res;
        integerPart /= 10;
      }
      if(v > 1e-9) {
        res += '.';
        int ns = 0;
        int ip;
        for(int i = 0; i < 10; i++) {
          v *= 10;
          ip = (int)v;
          v = v - ip;
          if(ip) {
            for (int j = 0; j < ns; j++) {
              res += '0';
            }
            ns = 0;
            res += (char)('0' + ip);
          } else {
            ns++;
          }
        }
      }
    }
    if(sign) return '-' + res;
    else return res;

  }
} /* mg */

#endif /* end of include guard: MG_UTILS */
