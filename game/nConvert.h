#ifndef _CONVERT_H_
#define _CONVERT_H_
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "game/nObject.h"

const static std::string stringType  = "string";
const static std::string intType     = "int";
const static std::string floatType   = "float";
const static std::string doubleType  = "double";

class Convert
{
    public:
        static int ToInt(std::string source)
        {
            return atoi(source.c_str());
        }
        static int ToInt(const char * source)
        {
            return atoi(source);
        }
        static int ToInt(double source)
        {
            return (int)source;
        }
        static int ToInt(float source)
        {
            return (int)source;
        }
        static float ToFloat(std::string source)
        {
            return atof(source.c_str());
        }
        static float ToFloat(const char * source)
        {
            return atof(source);
        }
        static float ToFloat(int source)
        {
            return (float)source;
        }
        static double ToDouble(std::string source)
        {
            return strtod(source.c_str(),NULL);
        }
        static double ToDouble(const char * source)
        {
            return strtod(source, NULL);
        }
        static std::string ToString(double source)
        {
            char buffer[20];
            snprintf(buffer,sizeof(buffer),"%f", source);
            return std::string(buffer);
        }
        static std::string ToString(object source)
        {
            std::string ret = object_cast<std::string>(source);
            return ret;
        }
};

#endif
