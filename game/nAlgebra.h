#ifndef _NUMBERICAL_FORMULA_H_
#define _NUMBERICAL_FORMULA_H_

#include <vector>
#include <string>
#include <stdarg.h>
#include <sstream>
#include "nConvert.h"
#include "game/nSpecialType.h"

namespace Game{
    enum Rounding
    {
        UP,
        DOWN,
        NONEED,
    };

    class Algebra
    {
        public:
            Algebra() { }
        public:
            static double Deal(std::string inputs);
        protected:
            static double Formular(std::string Inputs);//最简式运算
            static double Calculate(std::vector<char> Locs, std::vector<double> Values);//处理四则混合运算等基础运算
            static bool IsOperator(char chr);//判断一个符号是否是基本算符
            static void RebuildOperator(std::vector<int>& Loc,  std::vector<char> &Val);//对负号的处理和重构
            static bool CheckFunction(std::vector<int> Loc, int Len);//判断算符组是否合法
    };

    class NumericalFormula : public SpecialTypeClass
    {
        //DECLARE_CLASS(NumericalFormula) 
        public:
            /*static void * createInstance(std::string content)
            {
                return new NumericalFormula(content);
            }*/
            NumericalFormula()
            {
            }

            void Init(std::string input)
            {
                inputStr = input;
                round = NONEED;
                if(inputStr[0] == '[' && inputStr[input.length() - 1] == ']'){
                    round = UP; 
                    inputStr = inputStr.substr(1, inputStr.length() - 1); 
                }   
                else if(inputStr[0] == '<' && inputStr[input.length() - 1] == '>'){
                    round = DOWN;
                    inputStr = inputStr.substr(1, inputStr.length() - 1); 
                }   
                for(int i = 0; i < input.length(); ++i){
                    if((inputStr[i] >= 'a' && inputStr[i] <= 'z') || (inputStr[i] >= 'A' && inputStr[i] <= 'Z')){
                        inputStr[i] = ';';
                    }   
                }   

            }

            ~NumericalFormula() { }

        public:
            double NumericalResult;
            std::string inputStr;
        public:
            double GetFormulaCalculateResult(double *variables, int nums);
        private:
            double RoundingResult(std::string subStr);
        private:
            Rounding round;
    };
};

#endif
