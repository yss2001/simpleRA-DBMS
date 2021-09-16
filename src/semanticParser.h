#ifndef SEMANTICPARSER_H
#define SEMANTICPARSER_H
#include"syntacticParser.h"

bool semanticParse();

bool semanticParseCLEAR();
bool semanticParseCROSS();
bool semanticParseDISTINCT();
bool semanticParseEXPORT();
bool semanticParseEXPORTMATRIX();
bool semanticParseINDEX();
bool semanticParseJOIN();
bool semanticParseLIST();
bool semanticParseLOAD();
bool semanticParseLOADMATRIX();
bool semanticParsePRINT();
bool semanticParsePRINTMATRIX();
//bool semanticParseTRANSPOSE();
bool semanticParsePROJECTION();
bool semanticParseRENAME();
bool semanticParseSELECTION();
bool semanticParseSORT();
bool semanticParseSOURCE();
#endif
