// PresetsStruct.cpp
#include "PresetsStruct.h"

// Definition der externen Variablen
int _blockSIZE = 1024;

// Definition der Funktion
TextPreset resetTP(TextPreset tp)
{
    tp.sender = "";
    tp.subject = "";
    tp.text = "";
    tp.argument = "";
    tp.packageNUM = 1;
    tp.length = 0;
    tp.type = 0;
    tp.delim = "\n";
    tp.infoString = "";
    tp.error = false;
    tp.ID = 0;
    tp.username = "";
    return tp;
}
