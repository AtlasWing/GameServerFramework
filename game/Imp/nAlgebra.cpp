#include "game/nAlgebra.h"
#include "base/nEncoding.h"

namespace Game{
    double Algebra::Deal(std::string inputs)
    {
        int i;
        std::vector<int> bra;
        std::vector<int> ket;
        int j = 0;
        int p0 = inputs.find(")(");
        if (p0 != -1) 
        {   
            inputs = inputs.replace(p0, 2, ")*(");
        }   
        for (int i = 0; i < inputs.length(); i++)
        {   
            if (inputs[i] == '(' && j++ == 0)
            {   
                bra.push_back(i);
            }   
            if (inputs[i] == ')' && --j == 0)
            {   
                ket.push_back(i);
            }   
        }   
        if (!(bra.size() == ket.size()))
            return 0;
        if (bra.size() == 0)
            return Formular(inputs);
        std::string replace = inputs.substr(0, (int)bra[0]);
        std::string temp = inputs.substr((int)bra[0] + 1, (int)ket[0] - (int)bra[0] - 1); 
        if (temp.find('(') == -1) 
        {   
            replace = replace + Convert::ToString(Formular(temp)); // 无括号时的处理
        }   
        else
        {
            replace = replace + Convert::ToString(Deal(temp));
        }
        for (size_t i = 1; i < bra.size(); i++)
        {
            replace = replace + inputs.substr((int)ket[i - 1] + 1, (int)bra[i] - (int)ket[i - 1] - 1);
            temp = inputs.substr((int)bra[i] + 1, (int)ket[i] - (int)bra[i] - 1);
            if (temp.find('(') == -1)
            {
                replace = replace + Convert::ToString(Formular(temp));
            }
            else
            {
                replace = replace + Convert::ToString(Deal(temp));
            }
        }
        replace = replace + inputs.substr(ket[ket.size() - 1] + 1, inputs.length() - ket[ket.size() - 1] - 1);
        return Formular(replace);
    }

    double Algebra::Formular(std::string Inputs)//最简式运算
    {
        int Len = Inputs.length();
        std::vector<int> OpeLoc;
        std::vector<char> Ope;
        int i;
        for (i = 0; i < Len; i++)
        {
            if (IsOperator(Inputs[i]))
            {
                OpeLoc.push_back(i);
                Ope.push_back(Inputs[i]);
            }
        }
        if (OpeLoc.size() == 0)
        {
            return Convert::ToDouble(Inputs);
        }
        RebuildOperator(OpeLoc, Ope);
        if (!CheckFunction(OpeLoc, Len))
        {
            return 0;//判断算符组是否合法
        }
        std::vector<double> Val;
        int j = 0;
        for (i = 0; i < OpeLoc.size(); i++)
        {
            Val.push_back(Convert::ToDouble(Inputs.substr(j, OpeLoc[i] - j).c_str()));
            j = OpeLoc[i] + 1;
        }
        Val.push_back(Convert::ToDouble(Inputs.substr(j, Len - j))); //处理最后一个数据的录入
        return Calculate(Ope, Val);
    }

