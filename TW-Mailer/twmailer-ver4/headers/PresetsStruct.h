#ifndef PRESETSSTRUCT_H
#define PRESETSSTRUCT_H

#include <string>

// Deklaration der externen Variablen
extern int _blockSIZE;

// Das Struct, das überall benutzt wird, um Messages zu parsen, zu schicken und zu speichern
struct TextPreset
{
    int packageNUM;
    std::string delim;
    int length;
    int type;
    std::string argument;
    std::string sender;
    std::string subject;
    std::string text;
    bool error;
    int ID;
    std::string username;
    std::string infoString;
    std::string fileLocal;
};

// Enum, der dem Server bzw. User sagt, welchen Typ die Nachricht hat
enum type
{
    SEND = 1,
    READ = 2,
    LIST = 3,
    DEL = 4,
    QUIT = 5,
    COMMENT = 0,
};

// Deklaration der Funktion
TextPreset resetTP(TextPreset tp);

#endif // PRESETSSTRUCT_H