    double Algebra::Calculate(std::vector<char> Locs, std::vector<double> Values)//处理四则混合运算等基础运算
    {
        int i;
        for (i = 0; i < Locs.size(); i++)//处理逻辑运算
        {
            if (Locs[i] == '&')
            {
                Values[i] = (double)((int)Values[i] & (int)Values[i + 1]);
                Values.erase(Values.begin() + i + 1);
                Locs.erase(Locs.begin() + i);
                i--;
            }
            else if (Locs[i] == '|')
            {
                Values[i] = (double)((int)Values[i] | (int)Values[i + 1]);
                Values.erase(Values.begin() + i + 1);
                Locs.erase(Locs.begin() + i);
                i--;
            }
            else if (Locs[i] == '$')
            {
                Values[i] = (double)((int)Values[i] ^ (int)Values[i + 1]);
                Values.erase(Values.begin() + i + 1);
                Locs.erase(Locs.begin() + i);
                i--;
            }
            else if (Locs[i] == '#')
            {
                Values[i] = (double)((int)Values[i] ^ (int)Values[i + 1]);
                Values.erase(Values.begin() + i + 1);
                Locs.erase(Locs.begin() + i);
                i--;
            }
        }
        for (i = 0; i < Locs.size(); i++)//处理科学计数法
        {
            if (Locs[i] == 'E')
            {
                Values[i] = (double)Values[i] * pow(10, Values[i + 1]);
                Values.erase(Values.begin() + i + 1);
                Locs.erase(Locs.begin() + i);
                i--;
            }
            else if (Locs[i] == '^')
            {
                Values[i] = pow(Values[i], Values[i - 1]);
                Values.erase(Values.begin() + i + 1);
                Locs.erase(Locs.begin() + i);
                i--;
            }
        }
        for (i = 0; i < Locs.size(); i++)//处理乘法、除法、整除和求余
        {
            if (Locs[i] == '*')
            {
                Values[i] = Values[i] * Values[i + 1];
                Values.erase(Values.begin() + 1 + i);
                Locs.erase(Locs.begin() + i);
                i--;
            }
            else if (Locs[i] == '/')
            {
                Values[i] = Values[i] / Values[i + 1];
                Values.erase(Values.begin() + 1 + i);
                Locs.erase(Locs.begin() + i);
                i--;
            }
            else if (Locs[i] == '\\')
            {
                Values[i] = Values[i] / Values[i + 1];
                Values.erase(Values.begin() + 1 + i);
                Locs.erase(Locs.begin() + i);
                i--;
            }
            else if (Locs[i] == '%')
            {
                Values[i] = Values[i] - Values[i + 1] * (int)(Values[i] / Values[i + 1]);
                Values.erase(Values.begin() + 1 + i);
                Locs.erase(Locs.begin() + i);
                i--;
            }
        }
        for (i = 0; i < Locs.size(); i++)//处理加法和减法
        {
            if (Locs[i] == '+')
            {
                Values[i] = Values[i] + Values[i + 1];
                Values.erase(Values.begin() + 1 + i);
                Locs.erase(Locs.begin() + i);
                i--;
            }
            else if (Locs[i] == '-')
            {
                Values[i] = Values[i] - Values[i + 1];
                Values.erase(Values.begin() + 1 + i);
                Locs.erase(Locs.begin() + i);
                i--;
            }
        }
        return Values[0];
    }

    bool Algebra::IsOperator(char chr)//判断一个符号是否是基本算符
    {
        if (chr == '+' | chr == '-' | chr == '*' | chr == '/')
            return true;//判断是否是四则混合运算算符
        if (chr == '\\' | chr == 'E' | chr == '%' | chr == '^')
            return true;//判断是否是整除或科学计数法
        if (chr == '&' | chr == '|' | chr == '$' | chr == '#')
            return true;//判断是否是逻辑运算（与、或、异或、同或）
        return false;
    }

    void Algebra::RebuildOperator(std::vector<int>& Loc,  std::vector<char> &Val)//对负号的处理和重构
    {
        std::vector<int> DelItem;
        if (Loc[0] == 0 & Val[0] == '-')
        {
            DelItem.push_back(0);
        }
        int i;
        for (i = 1; i < Loc.size(); i++)//判断第一个符号是否是负号
        {
            if (Val[i] == '-' & Val[i - 1] != '-' & Loc[i] - Loc[i - 1] == 1)//判断是否有相邻的算符且后一个是负号
            {
                DelItem.push_back(i);
            }
        }
        for (i = DelItem.size() - 1; i >= 0; i--)
        {
            Val.erase(Val.begin() + DelItem[i]);
            Loc.erase(Loc.begin() + DelItem[i]);
        }
    }

    bool Algebra::CheckFunction(std::vector<int> Loc, int Len)//判断算符组是否合法
    {
        if (Loc[0] == 0)
        {
            return false;
        }
        int i;
        for (i = 1; i < Loc.size(); i++)
        {
            if (Loc[i] - Loc[i - 1] == 1)
            {
                return false;
            }
        }
        if (Loc[Loc.size() - 1] == Len - 1)
            return false;
        return true;
    }

    double NumericalFormula::GetFormulaCalculateResult(double *variables, int nums)
    {
        int startIndex = 0;
        std::string subStr = "";
        if(Tidy::nEncoding::isNumber(inputStr.c_str())){
            return Convert::ToDouble(inputStr);
        }
        int j = 0;
        while (j < nums)
        {
            int length = inputStr.find(";", startIndex);
            if (length == -1)
                break;
            subStr += inputStr.substr(startIndex, length) + Convert::ToString(variables[j]);
            inputStr = inputStr.substr(length + 1);
            j++;
        }
        subStr += inputStr;
        return RoundingResult(subStr);
    }

    double NumericalFormula::RoundingResult(std::string subStr)
    {
        double res = Algebra::Deal(subStr);
        switch (round)
        {
            case DOWN:
                {
                    res = floor(res);
                }
                break;
            case UP:
                {
                    res = ceil(res);
                }
                break;
            default:
                break;
        }
        return res;
    }
};

