#define RC_WINDOWS

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <inttypes.h>
#include <SDL2/SDL.h>
#include <ctype.h>

using namespace std;

const int RC_DATA_SEGMENT_SWITCH = 0;
const int RC_CODE_SEGMENT_SWITCH = 1;

fstream rc_file;

string rc_tokens[2048];
//string rc_tmp[256];
int rc_tokens_count = 0;
int rc_equal_flag = 1;
int tmp_count = 0;

vector <unsigned char> RC_DATA_SEGMENT (65536);
vector <unsigned char> RC_CODE_SEGMENT (65536);

string rc_parser_output = "";
//string tmp[131072];
vector <string> tmp (10000);

string rc_preprocess[99];

int current_scope = 0;
string relative_scope = "PROGRAM";

int loop_handle = 0;
int current_loop = 0;

int current_if = 0;
int block_handle = 0;

int current_condition[99];

unsigned int rc_data_offset = 0;
unsigned int rc_code_offset = 0;

int rc_segment_switch = 0;

int vm_data_size = 8;//sizeof(double);

vector <string> rc_id(32000);
//string rc_tmp_id[99];
//variable           ex.  a = 5;                    a # 0 0 5 0;    first character is ID, second character is data type, third character is dimensions (0 if not array), fourth is number of indexes, 5th = value, 6th = index
//array              ex.  dim b[3]={4,5,6};         b # 1 3 0 4 1 5 2 6; 1st=ID, 2nd=data type, 3rd=dimensions, 4th=number of indexes, 5th=first index, 6th=first index value etc.
//function           ex.  function c(d=2,f)         c #f 2 d # 0 2 f # 0 0; 1st=ID, 2nd=data type and f for function, 3rd=number of arguments, the rest follow the same rules as variable


//string rc_utype[2048];
unsigned int rc_utype_count = 0;
//ex. type rc_type
//      dim x
//      dim y[5]
//    end type
//
//   dim r as rc_type
//
//   rc_utype[0] = "rc_type";
//   rc_utype[1] = "x # 0 0 1 0";
//   rc_utype[2] = "y #A 0 0 5 1;
//   rc_utype[3] = "end";

int rc_id_count = 0;
int rc_getID_index = 0;

int rc_loop_handle = 0;

bool rc_isString = false;
string rc_result = "";

uint64_t wcon_start[2048];
uint64_t wcon_end[2048];
uint64_t wcon_index = 0;

uint64_t m_index = 0;
uint64_t s_index = 0;
uint64_t f_index = 0;

unsigned int max_m = 0;
unsigned int max_s = 0;
unsigned int max_f = 0;

unsigned int rtn_stack_size = 0;
unsigned int rtn_stack_index = 0;

unsigned int num_stack_size = 0;
unsigned int str_stack_size = 0;

string select_result = "";

int src_line_num = 1;
vector <int> end_of_line(2000);
int end_of_line_count = 0;

int nid_count = 0;
int sid_count = 0;

vector <uint64_t> nid_byref_index(32000);
vector <uint64_t> sid_byref_index(32000);
uint64_t nid_byref_count = 0;
uint64_t sid_byref_count = 0;

int search_index = 0;

string rc_keyword [] = {
    "AND", //1
    "OR", //2
    "XOR", //3
    "EXIT", //4
    "MOD", //5
    "SHL", //6
    "SHR", //7
    "FUNCTION", //8
    "RETURN", //9
    "DIM", //10
    "END", //11
    "FOR", //12
    "TO", //13
    "STEP", //14
    "NEXT", //15
    "WHILE", //16
    "WEND", //17
    "DO", //18
    "LOOP", //19
    "UNTIL", //20
    "SELECT", //21
    "CASE", //22
    "DEFAULT", //23
    "IF", //24
    "ELSE", //25
    "THEN", //26
    "TEXT_INPUT", //27
    "TEXT_OUTPUT", //28
    "TEXT_APPEND", //29
    "TEXT_INPUT_PLUS", //30
    "TEXT_OUTPUT_PLUS", //31
    "TEXT_APPEND_PLUS", //32
    "BINARY_INPUT", //33
    "BINARY_OUTPUT", //34
    "BINARY_APPEND", //35
    "BINARY_INPUT_PLUS", //36
    "BINARY_OUTPUT_PLUS", //37
    "BINARY_APPEND_PLUS", //38
    "DELETE", //39
    "TRUE", //40
    "FALSE", //41
    "INCLUDE", //42
    "BYREF", //43
    "PRINT", //44
    "SUB", //45
    "NOT", //46
    "ELSEIF", //47
    "K_0", //48
    "K_1", //49
    "K_2", //50
    "K_3", //51
    "K_4", //52
    "K_5", //53
    "K_6", //54
    "K_7", //55
    "K_8", //56
    "K_9", //57
    "K_A", //58
    "K_AC_BACK", //59
    "K_AC_BOOKMARKS", //60
    "K_AC_FORWARD", //61
    "K_AC_HOME", //62
    "K_AC_REFRESH", //63
    "K_AC_SEARCH", //64
    "K_AC_STOP", //65
    "K_AGAIN", //66
    "K_ALTERASE", // 67
    "K_APPLICATION", //68
    "K_AUDIOMUTE", //69
    "K_AUDIONEXT", //70
    "K_AUDIOPLAY", //71
    "K_AUDIOPREV", //72
    "K_AUDIOSTOP", // 73
    "K_B", //74
    "K_BACKSLASH", //75
    "K_BACKSPACE", //76
    "K_BRIGHTNESSDOWN", //77
    "K_BRIGHTNESSUP", //78
    "K_C", //79
    "K_CALCULATOR", //80
    "K_CANCEL", //81
    "K_CAPSLOCK", //82
    "K_CLEAR", //83
    "K_CLEARAGAIN", //84
    "K_COMPUTER", //85
    "K_COPY", //86
    "K_CRSEL", //87
    "K_CURRENCYSUBUNIT", //88
    "K_CURRENCYUNIT", //89
    "K_CUT", //90
    "K_D", //91
    "K_DECIMALSEPARATOR", //92
    "K_DELETE", //93
    "K_DISPLAYSWITCH", //94
    "K_DOWN", //95
    "K_E", //96
    "K_EJECT", //97
    "K_END", //98
    "K_EQUAL", //99
    "K_ESCAPE", //100
    "K_EXECUTE", //101
    "K_EXSEL", //102
    "K_F", //103
    "K_F1", //104
    "K_F2", //105
    "K_F3", //106
    "K_F4", //107
    "K_F5", //108
    "K_F6", //109
    "K_F7", //110
    "K_F8", //111
    "K_F9", //112
    "K_F10", //113
    "K_F11", //114
    "K_F12", //115
    "K_F13", //116
    "K_F14", //117
    "K_F15", //118
    "K_F16", //119
    "K_F17", //120
    "K_F18", //121
    "K_F19", //122
    "K_F20", //123
    "K_F21", //124
    "K_F22", //125
    "K_F23", //126
    "K_F24", //127
    "K_FIND", //128
    "K_G", //129
    "K_GRAVE", //130
    "K_H", //131
    "K_HELP", //132
    "K_HOME", //133
    "K_I", //134
    "K_INSERT", //135
    "K_J", //136
    "K_K", //137
    "K_KBDLLUMDOWN", //138
    "K_KBDLLUMTOGGLE", //139
    "K_KBDLLUMUP", //140
    "KEYPAD_0", //141
    "KEYPAD_00", //142
    "KEYPAD_000", //143
    "KEYPAD_1", //144
    "KEYPAD_2", //145
    "KEYPAD_3", //146
    "KEYPAD_4", //147
    "KEYPAD_5", //148
    "KEYPAD_6", //149
    "KEYPAD_7", //150
    "KEYPAD_8", //151
    "KEYPAD_9", //152
    "KEYPAD_A", //153
    "KEYPAD_AMPERSAND", //154
    "KEYPAD_AT", //155
    "KEYPAD_B", //156
    "KEYPAD_BACKSPACE", //157
    "KEYPAD_BINARY", //158
    "KEYPAD_C", //159
    "KEYPAD_CLEAR", //160
    "KEYPAD_CLEARENTRY", //161
    "KEYPAD_COLON", //162
    "KEYPAD_COMMA", //163
    "KEYPAD_D", //164
    "KEYPAD_DBLAMPERSAND", //165
    "KEYPAD_DBLVERTICALBAR", // 166
    "KEYPAD_DECIMAL", //167
    "KEYPAD_DIVIDE", //168
    "KEYPAD_E", //169
    "KEYPAD_ENTER", //170
    "KEYPAD_EQUAL", //171
    "KEYPAD_EQUALAS400", //172
    "KEYPAD_EXCLAM", //173
    "KEYPAD_F", //174
    "KEYPAD_GREATER", //175
    "KEYPAD_HASH", //176
    "KEYPAD_HEXADECIMAL", //177
    "KEYPAD_LEFTBRACE", //178
    "KEYPAD_LEFTPAREN", //179
    "KEYPAD_LESS", //180
    "KEYPAD_MEMADD", //181
    "KEYPAD_MEMCLEAR", //182
    "KEYPAD_MEMDIVIDE", //183
    "KEYPAD_MEMMULTIPLY", //184
    "KEYPAD_MEMRECALL", //185
    "KEYPAD_MEMSTORE", // 186
    "KEYPAD_MEMSUBTRACT", //187
    "KEYPAD_MINUS", //188
    "KEYPAD_MULTIPLY", //189
    "KEYPAD_OCTAL", //190
    "KEYPAD_PERCENT", //191
    "KEYPAD_PERIOD", //192
    "KEYPAD_PLUS", //193
    "KEYPAD_PLUSMINUS", //194
    "KEYPAD_POWER", //195
    "KEYPAD_RIGHTBRACE", //196
    "KEYPAD_RIGHTPAREN", //197
    "KEYPAD_SPACE", //198
    "KEYPAD_TAB", //199
    "KEYPAD_VERTICALBAR", //200
    "KEYPAD_XOR", //201
    "K_L", //202
    "K_LALT", //203
    "K_LCTRL", //204
    "K_LEFT", //205
    "K_LEFTBRACKET", //206
    "K_LGUI", //207
    "K_LEFTSHIFT", //208
    "K_M", //209
    "K_MAIL", //210
    "K_MEDIASELECT", //211
    "K_MENU", //212
    "K_MINUS", // 213
    "K_MODESWITCH", //214
    "K_MUTE", //215
    "K_N", //216
    "K_NUMLOCK", //217
    "K_O", //218
    "K_OPER", // 219
    "K_OUT", //220
    "K_P", //221
    "K_PAGEDOWN", //222
    "K_PAGEUP", //223
    "K_PASTE", // 224
    "K_PAUSE", //225
    "K_POWER", //226
    "K_PRINTSCREEN", //227
    "K_PRIOR", //228
    "K_Q", //229
    "K_R", //230
    "K_RALT", //231
    "K_RCTRL", //232
    "K_RETURN", //233
    "K_RETURN2", //234
    "K_RGUI", //235
    "K_RIGHT", //236
    "K_RIGHTBRACKET", //237
    "K_RSHIFT", //238
    "K_S", //239
    "K_SCROLLLOCK", //240
    "K_SELECT", //241
    "K_SEMICOLON", //242
    "K_SEPARATOR", //243
    "K_SLASH", //244
    "K_SLEEP", //245
    "K_SPACE", //246
    "K_STOP", //247
    "K_SYSREQ", //248
    "K_T", //249
    "K_TAB", //250
    "K_THOUSANDSEPARATOR", //251
    "K_U", //252
    "K_UNDO", //253
    "K_UNKNOWN", //254
    "K_UP", //255
    "K_V", //256
    "K_VOLUMEDOWN", //257
    "K_VOLUMEUP", //258
    "K_W", //259
    "K_WWW", //260
    "K_X", //261
    "K_Y", //262
    "K_Z", //263
    "WINDOWPOS_CENTERED", //264
    "HAT_UP", //265
    "HAT_DOWN", //266
    "HAT_LEFT", //267
    "HAT_RIGHT", //268
    "HAT_RIGHTUP", //269
    "HAT_RIGHTDOWN", //270
    "HAT_LEFTUP", //271
    "HAT_LEFTDOWN", //272
    "HAT_CENTERED" //273
};

//int rc_keywords_count = 273;

int rc_keywords_count = sizeof(rc_keyword)/sizeof(rc_keyword[0]);

string vm_str_args[16];

union u_double
{
    double f;
    unsigned char data[sizeof(double)];
};

union u_index
{
    uint64_t i;
    unsigned char data[sizeof(uint64_t)];
};

vector <string> rc_label(64);
int rc_label_count = 0;

string rc_intern_arg_id[20];
string intern_type = "";

string rc_intToString(int a)
{
    stringstream ss;
    ss << a;
    string str = ss.str();
    return str;
}

string StringToUpper(string strToConvert)
{//change each element of the string to upper case
   for(unsigned int i=0;i<strToConvert.length();i++)
   {
      strToConvert[i] = toupper(strToConvert[i]);
   }
   return strToConvert;//return the converted string
}

string StringToLower(string strToConvert)
{//change each element of the string to lower case
   for(unsigned int i=0;i<strToConvert.length();i++)
   {
      strToConvert[i] = tolower(strToConvert[i]);
   }
   return strToConvert;//return the converted string
}

int rc_isBuiltIn_ID(string s_id, int arg_count, int tmp_index)
{
    if(s_id.compare("PROGRAM.FPRINT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 0";
        //cout << "TESTING PRINT in intern\n";
        return 1;
    }
    else if(s_id.compare("PROGRAM.INPUT$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 1";
        return 1;
    }
    else if(s_id.compare("PROGRAM.ARRAYDIM")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 3";
        return 1;
    }
    else if(s_id.compare("PROGRAM.ARRAYSIZE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 4";
        return 1;
    }
    else if(s_id.compare("PROGRAM.ASC")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 5";
        return 1;
    }
    else if(s_id.compare("PROGRAM.CHR$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 6";
        return 1;
    }
    else if(s_id.compare("PROGRAM.INSERT$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 7";
        return 1;
    }
    else if(s_id.compare("PROGRAM.INSTR")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 8";
        return 1;
    }
    else if(s_id.compare("PROGRAM.LCASE$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 9";
        return 1;
    }
    else if(s_id.compare("PROGRAM.LEFT$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 10";
        return 1;
    }
    else if(s_id.compare("PROGRAM.LENGTH")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 11";
        return 1;
    }
    else if(s_id.compare("PROGRAM.LTRIM$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 12";
        return 1;
    }
    else if(s_id.compare("PROGRAM.MID$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 13";
        return 1;
    }
    else if(s_id.compare("PROGRAM.REPLACE$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 14";
        return 1;
    }
    else if(s_id.compare("PROGRAM.REPLACESUBSTR$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 15";
        return 1;
    }
    else if(s_id.compare("PROGRAM.REVERSE$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 16";
        return 1;
    }
    else if(s_id.compare("PROGRAM.RIGHT$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 17";
        return 1;
    }
    else if(s_id.compare("PROGRAM.RTRIM$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 18";
        return 1;
    }
    else if(s_id.compare("PROGRAM.FILLSTRING$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 19";
        return 1;
    }
    else if(s_id.compare("PROGRAM.STR$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 20";
        return 1;
    }
    else if(s_id.compare("PROGRAM.TALLY")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 21";
        return 1;
    }
    else if(s_id.compare("PROGRAM.TRIM$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 22";
        return 1;
    }
    else if(s_id.compare("PROGRAM.UCASE$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 23";
        return 1;
    }
    else if(s_id.compare("PROGRAM.VAL")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 24";
        return 1;
    }
    else if(s_id.compare("PROGRAM.ABS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 25";
        return 1;
    }
    else if(s_id.compare("PROGRAM.ACOS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 26";
        return 1;
    }
    else if(s_id.compare("PROGRAM.ANDBIT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 27";
        return 1;
    }
    else if(s_id.compare("PROGRAM.ASIN")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 28";
        return 1;
    }
    else if(s_id.compare("PROGRAM.ATAN")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 29";
        return 1;
    }
    else if(s_id.compare("PROGRAM.BIN$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 30";
        return 1;
    }
    else if(s_id.compare("PROGRAM.COS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 31";
        return 1;
    }
    else if(s_id.compare("PROGRAM.EXP")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 32";
        return 1;
    }
    else if(s_id.compare("PROGRAM.FRAC")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 33";
        return 1;
    }
    else if(s_id.compare("PROGRAM.HEX$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 34";
        return 1;
    }
    else if(s_id.compare("PROGRAM.INT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 35";
        return 1;
    }
    else if(s_id.compare("PROGRAM.LOG")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 36";
        return 1;
    }
    else if(s_id.compare("PROGRAM.MAX")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 37";
        return 1;
    }
    else if(s_id.compare("PROGRAM.MIN")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 38";
        return 1;
    }
    else if(s_id.compare("PROGRAM.ORBIT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 39";
        return 1;
    }
    else if(s_id.compare("PROGRAM.RANDOMIZE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 40";
        return 1;
    }
    else if(s_id.compare("PROGRAM.RAND")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 41";
        return 1;
    }
    else if(s_id.compare("PROGRAM.ROUND")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 42";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SIGN")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 43";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SIN")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 44";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SQRT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 45";
        return 1;
    }
    else if(s_id.compare("PROGRAM.TAN")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 46";
        return 1;
    }
    else if(s_id.compare("PROGRAM.XORBIT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 47";
        return 1;
    }
    else if(s_id.compare("PROGRAM.FILEOPEN")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 48";
        return 1;
    }
    else if(s_id.compare("PROGRAM.FILECLOSE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 49";
        return 1;
    }
    else if(s_id.compare("PROGRAM.READBYTE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 50";
        return 1;
    }
    else if(s_id.compare("PROGRAM.WRITEBYTE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 51";
        return 1;
    }
    else if(s_id.compare("PROGRAM.READLINE$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 52";
        return 1;
    }
    else if(s_id.compare("PROGRAM.WRITELINE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 53";
        return 1;
    }
    else if(s_id.compare("PROGRAM.COPYFILE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 54";
        return 1;
    }
    else if(s_id.compare("PROGRAM.REMOVEFILE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 55";
        return 1;
    }
    else if(s_id.compare("PROGRAM.FILEEXISTS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 56";
        return 1;
    }
    else if(s_id.compare("PROGRAM.MOVEFILE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 57";
        return 1;
    }
    else if(s_id.compare("PROGRAM.RENAMEFILE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 58";
        return 1;
    }
    else if(s_id.compare("PROGRAM.FILELENGTH")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 59";
        return 1;
    }
    else if(s_id.compare("PROGRAM.TELL")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 60";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SEEK")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 61";
        return 1;
    }
    else if(s_id.compare("PROGRAM.EOF")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 62";
        return 1;
    }
    else if(s_id.compare("PROGRAM.FREEFILE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 63";
        return 1;
    }
    else if(s_id.compare("PROGRAM.CHANGEDIR")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 64";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DIREXISTS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 65";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DIRFIRST$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 66";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DIR$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 67";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DIRNEXT$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 68";
        return 1;
    }
    else if(s_id.compare("PROGRAM.MAKEDIR")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 69";
        return 1;
    }
    else if(s_id.compare("PROGRAM.REMOVEDIR")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 70";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DATE$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 71";
        return 1;
    }
    else if(s_id.compare("PROGRAM.TICKS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 72";
        return 1;
    }
    else if(s_id.compare("PROGRAM.TIME$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 73";
        return 1;
    }
    else if(s_id.compare("PROGRAM.TIMER")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 74";
        return 1;
    }
    else if(s_id.compare("PROGRAM.WAIT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 75";
        return 1;
    }
    else if(s_id.compare("PROGRAM.WINDOWOPEN")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 76";
        return 1;
    }
    else if(s_id.compare("PROGRAM.WINDOWCLOSE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 77";
        return 1;
    }
    else if(s_id.compare("PROGRAM.RAISEWINDOW")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 78";
        return 1;
    }
    else if(s_id.compare("PROGRAM.WINDOW")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 79";
        return 1;
    }
    else if(s_id.compare("PROGRAM.UPDATE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 80";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SHOWWINDOW")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 81";
        return 1;
    }
    else if(s_id.compare("PROGRAM.HIDEWINDOW")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 82";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETWINDOWTITLE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 83";
        return 1;
    }
    else if(s_id.compare("PROGRAM.WINDOWTITLE$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 84";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETWINDOWPOSITION")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 85";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETWINDOWPOSITION")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 86";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETWINDOWSIZE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 87";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETWINDOWSIZE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 88";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETWINDOWMINSIZE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 89";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETWINDOWMINSIZE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 90";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETWINDOWMAXSIZE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 91";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETWINDOWMAXSIZE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 92";
        return 1;
    }
    else if(s_id.compare("PROGRAM.WINDOWISFULLSCREEN")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 93";
        return 1;
    }
    else if(s_id.compare("PROGRAM.WINDOWISVISIBLE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 94";
        return 1;
    }
    else if(s_id.compare("PROGRAM.WINDOWISBORDERED")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 95";
        return 1;
    }
    else if(s_id.compare("PROGRAM.WINDOWISRESIZABLE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 96";
        return 1;
    }
    else if(s_id.compare("PROGRAM.WINDOWISMINIMIZED")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 97";
        return 1;
    }
    else if(s_id.compare("PROGRAM.WINDOWISMAXIMIZED")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 98";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETWINDOWFULLSCREEN")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 99";
        return 1;
    }
    else if(s_id.compare("PROGRAM.MAXIMIZEWINDOW")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 100";
        return 1;
    }
    else if(s_id.compare("PROGRAM.MINIMIZEWINDOW")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 101";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETWINDOWBORDER")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 102";
        return 1;
    }
    else if(s_id.compare("PROGRAM.CANVASOPEN")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 104";
        return 1;
    }
    else if(s_id.compare("PROGRAM.CANVASCLOSE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 105";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETCANVASVISIBLE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 240";
        return 1;
    }
    else if(s_id.compare("PROGRAM.CANVASISVISIBLE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 241";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETCANVASVIEWPORT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 106";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETCANVASVIEWPORT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 107";
        return 1;
    }
    else if(s_id.compare("PROGRAM.CANVAS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 108";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETCANVASOFFSET")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 109";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETCANVASOFFSET")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 110";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETCANVASSIZE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 111";
        return 1;
    }
    else if(s_id.compare("PROGRAM.CLEARCANVAS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 112";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETCANVASALPHA")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 113";
        return 1;
    }
    else if(s_id.compare("PROGRAM.COPYCANVAS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 114";
        return 1;
    }
    else if(s_id.compare("PROGRAM.CLONECANVAS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 115";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETCANVASZ")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 116";
        return 1;
    }
    else if(s_id.compare("PROGRAM.BOX")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 117";
        return 1;
    }
    else if(s_id.compare("PROGRAM.BOXFILL")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 118";
        return 1;
    }
    else if(s_id.compare("PROGRAM.CIRCLE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 119";
        return 1;
    }
    else if(s_id.compare("PROGRAM.CIRCLEFILL")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 120";
        return 1;
    }
    else if(s_id.compare("PROGRAM.ELLIPSE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 121";
        return 1;
    }
    else if(s_id.compare("PROGRAM.ELLIPSEFILL")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 122";
        return 1;
    }
    else if(s_id.compare("PROGRAM.PIXEL")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 123";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETCOLOR")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 124";
        return 1;
    }
    else if(s_id.compare("PROGRAM.LINE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 125";
        return 1;
    }
    else if(s_id.compare("PROGRAM.FLOODFILL")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 126";
        return 1;
    }
    else if(s_id.compare("PROGRAM.RGB")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 127";
        return 1;
    }
    else if(s_id.compare("PROGRAM.RGBA")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 128";
        return 1;
    }
    else if(s_id.compare("PROGRAM.PSET")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 129";
        return 1;
    }
    else if(s_id.compare("PROGRAM.CREATEIMAGE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 242";
        return 1;
    }
    else if(s_id.compare("PROGRAM.CREATEIMAGE_EX")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 243";
        return 1;
    }
    else if(s_id.compare("PROGRAM.LOADIMAGE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 130";
        return 1;
    }
    else if(s_id.compare("PROGRAM.COLORKEY")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 131";
        return 1;
    }
    else if(s_id.compare("PROGRAM.COPYIMAGE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 132";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DELETEIMAGE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 133";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETIMAGEALPHA")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 134";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETIMAGEALPHA")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 146";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETIMAGESIZE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 135";
        return 1;
    }
    else if(s_id.compare("PROGRAM.FLIPIMAGE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 136";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DRAWIMAGE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 137";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DRAWIMAGE_BLIT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 144";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DRAWIMAGE_ROTATE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 138";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DRAWIMAGE_ROTATE_EX")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 139";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DRAWIMAGE_ZOOM")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 140";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DRAWIMAGE_ZOOM_EX")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 141";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DRAWIMAGE_ROTOZOOM")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 142";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DRAWIMAGE_ROTOZOOM_EX")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 143";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DRAWIMAGE_BLIT_EX")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 145";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETCURSOR")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 160";
        return 1;
    }
    else if(s_id.compare("PROGRAM.PRINTS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 163";
        return 1;
    }
    else if(s_id.compare("PROGRAM.INPUTS$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 164";
        return 1;
    }
    else if(s_id.compare("PROGRAM.LOCATE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 165";
        return 1;
    }
    else if(s_id.compare("PROGRAM.FONTISLOADED")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 244";
        return 1;
    }
    else if(s_id.compare("PROGRAM.FONT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 245";
        return 1;
    }
    else if(s_id.compare("PROGRAM.LOADFONT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 246";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DELETEFONT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 247";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETFONTSTYLE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 248";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DRAWTEXT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 168";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DRAWTEXT_SHADED")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 249";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DRAWTEXT_BLENDED")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 250";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETTEXTSIZE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 251";
        return 1;
    }
    else if(s_id.compare("PROGRAM.INKEY")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 169";
        return 1;
    }
    else if(s_id.compare("PROGRAM.KEY")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 170";
        return 1;
    }
    else if(s_id.compare("PROGRAM.WAITKEY")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 171";
        return 1;
    }
    else if(s_id.compare("PROGRAM.HIDEMOUSE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 172";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SHOWMOUSE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 173";
        return 1;
    }
    else if(s_id.compare("PROGRAM.MOUSEISVISIBLE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 174";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETMOUSE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 175";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETMOUSEWHEEL")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 176";
        return 1;
    }
    else if(s_id.compare("PROGRAM.IMAGEEXISTS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 177";
        return 1;
    }
    else if(s_id.compare("PROGRAM.NUMJOYSTICKS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 178";
        return 1;
    }
    else if(s_id.compare("PROGRAM.JOYAXIS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 179";
        return 1;
    }
    else if(s_id.compare("PROGRAM.JOYBUTTON")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 180";
        return 1;
    }
    else if(s_id.compare("PROGRAM.JOYNAME$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 181";
        return 1;
    }
    else if(s_id.compare("PROGRAM.LOADSOUND")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 182";
        return 1;
    }
    else if(s_id.compare("PROGRAM.LOADMUSIC")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 183";
        return 1;
    }
    else if(s_id.compare("PROGRAM.PLAYSOUND")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 184";
        return 1;
    }
    else if(s_id.compare("PROGRAM.PLAYSOUNDTIMED")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 185";
        return 1;
    }
    else if(s_id.compare("PROGRAM.PLAYMUSIC")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 186";
        return 1;
    }
    else if(s_id.compare("PROGRAM.PAUSESOUND")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 187";
        return 1;
    }
    else if(s_id.compare("PROGRAM.RESUMESOUND")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 188";
        return 1;
    }
    else if(s_id.compare("PROGRAM.PAUSEMUSIC")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 189";
        return 1;
    }
    else if(s_id.compare("PROGRAM.RESUMEMUSIC")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 190";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DELETESOUND")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 191";
        return 1;
    }
    else if(s_id.compare("PROGRAM.FADEMUSICIN")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 192";
        return 1;
    }
    else if(s_id.compare("PROGRAM.FADEMUSICOUT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 193";
        return 1;
    }
    else if(s_id.compare("PROGRAM.MUSICEXISTS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 194";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETMUSICVOLUME")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 195";
        return 1;
    }
    else if(s_id.compare("PROGRAM.MUSICVOLUME")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 196";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETMUSICPOSITION")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 197";
        return 1;
    }
    else if(s_id.compare("PROGRAM.MUSICPOSITION")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 198";
        return 1;
    }
    else if(s_id.compare("PROGRAM.REWINDMUSIC")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 199";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETSOUNDCHANNELS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 200";
        return 1;
    }
    else if(s_id.compare("PROGRAM.NUMSOUNDCHANNELS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 252";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SOUNDISENABLED")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 201";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SOUNDEXISTS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 202";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETCHANNELVOLUME")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 203";
        return 1;
    }
    else if(s_id.compare("PROGRAM.CHANNELVOLUME")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 204";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETSOUNDVOLUME")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 205";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SOUNDVOLUME")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 206";
        return 1;
    }
    else if(s_id.compare("PROGRAM.STOPMUSIC")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 207";
        return 1;
    }
    else if(s_id.compare("PROGRAM.STOPSOUND")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 208";
        return 1;
    }
    else if(s_id.compare("PROGRAM.TCP_SOCKETOPEN")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 209";
        return 1;
    }
    else if(s_id.compare("PROGRAM.TCP_SOCKETCLOSE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 210";
        return 1;
    }
    else if(s_id.compare("PROGRAM.TCP_REMOTEHOST")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 211";
        return 1;
    }
    else if(s_id.compare("PROGRAM.TCP_REMOTEPORT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 212";
        return 1;
    }
    else if(s_id.compare("PROGRAM.TCP_GETDATA")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 213";
        return 1;
    }
    else if(s_id.compare("PROGRAM.TCP_SENDDATA")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 214";
        return 1;
    }
    else if(s_id.compare("PROGRAM.TCP_ACCEPTSOCKET")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 215";
        return 1;
    }
    else if(s_id.compare("PROGRAM.UDP_SOCKETOPEN")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 216";
        return 1;
    }
    else if(s_id.compare("PROGRAM.UDP_GETDATA")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 217";
        return 1;
    }
    else if(s_id.compare("PROGRAM.UDP_LENGTH")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 219";
        return 1;
    }
    else if(s_id.compare("PROGRAM.UDP_MAXLENGTH")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 222";
        return 1;
    }
    else if(s_id.compare("PROGRAM.UDP_REMOTEHOST$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 223";
        return 1;
    }
    else if(s_id.compare("PROGRAM.UDP_REMOTEPORT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 224";
        return 1;
    }
    else if(s_id.compare("PROGRAM.UDP_SOCKETCLOSE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 225";
        return 1;
    }
    else if(s_id.compare("PROGRAM.UDP_SENDDATA")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 226";
        return 1;
    }
    else if(s_id.compare("PROGRAM.TOUCHPRESSURE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 227";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETTOUCH")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 228";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETMULTITOUCH")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 229";
        return 1;
    }
    else if(s_id.compare("PROGRAM.ZONEINPUT$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 230";
        return 1;
    }
    else if(s_id.compare("PROGRAM.READINPUT_START")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 233";
        return 1;
    }
    else if(s_id.compare("PROGRAM.READINPUT_STOP")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 234";
        return 1;
    }
    else if(s_id.compare("PROGRAM.CANVASCLIP")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 235";
        return 1;
    }
    else if(s_id.compare("PROGRAM.WINDOWCLIP")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 236";
        return 1;
    }
    else if(s_id.compare("PROGRAM.RENDERTEXT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 237";
        return 1;
    }
    else if(s_id.compare("PROGRAM.READINPUT_TEXT$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 238";
        return 1;
    }
    else if(s_id.compare("PROGRAM.LOCALIP$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 253";
        return 1;
    }
    else if(s_id.compare("PROGRAM.READINPUT_SETTEXT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 254";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETTOUCHFINGER")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 255";
        return 1;
    }
    else if(s_id.compare("PROGRAM.NUMFINGERS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 256";
        return 1;
    }
    else if(s_id.compare("PROGRAM.LOADIMAGE_EX")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 257";
        return 1;
    }
    else if(s_id.compare("PROGRAM.RECT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 258";
        return 1;
    }
    else if(s_id.compare("PROGRAM.RECTFILL")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 259";
        return 1;
    }
    else if(s_id.compare("PROGRAM.OS$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 260";
        return 1;
    }
    else if(s_id.compare("PROGRAM.NUMJOYBUTTONS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 261";
        return 1;
    }
    else if(s_id.compare("PROGRAM.NUMJOYAXES")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 262";
        return 1;
    }
    else if(s_id.compare("PROGRAM.LOADVIDEO")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 263";
        return 1;
    }
    else if(s_id.compare("PROGRAM.PLAYVIDEO")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 264";
        return 1;
    }
    else if(s_id.compare("PROGRAM.PAUSEVIDEO")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 265";
        return 1;
    }
    else if(s_id.compare("PROGRAM.STOPVIDEO")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 266";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETVIDEOPOSITION")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 267";
        return 1;
    }
    else if(s_id.compare("PROGRAM.RESUMEVIDEO")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 269";
        return 1;
    }
    else if(s_id.compare("PROGRAM.VIDEOPOSITION")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 270";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DELETEVIDEO")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 272";
        return 1;
    }
    else if(s_id.compare("PROGRAM.DELETEMUSIC")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 273";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SYSTEM")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 274";
        return 1;
    }
    else if(s_id.compare("PROGRAM.VIDEOISPLAYING")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 275";
        return 1;
    }
    else if(s_id.compare("PROGRAM.ROUNDRECT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 276";
        return 1;
    }
    else if(s_id.compare("PROGRAM.ROUNDRECTFILL")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 277";
        return 1;
    }
    else if(s_id.compare("PROGRAM.VIDEOEND")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 278";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETVIDEOSTATS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 279";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETVIDEODRAWRECT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 280";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETVIDEODRAWRECT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 281";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETVIDEOSIZE")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 282";
        return 1;
    }
    else if(s_id.compare("PROGRAM.VIDEOEXISTS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 283";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETVIDEOALPHA")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 284";
        return 1;
    }
    else if(s_id.compare("PROGRAM.CREATESOUND")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 285";
        return 1;
    }
    else if(s_id.compare("PROGRAM.COMMAND$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 286";
        return 1;
    }
    else if(s_id.compare("PROGRAM.NUMCOMMANDS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 287";
        return 1;
    }
    else if(s_id.compare("PROGRAM.STR_F$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 288";
        return 1;
    }
    else if(s_id.compare("PROGRAM.STR_S$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 289";
        return 1;
    }
    else if(s_id.compare("PROGRAM.CLS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 290";
        return 1;
    }
    else if(s_id.compare("PROGRAM.ENV$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 291";
        return 1;
    }
    else if(s_id.compare("PROGRAM.SETENV")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 292";
        return 1;
    }
    else if(s_id.compare("PROGRAM.PREFPATH$")==0)
    {
        intern_type = "$";
        tmp[tmp_index] = "intern 293";
        return 1;
    }
    else if(s_id.compare("PROGRAM.NUMJOYHATS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 294";
        return 1;
    }
    else if(s_id.compare("PROGRAM.JOYHAT")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 295";
        return 1;
    }
    else if(s_id.compare("PROGRAM.NUMJOYTRACKBALLS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 296";
        return 1;
    }
    else if(s_id.compare("PROGRAM.GETJOYTRACKBALL")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 297";
        return 1;
    }
    else if(s_id.compare("PROGRAM.WINDOWHASINPUTFOCUS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 298";
        return 1;
    }
    else if(s_id.compare("PROGRAM.WINDOWHASMOUSEFOCUS")==0)
    {
        intern_type = "#";
        tmp[tmp_index] = "intern 299";
        return 1;
    }

    return 0;
}

int rc_clearTokens()
{
    for(int i = 0; i < rc_tokens_count; i++)
        rc_tokens[i] = "";

    rc_tokens_count = 0;
    return 0;
}

int rc_output_tmp()
{
    //for(int i = 0; i < tmp_count; i++)
        //cout << "TMP["<<i<<"]="<<tmp[i]<<endl;
    return 0;
}

bool rc_isKeyword(string sline)
{
    for(int i = 0; i < rc_keywords_count; i++)
    {
        if(sline.compare(rc_keyword[i])==0)
            return true;
    }
    return false;
}

string rc_keywordToken(string sline)
{
    if(sline.compare("AND")==0)
        return "<and>";
    else if(sline.compare("OR")==0)
        return "<or>";
    else if(sline.compare("XOR")==0)
        return "<xor>";
    else if(sline.compare("EXIT")==0)
        return "<exit>";
    else if(sline.compare("MOD")==0)
        return "<mod>";
    else if(sline.compare("SHL")==0)
        return "<shl>";
    else if(sline.compare("SHR")==0)
        return "<shr>";
    else if(sline.compare("FUNCTION")==0)
        return "<function>";
    else if(sline.compare("RETURN")==0)
        return "<return>";
    else if(sline.compare("DIM")==0)
        return "<dim>";
    else if(sline.compare("END")==0)
        return "<end>";
    else if(sline.compare("FOR")==0)
        return "<for>";
    else if(sline.compare("TO")==0)
        return "<to>";
    else if(sline.compare("STEP")==0)
        return "<step>";
    else if(sline.compare("NEXT")==0)
        return "<next>";
    else if(sline.compare("WHILE")==0)
        return "<while>";
    else if(sline.compare("WEND")==0)
        return "<wend>";
    else if(sline.compare("DO")==0)
        return "<do>";
    else if(sline.compare("LOOP")==0)
        return "<loop>";
    else if(sline.compare("UNTIL")==0)
        return "<until>";
    else if(sline.compare("SELECT")==0)
        return "<select>";
    else if(sline.compare("CASE")==0)
        return "<case>";
    else if(sline.compare("DEFAULT")==0)
        return "<default>";
    else if(sline.compare("IF")==0)
        return "<if>";
    else if(sline.compare("ELSE")==0)
        return "<else>";
    else if(sline.compare("THEN")==0)
        return "<then>";
    else if(sline.compare("TEXT_INPUT")==0)
        return "<num>1";
    else if(sline.compare("TEXT_OUTPUT")==0)
        return "<num>2";
    else if(sline.compare("TEXT_APPEND")==0)
        return "<num>3";
    else if(sline.compare("TEXT_INPUT_PLUS")==0)
        return "<num>4";
    else if(sline.compare("TEXT_OUTPUT_PLUS")==0)
        return "<num>5";
    else if(sline.compare("TEXT_APPEND_PLUS")==0)
        return "<num>6";
    else if(sline.compare("BINARY_INPUT")==0)
        return "<num>7";
    else if(sline.compare("BINARY_OUTPUT")==0)
        return "<num>8";
    else if(sline.compare("BINARY_APPEND")==0)
        return "<num>9";
    else if(sline.compare("BINARY_INPUT_PLUS")==0)
        return "<num>10";
    else if(sline.compare("BINARY_OUTPUT_PLUS")==0)
        return "<num>11";
    else if(sline.compare("BINARY_APPEND_PLUS")==0)
        return "<num>12";
    else if(sline.compare("DELETE")==0)
        return "<delete>";
    else if(sline.compare("TRUE")==0)
        return "<num>1";
    else if(sline.compare("FALSE")==0)
        return "<num>0";
    else if(sline.compare("INCLUDE")==0)
        return "<include>";
    else if(sline.compare("BYREF")==0)
        return "<byref>";
    else if(sline.compare("PRINT")==0)
        return "<print>";
    else if(sline.compare("SUB")==0)
        return "<procedure>";
    else if(sline.compare("NOT")==0)
        return "<not>";
    else if(sline.compare("ELSEIF")==0)
        return "<elseif>";
    else if(sline.compare("K_0")==0)
        return "<num>" + rc_intToString(SDLK_0);
    else if(sline.compare("K_1")==0)
        return "<num>" + rc_intToString(SDLK_1);
    else if(sline.compare("K_2")==0)
        return "<num>" + rc_intToString(SDLK_2);
    else if(sline.compare("K_3")==0)
        return "<num>" + rc_intToString(SDLK_3);
    else if(sline.compare("K_4")==0)
        return "<num>" + rc_intToString(SDLK_4);
    else if(sline.compare("K_5")==0)
        return "<num>" + rc_intToString(SDLK_5);
    else if(sline.compare("K_6")==0)
        return "<num>" + rc_intToString(SDLK_6);
    else if(sline.compare("K_7")==0)
        return "<num>" + rc_intToString(SDLK_7);
    else if(sline.compare("K_8")==0)
        return "<num>" + rc_intToString(SDLK_8);
    else if(sline.compare("K_9")==0)
        return "<num>" + rc_intToString(SDLK_9);
    else if(sline.compare("K_A")==0)
        return "<num>" + rc_intToString(SDLK_a);
    else if(sline.compare("K_AC_BACK")==0)
        return "<num>" + rc_intToString(SDLK_AC_BACK);
    else if(sline.compare("K_AC_BOOKMARKS")==0)
        return "<num>" + rc_intToString(SDLK_AC_BOOKMARKS);
    else if(sline.compare("K_AC_FORWARD")==0)
        return "<num>" + rc_intToString(SDLK_AC_FORWARD);
    else if(sline.compare("K_AC_HOME")==0)
        return "<num>" + rc_intToString(SDLK_AC_HOME);
    else if(sline.compare("K_AC_REFRESH")==0)
        return "<num>" + rc_intToString(SDLK_AC_REFRESH);
    else if(sline.compare("K_AC_SEARCH")==0)
        return "<num>" + rc_intToString(SDLK_AC_SEARCH);
    else if(sline.compare("K_AC_STOP")==0)
        return "<num>" + rc_intToString(SDLK_AC_STOP);
    else if(sline.compare("K_AGAIN")==0)
        return "<num>" + rc_intToString(SDLK_AGAIN);
    else if(sline.compare("K_ALTERASE")==0)
        return "<num>" + rc_intToString(SDLK_ALTERASE);
    else if(sline.compare("K_APPLICATION")==0)
        return "<num>" + rc_intToString(SDLK_APPLICATION);
    else if(sline.compare("K_AUDIOMUTE")==0)
        return "<num>" + rc_intToString(SDLK_AUDIOMUTE);
    else if(sline.compare("K_AUDIONEXT")==0)
        return "<num>" + rc_intToString(SDLK_AUDIONEXT);
    else if(sline.compare("K_AUDIOPLAY")==0)
        return "<num>" + rc_intToString(SDLK_AUDIOPLAY);
    else if(sline.compare("K_AUDIOPREV")==0)
        return "<num>" + rc_intToString(SDLK_AUDIOPREV);
    else if(sline.compare("K_AUDIOSTOP")==0)
        return "<num>" + rc_intToString(SDLK_AUDIOSTOP);
    else if(sline.compare("K_B")==0)
        return "<num>" + rc_intToString(SDLK_b);
    else if(sline.compare("K_BACKSLASH")==0)
        return "<num>" + rc_intToString(SDLK_BACKSLASH);
    else if(sline.compare("K_BACKSPACE")==0)
        return "<num>" + rc_intToString(SDLK_BACKSPACE);
    else if(sline.compare("K_BRIGHTNESSDOWN")==0)
        return "<num>" + rc_intToString(SDLK_BRIGHTNESSDOWN);
    else if(sline.compare("K_BRIGHTNESSUP")==0)
        return "<num>" + rc_intToString(SDLK_BRIGHTNESSUP);
    else if(sline.compare("K_C")==0)
        return "<num>" + rc_intToString(SDLK_c);
    else if(sline.compare("K_CALCULATOR")==0)
        return "<num>" + rc_intToString(SDLK_CALCULATOR);
    else if(sline.compare("K_CANCEL")==0)
        return "<num>" + rc_intToString(SDLK_CANCEL);
    else if(sline.compare("K_CAPSLOCK")==0)
        return "<num>" + rc_intToString(SDLK_CAPSLOCK);
    else if(sline.compare("K_CLEAR")==0)
        return "<num>" + rc_intToString(SDLK_CLEAR);
    else if(sline.compare("K_CLEARAGAIN")==0)
        return "<num>" + rc_intToString(SDLK_CLEARAGAIN);
    else if(sline.compare("K_COMPUTER")==0)
        return "<num>" + rc_intToString(SDLK_COMPUTER);
    else if(sline.compare("K_COPY")==0)
        return "<num>" + rc_intToString(SDLK_COPY);
    else if(sline.compare("K_CRSEL")==0)
        return "<num>" + rc_intToString(SDLK_CRSEL);
    else if(sline.compare("K_CURRENCYSUBUNIT")==0)
        return "<num>" + rc_intToString(SDLK_CURRENCYSUBUNIT);
    else if(sline.compare("K_CURRENCYUNIT")==0)
        return "<num>" + rc_intToString(SDLK_CURRENCYUNIT);
    else if(sline.compare("K_CUT")==0)
        return "<num>" + rc_intToString(SDLK_CUT);
    else if(sline.compare("K_D")==0)
        return "<num>" + rc_intToString(SDLK_d);
    else if(sline.compare("K_DECIMALSEPARATOR")==0)
        return "<num>" + rc_intToString(SDLK_DECIMALSEPARATOR);
    else if(sline.compare("K_DELETE")==0)
        return "<num>" + rc_intToString(SDLK_DELETE);
    else if(sline.compare("K_DISPLAYSWITCH")==0)
        return "<num>" + rc_intToString(SDLK_DISPLAYSWITCH);
    else if(sline.compare("K_DOWN")==0)
        return "<num>" + rc_intToString(SDLK_DOWN);
    else if(sline.compare("K_E")==0)
        return "<num>" + rc_intToString(SDLK_e);
    else if(sline.compare("K_EJECT")==0)
        return "<num>" + rc_intToString(SDLK_EJECT);
    else if(sline.compare("K_END")==0)
        return "<num>" + rc_intToString(SDLK_END);
    else if(sline.compare("K_EQUAL")==0)
        return "<num>" + rc_intToString(SDLK_EQUALS);
    else if(sline.compare("K_ESCAPE")==0)
        return "<num>" + rc_intToString(SDLK_ESCAPE);
    else if(sline.compare("K_EXECUTE")==0)
        return "<num>" + rc_intToString(SDLK_EXECUTE);
    else if(sline.compare("K_EXSEL")==0)
        return "<num>" + rc_intToString(SDLK_EXSEL);
    else if(sline.compare("K_F")==0)
        return "<num>" + rc_intToString(SDLK_f);
    else if(sline.compare("K_F1")==0)
        return "<num>" + rc_intToString(SDLK_F1);
    else if(sline.compare("K_F2")==0)
        return "<num>" + rc_intToString(SDLK_F2);
    else if(sline.compare("K_F3")==0)
        return "<num>" + rc_intToString(SDLK_F3);
    else if(sline.compare("K_F4")==0)
        return "<num>" + rc_intToString(SDLK_F4);
    else if(sline.compare("K_F5")==0)
        return "<num>" + rc_intToString(SDLK_F5);
    else if(sline.compare("K_F6")==0)
        return "<num>" + rc_intToString(SDLK_F6);
    else if(sline.compare("K_F7")==0)
        return "<num>" + rc_intToString(SDLK_F7);
    else if(sline.compare("K_F8")==0)
        return "<num>" + rc_intToString(SDLK_F8);
    else if(sline.compare("K_F9")==0)
        return "<num>" + rc_intToString(SDLK_F9);
    else if(sline.compare("K_F10")==0)
        return "<num>" + rc_intToString(SDLK_F10);
    else if(sline.compare("K_F11")==0)
        return "<num>" + rc_intToString(SDLK_F11);
    else if(sline.compare("K_F12")==0)
        return "<num>" + rc_intToString(SDLK_F12);
    else if(sline.compare("K_F13")==0)
        return "<num>" + rc_intToString(SDLK_F13);
    else if(sline.compare("K_F14")==0)
        return "<num>" + rc_intToString(SDLK_F14);
    else if(sline.compare("K_F15")==0)
        return "<num>" + rc_intToString(SDLK_F15);
    else if(sline.compare("K_F16")==0)
        return "<num>" + rc_intToString(SDLK_F16);
    else if(sline.compare("K_F17")==0)
        return "<num>" + rc_intToString(SDLK_F17);
    else if(sline.compare("K_F18")==0)
        return "<num>" + rc_intToString(SDLK_F18);
    else if(sline.compare("K_F19")==0)
        return "<num>" + rc_intToString(SDLK_F19);
    else if(sline.compare("K_F20")==0)
        return "<num>" + rc_intToString(SDLK_F20);
    else if(sline.compare("K_F21")==0)
        return "<num>" + rc_intToString(SDLK_F21);
    else if(sline.compare("K_F22")==0)
        return "<num>" + rc_intToString(SDLK_F22);
    else if(sline.compare("K_F23")==0)
        return "<num>" + rc_intToString(SDLK_F23);
    else if(sline.compare("K_F24")==0)
        return "<num>" + rc_intToString(SDLK_F24);
    else if(sline.compare("K_FIND")==0)
        return "<num>" + rc_intToString(SDLK_FIND);
    else if(sline.compare("K_G")==0)
        return "<num>" + rc_intToString(SDLK_g);
    else if(sline.compare("K_GRAVE")==0)
        return "<num>" + rc_intToString(SDLK_BACKQUOTE);
    else if(sline.compare("K_H")==0)
        return "<num>" + rc_intToString(SDLK_h);
    else if(sline.compare("K_HELP")==0)
        return "<num>" + rc_intToString(SDLK_HELP);
    else if(sline.compare("K_HOME")==0)
        return "<num>" + rc_intToString(SDLK_HOME);
    else if(sline.compare("K_I")==0)
        return "<num>" + rc_intToString(SDLK_i);
    else if(sline.compare("K_INSERT")==0)
        return "<num>" + rc_intToString(SDLK_INSERT);
    else if(sline.compare("K_J")==0)
        return "<num>" + rc_intToString(SDLK_j);
    else if(sline.compare("K_K")==0)
        return "<num>" + rc_intToString(SDLK_k);
    else if(sline.compare("K_KBDILLUMDOWN")==0)
        return "<num>" + rc_intToString(SDLK_KBDILLUMDOWN);
    else if(sline.compare("K_KBDILLUMTOGGLE")==0)
        return "<num>" + rc_intToString(SDLK_KBDILLUMTOGGLE);
    else if(sline.compare("K_KBDILLUMUP")==0)
        return "<num>" + rc_intToString(SDLK_KBDILLUMUP);
    else if(sline.compare("KEYPAD_0")==0)
        return "<num>" + rc_intToString(SDLK_KP_0);
    else if(sline.compare("KEYPAD_00")==0)
        return "<num>" + rc_intToString(SDLK_KP_00);
    else if(sline.compare("KEYPAD_000")==0)
        return "<num>" + rc_intToString(SDLK_KP_000);
    else if(sline.compare("KEYPAD_1")==0)
        return "<num>" + rc_intToString(SDLK_KP_1);
    else if(sline.compare("KEYPAD_2")==0)
        return "<num>" + rc_intToString(SDLK_KP_2);
    else if(sline.compare("KEYPAD_3")==0)
        return "<num>" + rc_intToString(SDLK_KP_3);
    else if(sline.compare("KEYPAD_4")==0)
        return "<num>" + rc_intToString(SDLK_KP_4);
    else if(sline.compare("KEYPAD_5")==0)
        return "<num>" + rc_intToString(SDLK_KP_5);
    else if(sline.compare("KEYPAD_6")==0)
        return "<num>" + rc_intToString(SDLK_KP_6);
    else if(sline.compare("KEYPAD_7")==0)
        return "<num>" + rc_intToString(SDLK_KP_7);
    else if(sline.compare("KEYPAD_8")==0)
        return "<num>" + rc_intToString(SDLK_KP_8);
    else if(sline.compare("KEYPAD_9")==0)
        return "<num>" + rc_intToString(SDLK_KP_9);
    else if(sline.compare("KEYPAD_A")==0)
        return "<num>" + rc_intToString(SDLK_KP_A);
    else if(sline.compare("KEYPAD_AMPERSAND")==0)
        return "<num>" + rc_intToString(SDLK_KP_AMPERSAND);
    else if(sline.compare("KEYPAD_AT")==0)
        return "<num>" + rc_intToString(SDLK_KP_AT);
    else if(sline.compare("KEYPAD_B")==0)
        return "<num>" + rc_intToString(SDLK_KP_B);
    else if(sline.compare("KEYPAD_BACKSPACE")==0)
        return "<num>" + rc_intToString(SDLK_KP_BACKSPACE);
    else if(sline.compare("KEYPAD_BINARY")==0)
        return "<num>" + rc_intToString(SDLK_KP_BINARY);
    else if(sline.compare("KEYPAD_C")==0)
        return "<num>" + rc_intToString(SDLK_KP_C);
    else if(sline.compare("KEYPAD_CLEAR")==0)
        return "<num>" + rc_intToString(SDLK_KP_CLEAR);
    else if(sline.compare("KEYPAD_CLEARENTRY")==0)
        return "<num>" + rc_intToString(SDLK_KP_CLEARENTRY);
    else if(sline.compare("KEYPAD_COLON")==0)
        return "<num>" + rc_intToString(SDLK_KP_COLON);
    else if(sline.compare("KEYPAD_COMMA")==0)
        return "<num>" + rc_intToString(SDLK_KP_COMMA);
    else if(sline.compare("KEYPAD_D")==0)
        return "<num>" + rc_intToString(SDLK_KP_D);
    else if(sline.compare("KEYPAD_DBLAMPERSAND")==0)
        return "<num>" + rc_intToString(SDLK_KP_DBLAMPERSAND);
    else if(sline.compare("KEYPAD_DBLVERTICALBAR")==0)
        return "<num>" + rc_intToString(SDLK_KP_DBLVERTICALBAR);
    else if(sline.compare("KEYPAD_DECIMAL")==0)
        return "<num>" + rc_intToString(SDLK_KP_DECIMAL);
    else if(sline.compare("KEYPAD_DIVIDE")==0)
        return "<num>" + rc_intToString(SDLK_KP_DIVIDE);
    else if(sline.compare("KEYPAD_E")==0)
        return "<num>" + rc_intToString(SDLK_KP_E);
    else if(sline.compare("KEYPAD_ENTER")==0)
        return "<num>" + rc_intToString(SDLK_KP_ENTER);
    else if(sline.compare("KEYPAD_EQUAL")==0)
        return "<num>" + rc_intToString(SDLK_KP_EQUALS);
    else if(sline.compare("KEYPAD_EQUALAS400")==0)
        return "<num>" + rc_intToString(SDLK_KP_EQUALSAS400);
    else if(sline.compare("KEYPAD_EXCLAM")==0)
        return "<num>" + rc_intToString(SDLK_KP_EXCLAM);
    else if(sline.compare("KEYPAD_F")==0)
        return "<num>" + rc_intToString(SDLK_KP_F);
    else if(sline.compare("KEYPAD_GREATER")==0)
        return "<num>" + rc_intToString(SDLK_KP_GREATER);
    else if(sline.compare("KEYPAD_HASH")==0)
        return "<num>" + rc_intToString(SDLK_KP_HASH);
    else if(sline.compare("KEYPAD_HEXADECIMAL")==0)
        return "<num>" + rc_intToString(SDLK_KP_HEXADECIMAL);
    else if(sline.compare("KEYPAD_LEFTBRACE")==0)
        return "<num>" + rc_intToString(SDLK_KP_LEFTBRACE);
    else if(sline.compare("KEYPAD_LEFTPAREN")==0)
        return "<num>" + rc_intToString(SDLK_KP_LEFTPAREN);
    else if(sline.compare("KEYPAD_LESS")==0)
        return "<num>" + rc_intToString(SDLK_KP_LESS);
    else if(sline.compare("KEYPAD_MEMADD")==0)
        return "<num>" + rc_intToString(SDLK_KP_MEMADD);
    else if(sline.compare("KEYPAD_MEMCLEAR")==0)
        return "<num>" + rc_intToString(SDLK_KP_MEMCLEAR);
    else if(sline.compare("KEYPAD_MEMDIVIDE")==0)
        return "<num>" + rc_intToString(SDLK_KP_MEMDIVIDE);
    else if(sline.compare("KEYPAD_MEMMULTIPLY")==0)
        return "<num>" + rc_intToString(SDLK_KP_MEMMULTIPLY);
    else if(sline.compare("KEYPAD_OCTAL")==0)
        return "<num>" + rc_intToString(SDLK_KP_OCTAL);
    else if(sline.compare("KEYPAD_PERCENT")==0)
        return "<num>" + rc_intToString(SDLK_KP_PERCENT);
    else if(sline.compare("KEYPAD_PERIOD")==0)
        return "<num>" + rc_intToString(SDLK_KP_PERIOD);
    else if(sline.compare("KEYPAD_PLUS")==0)
        return "<num>" + rc_intToString(SDLK_KP_PLUS);
    else if(sline.compare("KEYPAD_PLUSMINUS")==0)
        return "<num>" + rc_intToString(SDLK_KP_PLUSMINUS);
    else if(sline.compare("KEYPAD_POWER")==0)
        return "<num>" + rc_intToString(SDLK_KP_POWER);
    else if(sline.compare("KEYPAD_RIGHTBRACE")==0)
        return "<num>" + rc_intToString(SDLK_KP_RIGHTBRACE);
    else if(sline.compare("KEYPAD_RIGHTPAREN")==0)
        return "<num>" + rc_intToString(SDLK_KP_RIGHTPAREN);
    else if(sline.compare("KEYPAD_SPACE")==0)
        return "<num>" + rc_intToString(SDLK_KP_SPACE);
    else if(sline.compare("KEYPAD_TAB")==0)
        return "<num>" + rc_intToString(SDLK_KP_TAB);
    else if(sline.compare("KEYPAD_VERTICALBAR")==0)
        return "<num>" + rc_intToString(SDLK_KP_VERTICALBAR);
    else if(sline.compare("KEYPAD_XOR")==0)
        return "<num>" + rc_intToString(SDLK_KP_XOR);
    else if(sline.compare("K_L")==0)
        return "<num>" + rc_intToString(SDLK_l);
    else if(sline.compare("K_L")==0)
        return "<num>" + rc_intToString(SDLK_l);
    else if(sline.compare("K_LALT")==0)
        return "<num>" + rc_intToString(SDLK_LALT);
    else if(sline.compare("K_LCTRL")==0)
        return "<num>" + rc_intToString(SDLK_LCTRL);
    else if(sline.compare("K_LEFT")==0)
        return "<num>" + rc_intToString(SDLK_LEFT);
    else if(sline.compare("K_LEFTBRACKET")==0)
        return "<num>" + rc_intToString(SDLK_LEFTBRACKET);
    else if(sline.compare("K_LGUI")==0)
        return "<num>" + rc_intToString(SDLK_LGUI);
    else if(sline.compare("K_LSHIFT")==0)
        return "<num>" + rc_intToString(SDLK_LSHIFT);
    else if(sline.compare("K_M")==0)
        return "<num>" + rc_intToString(SDLK_m);
    else if(sline.compare("K_MAIL")==0)
        return "<num>" + rc_intToString(SDLK_MAIL);
    else if(sline.compare("K_MEDIASELECT")==0)
        return "<num>" + rc_intToString(SDLK_MEDIASELECT);
    else if(sline.compare("K_MENU")==0)
        return "<num>" + rc_intToString(SDLK_MENU);
    else if(sline.compare("K_MINUS")==0)
        return "<num>" + rc_intToString(SDLK_MINUS);
    else if(sline.compare("K_MODESWITCH")==0)
        return "<num>" + rc_intToString(SDLK_MODE);
    else if(sline.compare("K_MUTE")==0)
        return "<num>" + rc_intToString(SDLK_MUTE);
    else if(sline.compare("K_N")==0)
        return "<num>" + rc_intToString(SDLK_n);
    else if(sline.compare("K_NUMLOCK")==0)
        return "<num>" + rc_intToString(SDLK_NUMLOCKCLEAR);
    else if(sline.compare("K_O")==0)
        return "<num>" + rc_intToString(SDLK_o);
    else if(sline.compare("K_OPER")==0)
        return "<num>" + rc_intToString(SDLK_OPER);
    else if(sline.compare("K_OUT")==0)
        return "<num>" + rc_intToString(SDLK_OUT);
    else if(sline.compare("K_P")==0)
        return "<num>" + rc_intToString(SDLK_p);
    else if(sline.compare("K_PAGEDOWN")==0)
        return "<num>" + rc_intToString(SDLK_PAGEDOWN);
    else if(sline.compare("K_PAGEUP")==0)
        return "<num>" + rc_intToString(SDLK_PAGEUP);
    else if(sline.compare("K_PASTE")==0)
        return "<num>" + rc_intToString(SDLK_PASTE);
    else if(sline.compare("K_PAUSE")==0)
        return "<num>" + rc_intToString(SDLK_PAUSE);
    else if(sline.compare("K_POWER")==0)
        return "<num>" + rc_intToString(SDLK_POWER);
    else if(sline.compare("K_PRINTSCREEN")==0)
        return "<num>" + rc_intToString(SDLK_PRINTSCREEN);
    else if(sline.compare("K_PRIOR")==0)
        return "<num>" + rc_intToString(SDLK_PRIOR);
    else if(sline.compare("K_Q")==0)
        return "<num>" + rc_intToString(SDLK_q);
    else if(sline.compare("K_R")==0)
        return "<num>" + rc_intToString(SDLK_r);
    else if(sline.compare("K_RALT")==0)
        return "<num>" + rc_intToString(SDLK_RALT);
    else if(sline.compare("K_RCTRL")==0)
        return "<num>" + rc_intToString(SDLK_RCTRL);
    else if(sline.compare("K_RETURN")==0)
        return "<num>" + rc_intToString(SDLK_RETURN);
    else if(sline.compare("K_RETURN2")==0)
        return "<num>" + rc_intToString(SDLK_RETURN2);
    else if(sline.compare("K_RGUI")==0)
        return "<num>" + rc_intToString(SDLK_RGUI);
    else if(sline.compare("K_RIGHT")==0)
        return "<num>" + rc_intToString(SDLK_RIGHT);
    else if(sline.compare("K_RIGHTBRACKET")==0)
        return "<num>" + rc_intToString(SDLK_RIGHTBRACKET);
    else if(sline.compare("K_RSHIFT")==0)
        return "<num>" + rc_intToString(SDLK_RSHIFT);
    else if(sline.compare("K_S")==0)
        return "<num>" + rc_intToString(SDLK_s);
    else if(sline.compare("K_SCROLLLOCK")==0)
        return "<num>" + rc_intToString(SDLK_SCROLLLOCK);
    else if(sline.compare("K_SELECT")==0)
        return "<num>" + rc_intToString(SDLK_SELECT);
    else if(sline.compare("K_SEMICOLON")==0)
        return "<num>" + rc_intToString(SDLK_SEMICOLON);
    else if(sline.compare("K_SEPARATOR")==0)
        return "<num>" + rc_intToString(SDLK_SEPARATOR);
    else if(sline.compare("K_SLASH")==0)
        return "<num>" + rc_intToString(SDLK_SLASH);
    else if(sline.compare("K_SLEEP")==0)
        return "<num>" + rc_intToString(SDLK_SLEEP);
    else if(sline.compare("K_SPACE")==0)
        return "<num>" + rc_intToString(SDLK_SPACE);
    else if(sline.compare("K_STOP")==0)
        return "<num>" + rc_intToString(SDLK_STOP);
    else if(sline.compare("K_SYSREQ")==0)
        return "<num>" + rc_intToString(SDLK_SYSREQ);
    else if(sline.compare("K_T")==0)
        return "<num>" + rc_intToString(SDLK_t);
    else if(sline.compare("K_TAB")==0)
        return "<num>" + rc_intToString(SDLK_TAB);
    else if(sline.compare("K_THOUSANDSEPARATOR")==0)
        return "<num>" + rc_intToString(SDLK_THOUSANDSSEPARATOR);
    else if(sline.compare("K_U")==0)
        return "<num>" + rc_intToString(SDLK_u);
    else if(sline.compare("K_UNDO")==0)
        return "<num>" + rc_intToString(SDLK_UNDO);
    else if(sline.compare("K_UNKNOWN")==0)
        return "<num>" + rc_intToString(SDLK_UNKNOWN);
    else if(sline.compare("K_UP")==0)
        return "<num>" + rc_intToString(SDLK_UP);
    else if(sline.compare("K_V")==0)
        return "<num>" + rc_intToString(SDLK_v);
    else if(sline.compare("K_VOLUMEDOWN")==0)
        return "<num>" + rc_intToString(SDLK_VOLUMEDOWN);
    else if(sline.compare("K_VOLUMEUP")==0)
        return "<num>" + rc_intToString(SDLK_VOLUMEUP);
    else if(sline.compare("K_W")==0)
        return "<num>" + rc_intToString(SDLK_w);
    else if(sline.compare("K_WWW")==0)
        return "<num>" + rc_intToString(SDLK_WWW);
    else if(sline.compare("K_X")==0)
        return "<num>" + rc_intToString(SDLK_x);
    else if(sline.compare("K_Y")==0)
        return "<num>" + rc_intToString(SDLK_y);
    else if(sline.compare("K_Z")==0)
        return "<num>" + rc_intToString(SDLK_z);
    else if(sline.compare("WINDOWPOS_CENTERED")==0)
        return "<num>" + rc_intToString(SDL_WINDOWPOS_CENTERED);
    else if(sline.compare("HAT_RIGHT")==0)
        return "<num>" + rc_intToString(SDL_HAT_RIGHT);
    else if(sline.compare("HAT_LEFT")==0)
        return "<num>" + rc_intToString(SDL_HAT_LEFT);
    else if(sline.compare("HAT_UP")==0)
        return "<num>" + rc_intToString(SDL_HAT_UP);
    else if(sline.compare("HAT_DOWN")==0)
        return "<num>" + rc_intToString(SDL_HAT_DOWN);
    else if(sline.compare("HAT_LEFTUP")==0)
        return "<num>" + rc_intToString(SDL_HAT_LEFTUP);
    else if(sline.compare("HAT_LEFTDOWN")==0)
        return "<num>" + rc_intToString(SDL_HAT_LEFTDOWN);
    else if(sline.compare("HAT_RIGHTUP")==0)
        return "<num>" + rc_intToString(SDL_HAT_RIGHTUP);
    else if(sline.compare("HAT_RIGHTDOWN")==0)
        return "<num>" + rc_intToString(SDL_HAT_RIGHTDOWN);
    else if(sline.compare("HAT_CENTERED")==0)
        return "<num>" + rc_intToString(SDL_HAT_CENTERED);
}

int rc_isSpecialCharacter(std::string sline)
{
    if(sline.compare(" ")==0)
    {
        return 1;
    }
    else if(sline.compare("\t")==0)
    {
        return 1;
    }
    else if(sline.compare("\n")==0)
    {
        return 1;
    }
    else if(sline.compare("\r")==0)
    {
        return 1;
    }
    else if(sline.compare("\b")==0)
    {
        return 1;
    }
    else if(sline.compare("`")==0)
    {
        return 1;
    }
    else if(sline.compare("~")==0)
    {
        return 1;
    }
    else if(sline.compare("!")==0)
    {
        return 1;
    }
    else if(sline.compare("@")==0)
    {
        return 1;
    }
    else if(sline.compare("#")==0)
    {
        return 1;
    }
    else if(sline.compare("%")==0)
    {
        return 1;
    }
    else if(sline.compare("^")==0)
    {
        return 1;
    }
    else if(sline.compare("&")==0)
    {
        return 1;
    }
    else if(sline.compare("*")==0)
    {
        return 1;
    }
    else if(sline.compare("(")==0)
    {
        return 1;
    }
    else if(sline.compare(")")==0)
    {
        return 1;
    }
    else if(sline.compare("-")==0)
    {
        return 1;
    }
    else if(sline.compare("=")==0)
    {
        return 1;
    }
    else if(sline.compare("+")==0)
    {
        return 1;
    }
    else if(sline.compare(",")==0)
    {
        return 1;
    }
    else if(sline.compare(".")==0)
    {
        return 1;
    }
    else if(sline.compare("/")==0)
    {
        return 1;
    }
    else if(sline.compare("<")==0)
    {
        return 1;
    }
    else if(sline.compare(">")==0)
    {
        return 1;
    }
    else if(sline.compare("?")==0)
    {
        return 1;
    }
    else if(sline.compare(";")==0)
    {
        return 1;
    }
    else if(sline.compare(":")==0)
    {
        return 1;
    }
    else if(sline.compare("'")==0)
    {
        return 1;
    }
    else if(sline.compare("\"")==0)
    {
        return 1;
    }
    else if(sline.compare("[")==0)
    {
        return 1;
    }
    else if(sline.compare("]")==0)
    {
        return 1;
    }
    else if(sline.compare("\\")==0)
    {
        return 1;
    }
    else if(sline.compare("{")==0)
    {
        return 1;
    }
    else if(sline.compare("}")==0)
    {
        return 1;
    }
    else if(sline.compare("|")==0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

bool rc_isNumber(string sline)
{
    //cout<<"debg: "<<sline<<"\n";
    for(int i = 0; i < sline.length(); i++)
    {
        if(sline.substr(i,1).compare("0")!=0 && sline.substr(i,1).compare("1")!=0 && sline.substr(i,1).compare("2")!=0 && sline.substr(i,1).compare("3")!=0 && sline.substr(i,1).compare("4")!=0
           && sline.substr(i,1).compare("5")!=0 && sline.substr(i,1).compare("6")!=0 && sline.substr(i,1).compare("7")!=0 && sline.substr(i,1).compare("8")!=0
           && sline.substr(i,1).compare("9")!=0)
        {
            //cout<<"debug: "<<sline.substr(i,1) <<"\n";
            return false;
        }
    }
    return true;
}

bool rc_isValidIdentifier(string sline)
{
    sline = StringToUpper(sline.substr(0,1));
    if(sline.compare("_") == 0 || sline.compare("A") == 0 || sline.compare("B") == 0 || sline.compare("C") == 0 || sline.compare("D") == 0 || sline.compare("E") == 0 ||
       sline.compare("F") == 0 || sline.compare("G") == 0 || sline.compare("H") == 0 || sline.compare("I") == 0 || sline.compare("J") == 0 || sline.compare("K") == 0 ||
       sline.compare("L") == 0 || sline.compare("M") == 0 || sline.compare("N") == 0 || sline.compare("O") == 0 || sline.compare("P") == 0 || sline.compare("Q") == 0 ||
       sline.compare("R") == 0 || sline.compare("S") == 0 || sline.compare("T") == 0 || sline.compare("U") == 0 || sline.compare("V") == 0 || sline.compare("W") == 0 ||
       sline.compare("X") == 0 || sline.compare("Y") == 0 || sline.compare("Z") == 0)
        return true;
    else
        return false;
}

int rc_tokenizer(std::string sline)
{
    sline = " " + sline + " ";
    //cout<<"super debug: "<<sline<<endl;
    std::string tokens = "";
    int spc_char = 0;
    bool isNumber = false;
    bool isOperator = true;
    bool isId = false;
    int currentIndex = 0;
    for(int i = 0; i < sline.length(); i++)
    {

        if(rc_isSpecialCharacter(sline.substr(i,1))==0 || sline.substr(i,1).compare(".")==0)
        {
            if(isOperator == true)
            {
                isOperator = false;

                if(rc_isNumber(sline.substr(i,1))==true)
                {
                    //tokens.append("<num>");
                    rc_tokens[currentIndex] = "<num>";
                    //currentIndex++;
                    isNumber = true;
                }
                else
                {
                    //tokens.append("<id>");
                    rc_tokens[currentIndex] = "<id>";
                    //currentIndex++;
                    isNumber = false;
                }
            }
            //tokens.append(sline.substr(i,1));
            if(isNumber == true && rc_isNumber(sline.substr(i,1))== false && sline.substr(i,1).compare(".")!=0)
            {
                cout<<"invalid number\n";
                return 1;
            }

            if(rc_tokens[currentIndex].compare("<id>")==0 && rc_isValidIdentifier(sline.substr(i))==false)
            {
                cout << "invalid identifier in: " << sline << endl;
                cout << "invalid character \"" << (int)sline.substr(i).c_str()[0] << "\"" << endl;
                return 1;
            }

            rc_tokens[currentIndex].append(StringToUpper(sline.substr(i,1)));
            //currentIndex++;
            //i--;
        }
        else if(sline.substr(i,1).compare("\r")==0)
        {
            continue;
        }
        else if(sline.substr(i,2).compare("<=")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
            //tokens.append("<pwr>");
            rc_tokens[currentIndex] = "<less_equal>";
            currentIndex++;
            i++;
        }
        else if(sline.substr(i,2).compare(">=")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
            //tokens.append("<pwr>");
            rc_tokens[currentIndex] = "<greater_equal>";
            currentIndex++;
            i++;
        }
        else if(sline.substr(i,2).compare("<>")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
            //tokens.append("<pwr>");
            rc_tokens[currentIndex] = "<not_equal>";
            currentIndex++;
            i++;
        }
        else if(sline.substr(i,1).compare(",")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
            //tokens.append("<pwr>");
            rc_tokens[currentIndex] = "<sep>";
            currentIndex++;
        }
        else if(sline.substr(i,1).compare("&")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
            //tokens.append("<pwr>");
            rc_tokens[currentIndex] = "<str_append>";
            currentIndex++;
        }
        else if(sline.substr(i,1).compare("<")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
            //tokens.append("<pwr>");
            rc_tokens[currentIndex] = "<less>";
            currentIndex++;
        }
        else if(sline.substr(i,1).compare(">")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
            //tokens.append("<pwr>");
            rc_tokens[currentIndex] = "<greater>";
            currentIndex++;
        }
        else if(sline.substr(i,1).compare(" ")==0 || sline.substr(i,1).compare("\t")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
        }
        else if(sline.substr(i,1).compare("^")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
            //tokens.append("<pwr>");
            rc_tokens[currentIndex] = "<pwr>";
            currentIndex++;
        }
        else if(sline.substr(i,1).compare("*")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
            //tokens.append("<mul>");
            rc_tokens[currentIndex] = "<mul>";
            currentIndex++;
        }
        else if(sline.substr(i,1).compare("(")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isOperator = true;
            isNumber = false;
            //tokens.append("<par>");
            rc_tokens[currentIndex] = "<par>";
            currentIndex++;
        }
        else if(sline.substr(i,1).compare(")")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
            //tokens.append("</par>");
            rc_tokens[currentIndex] = "</par>";
            currentIndex++;
        }
        else if(sline.substr(i,1).compare("[")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
            //tokens.append("</par>");
            rc_tokens[currentIndex] = "<square>";
            currentIndex++;
        }
        else if(sline.substr(i,1).compare("]")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
            //tokens.append("</par>");
            rc_tokens[currentIndex] = "</square>";
            currentIndex++;
        }
        else if(sline.substr(i,1).compare("-")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
            //tokens.append("<sub>");
            rc_tokens[currentIndex] = "<sub>";
            currentIndex++;
        }
        else if(sline.substr(i,1).compare("=")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
            //tokens.append("<equal>");
            rc_tokens[currentIndex] = "<equal>";
            currentIndex++;
        }
        else if(sline.substr(i,1).compare("+")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
            //tokens.append("<add>");
            rc_tokens[currentIndex] = "<add>";
            currentIndex++;
        }
        else if(sline.substr(i,1).compare("/")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
            //tokens.append("<div>");
            rc_tokens[currentIndex] = "<div>";
            currentIndex++;
        }
        else if(sline.substr(i,1).compare("\"")==0)
        {
            if(isOperator == false)
            {
                currentIndex++;
            }
            isId = false;
            isOperator = true;
            isNumber = false;
            rc_tokens[currentIndex] = "<string>";
            for(i+=1; sline.substr(i,1).compare("\"")!=0; i++)
            {
                if(sline.substr(i,1).compare("\\")==0)
                {
                    if(sline.substr(i,2).compare("\\\\")==0)
                    {
                        rc_tokens[currentIndex] += "\\";
                        i++;
                        continue;
                    }
                    if(sline.substr(i,2).compare("\\t")==0)
                    {
                        rc_tokens[currentIndex] += "\t";
                        i++;
                        continue;
                    }
                    if(sline.substr(i,2).compare("\\n")==0)
                    {
                        rc_tokens[currentIndex] += "\n";
                        i++;
                        continue;
                    }
                    if(sline.substr(i,2).compare("\\b")==0)
                    {
                        rc_tokens[currentIndex] += "\b";
                        i++;
                        continue;
                    }
                    if(sline.substr(i,2).compare("\\q")==0)
                    {
                        rc_tokens[currentIndex] += "\"";
                        i++;
                        continue;
                    }
                    if(sline.substr(i,2).compare("\\r")==0)
                    {
                        rc_tokens[currentIndex] += "\r";
                        i++;
                        continue;
                    }
                    continue;
                }
                rc_tokens[currentIndex] += sline.substr(i,1);
            }
            currentIndex++;
        }
        else if(sline.substr(i,1).compare("'")==0)
        {
            rc_tokens_count = currentIndex;
            //cout << "Comment: " << sline.substr(i) << " -> " << rc_tokens_count << endl; string s; cin >> s;
            return 0;
        }
        else
        {
            cout<<"illegal character\n";
            return 1;
        }
    }
    rc_tokens_count = currentIndex;

    string scanId = "";

    for(int i = 0; i < rc_tokens_count; i++)
    {
        scanId = rc_tokens[i].substr(rc_tokens[i].find_first_of(">")+1);
        if(rc_isKeyword(scanId) && rc_tokens[i].substr(0,4).compare("<id>")==0)
        {
            rc_tokens[i] = rc_keywordToken(scanId);
        }
    }

    return 0;
}

int rc_str_findLastOf(string strToCheck, string sts)
{
    int last = strToCheck.length();
    for(int i = 0; i < strToCheck.length(); i++)
    {
        if(strToCheck.substr(i, sts.length()).compare(sts)==0)
        {
            last = i;
            //cout<<"dbg: "<<last<<"\n";
        }
    }
    return last;
}

int rc_str_findFirstOf(string strToCheck, string sts)
{
    int first = strToCheck.length();
    for(int i = 0; i < strToCheck.length(); i++)
    {
        if(strToCheck.substr(i, sts.length()).compare(sts)==0)
        {
            return i;
            //cout<<"dbg: "<<last<<"\n";
        }
    }
    return first;
}

int rc_getLastTokenOf(string sts)
{
    int last = -1;
    for(int i = 0; i < rc_tokens_count; i++)
    {
        if(rc_tokens[i].substr(0,sts.length()).compare(sts)==0)
        {
            last = i;
        }
    }
    return last;
}

int rc_setIDType(int id_index, string id_type)
{
    int n = 0;
    string s_id = "";
    for(int i = 0; i < rc_id[id_index].length(); i++)
    {
        if(rc_id[id_index].substr(i,1).compare(" ")==0)
        {
            n++;
            s_id += " ";
        }
        else if(n == 1 && rc_id[id_index].substr(i,1).compare(" ")!=0)
        {
            s_id += id_type;
            string s = rc_id[id_index].substr(i);
            i += s.find_first_of(" ")-1;
            n++;
        }
        else
        {
            s_id += rc_id[id_index].substr(i,1);
        }
    }
    rc_id[id_index] = s_id;
    //cout << "RC_SET_ID = ~" << s_id << "~" << endl;
    return 0;
}

string rc_searchID(string id)
{
    int s_loop = 0;
    string search_scope = relative_scope;

    //int dbg1=0;
    //cout << "start scope = " << search_scope << endl;

    while(s_loop==0)
    {
        for(int i = 0; i < rc_id_count; i++)
        {
            if(rc_id[i].substr(0,rc_id[i].find_first_of(" ")).compare(search_scope+"."+id)==0)
            {
                search_index = i;
                return search_scope+"."+id;
            }
        }

        if(search_scope.compare("PROGRAM")==0)
        {
            //cout << "SEARCH = " << search_scope << "." << id << endl;
            s_loop = 1;
        }
        else
        {
            //cout << "current search scope = " << search_scope << endl;
            search_scope = search_scope.substr(0,search_scope.find_last_of("."));
            //cout << "new search scope = " << search_scope << endl;
        }
    }
    return "";
}

string rc_getID(int index)
{
    if(index < rc_id_count)
    {
        return rc_id[index].substr(0,rc_id[index].find_first_of(" "));
    }

    return "";
}

string rc_getID_data(string id, string id_query)
{
    int id_index = 0;
    int line_item = 0;
    int current_count = 0;
    string current_item = "";

    //cout << "\n^^" << id << "\n\n";

    for(id_index = 0; id_index < rc_id_count; id_index++)
    {
        if(rc_id[id_index].substr(0,rc_id[id_index].find_first_of(" ")).compare(id)==0)
        {
            //cout << "CHECK1 --- " << rc_id[id_index].substr(rc_getID(id_index).find_last_of(".")+1, id.length()) << " " << id_query << " " << id << endl;
            rc_getID_index = id_index;
            if(id_query.compare("id_type") == 0)
            {
                line_item = 2;
                //cout << "RIGHT TRACK\n";
            }
            else if(id_query.compare("id_address") == 0)
            {
                line_item = 3;
            }
            else if(id_query.compare("id_scope") == 0)
            {
                line_item = 4;
            }
            else if(id_query.compare("id_dim") == 0 || id_query.compare("id_args")==0)
            {
                line_item = 5;
            }
            else if(id_query.compare("id_index") == 0)
            {
                line_item = 6;
            }
            else if(id_query.compare("id_overload") == 0)
            {
                line_item = 7;
            }
            else
            {
                cout << "getID_data() invalid search\n";
                return "";
            }

            if(line_item == 0)
            {
                cout << "--" << id_query <<"-- not a valid query\n";
                return "";
            }
            else
            {
                //cout << "line_item == " << line_item << endl;
            }

            for(int i = 0; i < rc_id[id_index].length(); i++)
            {
                if(rc_id[id_index].substr(i,1).compare(" ") == 0)
                {
                    current_count++;
                    //cout<<"ITEM "<<current_count<<" = "<<current_item<<endl;

                    if(current_count == line_item)
                    {
                        //cout<<"CI: "<<current_count<<" "<<current_item<<endl;
                        //cout << i << " ~line item = " << line_item << "\ncurrent item = " << current_item << endl;
                        return current_item;
                    }
                    else
                    {
                        //cout << i << " ***line item = " << line_item << "\ncurrent item = " << current_item << endl;
                        current_item = "";
                    }
                }
                else
                {
                    current_item += rc_id[id_index].substr(i,1);
                    //cout<<"tr: "<<current_item<<endl;
                }

            }

        }
        else
        {
            //cout << " CHECK<ARBY'S> = " << rc_id[id_index].substr(0,rc_id[id_index].find_first_of(" ")) << " != " << id << endl;
        }
    }

    if(current_item.compare("")!=0)
    {
        //cout << "%line item = " << line_item << "\ncurrent item = " << current_item << endl;
        //return current_item;
    }

    //cout << "ID_QUERY "<< id_query << " for ID( " << id << " ) does not match " << current_item << " != item#" << rc_intToString(line_item) << "\n";
    return "";
}

int rc_evalPtr()
{
    //cout <<"eval ptr\n";
    string eid = "";
    string eid_type = "";
    unsigned int eid_addr = 0;
    unsigned int eid_dim = 0;
    unsigned int eid_rc_index = 0;
    string f_arg[99];
    unsigned int f_token[99];
    //cout << rc_tokens[0] << endl;
    int f_scope = 0;
    for(int i = 0; i < rc_tokens_count; i++)
    {
        if(rc_tokens[i].substr(0,4).compare("<id>")==0)
        {
            eid = rc_searchID(rc_tokens[i].substr(4));
            eid_type = rc_getID_data(eid,"id_type");
            if(eid_type.compare("#F")==0 || eid_type.compare("$F")==0 || eid_type.compare("#P")==0 || eid_type.compare("$P")==0)
            {
                eid_addr = atoi(rc_getID_data(eid,"id_address").c_str());
                eid_rc_index = rc_getID_index;
                eid_dim = atoi(rc_getID_data(eid,"id_dim").c_str());
                f_scope = 1;
                int f_block_start = i + 2;
                for(int n = 0; n < eid_dim; n++)
                {
                    f_arg[n] = "";
                    for(int ni = f_block_start; ni < rc_tokens_count; ni++) //ni = i+2 because i+1 should be <par>
                    {
                        f_block_start++;

                        if(rc_tokens[ni].compare("<par>")==0)
                            f_scope++;

                        if(rc_tokens[ni].compare("</par>")==0)
                            f_scope--;

                        if(f_scope == 1 && rc_tokens[ni].compare("<sep>")==0)
                            break;

                        if(f_scope == 0)
                            break;

                        if(f_arg[n].compare("")==0 && rc_tokens[ni].substr(0,4).compare("<id>")==0)
                        {
                            //cout << "TESTING PRINT in evalptr 1\n";
                            f_arg[n] = rc_searchID(rc_tokens[ni].substr(4));
                            //cout << "F_ARG = " << f_arg[n] << endl;
                            f_token[n] = ni;
                            if(f_arg[n].compare("")==0)
                            {
                                f_arg[n] = rc_searchID(rc_tokens[ni].substr(4)+"$");
                                if(f_arg[n].compare("")==0)
                                {
                                    cout << rc_tokens[ni].substr(4) << " does not exist\n";
                                    //string s; cin >> s;
                                    return 2;
                                }
                            }
                        }
                        else
                            f_arg[n] += rc_tokens[ni];

                        //cout << "f_arg = " << f_arg[n] << endl;

                    }
                }
                //cout << eid_addr << "  " << eid_rc_index << "  " << eid_dim << endl;
                unsigned int arg_num = 0;
                string arg_id = "";
                string arg_type = "";
                //cout << "TESTING PRINT at the begining of second FOR\n" << eid_dim << endl;

                for(int n = eid_rc_index+1; n < rc_id_count; n++)
                {
                    if(arg_num == eid_dim)
                        break;

                    //cout << ">>" << n << endl;
                    arg_id = rc_id[n].substr(0, rc_id[n].find_first_of(" "));
                    //cout << arg_id << "~" << endl;
                    arg_type = rc_getID_data(arg_id, "id_type");
                    if(arg_type.length()>1)
                    if(arg_type.substr(1,1).compare("&")==0)
                    {
                        //cout << rc_id[n].substr(0, rc_id[n].find_first_of(" ")) << " points to " << f_arg[arg_num] << endl;
                        rc_tokens[f_token[arg_num]] = "<ref>" + f_arg[arg_num];
                    }
                    if(rc_getID_data(arg_id, "id_overload").compare("~")!=0)
                        arg_num++;
                }
                //cout << "TESTING PRINT at the end of second FOR\n";
            }
            else
                eid_addr = atoi(rc_getID_data(eid,"id_index").c_str());
            //cout << "Found ID " << eid << " Type=" << eid_type << " ADDRESS=" << eid_addr << endl<<endl;
        }
        else
        {
            //cout <<"NXT---"<<rc_tokens[i]<<endl;
        }
    }
    return 0;
}

string rc_getNextToken(unsigned int nt_start, unsigned int nt_end)
{
    string next_token = "";
    for(int i = nt_start; i <= nt_end; i++)
    {
        if(rc_tokens[i].substr(0,1).compare("<")==0)
        {
            next_token = rc_tokens[i];
            break;
        }
    }
    return next_token;
}

int rc_preParse(int l_expr = 0)
{
    string id = "";
    string id_type = "";
    int s_addr = 0;
    //cout << "Check 1\n";
    if(rc_evalPtr()==2)
    {
        cout << "Byref Error\n";
        //string s; cin >> s;
        return 2;
    }
    //cout << "Check 2\n";
    //for(int i = 0; i < rc_tokens_count; i++)
    //    cout << "RCT " << i << ": " << rc_tokens[i] << endl;
    for(int i = 0; i < rc_tokens_count; i++)
    {
        if(rc_tokens[i].substr(0,8).compare("<string>")==0)
        {
            s_addr = rc_data_offset;
            for(int j = 8; j < rc_tokens[i].length(); j++)
            {
                RC_DATA_SEGMENT[rc_data_offset] = rc_tokens[i].substr(j,1)[0];
                rc_data_offset++;
            }
            RC_DATA_SEGMENT[rc_data_offset] = 0;
            rc_data_offset++;

            tmp[tmp_count] = "mov$ s" + rc_intToString(s_index) + " @" + rc_intToString(s_addr);
            tmp_count++;
            rc_tokens[i] = "<vm_var>s" + rc_intToString(s_index);
            s_index++;
        }
        else if(rc_tokens[i].substr(0,4).compare("<id>")==0)
        {
            id = rc_searchID(rc_tokens[i].substr(4));

            if(id.compare("")==0)
                id = rc_searchID(rc_tokens[i].substr(4)+"$");

            if(id.compare("")!=0)
            {
                id_type = rc_getID_data(id,"id_type");
                s_addr = atoi(rc_getID_data(id, "id_index").c_str());
                //cout << id << " S_ADDR = " << s_addr << endl << endl;
                if(id_type.compare("$")==0)
                {
                    tmp[tmp_count] = "mov$ s" + rc_intToString(s_index) + " " + id;
                    tmp_count++;
                    rc_tokens[i] = "<vm_var>s" + rc_intToString(s_index);
                    s_index++;
                }
                else if(id_type.compare("#")==0)
                {
                    tmp[tmp_count] = "mov m" + rc_intToString(m_index) + " " + id;
                    tmp_count++;
                    rc_tokens[i] = "<vm_var>m" + rc_intToString(m_index);
                    m_index++;
                }
                else if( (id_type.compare("#&")==0 || id_type.compare("$&")==0) && (rc_tokens[i+1].compare("<par>")==0 || rc_tokens[i+1].compare("<square>")==0))
                {
                    int isSqr = 0;
                    if(rc_tokens[i+1].compare("<square>")!=0 && rc_tokens[i+1].compare("<par>")!=0)
                    {
                        cout << "Expected [ \n";
                        return 2;
                    }
                    if(rc_tokens[i+1].compare("<square>")==0)
                    {
                        isSqr = 1;
                        rc_tokens[i+1] = "<par>";
                    }

                    int n = i+1;
                    if(isSqr == 1)
                    {
                        for(; n < rc_tokens_count; n++)
                        {
                            if(rc_tokens[n].compare("</square>")==0)
                                break;
                        }
                        if(rc_tokens[n].compare("</square>")==0)
                            rc_tokens[n] = "</par>";
                        else
                        {
                            cout << "Expected ] \n";
                            return 2;
                        }
                    }
                }
                else if(id_type.compare("$&")==0 && l_expr == 2)
                {
                    tmp[tmp_count] = "mov$ s" + rc_intToString(s_index) + " " + id;
                    tmp_count++;
                    rc_tokens[i] = "<vm_var>s" + rc_intToString(s_index);
                    s_index++;
                }
                else if(id_type.compare("#&")==0 && l_expr == 1)
                {
                    tmp[tmp_count] = "mov m" + rc_intToString(m_index) + " " + id;
                    tmp_count++;
                    rc_tokens[i] = "<vm_var>m" + rc_intToString(m_index);
                    m_index++;
                }
                else if(id_type.substr(1,1).compare("A")==0)
                {
                    //cout << "parse ptr DBG\n"; string s; cin >> s;
                    int isSqr = 0;
                    if(rc_tokens[i+1].compare("<square>")!=0 && rc_tokens[i+1].compare("<par>")!=0)
                    {
                        cout << "Expected [ \n";
                        return 2;
                    }
                    if(rc_tokens[i+1].compare("<square>")==0)
                    {
                        isSqr = 1;
                        rc_tokens[i+1] = "<par>";
                    }

                    int n = i+1;
                    if(isSqr == 1)
                    {
                        for(; n < rc_tokens_count; n++)
                        {
                            if(rc_tokens[n].compare("</square>")==0)
                                break;
                        }
                        if(rc_tokens[n].compare("</square>")==0)
                            rc_tokens[n] = "</par>";
                        else
                        {
                            cout << "Expected ] \n";
                            return 2;
                        }
                    }
                }
                else if(id_type.substr(1,1).compare("F")==0 || id_type.substr(1,1).compare("P")==0)
                {
                    //cout << "parse ptr DBG\n"; string s; cin >> s;
                    int isSqr = 0;
                    if(rc_tokens[i+1].compare("<par>")!=0)
                    {
                        cout << "Expected ( \n";
                        return 2;
                    }
                }
            }
            else
            {
                cout << "*Identifier not defined: <-----(" << rc_tokens[i] << ")----->" << endl;
                return 2;
            }
        }
    }

    //for(int i = 0; i < rc_tokens_count; i++)
    //    cout<<"RC_TK " << i << ": " << rc_tokens[i] << endl;
    return 0;
}

int rc_mathParser(int m_init = 0, int s_init = 0)
{
    //cout << "RC_MPAR\n"; string s; cin >> s;
    bool parseLoop = true;
    string parseString = "";
    int currentIndex = tmp_count;
    int t_index = 0;
    m_index = m_init;
    s_index = s_init;

    int start_clear = 0;
    int end_clear = 0;
    int isIntern = 0;

    string v1 = "";
    string v1_type = "";
    string v2 = "";
    string v2_type = "";

    int op_flag = 0;

    int currentBlock_start = 0;
    int currentBlock_end = 0;

    int current_op = 0;

    int equal_op = rc_equal_flag;

    int f_index = 0;

    string id;
    string id_type;  //add #f 2 x # 0 0 y # 0 0
    int id_dim;
    string arg_id[99];
    string arg_type[99];
    string arg_value[99];

    int arg_count = 0;
    string tmp_id = "";

    int n_num = 0;

    string vms1 = "";
    string vms2 = "";

    string _tmp_space = "";

    int mi_place = 0;
    int si_place = 0;

    unsigned int ns_use = 0;
    unsigned int ss_use = 0;

    while(parseLoop)
    {
        //parenthesis
        rc_isString = false;
        t_index = rc_getLastTokenOf("<par>");
        if(t_index >= 0)
        {
            currentBlock_start = t_index;
            for(int i = t_index; rc_tokens[i].compare("</par>")!=0 && i < rc_tokens_count; i++)
            {
                currentBlock_end = i+1;
            }

            if(rc_tokens[currentBlock_end].compare("</par>")!=0)
            {
                cout<<"expected ) but found end of line"<<endl;
                return 2;
            }
        }
        else
        {
            currentBlock_start = 0;
            currentBlock_end = rc_tokens_count;
        }

        //set negative numbers
        n_num = 0;
        for(int i = currentBlock_start; i < currentBlock_end; i++)
        {
             if(rc_tokens[i].compare("<sub>")==0 && n_num == 0 && i < currentBlock_end-1)
             {
                 if(rc_tokens[i+1].substr(0,5).compare("<num>")==0)
                 {
                     rc_tokens[i] = "";
                     rc_tokens[i+1] = "<num>-" + rc_tokens[i+1].substr(5);
                     //cout << "NG ---- " << rc_tokens[i+1] << endl;
                 }
                 else
                 {
                     cout << "expected number\n\n";
                     return 2;
                 }
             }
             else if(rc_tokens[i].substr(0,4).compare("<id>")==0 || rc_tokens[i].substr(0,5).compare("<num>")==0 || rc_tokens[i].substr(0,8).compare("<vm_var>")==0
                     || rc_tokens[i].compare("")==0)
             {
                 //cout << "N_NUM1 ----- " << rc_tokens[i] << endl;
                 n_num = 1;
             }
             else
             {
                 //cout << "N_NUM2 ----- " << rc_tokens[i] << endl;
                 n_num = 0;
             }

        }
        //cout << "TITTIES" << endl;
        //for(int i = 0; i < rc_tokens_count; i++)
        //    cout << "rc_tokens[" << i << "] = " << rc_tokens[i] << endl;
        //cout << "FUCK DA BULLSHIT" << endl;

        //store all numbers in current block
        for(int i = currentBlock_start; i < currentBlock_end; i++)
        {
            if(rc_tokens[i].substr(0,5).compare("<num>")==0)
            {
                tmp[currentIndex] = "mov m" + rc_intToString(m_index) + " " + rc_tokens[i].substr(5);
                currentIndex++;
                rc_tokens[i] = "<vm_var>m" + rc_intToString(m_index);
                m_index++;
            }
        }

        //cout << "THIS IS STILL BULLSHIT" << endl;

        //check for string in current block; if a string is found the compiler treats the entire block as a string; checks for id$ also
        for(int i = currentBlock_start; i < currentBlock_end; i++)
        {
            if(rc_tokens[i].substr(0,8).compare("<string>")==0 || rc_tokens[i].substr(0,9).compare("<vm_var>s")==0)
            {
                rc_isString = true;
                //cout << "STR || STR_VM_VAR\n";
                //return 1;
            }
            else if(rc_tokens[i].substr(0,4).compare("<id>")==0)
            {
                id = rc_searchID(rc_tokens[i].substr(4));
                id_type = rc_getID_data(id, "id_type");
                if(id_type.substr(0,1).compare("$")==0)
                {
                    rc_isString = true;
                    //cout << "STR ID\n";
                    //return 1;
                }
            }
        }

        //cout << "What the HELL" << endl;

        //exponents
        op_flag = 0;
        for(int i = currentBlock_start; i < currentBlock_end; i++)
        {
            if(rc_tokens[i].substr(0,5).compare("<num>")==0)
            {
                if(op_flag == 0)
                {
                    v1 = rc_tokens[i].substr(5);
                    start_clear = i;
                }
                else
                {
                    v2 = rc_tokens[i].substr(5);
                    op_flag = 2;
                    end_clear = i;
                }
            }
            else if(rc_tokens[i].substr(0,4).compare("<id>")==0)
            {
                if(op_flag == 0)
                {
                    v1 = rc_searchID(rc_tokens[i].substr(4));
                    if(v1.compare("")==0)
                    {
                        cout << "Variable Does not Exist in Current Scope\n";
                        return 2;
                    }
                    v1_type = rc_getID_data(v1, "id_type");
                    if(v1_type.substr(0,1).compare("#")!=0)
                    {
                        v1 = "";
                    }
                    else
                    start_clear = i;
                }
                else
                {
                    v2 = rc_searchID(rc_tokens[i].substr(4));
                    if(v2.compare("")==0)
                    {
                        cout << "Variable Does not Exist in Current Scope\n";
                        return 2;
                    }
                    v2_type = rc_getID_data(v1, "id_type");
                    if(v2_type.substr(0,1).compare("#")!=0)
                    {
                        v2 = "";
                        op_flag = 0;
                    }
                    else
                    {
                        op_flag = 2;
                        end_clear = i;
                    }
                }
            }
            else if(rc_tokens[i].substr(0,8).compare("<vm_var>")==0)
            {
                if(op_flag == 0)
                {
                    if(rc_tokens[i].substr(8,1).compare("m")==0)
                    {
                        v1 = rc_tokens[i].substr(8);
                        start_clear = i;
                    }
                    else
                    {
                        v1 = "";
                    }
                }
                else
                {
                    if(rc_tokens[i].substr(8,1).compare("m")==0)
                    {
                        v2 = rc_tokens[i].substr(8);
                        op_flag = 2;
                        end_clear = i;
                    }
                    else
                    {
                        v1 = "";
                        v2 = "";
                        op_flag = 0;
                    }
                }
            }
            else if(rc_tokens[i].substr(0,5).compare("<pwr>")==0 && v1.compare("")!=0)
            {
                op_flag = 1;
                current_op = 1;
                //cout<<"op 1"<<endl;
            }

            if(op_flag == 2)
            {
                //tmp[currentIndex] = "mov m" + rc_intToString(m_index) + " " + v1;
                //currentIndex++;
                if(current_op == 1)
                {
                    tmp[currentIndex] = "pwr " + v1 + " " + v2;
                }

                currentIndex++;
                //cout<<tmp[currentIndex-2]<<endl;
                //cout<<tmp[currentIndex-1]<<endl;
                rc_tokens[start_clear] = "<vm_var>" + v1;
                for(int j = start_clear+1; j <= end_clear;j++)
                {
                    rc_tokens[j] = " ";
                }
                m_index++;
                op_flag = 0;
                i = currentBlock_start-1;

            }
        }

        //cout << "COME ON" << endl;

        //multiply, divide, and mod
        op_flag = 0;
        for(int i = currentBlock_start; i < currentBlock_end; i++)
        {
            if(rc_tokens[i].substr(0,5).compare("<num>")==0)
            {
                if(op_flag == 0)
                {
                    v1 = rc_tokens[i].substr(5);
                    start_clear = i;
                }
                else
                {
                    v2 = rc_tokens[i].substr(5);
                    op_flag = 2;
                    end_clear = i;
                }
            }
            else if(rc_tokens[i].substr(0,4).compare("<id>")==0)
            {
                if(op_flag == 0)
                {
                    v1 = rc_searchID(rc_tokens[i].substr(4));
                    if(v1.compare("")==0)
                    {
                        cout << "Variable Does not Exist in Current Scope\n";
                        return 2;
                    }
                    v1_type = rc_getID_data(v1, "id_type");
                    if(v1_type.substr(0,1).compare("#")!=0)
                    {
                        v1 = "";
                    }
                    else
                    start_clear = i;
                }
                else
                {
                    v2 = rc_searchID(rc_tokens[i].substr(4));
                    if(v2.compare("")==0)
                    {
                        cout << "Variable Does not Exist in Current Scope\n";
                        return 2;
                    }
                    v2_type = rc_getID_data(v1, "id_type");
                    if(v2_type.substr(0,1).compare("#")!=0)
                    {
                        v2 = "";
                        op_flag = 0;
                    }
                    else
                    {
                        op_flag = 2;
                        end_clear = i;
                    }
                }
            }
            else if(rc_tokens[i].substr(0,8).compare("<vm_var>")==0)
            {
                if(op_flag == 0)
                {
                    if(rc_tokens[i].substr(8,1).compare("m")==0)
                    {
                        v1 = rc_tokens[i].substr(8);
                        start_clear = i;
                    }
                    else
                    {
                        v1 = "";
                    }
                }
                else
                {
                    if(rc_tokens[i].substr(8,1).compare("m")==0)
                    {
                        v2 = rc_tokens[i].substr(8);
                        op_flag = 2;
                        end_clear = i;
                    }
                    else
                    {
                        v1 = "";
                        v2 = "";
                        op_flag = 0;
                    }
                }
            }
            else if(rc_tokens[i].substr(0,5).compare("<mul>")==0 && v1.compare("")!=0)
            {
                op_flag = 1;
                current_op = 1;
                //cout<<"op 1"<<endl;
            }
            else if(rc_tokens[i].substr(0,5).compare("<div>")==0 && v1.compare("")!=0)
            {
                op_flag = 1;
                current_op = 2;
            }
            else if(rc_tokens[i].substr(0,5).compare("<mod>")==0 && v1.compare("")!=0)
            {
                op_flag = 1;
                current_op = 3;
            }

            if(op_flag == 2)
            {
                //tmp[currentIndex] = "mov m" + rc_intToString(m_index) + " " + v1;
                //currentIndex++;
                if(current_op == 1)
                {
                    tmp[currentIndex] = "mul " + v1 + " " + v2;
                }
                else if(current_op == 2)
                {
                    tmp[currentIndex] = "div " + v1 + " " + v2;
                }
                else if(current_op == 3)
                {
                    tmp[currentIndex] = "mod " + v1 + " " + v2;
                }
                currentIndex++;
                //cout<<tmp[currentIndex-2]<<endl;
                //cout<<tmp[currentIndex-1]<<endl;
                rc_tokens[start_clear] = "<vm_var>" + v1;
                for(int j = start_clear+1; j <= end_clear;j++)
                {
                    rc_tokens[j] = " ";
                }
                m_index++;
                op_flag = 0;
                i = currentBlock_start-1;

            }
        }

        //add and subtract
        op_flag = 0;
        for(int i = currentBlock_start; i < currentBlock_end; i++)
        {
            if(rc_tokens[i].substr(0,5).compare("<num>")==0)
            {
                if(op_flag == 0)
                {
                    v1 = rc_tokens[i].substr(5);
                    start_clear = i;
                }
                else
                {
                    v2 = rc_tokens[i].substr(5);
                    op_flag = 2;
                    end_clear = i;
                }
            }
            else if(rc_tokens[i].substr(0,4).compare("<id>")==0)
            {
                if(op_flag == 0)
                {
                    v1 = rc_searchID(rc_tokens[i].substr(4));
                    if(v1.compare("")==0)
                    {
                        cout << "Variable Does not Exist in Current Scope\n";
                        return 2;
                    }
                    v1_type = rc_getID_data(v1, "id_type");
                    if(v1_type.substr(0,1).compare("#")!=0)
                    {
                        v1 = "";
                    }
                    else
                    start_clear = i;
                }
                else
                {
                    v2 = rc_searchID(rc_tokens[i].substr(4));
                    if(v2.compare("")==0)
                    {
                        cout << "Variable Does not Exist in Current Scope\n";
                        return 2;
                    }
                    v2_type = rc_getID_data(v1, "id_type");
                    if(v2_type.substr(0,1).compare("#")!=0)
                    {
                        v2 = "";
                        op_flag = 0;
                    }
                    else
                    {
                        op_flag = 2;
                        end_clear = i;
                    }
                }
            }
            else if(rc_tokens[i].substr(0,8).compare("<vm_var>")==0)
            {
                if(op_flag == 0)
                {
                    if(rc_tokens[i].substr(8,1).compare("m")==0)
                    {
                        v1 = rc_tokens[i].substr(8);
                        start_clear = i;
                    }
                    else
                    {
                        v1 = "";
                    }
                }
                else
                {
                    if(rc_tokens[i].substr(8,1).compare("m")==0)
                    {
                        v2 = rc_tokens[i].substr(8);
                        op_flag = 2;
                        end_clear = i;
                    }
                    else
                    {
                        v1 = "";
                        v2 = "";
                        op_flag = 0;
                    }
                }
            }
            else if(rc_tokens[i].substr(0,5).compare("<add>")==0 && v1.compare("")!=0)
            {
                op_flag = 1;
                current_op = 1;
                //cout<<"op 1"<<endl;
            }
            else if(rc_tokens[i].substr(0,5).compare("<sub>")==0 && v1.compare("")!=0)
            {
                op_flag = 1;
                current_op = 2;
            }

            if(op_flag == 2)
            {
                //tmp[currentIndex] = "mov m" + rc_intToString(m_index) + " " + v1;
                //currentIndex++;
                if(current_op == 1)
                {
                    tmp[currentIndex] = "add " + v1 + " " + v2;
                }
                else if(current_op == 2)
                {
                    tmp[currentIndex] = "sub " + v1 + " " + v2;
                }
                currentIndex++;
                //cout<<tmp[currentIndex-2]<<endl;
                //cout<<tmp[currentIndex-1]<<endl;
                rc_tokens[start_clear] = "<vm_var>" + v1;
                for(int j = start_clear+1; j <= end_clear;j++)
                {
                    rc_tokens[j] = " ";
                }
                m_index++;
                op_flag = 0;
                i = currentBlock_start-1;

            }
        }

        //bit-shift
        op_flag = 0;
        for(int i = currentBlock_start; i < currentBlock_end; i++)
        {
            if(rc_tokens[i].substr(0,5).compare("<num>")==0)
            {
                if(op_flag == 0)
                {
                    v1 = rc_tokens[i].substr(5);
                    start_clear = i;
                }
                else
                {
                    v2 = rc_tokens[i].substr(5);
                    op_flag = 2;
                    end_clear = i;
                }
            }
            else if(rc_tokens[i].substr(0,4).compare("<id>")==0)
            {
                if(op_flag == 0)
                {
                    v1 = rc_searchID(rc_tokens[i].substr(4));
                    if(v1.compare("")==0)
                    {
                        cout << "Variable Does not Exist in Current Scope\n";
                        return 2;
                    }
                    v1_type = rc_getID_data(v1, "id_type");
                    if(v1_type.substr(0,1).compare("#")!=0)
                    {
                        v1 = "";
                    }
                    else
                    start_clear = i;
                }
                else
                {
                    v2 = rc_searchID(rc_tokens[i].substr(4));
                    if(v2.compare("")==0)
                    {
                        cout << "Variable Does not Exist in Current Scope\n";
                        return 2;
                    }
                    v2_type = rc_getID_data(v1, "id_type");
                    if(v2_type.substr(0,1).compare("#")!=0)
                    {
                        v2 = "";
                        op_flag = 0;
                    }
                    else
                    {
                        op_flag = 2;
                        end_clear = i;
                    }
                }
            }
            else if(rc_tokens[i].substr(0,8).compare("<vm_var>")==0)
            {
                if(op_flag == 0)
                {
                    if(rc_tokens[i].substr(8,1).compare("m")==0)
                    {
                        v1 = rc_tokens[i].substr(8);
                        start_clear = i;
                    }
                    else
                    {
                        v1 = "";
                    }
                }
                else
                {
                    if(rc_tokens[i].substr(8,1).compare("m")==0)
                    {
                        v2 = rc_tokens[i].substr(8);
                        op_flag = 2;
                        end_clear = i;
                    }
                    else
                    {
                        v1 = "";
                        v2 = "";
                        op_flag = 0;
                    }
                }
            }
            else if(rc_tokens[i].substr(0,5).compare("<shl>")==0 && v1.compare("")!=0)
            {
                op_flag = 1;
                current_op = 1;
                //cout<<"op 1"<<endl;
            }
            else if(rc_tokens[i].substr(0,5).compare("<shr>")==0 && v1.compare("")!=0)
            {
                op_flag = 1;
                current_op = 2;
            }

            if(op_flag == 2)
            {
                //tmp[currentIndex] = "mov m" + rc_intToString(m_index) + " " + v1;
                //currentIndex++;
                if(current_op == 1)
                {
                    tmp[currentIndex] = "shl " + v1 + " " + v2;
                }
                else if(current_op == 2)
                {
                    tmp[currentIndex] = "shr " + v1 + " " + v2;
                }
                currentIndex++;
                //cout<<tmp[currentIndex-2]<<endl;
                //cout<<tmp[currentIndex-1]<<endl;
                rc_tokens[start_clear] = "<vm_var>" + v1;
                for(int j = start_clear+1; j <= end_clear;j++)
                {
                    rc_tokens[j] = " ";
                }
                m_index++;
                op_flag = 0;
                i = currentBlock_start-1;

            }
        }

        //string append
        op_flag = 0;
        if(rc_isString)
        for(int i = currentBlock_start; i < currentBlock_end; i++)
        {
            if(rc_tokens[i].substr(0,5).compare("<num>")==0)
            {
                if(op_flag == 0)
                {
                    tmp[currentIndex] = "str$ s" + rc_intToString(s_index) + " " + rc_tokens[i].substr(5);
                    currentIndex++;
                    v1 = "s"+rc_intToString(s_index);
                    s_index++;
                    start_clear = i;
                }
                else
                {
                    tmp[currentIndex] = "str$ s" + rc_intToString(s_index) + " " + rc_tokens[i].substr(5);
                    currentIndex++;
                    v2 = "s" + rc_intToString(s_index);
                    s_index++;
                    op_flag = 2;
                    end_clear = i;
                }
            }
            else if(rc_tokens[i].substr(0,4).compare("<id>")==0)
            {
                if(op_flag == 0)
                {
                    v1 = rc_searchID(rc_tokens[i].substr(4));
                    if(v1.compare("")==0)
                    {
                        cout << "Variable Does not Exist in Current Scope\n";
                        return 2;
                    }
                    v1_type = rc_getID_data(v1, "id_type");
                    if(v1_type.substr(0,1).compare("$")!=0)
                    {
                        tmp[currentIndex] = "str$ s" + rc_intToString(s_index) + " " + v1;
                        currentIndex++;
                        v1 = "s" + rc_intToString(s_index);
                        s_index++;
                    }
                    else
                    {
                        tmp[currentIndex] = "mov$ s" + rc_intToString(s_index) + " " + v1;
                        currentIndex++;
                        v1 = "s" + rc_intToString(s_index);
                        s_index++;
                    }
                    start_clear = i;
                }
                else
                {
                    v2 = rc_searchID(rc_tokens[i].substr(4));
                    if(v2.compare("")==0)
                    {
                        cout << "Variable Does not Exist in Current Scope\n";
                        return 2;
                    }
                    v2_type = rc_getID_data(v2, "id_type");
                    if(v2_type.substr(0,1).compare("$")!=0)
                    {
                        tmp[currentIndex] = "str$ s" + rc_intToString(s_index) + " " + v2;
                        currentIndex++;
                        v2 = "s" + rc_intToString(s_index);
                        s_index++;
                    }
                    else
                    {
                        tmp[currentIndex] = "mov$ s" + rc_intToString(s_index) + " " + v2;
                        currentIndex++;
                        v2 = "s" + rc_intToString(s_index);
                        s_index++;
                    }
                    op_flag = 2;
                    end_clear = i;
                }
            }
            else if(rc_tokens[i].substr(0,8).compare("<vm_var>")==0)
            {
                if(op_flag == 0)
                {
                    if(rc_tokens[i].substr(8,1).compare("s")==0)
                    {
                        v1 = rc_tokens[i].substr(8);
                    }
                    else
                    {
                        tmp[currentIndex] = "str$ s" + rc_intToString(s_index) + " " + rc_tokens[i].substr(8);
                        currentIndex++;
                        v1 = "s" + rc_intToString(s_index);
                        s_index++;
                    }
                    start_clear = i;
                }
                else
                {
                    if(rc_tokens[i].substr(8,1).compare("s")==0)
                    {
                        v2 = rc_tokens[i].substr(8);
                    }
                    else
                    {
                        tmp[currentIndex] = "str$ s" + rc_intToString(s_index) + " " + rc_tokens[i].substr(8);
                        currentIndex++;
                        v2 = "s" + rc_intToString(s_index);
                        s_index++;
                    }
                    //cout << "V1$ = " << v1 << endl;
                    //cout << "V2$ = " << v2 << endl;
                    //string dbgIn = ""; cin >> dbgIn;
                    op_flag = 2;
                    end_clear = i;
                }
            }
            else if(rc_tokens[i].substr(0,5).compare("<add>")==0 || rc_tokens[i].substr(0,12).compare("<str_append>")==0)
            {
                op_flag = 1;
                current_op = 1;
                //cout<<"op 1"<<endl;
            }
            else if(rc_tokens[i].substr(0,5).compare("<sub>")==0)
            {
                cout << "can't conduct subtract operation on string\n";
                return 2;
                //op_flag = 1;
                //current_op = 2;
            }

            if(op_flag == 2)
            {
                tmp[currentIndex] = "add$ " + v1 + " " + v2;
                currentIndex++;
                rc_tokens[start_clear] = "<vm_var>"+v1;
                for(int j = start_clear+1; j <= end_clear;j++)
                {
                    rc_tokens[j] = " ";
                }
                m_index++;
                op_flag = 0;
                i = currentBlock_start-1;

            }
        }

        //equality
        op_flag = 0;
        for(int i = currentBlock_start; i < currentBlock_end; i++)
        {
            if(rc_tokens[i].substr(0,5).compare("<num>")==0)
            {
                if(op_flag == 0)
                {
                    v1 = rc_tokens[i].substr(5);
                    start_clear = i;
                }
                else
                {
                    v2 = rc_tokens[i].substr(5);
                    op_flag = 2;
                    end_clear = i;
                }
            }
            else if(rc_tokens[i].substr(0,4).compare("<id>")==0)
            {
                if(op_flag == 0)
                {
                    v1 = rc_searchID(rc_tokens[i].substr(4));
                    if(v1.compare("")==0)
                    {
                        cout << "Variable Does not Exist in Current Scope\n";
                        return 2;
                    }
                    v1_type = rc_getID_data(v1, "id_type");
                    if(v1_type.substr(0,1).compare("#")!=0)
                    {
                        v1 = "";
                    }
                    else
                    start_clear = i;
                }
                else
                {
                    v2 = rc_searchID(rc_tokens[i].substr(4));
                    if(v2.compare("")==0)
                    {
                        cout << "Variable Does not Exist in Current Scope\n";
                        return 2;
                    }
                    v2_type = rc_getID_data(v1, "id_type");
                    if(v2_type.substr(0,1).compare("#")!=0)
                    {
                        v2 = "";
                        op_flag = 0;
                    }
                    else
                    {
                        op_flag = 2;
                        end_clear = i;
                    }
                }
            }
            else if(rc_tokens[i].substr(0,8).compare("<vm_var>")==0)
            {
                if(op_flag == 0)
                {
                    if(rc_tokens[i].substr(8,1).compare("m")==0)
                    {
                        v1 = rc_tokens[i].substr(8);
                        start_clear = i;
                    }
                    else if(rc_tokens[i].substr(8,1).compare("s")==0)
                    {
                        v1 = rc_tokens[i].substr(8);
                        start_clear = i;
                    }
                    else
                    {
                        v1 = "";
                    }
                }
                else
                {
                    if(rc_tokens[i].substr(8,1).compare("m")==0)
                    {
                        v2 = rc_tokens[i].substr(8);
                        //cout << "Check Equal op: " << v1 << " " << equal_op << " " <<v2 << endl;
                        //string a = ""; cin >> a;
                        op_flag = 2;
                        end_clear = i;
                    }
                    else if(rc_tokens[i].substr(8,1).compare("s")==0)
                    {
                        v2 = rc_tokens[i].substr(8);
                        op_flag = 2;
                        end_clear = i;
                    }
                    else
                    {
                        v1 = "";
                        v2 = "";
                        op_flag = 0;
                    }
                }
            }
            else if(rc_tokens[i].compare("<equal>")==0)
            {
                op_flag = 1;
                current_op = 1;
                if(v1.substr(0,1).compare("s")==0)
                    equal_op = 7;
                else
                    equal_op = 1;
                //cout<<"op 1"<<endl;
            }
            else if(rc_tokens[i].compare("<greater>")==0)
            {
                op_flag = 1;
                equal_op = 2;
            }
            else if(rc_tokens[i].compare("<less>")==0)
            {
                //cout << "****LESS OP_FLAG SET****" << endl;
                op_flag = 1;
                equal_op = 3;
            }
            else if(rc_tokens[i].compare("<greater_equal>")==0)
            {
                op_flag = 1;
                equal_op = 4;
            }
            else if(rc_tokens[i].compare("<less_equal>")==0)
            {
                op_flag = 1;
                equal_op = 5;
            }
            else if(rc_tokens[i].compare("<not_equal>")==0)
            {
                op_flag = 1;
                if(v1.substr(0,1).compare("s")==0)
                    equal_op = 8;
                else
                    equal_op = 6;
            }

            if(op_flag == 2 && (v1.compare("")==0 || v2.compare("")==0))
            {
                op_flag = 0;
            }

            if(op_flag == 2 && equal_op == 1)
            {
                //cout << "PAUSE HERE: ";
                //string a = ""; cin >> a;
                tmp[currentIndex] = "cmp " + v1 + " " + v2;
                currentIndex++;
                tmp[currentIndex] = "mov " + v1 + " %EQUAL_FLAG";
                currentIndex++;

                //cout<<tmp[currentIndex-2]<<endl;
                //cout<<tmp[currentIndex-1]<<endl;
                rc_tokens[start_clear] = "<vm_var>"+v1;
                for(int j = start_clear+1; j <= end_clear;j++)
                {
                    rc_tokens[j] = " ";
                }
                op_flag = 0;
                i = currentBlock_start-1;
            }
            if(op_flag == 2 && equal_op == 7)
            {
                tmp[currentIndex] = "cmp$ " + v1 + " " + v2;
                currentIndex++;
                v1 = "m" + rc_intToString(m_index);
                m_index++;
                tmp[currentIndex] = "mov " + v1 + " %EQUAL_FLAG";
                currentIndex++;

                //cout<<tmp[currentIndex-2]<<endl;
                //cout<<tmp[currentIndex-1]<<endl;
                rc_tokens[start_clear] = "<vm_var>"+v1;
                for(int j = start_clear+1; j <= end_clear;j++)
                {
                    rc_tokens[j] = " ";
                }
                op_flag = 0;
                i = currentBlock_start-1;
            }
            else if(op_flag == 2 && equal_op == 2)
            {
                tmp[currentIndex] = "cmp " + v1 + " " + v2;
                currentIndex++;
                tmp[currentIndex] = "mov " + v1 + " %GREATER_FLAG";
                currentIndex++;

                //cout<<tmp[currentIndex-2]<<endl;
                //cout<<tmp[currentIndex-1]<<endl;
                rc_tokens[start_clear] = "<vm_var>"+v1;
                for(int j = start_clear+1; j <= end_clear;j++)
                {
                    rc_tokens[j] = " ";
                }
                op_flag = 0;
                i = currentBlock_start-1;
            }
            else if(op_flag == 2 && equal_op == 3)
            {
                tmp[currentIndex] = "cmp " + v1 + " " + v2;
                currentIndex++;
                tmp[currentIndex] = "mov " + v1 + " %LESS_FLAG";
                currentIndex++;

                //cout<<tmp[currentIndex-2]<<endl;
                //cout<<tmp[currentIndex-1]<<endl;
                rc_tokens[start_clear] = "<vm_var>"+v1;
                for(int j = start_clear+1; j <= end_clear;j++)
                {
                    //cout << "clear " << rc_tokens[j] << endl;
                    rc_tokens[j] = " ";
                }
                op_flag = 0;
                i = currentBlock_start-1;
            }
            else if(op_flag == 2 && equal_op == 4)
            {
                tmp[currentIndex] = "cmp " + v1 + " " + v2;
                currentIndex++;
                tmp[currentIndex] = "mov " + v1 + " %GREATER_EQUAL_FLAG";
                currentIndex++;

                //cout<<tmp[currentIndex-2]<<endl;
                //cout<<tmp[currentIndex-1]<<endl;
                rc_tokens[start_clear] = "<vm_var>"+v1;
                for(int j = start_clear+1; j <= end_clear;j++)
                {
                    rc_tokens[j] = " ";
                }
                op_flag = 0;
                i = currentBlock_start-1;
            }
            else if(op_flag == 2 && equal_op == 5)
            {
                tmp[currentIndex] = "cmp " + v1 + " " + v2;
                currentIndex++;
                tmp[currentIndex] = "mov " + v1 + " %LESS_EQUAL_FLAG";
                currentIndex++;

                //cout<<tmp[currentIndex-2]<<endl;
                //cout<<tmp[currentIndex-1]<<endl;
                rc_tokens[start_clear] = "<vm_var>"+v1;
                for(int j = start_clear+1; j <= end_clear;j++)
                {
                    rc_tokens[j] = " ";
                }
                op_flag = 0;
                i = currentBlock_start-1;
            }
            else if(op_flag == 2 && equal_op == 6)
            {
                tmp[currentIndex] = "cmp " + v1 + " " + v2;
                currentIndex++;
                tmp[currentIndex] = "mov " + v1 + " %NOT_EQUAL_FLAG";
                currentIndex++;

                //cout<<tmp[currentIndex-2]<<endl;
                //cout<<tmp[currentIndex-1]<<endl;
                rc_tokens[start_clear] = "<vm_var>"+v1;
                for(int j = start_clear+1; j <= end_clear;j++)
                {
                    rc_tokens[j] = " ";
                }
                op_flag = 0;
                i = currentBlock_start-1;
            }
            else if(op_flag == 2 && equal_op == 8)
            {
                tmp[currentIndex] = "cmp$ " + v1 + " " + v2;
                currentIndex++;
                v1 = "m" + rc_intToString(m_index);
                m_index++;
                tmp[currentIndex] = "mov " + v1 + " %NOT_EQUAL_FLAG";
                currentIndex++;

                //cout<<tmp[currentIndex-2]<<endl;
                //cout<<tmp[currentIndex-1]<<endl;
                rc_tokens[start_clear] = "<vm_var>"+v1;
                for(int j = start_clear+1; j <= end_clear;j++)
                {
                    //cout << "clear tokens[" << rc_tokens[j] << endl;
                    rc_tokens[j] = " ";
                }
                op_flag = 0;
                i = currentBlock_start-1;
            }
        }


        //logical AND, logical OR, logical XOR
        op_flag = 0;
        for(int i = currentBlock_start; i < currentBlock_end; i++)
        {
            if(rc_tokens[i].substr(0,5).compare("<num>")==0)
            {
                if(op_flag == 0)
                {
                    v1 = rc_tokens[i].substr(5);
                    start_clear = i;
                }
                else
                {
                    v2 = rc_tokens[i].substr(5);
                    op_flag = 2;
                    end_clear = i;
                }
            }
            else if(rc_tokens[i].substr(0,4).compare("<id>")==0)
            {
                if(op_flag == 0)
                {
                    v1 = rc_searchID(rc_tokens[i].substr(4));
                    if(v1.compare("")==0)
                    {
                        cout << "Variable Does not Exist in Current Scope\n";
                        return 2;
                    }
                    v1_type = rc_getID_data(v1, "id_type");
                    if(v1_type.substr(0,1).compare("#")!=0)
                    {
                        v1 = "";
                    }
                    else
                    start_clear = i;
                }
                else
                {
                    v2 = rc_searchID(rc_tokens[i].substr(4));
                    if(v2.compare("")==0)
                    {
                        cout << "Variable Does not Exist in Current Scope\n";
                        return 2;
                    }
                    v2_type = rc_getID_data(v1, "id_type");
                    if(v2_type.substr(0,1).compare("#")!=0)
                    {
                        v2 = "";
                        op_flag = 0;
                    }
                    else
                    {
                        op_flag = 2;
                        end_clear = i;
                    }
                }
            }
            else if(rc_tokens[i].substr(0,8).compare("<vm_var>")==0)
            {
                if(op_flag == 0)
                {
                    if(rc_tokens[i].substr(8,1).compare("m")==0)
                    {
                        v1 = rc_tokens[i].substr(8);
                        start_clear = i;
                    }
                    else
                    {
                        v1 = "";
                    }
                }
                else
                {
                    if(rc_tokens[i].substr(8,1).compare("m")==0)
                    {
                        v2 = rc_tokens[i].substr(8);
                        op_flag = 2;
                        end_clear = i;
                    }
                    else
                    {
                        v1 = "";
                        v2 = "";
                        op_flag = 0;
                    }
                }
            }
            else if(rc_tokens[i].substr(0,5).compare("<and>")==0 && v1.compare("")!=0)
            {
                op_flag = 1;
                current_op = 1;
                //cout<<"op 1"<<endl;
            }
            else if(rc_tokens[i].substr(0,4).compare("<or>")==0 && v1.compare("")!=0)
            {
                //cout <<"\n\nOR\n\n";
                op_flag = 1;
                current_op = 2;
            }
            else if(rc_tokens[i].substr(0,5).compare("<xor>")==0 && v1.compare("")!=0)
            {
                op_flag = 1;
                current_op = 3;
            }

            if(op_flag == 2)
            {
                if(current_op == 1)
                {
                    tmp[currentIndex] = "cmp " + v1 + " 0";
                    currentIndex++;
                    tmp[currentIndex] = "mov " + v1 + " %NOT_EQUAL_FLAG";
                    currentIndex++;
                    tmp[currentIndex] = "cmp " + v2 + " 0";
                    currentIndex++;
                    tmp[currentIndex] = "mov " + v2 + " %NOT_EQUAL_FLAG";
                    currentIndex++;
                    tmp[currentIndex] = "and " + v1 + " " + v2;
                    currentIndex++;
                }
                else if(current_op == 2)
                {
                    tmp[currentIndex] = "cmp " + v1 + " 0";
                    currentIndex++;
                    tmp[currentIndex] = "mov " + v1 + " %NOT_EQUAL_FLAG";
                    currentIndex++;
                    tmp[currentIndex] = "cmp " + v2 + " 0";
                    currentIndex++;
                    tmp[currentIndex] = "mov " + v2 + " %NOT_EQUAL_FLAG";
                    currentIndex++;
                    tmp[currentIndex] = "or " + v1 + " " + v2;
                    currentIndex++;
                }
                else if(current_op == 3)
                {
                    tmp[currentIndex] = "cmp " + v1 + " 0";
                    currentIndex++;
                    tmp[currentIndex] = "mov " + v1 + " %NOT_EQUAL_FLAG";
                    currentIndex++;
                    tmp[currentIndex] = "cmp " + v2 + " 0";
                    currentIndex++;
                    tmp[currentIndex] = "mov " + v2 + " %NOT_EQUAL_FLAG";
                    currentIndex++;
                    tmp[currentIndex] = "xor " + v1 + " " + v2;
                    currentIndex++;
                }

                rc_tokens[start_clear] = "<vm_var>" + v1;
                for(int j = start_clear+1; j <= end_clear;j++)
                {
                    rc_tokens[j] = " ";
                }
                m_index++;
                op_flag = 0;
                i = currentBlock_start-1;

            }
        }

        //not
        op_flag = 0;
        for(int i = currentBlock_start; i < currentBlock_end; i++)
        {
            if(rc_tokens[i].substr(0,5).compare("<num>")==0)
            {
                if(op_flag == 1)
                {
                    v2 = rc_tokens[i].substr(5);
                    op_flag = 2;
                    end_clear = i;
                }
            }
            else if(rc_tokens[i].substr(0,4).compare("<id>")==0)
            {
                if(op_flag == 1)
                {
                    v2 = rc_searchID(rc_tokens[i].substr(4));
                    if(v2.compare("")==0)
                    {
                        cout << "Variable Does not Exist in Current Scope\n";
                        return 2;
                    }
                    v2_type = rc_getID_data(v1, "id_type");
                    if(v2_type.substr(0,1).compare("#")!=0)
                    {
                        v2 = "";
                        op_flag = 0;
                    }
                    else
                    {
                        op_flag = 2;
                        end_clear = i;
                    }
                }
            }
            else if(rc_tokens[i].substr(0,8).compare("<vm_var>")==0)
            {
                if(op_flag == 1)
                {
                    if(rc_tokens[i].substr(8,1).compare("m")==0)
                    {
                        v2 = rc_tokens[i].substr(8);
                        op_flag = 2;
                        end_clear = i;
                    }
                    else
                    {
                        v1 = "";
                        v2 = "";
                        op_flag = 0;
                    }
                }
            }
            else if(rc_tokens[i].substr(0,5).compare("<not>")==0 && v1.compare("")!=0)
            {
                op_flag = 1;
                current_op = 1;
                start_clear = i;
                //cout<<"op 1"<<endl;
            }

            if(op_flag == 2)
            {
                tmp[currentIndex] = "not " + v2;

                currentIndex++;

                rc_tokens[start_clear] = "<vm_var>" + v2;
                for(int j = start_clear+1; j <= end_clear;j++)
                {
                    rc_tokens[j] = " ";
                }
                m_index++;
                op_flag = 0;
                i = currentBlock_start-1;

            }
        }

        //empty space error
        op_flag = 0;
        //cout << "Empty Space Error: V1 = " << v1 << "    V2 = " << v2 << endl;
        v1 = "";
        v2 = "";
        //for(int i = currentBlock_start+1; i < currentBlock_end; i++)
            //cout << "tk[" << i <<"] = " << rc_tokens[i] << endl;

        for(int i = currentBlock_start+1; i < currentBlock_end; i++)
        {
            if(rc_tokens[i].substr(0,5).compare("<num>")==0)
            {
                if(op_flag == 0)
                {
                    v1 = rc_tokens[i].substr(5);
                    start_clear = i;
                    op_flag = 1;
                }
                else
                {
                    v2 = rc_tokens[i].substr(5);
                    op_flag = 2;
                    end_clear = i;
                }
            }
            else if(rc_tokens[i].substr(0,4).compare("<id>")==0)
            {
                if(op_flag == 0)
                {
                    v1 = rc_searchID(rc_tokens[i].substr(4));
                    if(v1.compare("")==0)
                    {
                        cout << "Variable Does not Exist in Current Scope\n";
                        return 2;
                    }
                    v1_type = rc_getID_data(v1, "id_type");
                    if(v1_type.substr(0,1).compare("#")!=0 && v1_type.substr(0,1).compare("$")!=0)
                    {
                        v1 = "";
                    }
                    else
                    start_clear = i;
                    op_flag = 1;
                }
                else
                {
                    v2 = rc_searchID(rc_tokens[i].substr(4));
                    if(v2.compare("")==0)
                    {
                        cout << "Variable Does not Exist in Current Scope\n";
                        return 2;
                    }
                    v2_type = rc_getID_data(v1, "id_type");
                    if(v2_type.substr(0,1).compare("#")!=0 && v2_type.substr(0,1).compare("$")!=0)
                    {
                        v2 = "";
                        op_flag = 0;
                    }
                    else
                    {
                        op_flag = 2;
                        end_clear = i;
                    }
                }
            }
            else if(rc_tokens[i].substr(0,8).compare("<vm_var>")==0)
            {
                if(op_flag == 0)
                {
                    if(rc_tokens[i].substr(8,1).compare("m")==0)
                    {
                        //cout << "V1[" << i << "] = " << rc_tokens[i] << endl;
                        v1 = rc_tokens[i].substr(8);
                        start_clear = i;
                        op_flag = 1;
                    }
                    else if(rc_tokens[i].substr(8,1).compare("s")==0)
                    {
                        v1 = rc_tokens[i].substr(8);
                        start_clear = i;
                        op_flag = 1;
                    }
                    else
                    {
                        v1 = "";
                    }
                }
                else
                {
                    if(rc_tokens[i].substr(8,1).compare("m")==0)
                    {
                        //cout << "V2[" << i << "] = " << rc_tokens[i] << endl;
                        v2 = rc_tokens[i].substr(8);
                        op_flag = 2;
                        end_clear = i;
                    }
                    else if(rc_tokens[i].substr(8,1).compare("s")==0)
                    {
                        v2 = rc_tokens[i].substr(8);
                        op_flag = 2;
                        end_clear = i;
                    }
                    else
                    {
                        v1 = "";
                        v2 = "";
                        op_flag = 0;
                    }
                }
            }
            else if(rc_tokens[i].substr(0,5).compare("<sep>")==0)
            {
                v1 = "";
                op_flag = 0;
            }

            if(op_flag == 2)
            {
                cout << "illegal operation: " << v1 << ", " << v2 << endl;
                return 2;
            }
        }

        //cout << "start of function processing\n";string s; cin >> s;

        //function and array
        if(t_index > 0)
        if(rc_tokens[currentBlock_start-1].substr(0,4).compare("<id>")==0)
        {
            arg_count = 0;
            //cout<<currentBlock_start<<endl;
            //cout<<rc_tokens[currentBlock_start]<<endl;
            //rc_tokens[currentBlock_start-1] = "<id>" + rc_tokens[currentBlock_start-1].substr(4);
            id = rc_searchID(rc_tokens[currentBlock_start-1].substr(4));

            if(id.compare("")==0)
            {
                id = rc_searchID(rc_tokens[currentBlock_start-1].substr(4)+"$");
                if(id.compare("")==0)
                {
                    cout << "Could not find ID " << rc_tokens[currentBlock_start-1].substr(4) << "\n";
                    exit(-1);
                    return 2;
                }
                //cout << "word life" << endl;
            }
            else
            {
                //cout << "TFW " << id << endl;
            }
            id_type = rc_getID_data(id, "id_type");
            f_index = rc_getID_index;

            if(id_type.compare("#F")==0 || id_type.compare("$F") == 0)
            {
                rc_tokens[currentBlock_start-1] = "<fcn_id>" + id;
            }
            else if(id_type.compare("#P")==0 || id_type.compare("$P") == 0)
            {
                rc_tokens[currentBlock_start-1] = "<prc_id>" + id;
            }
            else if(id_type.compare("#A") == 0 || id_type.compare("$A") == 0)
            {
                rc_tokens[currentBlock_start-1] = "<arr_id>" + id;
            }
            else if(id_type.compare("#&") == 0 || id_type.compare("$&") == 0)
            {
                rc_tokens[currentBlock_start-1] = "<ref_id>" + id;
            }
            else
            {
                cout << "illegal argument in function/array\n\n";
                return 1;
            }

            if(id_type.substr(0,1).compare("$")==0)
                rc_isString = true;
            else
                rc_isString = false;

            //cout << "\n\nID_DIM\n";
            id_dim = atoi(rc_getID_data(id, "id_dim").c_str());
            //cout << endl << endl;

            if(rc_tokens[currentBlock_start-1].substr(0,8).compare("<fcn_id>")==0 ||
               rc_tokens[currentBlock_start-1].substr(0,8).compare("<prc_id>")==0)
            {
                mi_place = m_index;
                for(int i = 0; i < mi_place; i++)
                {
                    //cout << "OH YAAAAAAAH\n";
                    tmp[currentIndex] = "push m" + rc_intToString(i);
                    currentIndex++;
                    ns_use++;
                }
                si_place = s_index;
                for(int i = 0; i < si_place; i++)
                {
                    tmp[currentIndex] = "push$ s" + rc_intToString(i);
                    currentIndex++;
                    ss_use++;
                }
                string nt = "";
                for(int i = currentBlock_start; i <= currentBlock_end; i++)
                {
                    if(rc_tokens[i].substr(0,8).compare("<vm_var>")==0)
                    {
                        nt = rc_getNextToken(i+1, currentBlock_end);
                        if(nt.compare("<sep>")!=0 && nt.compare("</par>")!=0)
                        {
                            cout << "Syntax Error in argument\n"; //string s; cin >>s;
                            //for(int r = currentBlock_start; r < currentBlock_end; r++)
                                //cout << "rc_tokens[" << r << "] = " << rc_tokens[r] << endl;
                                //cin >> s;
                            return 2;
                        }
                        if(id.compare("PROGRAM.ARRAYSIZE")==0)
                        {
                            //cout << "HERE MOTHA aganin\n";
                            if(arg_count==1)
                            {
                                arg_id[arg_count] = rc_getID(f_index+3);
                            }
                        }
                        else
                            arg_id[arg_count] = rc_getID(f_index+arg_count+1);
                        rc_tokens[i] = "<arg>"+rc_tokens[i].substr(8);
                        //arg_id[arg_count] = rc_getID(f_index+arg_count+1);
                        //cout << "AID = " << arg_id[arg_count] << endl;
                        arg_type[arg_count] = rc_getID_data(arg_id[arg_count], "id_type");
                        arg_value[arg_count] = rc_tokens[i].substr(5);
                        //cout << "AVA = " << arg_value[arg_count] << endl;
                        if(arg_type[arg_count].substr(0,1).compare("#")==0)
                        {
                            //cout << "Check 1\n";
                            if(arg_value[arg_count].substr(0,1).compare("s")==0)
                            {
                                cout << "Expected number and found String\n";
                                return 2;
                            }
                            //cout << "Check 2\n";
                        }
                        else if(arg_type[arg_count].substr(0,1).compare("$")==0)
                        {
                            if(arg_value[arg_count].substr(0,1).compare("m")==0)
                            {
                                tmp[currentIndex] = "str$ s" + rc_intToString(s_index) + " " + arg_value[arg_count];
                                currentIndex++;
                                arg_value[arg_count] = "s" + rc_intToString(s_index);
                                s_index++;
                            }
                        }
                        arg_count++;
                    }
                    else if(rc_tokens[i].substr(0,4).compare("<id>")==0)
                    {
                        nt = rc_getNextToken(i+1, currentBlock_end);
                        if(nt.compare("<sep>")!=0 && nt.compare("</par>")!=0)
                        {
                            cout << "Syntax Error in argument\n"; //string s; cin >>s;
                            //for(int r = currentBlock_start; r < currentBlock_end; r++)
                                //cout << "rc_tokens[" << r << "] = " << rc_tokens[r] << endl;
                                //cin >> s;
                            return 2;
                        }
                        tmp[currentIndex] = "mov m" + rc_intToString(m_index) + " " + rc_tokens[i].substr(4);//v2 is id
                        //cout << tmp[currentIndex] << endl;
                        currentIndex++;
                        rc_tokens[i] = "<arg>m" + rc_intToString(m_index);
                        m_index++;
                        arg_id[arg_count] = rc_getID(f_index+arg_count+1);
                        arg_type[arg_count] = rc_getID_data(arg_id[arg_count],"id_type");
                        arg_value[arg_count] = rc_tokens[i].substr(5);
                        arg_count++;
                    }
                    else if(rc_tokens[i].substr(0,5).compare("<num>")==0)
                    {
                        nt = rc_getNextToken(i+1, currentBlock_end);
                        if(nt.compare("<sep>")!=0 && nt.compare("</par>")!=0)
                        {
                            cout << "Syntax Error in argument\n"; //string s; cin >>s;
                            //for(int r = currentBlock_start; r < currentBlock_end; r++)
                                //cout << "rc_tokens[" << r << "] = " << rc_tokens[r] << endl;
                                //cin >> s;
                            return 2;
                        }
                        tmp[currentIndex] = "mov m" + rc_intToString(m_index) + " " + rc_tokens[i].substr(5);
                        //cout << tmp[currentIndex] << endl;
                        currentIndex++;
                        rc_tokens[i] = "<arg>m" + rc_intToString(m_index);
                        m_index++;
                        arg_id[arg_count] = rc_getID(f_index+arg_count+1);
                        //cout << "ARG#" << arg_count << " = " << arg_id[arg_count] << endl;
                        arg_type[arg_count] = rc_getID_data(arg_id[arg_count],"id_type");
                        arg_value[arg_count] = rc_tokens[i].substr(5);
                        arg_count++;
                    }
                    else if(rc_tokens[i].substr(0,9).compare("<io_mode>")==0)
                    {
                        string iom_value = "";
                        if(rc_tokens[i].substr(9).compare("text_input")==0)
                            iom_value = "1";
                        else if(rc_tokens[i].substr(9).compare("text_output")==0)
                            iom_value = "2";
                        else if(rc_tokens[i].substr(9).compare("text_append")==0)
                            iom_value = "3";
                        else if(rc_tokens[i].substr(9).compare("text_truncate")==0)
                            iom_value = "4";
                        else if(rc_tokens[i].substr(9).compare("binary_input")==0)
                            iom_value = "5";
                        else if(rc_tokens[i].substr(9).compare("binary_output")==0)
                            iom_value = "6";
                        else if(rc_tokens[i].substr(9).compare("binary_append")==0)
                            iom_value = "7";
                        if(rc_tokens[i].substr(9).compare("binary_truncate")==0)
                            iom_value = "8";
                        tmp[currentIndex] = "mov m" + rc_intToString(m_index) + " " + iom_value;
                        //cout << tmp[currentIndex] << endl;
                        currentIndex++;
                        rc_tokens[i] = "<arg>m" + rc_intToString(m_index);
                        m_index++;
                        arg_id[arg_count] = rc_getID(f_index+arg_count+1);
                        //cout << "ARG#" << arg_count << " = " << arg_id[arg_count] << endl;
                        arg_type[arg_count] = rc_getID_data(arg_id[arg_count],"id_type");
                        arg_value[arg_count] = rc_tokens[i].substr(5);
                        arg_count++;
                    }
                    else if(rc_tokens[i].substr(0,5).compare("<ref>")==0)
                    {
                        nt = rc_getNextToken(i+1, currentBlock_end);
                        if(nt.compare("<sep>")!=0 && nt.compare("</par>")!=0)
                        {
                            cout << "Syntax Error in argument ref\n"; //string s; cin >>s;
                            //for(int r = currentBlock_start; r < currentBlock_end; r++)
                                //cout << "rc_tokens[" << r << "] = " << rc_tokens[r] << endl;
                                //cin >> s;
                            return 2;
                        }
                        //cout << "STOP\n"; string s=""; cin >> s;

                        arg_id[arg_count] = rc_getID(f_index+arg_count+1);

                        //cout << "Check 2";

                        //rc_tokens[i] = "<arg>" + rc_tokens[i].substr(5);
                        if(id.compare("PROGRAM.ARRAYDIM")==0)
                        {
                            if(rc_getID_data(rc_tokens[i].substr(5),"id_type").compare("#A")==0)
                                arg_id[arg_count] = rc_getID(f_index+arg_count+1);
                            else if(rc_getID_data(rc_tokens[i].substr(5),"id_type").compare("$A")==0)
                                arg_id[arg_count] = rc_getID(f_index+arg_count+2);
                            else
                            {
                                cout << "Invalid Argument Type for ByRef ID" << endl;
                            }
                        }
                        else if(id.compare("PROGRAM.ARRAYSIZE")==0)
                        {
                            //cout << "HERE MOTHA\n";
                            if(arg_count==0)
                            {
                                if(rc_getID_data(rc_tokens[i].substr(5),"id_type").compare("#A")==0)
                                {
                                    arg_id[arg_count] = rc_getID(f_index+arg_count+1);
                                }
                                else if(rc_getID_data(rc_tokens[i].substr(5),"id_type").compare("$A")==0)
                                {
                                    arg_id[arg_count] = rc_getID(f_index+arg_count+2);
                                }
                                else
                                {
                                    cout << "Invalid Argument Type for ByRef ID" << endl;
                                }
                            }
                            else if(arg_count==1)
                            {
                                arg_id[arg_count] = rc_getID(f_index+3);
                            }
                        }
                        //arg_id[arg_count] = rc_getID(f_index+arg_count+1);
                        //cout << "ARG#" << arg_count << " = " << arg_id[arg_count] << endl;
                        //if(rc_getID_data(arg_id[arg_count],"id_type").substr(1,1).compare("&")!=0)
                            //cout <<"DEBUG THIS SHIT SON\n";

                        arg_value[arg_count] = rc_tokens[i].substr(5);
                        arg_count++;
                    }
                    else if(rc_tokens[i].compare("<sep>")==0)
                    {
                        nt = rc_getNextToken(i+1, currentBlock_end);
                        if(nt.substr(0,8).compare("<vm_var>")!=0 && nt.substr(0,4).compare("<id>")!=0 && nt.substr(0,5).compare("<ref>")!=0)
                        {
                            cout << "Syntax Error in argument\n"; //string s; cin >>s;
                            //for(int r = currentBlock_start; r < currentBlock_end; r++)
                                //cout << "rc_tokens[" << r << "] = " << rc_tokens[r] << endl;
                                //cin >> s;
                            return 2;
                        }
                        //cout << "SEP: " << endl; string s; cin >> s;
                    }
                    else
                    {
                        //cout<<"rc "<<rc_tokens[i]<<endl;
                        rc_tokens[i] = "";
                    }
                }
            }
            else //array
            {
                for(int i = currentBlock_start; i <= currentBlock_end; i++)
                {
                    if(rc_tokens[i].substr(0,8).compare("<vm_var>")==0)
                    {
                        if(rc_tokens[i].substr(8,1).compare("m")!=0)
                        {
                            cout << "Must reference index in Array with a number\n";
                            return 1;
                        }
                        rc_tokens[i] = "<arg>"+rc_tokens[i].substr(8);
                        arg_value[arg_count] = rc_tokens[i].substr(5);
                        arg_count++;
                    }
                    else if(rc_tokens[i].substr(0,4).compare("<id>")==0)
                    {
                        rc_tokens[i] = rc_searchID(rc_tokens[i].substr(4));
                        if(rc_tokens[i].compare("")==0)
                        {
                            cout << "ID in index does not exist\n";
                            return 2;
                        }
                        tmp[currentIndex] = "mov m" + rc_intToString(m_index) + " " + rc_tokens[i];//v2 is id
                        //cout << "debug statement\n";
                        //cout << tmp[currentIndex] << endl;
                        currentIndex++;
                        rc_tokens[i] = "<arg>m" + rc_intToString(m_index);
                        m_index++;
                        arg_value[arg_count] = rc_tokens[i].substr(5);
                        arg_count++;
                    }
                    else if(rc_tokens[i].substr(0,5).compare("<num>")==0)
                    {
                        tmp[currentIndex] = "mov m" + rc_intToString(m_index) + " " + rc_tokens[i].substr(5);
                        //cout << tmp[currentIndex] << endl;
                        currentIndex++;
                        rc_tokens[i] = "<arg>m" + rc_intToString(m_index);
                        m_index++;
                        arg_value[arg_count] = rc_tokens[i].substr(5);
                        arg_count++;
                    }
                    else
                    {
                        //cout<<"rc "<<rc_tokens[i]<<endl;
                        rc_tokens[i] = "";
                    }
                }
            }

            if(rc_tokens[currentBlock_start-1].substr(0,8).compare("<ref_id>")==0)
            {
                id_dim = arg_count;
            }
            else if(atoi(rc_getID_data(id, "id_dim").c_str()) != arg_count)
            {
                cout << "found " << arg_count << " args, expected " << id_dim << " args\n";
                return 2;
            }


            //<fcn_id> evaluate
            if(rc_tokens[currentBlock_start-1].substr(0,8).compare("<fcn_id>")==0 ||
               rc_tokens[currentBlock_start-1].substr(0,8).compare("<prc_id>")==0)
            {
                //cout << "id dim = " << arg_id[1] << endl;
                for(int i = 0; i < id_dim; i ++)
                {
                    if(rc_getID_data(arg_id[i], "id_type").compare("$")==0)
                    {
                        tmp[currentIndex] = "mov$ " + arg_id[i] + " " + arg_value[i];//v1 is id
                        //cout << tmp[currentIndex] << endl;
                        currentIndex++;
                    }
                    else if(rc_getID_data(arg_id[i], "id_type").compare("#")==0)
                    {
                        tmp[currentIndex] = "mov " + arg_id[i] + " " + arg_value[i];//v1 is id
                        //cout << tmp[currentIndex] << endl;
                        currentIndex++;
                    }
                    else if(rc_getID_data(arg_id[i], "id_type").substr(1,1).compare("&")==0)
                    {
                        //cout << "STOP\n"; string s=""; cin >> s;
                        if(rc_getID_data(arg_value[i],"id_type").compare("#A")==0)
                            tmp[currentIndex] = "ptr " + arg_id[i] + " " + arg_value[i];//v1 is id
                        else if(rc_getID_data(arg_value[i],"id_type").compare("$A")==0)
                        {
                            tmp[currentIndex] = "ptr " + arg_id[i] + " " + arg_value[i];//v1 is id
                            //cout <<"SET\n";
                        }
                        else if(rc_getID_data(arg_value[i],"id_type").compare("#&")==0)
                            tmp[currentIndex] = "ptr " + arg_id[i] + " " + arg_value[i];
                        else if(rc_getID_data(arg_value[i],"id_type").compare("$&")==0)
                            tmp[currentIndex] = "ptr " + arg_id[i] + " " + arg_value[i];
                        else if(rc_getID_data(arg_value[i],"id_type").compare("#")==0)
                            tmp[currentIndex] = "ptr " + arg_id[i] + " " + arg_value[i];//v1 is id
                        else if(rc_getID_data(arg_value[i],"id_type").compare("$")==0)
                            tmp[currentIndex] = "ptr " + arg_id[i] + " " + arg_value[i];//v1 is id
                        else
                        {
                            //tmp[currentIndex] = "wip";
                            cout << "Invalid argument in ByRef ID: " << arg_value[i] << endl;
                            return 2;
                        }

                        //cout << tmp[currentIndex] << endl;
                        currentIndex++;
                    }
                }

                if(rc_isBuiltIn_ID(id,arg_count,currentIndex)==0)
                {
                    tmp[currentIndex] = "gosub [" + id + "]";//id
                    isIntern = 1;
                    //cout << "FUNCTION_ADDRESS: " << rc_getID_data(id, "id_address") << endl;
                    //cout << tmp[currentIndex] << endl;
                }
                else
                {
                    isIntern = 0;
                }
                currentIndex++;

                if(isIntern == 1 && rc_tokens[currentBlock_start-1].substr(0,8).compare("<fcn_id>")==0)
                {
                    if(id_type.substr(0,1).compare("$")==0)
                    {
                        tmp[currentIndex] = "pop$ s" + rc_intToString(s_index);
                    }
                    else if(id_type.substr(0,1).compare("#")==0)
                    {
                        tmp[currentIndex] = "pop m" + rc_intToString(m_index);
                    }
                }
                else if(rc_tokens[currentBlock_start-1].substr(0,8).compare("<fcn_id>")==0)
                {
                    if(intern_type.compare("$")==0)
                    {
                        tmp[currentIndex] = "pop$ s" + rc_intToString(s_index);
                        ss_use++;
                        id_type = "$F";
                    }
                    else if(intern_type.compare("#")==0)
                    {
                        tmp[currentIndex] = "pop m" + rc_intToString(m_index);
                        ns_use++;
                        id_type = "#F";
                    }
                }

                //cout << tmp[currentIndex] << endl;
                currentIndex++;

                for(int i = si_place-1; i >= 0; i--)
                {
                    tmp[currentIndex] = "pop$ s" + rc_intToString(i);
                    currentIndex++;
                }

                for(int i = mi_place-1; i >= 0; i--)
                {
                    tmp[currentIndex] = "pop m" + rc_intToString(i);
                    currentIndex++;
                }

            }
            //<arr_id> evaluate
            else if( (rc_tokens[currentBlock_start-1].substr(0,8).compare("<arr_id>")==0 && id_type.compare("#A")==0) ||
                     (rc_tokens[currentBlock_start-1].substr(0,8).compare("<ref_id>")==0 && id_type.compare("#&")==0) )
            {
                if(arg_count == 1)
                {
                    tmp[currentIndex] = "arr1 " + id;
                }
                else if(arg_count == 2)
                {
                    tmp[currentIndex] = "arr2 " + id;
                }
                else if(arg_count == 3)
                {
                    tmp[currentIndex] = "arr3 " + id;
                }
                else
                {
                    cout << "array exceeds Maximum dimensions\n";
                    return 1;
                }

                for(int i = 0; i < id_dim; i ++)
                {
                    tmp[currentIndex] += " " + arg_value[i];
                }

                tmp[currentIndex] += " m"+rc_intToString(m_index);

                currentIndex++;
            }
            else if( (rc_tokens[currentBlock_start-1].substr(0,8).compare("<arr_id>")==0 && id_type.compare("$A")==0) ||
                     (rc_tokens[currentBlock_start-1].substr(0,8).compare("<ref_id>")==0 && id_type.compare("$&")==0) )
            {
                if(arg_count == 1)
                {
                    tmp[currentIndex] = "arr1$ " + id;
                }
                else if(arg_count == 2)
                {
                    tmp[currentIndex] = "arr2$ " + id;
                }
                else if(arg_count == 3)
                {
                    tmp[currentIndex] = "arr3$ " + id;
                }
                else
                {
                    cout << "array exceeds Maximum dimensions\n";
                    return 1;
                }

                for(int i = 0; i < id_dim; i ++)
                {
                    tmp[currentIndex] += " " + arg_value[i];
                }

                tmp[currentIndex] += " s"+rc_intToString(s_index);

                currentIndex++;
            }

            for(int i = currentBlock_start-1; i <= currentBlock_end; i++)
            {
                rc_tokens[i] = "";
            }

            if(id_type.substr(0,1).compare("$")==0)
            {
                rc_tokens[currentBlock_start-1] = "<vm_var>s" + rc_intToString(s_index);
                s_index++;
            }
            else if(id_type.substr(0,1).compare("#")==0)
            {
                rc_tokens[currentBlock_start-1] = "<vm_var>m" + rc_intToString(m_index);
                m_index++;
            }

            //x=g(8,9,0)        mov !g.a, 8      mov !g.b,  9     mov !g.c, 0        jump !g        pop x

            //using rc_id with functions
            //rc_getID_data(id,"id_type", 0);

            //cout<<"function @ "<<rc_tokens[currentBlock_start-1].substr(4)<<endl;
        }

        v1= "";

        if(rc_tokens[currentBlock_start].compare("<par>")==0)
        {
            _tmp_space = rc_tokens[currentBlock_start+1];
            //cout << "_TMP_SPC = " << _tmp_space << endl;
            for(int i = currentBlock_start; i <= currentBlock_end; i++)
            {
                if(rc_tokens[i].substr(0,8).compare("<vm_var>")==0)
                    v1 = rc_tokens[i];
                rc_tokens[i] = "";
            }
            //if( (m_index-1) >= mi_start )
            //    rc_tokens[currentBlock_start] = v1;
            //else
            //    rc_tokens[currentBlock_start] = _tmp_space;

            rc_tokens[currentBlock_start] = v1;

            //for(int i = 0; i < rc_tokens_count; i++)
            //{
                //cout << "OPT [" << i << "]: " << rc_tokens[i] << endl;
            //}

        }

        if(currentBlock_start==0 && currentBlock_end == rc_tokens_count)
            break;

        //cout << "t_index = " << t_index << " value = " << rc_tokens[t_index] << endl; //test ( 1 , 5 ) + ( 5 - 4 )

    }

    if(ns_use > num_stack_size)
        num_stack_size = ns_use;

    if(ss_use > str_stack_size)
        str_stack_size = ss_use;

    int unresolved = 0;

    for(int i = 0; i < rc_tokens_count; i++)
    {
        //cout<<"fto: "<<rc_tokens[i]<<endl;
        if(rc_tokens[i].substr(0,8).compare("<vm_var>")==0)
        {
            rc_result = rc_tokens[i].substr(8);
            unresolved++;
            //cout << "RESULTS = " << rc_result << endl;
        }
    }

    if(rc_result.compare("")==0 || unresolved > 1)
    {
        cout << "could not resolve expression\n";
        return 2;
    }


    if(m_index == m_init)
    {
        //cout << tmp[0] << endl;
        m_index++;
    }

    if(s_index == s_init)
    {
        //cout << tmp[0] << endl;
        s_index++;
    }

    if(m_index > max_m)
        max_m = m_index;

    if(s_index > max_s)
        max_s = s_index;

    if(tmp_count == currentIndex)
    {
        tmp_count = currentIndex+1;
    }
    else
    {
        tmp_count = currentIndex;
    }

    return 0;
}

int rc_output_tokens()
{
    //cout << "\nDEBUG DEBUG DEBUG\n";
    //for(int i =0; i < rc_tokens_count; i++)
    //{
        //cout<<"DEBUG RC_TOKEN "<<i<<": "<<rc_tokens[i]<<endl;
    //}
    //cout << "\nDEBUG DEBUG DEBUG\n";
    return 0;
}

int rc_getArrays(int m_init, int s_init = s_index)
{
    //cout << "GetArrays" << endl;
    int m = m_init;
    int s = s_init;
    int a_start = 0;
    int a_end = 0;
    string foo;
    string eid = "";
    string id_args[3][999]; //i only need 3 but i initialize 4 for the hell of it
    int id_args_count1 = 0;
    int id_args_count2[4];
    string rc_tokens_copy[99];
    int p = 0;
    int rc_tcount = rc_tokens_count;
    int t_index = 0;

    for(int i = 0; i < rc_tokens_count; i++)
        rc_tokens_copy[i] = rc_tokens[i];

    int i = 0;

    string dbg_stop = "";

    while(true)
    {
        t_index = rc_getLastTokenOf("<par>")-1;
        //cout << "T_INDEX = " << t_index << endl;
        //cin >> dbg_stop;

        if(t_index < 0)
        {
            break;
        }

        //cout << "RCT for "<<t_index<<" = "<<rc_tokens[t_index]<<endl;

        //for(int i = t_index; i < rc_tcount; i++)
        //{
            i = t_index;
            if(rc_tokens[i].substr(0,4).compare("<id>")==0 && i+1 < rc_tcount)
            {
                //cout << "checkpoint 1\n";
                eid = rc_searchID(rc_tokens[i].substr(4));
                if(eid.compare("")==0)
                {
                    //cout << "checkpoint 2\n";
                    rc_tokens[t_index+1]="<xpar>";
                    break;
                }

                if(i+1<rc_tcount)
                if(rc_tokens[i+1].compare("<par>")==0 && (rc_getID_data(eid,"id_type").compare("#A")==0 || rc_getID_data(eid,"id_type").compare("$A")==0 ||
                                                          rc_getID_data(eid,"id_type").compare("#&")==0 || rc_getID_data(eid,"id_type").compare("$&")==0) )
                {
                    //cout << "checkpoint 3\n";
                    a_start = i;
                    //cout<<"I = "<<i<<" AND ASTART = "<<a_start<<endl;
                    //cin>>foo;
                    id_args_count1=0;
                    id_args_count2[0] = 0;
                    p=1;
                    rc_tokens[i+1] = "<xpar>";
                    for(i+=2; p!=0; i++)
                    {
                        if(rc_tokens[i].compare("<par>")==0)
                        {
                            p++;
                            rc_tokens[i] = "<xpar>";
                            //cout<<"xpar\n";
                        }
                        else if(rc_tokens[i].compare("</par>")==0)
                            p--;

                        if(p==0)
                        {
                            id_args_count1++;
                            a_end = i;

                            if(id_args_count1 > 3 || id_args_count1 == 0)
                                return 1;

                            break;
                        }


                        if(rc_tokens[i].compare("<sep>")==0 && p==0)
                        {
                            id_args_count1++;
                            id_args_count2[id_args_count1]=0;
                        }
                        else
                        {
                            //cout<<"sc 1    i = "<<i<<"\n";
                            id_args[id_args_count1][id_args_count2[id_args_count1]]=rc_tokens[i];
                            id_args_count2[id_args_count1]++;
                            //cout<<"sc 2\n";
                        }
                    }
                    //cout<<"CP 1\n";
                    int cp_a = tmp_count;

                    for(int a = 0; a < id_args_count1; a++)
                    {
                        rc_tokens_count = 0;
                        for(int b = 0; b < id_args_count2[a]; b++)
                        {
                            rc_tokens[b] = id_args[a][b];
                            //cout << a << " token = " << rc_tokens[b] << endl;
                            rc_tokens_count++;
                        }
                        rc_preParse();
                        //m = m_index;
                        s = s_index;
                        rc_mathParser(m_index,s_index);
                        id_args[a][0]="m"+rc_intToString(m);
                        //cout << "ARG = " << id_args[a][0] << endl;
                        m = m_index;
                        s = s_index;
                    }
                    m++;

                    //for(int a = cp_a; a < tmp_count; a++)
                    //    cout << "TMP[" << a << "] = " << tmp[a] << endl;

                    //cout <<"CP 2\n";

                    for(int c=0; c<rc_tcount; c++)
                        rc_tokens[c]=rc_tokens_copy[c];

                    //cout <<"CP 3\n";

                    for(int c=a_start; c<=a_end;c++)
                        rc_tokens[c] = "";

                    //cout<<"CP 4\n";

                    if(rc_getID_data(eid,"id_type").compare("#A")==0 || rc_getID_data(eid,"id_type").compare("#&")==0)
                    {
                        switch(id_args_count1)
                        {
                            case 1: tmp[tmp_count]="arr1 "+eid+" "+id_args[0][0]+" m"+rc_intToString(m_index); //arr1 id index location_to_store_value
                                    m_index++;
                                    tmp_count++;
                                    break;
                            case 2: tmp[tmp_count]="arr2 "+eid+" "+id_args[0][0]+" "+id_args[1][0]+" m"+rc_intToString(m_index);
                                    m_index++;
                                    tmp_count++;
                                    break;
                            case 3: tmp[tmp_count]="arr3 "+eid+" "+id_args[0][0]+" "+id_args[1][0]+" "+id_args[2][0]+" m"+rc_intToString(m_index);
                                    m_index++;
                                    tmp_count++;
                                    break;
                        }
                        rc_tokens[a_start] = "<vm_var>m"+rc_intToString(m_index-1);
                    }
                    else if(rc_getID_data(eid,"id_type").compare("$A")==0 || rc_getID_data(eid,"id_type").compare("$&")==0)
                    {
                        switch(id_args_count1)
                        {
                            case 1: tmp[tmp_count]="arr1$ "+eid+" "+id_args[0][0]+" s"+rc_intToString(s_index); //arr1 id index location_to_store_value
                                    s_index++;
                                    tmp_count++;
                                    break;
                            case 2: tmp[tmp_count]="arr2$ "+eid+" "+id_args[0][0]+" "+id_args[1][0]+" s"+rc_intToString(s_index);
                                    s_index++;
                                    tmp_count++;
                                    break;
                            case 3: tmp[tmp_count]="arr3$ "+eid+" "+id_args[0][0]+" "+id_args[1][0]+" "+id_args[2][0]+" s"+rc_intToString(s_index);
                                    s_index++;
                                    tmp_count++;
                                    break;
                        }
                        rc_tokens[a_start] = "<vm_var>s"+rc_intToString(s_index-1);
                    }
                    //cout <<"CP 5\n";

                    //rc_tokens[a_start] = "<vm_var>m"+rc_intToString(m_index-1);

                    for(i = 0; i < rc_tcount; i++)
                        rc_tokens_copy[i] = rc_tokens[i];

                    rc_tokens_count = rc_tcount-1;
                }
                else
                {
                    rc_tokens[t_index+1] = "<xpar>";
                }
            }
            else
            {
                rc_tokens[t_index+1] = "<xpar>";
                //rc_tokens[rc_getLastTokenOf("</par>")] = "</xpar>";
            }
        //}//end of for
        rc_output_tokens();
        //cout << "---ASTART----"<<a_start<<endl;
        //cin>>foo;
    }

    rc_tokens_count = 0;

    for(int i = 0; i < rc_tcount; i++)
    {
        if(rc_tokens[i].compare("")!=0)
        {
            if(rc_tokens[i].compare("<xpar>")==0)
            {
                rc_tokens[i] = "<par>";
            }
            rc_tokens[rc_tokens_count]=rc_tokens[i];
            rc_tokens_count++;
        }
    }

    //cout<<"***********NEW TOKENS*********************\n";
    //for(int i = 0; i < rc_tokens_count; i++)
    //    cout << "TOKEN "<<i<<":"<<rc_tokens[i]<<endl;
    //cout<<"*********ALL OVER**************************\n";

    return 0;
}

int rc_evalID(int eid_flag = 0)
{
    //cout <<"EVAL START: " << current_scope <<"\n";
    string eid = "";
    string eid_typeCheck = "";
    string eid_type = "";
    int eid_dim = 1;
    string sline = "";
    string id_args[99][99];
    for(int i = 0; i < 99; i++)
        id_args[i][0] = "0";
    int id_args_count1 = 0;
    int id_args_count2[99];
    for(int i = 0; i < 99; i++)
        id_args_count2[i] = 0;
    int current_index = 0;
    int s = 0;
    int p = 0;
    int m = 0;
    int m_array = 0;
    int n = 0;
    int fcn_id = 0;
    int token_count_holder = 0;
    bool dim_exist = false;
    unsigned int dim_index = 0;
    unsigned int dim_rc_id = 0;
    string eid2 = "";
    bool isNull = false;
    //cout <<"EVAL IF START: " << current_scope <<"\n";
    //cout << "EVAL_IF START\n";
    if(rc_tokens[0].compare("<dim>")==0)
    {
        //cout << "DIM" << endl;
        if(rc_tokens[1].substr(0,4).compare("<id>")==0)
        {
            string k = rc_tokens[1].substr(4);
            for(int t = 0; t < rc_keywords_count; t++)
            {
                if(k.compare(rc_keyword[t])==0 || k.substr(0,k.length()-1).compare(rc_keyword[t])==0)
                {
                    cout << "Illegal identifier assignment: " << k << " is a reserved keyword" << endl;
                    return 2;
                }
            }
            eid = rc_searchID(rc_tokens[1].substr(4));
            eid2 = rc_searchID(rc_tokens[1].substr(4)+"$");
            if(eid.compare("")!=0 || eid2.compare("") != 0)
            {
                if(eid.compare("") == 0)
                    eid = eid2;

                dim_exist = true;
                dim_rc_id = search_index;
                dim_index = atoi(rc_getID_data(eid,"id_index").c_str());
                eid_typeCheck = rc_getID_data(eid,"id_type");

                //cout << dim_rc_id << endl;
                //cout << dim_index << endl;
                //cout << eid_typeCheck << endl;
                //string s; cin >> s;
                //cout << "ID already exist in current scope\n";
                //return 2;
            }
            else
            {
                eid = relative_scope + "." + rc_tokens[1].substr(4);
                eid_typeCheck = eid.substr(eid.length()-1);
            }

            if(eid_typeCheck.compare("#F") == 0 || eid_typeCheck.compare("$F") == 0 || eid_typeCheck.compare("#P") == 0 || eid_typeCheck.compare("$P") == 0)
            {
                cout << "Cannot define function or sub procedure as array" << endl;
                return 2;
            }

            if(eid_typeCheck.compare("$")==0)
                eid_typeCheck = "$A";
            else
                eid_typeCheck = "#A";

            if(rc_tokens[2].compare("<square>")==0)
            {
                rc_tokens[2] = "<par>";
                s++;

                if(rc_tokens[3].compare("</square>")==0)
                {
                    cout << "must specify dimensions for array\n";
                    return 2;
                }
                //cout << "CP1\n";

                for(int i = 3; i < rc_tokens_count; i++)
                {
                    if(s<0)
                    {
                        cout << "closed [] block without opening\n";
                        return 2;
                    }

                    if(i == rc_tokens_count-1 && rc_tokens[i].compare("</square>")!=0)
                    {
                        cout << "expected ]\n";
                        return 2;
                    }

                    if(rc_tokens[i].compare("<square>")==0)
                    {
                        rc_tokens[i] = "<par>";
                        s++;
                    }
                    else if(rc_tokens[i].compare("</square>")==0)
                    {
                        rc_tokens[i] = "</par>";
                        s--;
                    }

                    //s should equal one and a comma for id_arg_count++

                    if(s==1 && rc_tokens[i].compare("<sep>")==0)
                    {
                        id_args_count1++;
                        eid_dim++;
                    }
                    else if(i != rc_tokens_count-1)
                    {
                        //cout << "ID_args_count1 = " << id_args_count1 << endl;
                        //cout << "ID_args_count2 = " << id_args_count2[id_args_count1] << endl;
                        id_args[id_args_count1][id_args_count2[id_args_count1]] = rc_tokens[i];
                        id_args_count2[id_args_count1]++;
                    }
                }

                if(s<0)
                {
                    cout << "closed [] block without opening\n";
                    return 2;
                }
                else if(s>0)
                {
                    cout << "opened [] block without closing\n";
                    return 2;
                }

                //i need to pass this to the parser before i can get what i need to store in database
                for(int i = 0; i <= id_args_count1; i++)
                {
                    for(int j = 0; j < id_args_count2[i]; j++)
                    {
                        //cout << "Arg " << i << ": " << id_args[i][j] << endl;
                        rc_tokens[j] = id_args[i][j];
                    }
                    rc_tokens_count = id_args_count2[i];
                    if(rc_preParse()!=0)
                        return 2;
                    if(rc_mathParser(m,s_index)!=0)
                        return 2;
                    m = m_index;
                    id_args[i][0] = rc_result; //cout << "\nDEBUG ID_ARG[" << i << "]: " << id_args[i][0] << endl;

                }

                if(eid_typeCheck.compare("#A")==0)
                    tmp[tmp_count] = "dim " + eid + " " + id_args[0][0] + " " + id_args[1][0] + " " + id_args[2][0];
                else
                    tmp[tmp_count] = "dim$ " + eid + " " + id_args[0][0] + " " + id_args[1][0] + " " + id_args[2][0];

                tmp_count++;

                for(int i = 1; i <= id_args_count1; i++)
                {
                    tmp[tmp_count] = "mul " + id_args[0][0] + " " + id_args[i][0];
                    //cout << "dbg_reserve-> " << tmp[tmp_count] << endl;
                    tmp_count++;
                }

                if(eid_typeCheck.compare("#A")==0)
                {
                    if(dim_exist)
                    {
                        tmp[tmp_count] = "alloc " + id_args[0][0] + " # @" + rc_intToString(dim_index);
                        tmp_count++;
                    }
                    else
                    {
                        tmp[tmp_count] = "alloc " + id_args[0][0] + " # @" + rc_intToString(nid_count);
                        tmp_count++;
                        nid_count++;
                    }
                }
                else
                {
                    if(dim_exist)
                    {
                        tmp[tmp_count] = "alloc " + id_args[0][0] + " $ @" + rc_intToString(dim_index);
                        tmp_count++;
                    }
                    else
                    {
                        tmp[tmp_count] = "alloc " + id_args[0][0] + " $ @" + rc_intToString(sid_count);
                        tmp_count++;
                        sid_count++;
                    }
                }



                //cout << "ED = " << eid_dim << endl;
                eid += " " + eid_typeCheck + " " + rc_intToString(rc_data_offset) + " " + rc_intToString(current_scope) + " " + rc_intToString(eid_dim) + " ";



                if(!dim_exist)
                {
                    if(eid_typeCheck.compare("#A")==0)
                        eid += rc_intToString(nid_count-1) + " ";
                    else
                        eid += rc_intToString(sid_count-1) + " ";
                    rc_id[rc_id_count] = eid+" ";
                    //cout << "rc_id [ " << rc_id_count << " ] = " << rc_id[rc_id_count] << endl; string s; cin >> s;
                    rc_id_count++;
                }
                else
                {
                    rc_id[dim_rc_id] = eid + rc_intToString(dim_index) + " ";
                    //cout << "rc_id [ " << dim_rc_id << " ] = " << rc_id[dim_rc_id] << endl; string s; cin >> s;
                }
                //cout << "EID = " << eid << endl;
                //rc_data_offset += 4;

                //cout << "END DIM" << endl;


                return 1; //i need to change this back to 0 as soon as i am done with passing args to parser individually
            }
            else
            {
                if(rc_tokens_count != 2)
                {
                    cout << "Expected ID, found expression\n";
                    return 2;
                }
                eid = relative_scope + "." + rc_tokens[1].substr(4);
                eid_typeCheck = eid.substr(eid.length()-1);

                if(eid_typeCheck.compare("$")==0)
                    eid_typeCheck = "$";
                else
                    eid_typeCheck = "#";
                eid_dim = 0;

                eid += " " + eid_typeCheck + " " + rc_intToString(rc_id_count) + " " + rc_intToString(current_scope) + " " + rc_intToString(eid_dim) + " ";

                if(eid_typeCheck.compare("#")==0)
                {
                    eid += rc_intToString(nid_count) + " 0 ";
                    nid_count++;
                }
                else
                {
                    eid += rc_intToString(sid_count) + " 0 ";
                    sid_count++;
                }

                rc_id[rc_id_count] = eid+" ";
                rc_id_count++;

                return 1;
                //cout << "expected [\n";
                //return 2;
            }
        }
        else
        {
            cout << "expected id\n";
            return 2;
        }
    }
    else if( (rc_tokens[0].compare("<function>")==0 || rc_tokens[0].compare("<procedure>")==0) && eid_flag == 0)
    {
        int fp_type = 0;
        if(rc_tokens[0].compare("<function>")==0)
            fp_type = 1;
        else
            fp_type = 2;
        rc_segment_switch = RC_DATA_SEGMENT_SWITCH;
        tmp[tmp_count] = ".DATA";
        tmp_count++;

        if(current_scope != 0)
        {
            cout << "cannot define function in this scope " << current_scope << "\n";
            return 2;
        }

        current_scope = 1;

        if(rc_tokens[1].substr(0,4).compare("<id>")==0)
        {
            if(rc_searchID(rc_tokens[1].substr(4)).compare("")!=0 || rc_searchID(rc_tokens[1].substr(4)+"$").compare("")!=0)
            {
                cout << "Cannot define function " << rc_tokens[1].substr(4) << ", ID already exist" << endl;
                return 2;
            }
            eid = relative_scope + "." + rc_tokens[1].substr(4);
            relative_scope = eid;
            eid_typeCheck = eid.substr(eid.length()-1);

            tmp[tmp_count] = "label [" + relative_scope + "]";
            tmp_count++;


            if(eid_typeCheck.compare("$")==0)
            {
                if(fp_type == 1)
                    eid_typeCheck = "$F";
                else
                    eid_typeCheck = "$P";
            }
            else
            {
                if(fp_type == 1)
                    eid_typeCheck = "#F";
                else
                    eid_typeCheck = "#P";
            }

            if(rc_tokens[2].compare("<par>")==0)
            {
                p++;

                for(int i = 3; i < rc_tokens_count; i++)
                {
                    if(rc_tokens[i].compare("<par>")==0)
                        p++;
                    if(rc_tokens[i].compare("</par>")==0)
                        p--;

                    if(p>1)
                    {
                        cout << "expected ID in function or procedure definition\n";
                        return 2;
                    }

                    if(p == 0 && i < rc_tokens_count-1)
                    {
                        cout << "illegal function or procedure definition\n";
                        return 2;
                    }

                    //p should equal one and a comma for id_arg_count++

                    if(p==1 && rc_tokens[i].compare("<sep>")==0)
                    {
                        //id_args_count1++;
                        eid_dim++;
                    }
                    else if(i != rc_tokens_count-1 && rc_tokens[i].substr(0,4).compare("<id>")==0)
                    {
                        if(rc_searchID(rc_tokens[i].substr(4)).compare("")!=0)
                        {
                            cout << "Argument ID already exist in current scope\n";
                            return 2;
                        }
                        if(id_args[id_args_count1][0].compare("0")==0)
                        {
                            id_args[id_args_count1][0] = rc_tokens[i].substr(4);
                            if(rc_tokens[i+1].compare("<sep>")==0 || rc_tokens[i+1].compare("</par>")==0)
                                id_args_count1++;
                        }
                        else
                        {
                            cout << "expected single ID for argument: "<< id_args[id_args_count1][0] <<endl;
                            return 2;
                        }
                    }
                    else if(rc_tokens[i].compare("<byref>")==0)
                    {
                        if(id_args[id_args_count1][1].compare("&")!=0)
                            id_args[id_args_count1][1] = "&";
                        else
                        {
                            cout << "Invalid ID definition\n";
                            return 2;
                        }
                    }
                    else if(i == rc_tokens_count-1 && rc_tokens[i].compare("</par>")==0)
                    {
                        //success
                    }
                    else
                    {
                        cout << "expected single id\n";
                        return 2;
                    }
                }

                fcn_id = rc_id_count;

                rc_id_count++;

                rc_id[fcn_id] = eid + " " + eid_typeCheck + " " + rc_intToString(rc_data_offset) + " " + rc_intToString(current_scope) + " " + rc_intToString(id_args_count1)+" 0 0 ";
                //cout << endl << rc_id[fcn_id] << endl;

                if(id_args_count1 == 0)
                {
                    //cout << "EID = " << rc_id[fcn_id] << endl;
                    return 1;
                }

                int byref_check = 0;

                for(int i = 0; i < id_args_count1; i++)
                {
                    byref_check = 0;
                    //tmp[tmp_count] = eid + "." + id_args[i][0] + " db data:" + rc_intToString(rc_data_offset);
                    rc_id[rc_id_count] = eid + "." + id_args[i][0];
                    if(id_args[i][0].substr(id_args[i][0].length()-1,1).compare("$")==0)
                    {
                        if(id_args[i][1].compare("&")==0)
                        {
                            rc_id[rc_id_count] += " $& ";
                            byref_check = 2;
                        }
                        else
                            rc_id[rc_id_count] += " $ ";
                    }
                    else
                    {
                        if(id_args[i][1].compare("&")==0)
                        {
                            //cout << "BYREF in fcn\n";
                            rc_id[rc_id_count] += " #& ";
                            byref_check = 1;
                        }
                        else
                            rc_id[rc_id_count] += " # ";
                    }

                    rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " " + rc_intToString(current_scope) + " 0 ";
                    //cout << "dbg_fcn: " << rc_id[rc_id_count] << endl;

                    if(id_args[i][0].substr(id_args[i][0].length()-1,1).compare("$")==0)
                    {
                        rc_id[rc_id_count] += rc_intToString(sid_count) + " ";
                        if(byref_check == 2)
                        {
                            sid_byref_index[sid_byref_count] = sid_count;
                            sid_byref_count++;
                        }
                        sid_count++;
                    }
                    else
                    {
                        rc_id[rc_id_count] += rc_intToString(nid_count) + " ";
                        if(byref_check == 1)
                        {
                            nid_byref_index[nid_byref_count] = nid_count;
                            nid_byref_count++;
                        }
                        nid_count++;
                    }

                    //cout << "rc_id = " << rc_id[rc_id_count] << endl;

                    //rc_data_offset+=4;
                    rc_id_count++;
                    //tmp_count++;
                }


                //cout << "~~EID = " << rc_id[fcn_id] << endl;

                //rc_clearTokens();


                return 1; //i need to change this back to 0 as soon as i am done with passing args to parser individually
            }
            else
            {
                cout << "expected (\n";
                return 2;
            }
        }
        else
        {
            cout << "expected id\n";
            return 2;
        }
    }
    else if(eid_flag == 0)
    {
        string aid_check = "";
        //cout << "DRAGON\n";

        //for(int t = 0; t < rc_tokens_count; t++)
        //    cout << rc_tokens[t] << endl;

        //cout << "EVAL_ELSE START\n";
        for(int i = 0; i < rc_tokens_count; i++)
        {
            if(rc_tokens[i].compare("<par>")==0)
                p++;

            if(rc_tokens[i].compare("</par>")==0)
                p--;

            if(rc_tokens[i].compare("<square>")==0)
            {
                s++;
                rc_tokens[i] = "<par>";
            }

            if(rc_tokens[i].compare("</square>")==0)
            {
                s--;
                rc_tokens[i] = "</par>";
            }

        }

        //string dr; cin >> dr;
        int pp_check = 0, mp_check = 0;


        if(rc_tokens_count >= 3)
        {
            if(rc_tokens[0].substr(0,4).compare("<id>")==0)
            {
                eid = rc_tokens[0].substr(4);
                //variable = expression
                if(rc_tokens[1].compare("<equal>")==0)
                {
                    //eid = rc_tokens[0].substr(4);
                    //rc_getArrays(m);
                    if(eid.substr(eid.length()-1).compare("$")==0)
                        eid_typeCheck = "$";
                    else
                        eid_typeCheck = "#";

                    for(int i = 2; i < rc_tokens_count; i++)
                    {
                        rc_tokens[i-2] = rc_tokens[i];
                    }
                    rc_tokens_count -= 2;

                    pp_check = rc_preParse(1);

                    if(pp_check==0)
                    {
                        s = s_index;
                        m = m_index;
                    }
                    else if(pp_check==2)
                    {
                        return 2;
                    }

                    mp_check = rc_mathParser(m,s);

                    if(mp_check==0)
                    {
                        m = m_index;
                        s= s_index;
                    }
                    else if(mp_check == 2)
                    {
                        return 2;
                    }

                string eid_tmp = rc_searchID(eid);

                if(eid_tmp.compare("") == 0)
                        eid_tmp = rc_searchID(eid+"$");

                 if(eid_tmp.compare("")==0)
                 {
                     rc_id[rc_id_count] = relative_scope+"."+eid+" "+eid_typeCheck+" "+rc_intToString(rc_id_count)+" "+rc_intToString(current_scope)+" 0 ";
                     if(eid_typeCheck.compare("#")==0)
                     {
                         rc_id[rc_id_count] += rc_intToString(nid_count) + " 0 ";
                         nid_count++;
                     }
                     else
                     {
                         rc_id[rc_id_count] += rc_intToString(sid_count) + " 0 ";
                         sid_count++;
                     }
                     //cout << "RC_ID --> " << rc_id[rc_id_count] << endl;
                     rc_id_count++;
                     eid = rc_searchID(eid);
                     //rc_data_offset+=4;
                 }
                 else
                    eid = eid_tmp;

                //eid = rc_searchID(eid);

                eid_type = rc_getID_data(eid,"id_type");

                bool f_rtn = false;

                if(eid_type.compare("#P") == 0 || eid_type.compare("$P")==0)
                {
                    cout << "SUB Procedure cannot have return value" << endl;
                    return 2;
                }

                if(eid_type.compare("#F")==0 || eid_type.compare("$F")==0)
                {
                    if(relative_scope.compare(eid)!=0)
                    {
                        cout << "Ivalid function return" << endl;
                        return 2;
                    }
                    f_rtn = true;
                    //cout << "Function = data " << endl;
                }

//                if(eid_type.compare("#N")==0)
//                {
//                    //cout << "TYPE NULL NUMBER" << endl;
//                    rc_setIDType(search_index, "#");
//                }
//                else if(eid_type.compare("$N")==0)
//                {
//                    //cout << "TYPE NULL STRING" << endl;
//                    rc_setIDType(search_index, "$");
//                }

                 if(eid_type.substr(0,1).compare("#")==0)
                 {
                     //cout << "EID = " << eid << endl;
                     if(rc_result.substr(0,1).compare("m")!=0)
                     {
                         cout << "Expected Numerical expression in Numeric ID assignment" << endl;
                         cout << "Found: " << rc_result << endl << endl;
                         return 2;
                     }
                     if(f_rtn)
                     {
                         tmp[tmp_count] = "push " + rc_result;
                         tmp_count++;
                         tmp[tmp_count] = "return";
                         tmp_count++;
                     }
                     else
                    {
                        tmp[tmp_count] = "mov "+ eid +" " + rc_result;//v1 is id
                        tmp_count++;
                    }
                 }
                 else if(eid_type.substr(0,1).compare("$")==0)
                 {
                     if(s==0)
                     {
                         tmp[tmp_count] = "str$ s0 " + rc_result;
                         rc_result = "s0";
                         tmp_count++;
                     }
                     if(f_rtn)
                     {
                         tmp[tmp_count] = "push$ " + rc_result;
                         tmp_count++;
                         tmp[tmp_count] = "return";
                         tmp_count++;
                     }
                     else
                     {
                        tmp[tmp_count] = "mov$ "+eid+" "+rc_result;//v1 is id
                        tmp_count++;
                     }
                 }
                 else
                 {
                     cout << "cant find type\n";
                     return 3;
                 }
                 return 1;

                }
                //array[a,b,..]    note: I must check the type to make sure it is an array
                else if(rc_tokens[1].compare("<par>")==0 && (rc_getID_data(rc_searchID(eid),"id_type").compare("#A")==0 || rc_getID_data(rc_searchID(eid),"id_type").compare("$A")==0 ||
                                                             rc_getID_data(rc_searchID(eid),"id_type").compare("#&")==0 || rc_getID_data(rc_searchID(eid),"id_type").compare("$&")==0 ||
                                                             rc_getID_data(rc_searchID(eid+"$"),"id_type").compare("$&")==0 || rc_getID_data(rc_searchID(eid+"$"),"id_type").compare("$A")==0))
                {
                    //cout << "dream\n"; //string rd; cin >> rd;
                    p = 1;
                    for(int i = 2; i < rc_tokens_count; i++)
                    {
                        if(rc_tokens[i].compare("<par>")==0)
                            p++;
                        else if(rc_tokens[i].compare("</par>")==0)
                            p--;

                        if(p==0)
                        {
                            id_args_count1++;
                            if(i+2 < rc_tokens_count)
                            {
                                if(rc_tokens[i+1].compare("<equal>")==0)
                                {
                                    current_index = 0;
                                    for(i = i+2; i < rc_tokens_count; i++)
                                    {
                                        rc_tokens[current_index] = rc_tokens[i];
                                        current_index++;
                                    }
                                    rc_tokens_count = current_index;

                                    //rc_getArrays(m);
                                    if(rc_preParse()!=0)
                                        return 2;

                                    m = m_index;
                                    s = s_index;
                                    if(rc_mathParser(m,s)!=0)
                                        return 2;
                                    m = m_index;
                                    s = s_index;
                                    m_array = m_index;
                                    string arr_store = rc_result;

                                    for(i = 0; i < id_args_count1; i++)
                                    {
                                        rc_tokens_count = 0;
                                        for(int i2 = 0; i2 < id_args_count2[i]; i2++)
                                        {
                                            rc_tokens[i2] = id_args[i][i2];
                                            rc_tokens_count++;
                                        }
                                        rc_getArrays(m_array);
                                        if(rc_preParse()!=0)
                                            return 2;
                                        m_array = m_index;

                                        if(rc_mathParser(m_array, s_index)!=0)
                                            return 2;
                                        m_array = m_index;
                                        id_args[i][0] = rc_result;//  "m"+rc_intToString(m_array-1);
                                    }

                                    string eid_tmp = rc_searchID(eid);
                                    if(eid_tmp.compare("")==0)
                                    {
                                        eid_tmp = rc_searchID(eid+"$");
                                        if(eid_tmp.compare("")==0)
                                        {
                                            cout << "ID not found\n";
                                            //string s; cin >> s;
                                            return 2;
                                        }
                                        //cout << "array dbg: " << eid_tmp << endl;
                                        //string s; cin >> s;
                                    }
                                    eid = eid_tmp;
                                    //cout << "array dbg: " << eid_tmp << endl;
                                    //string arr_dbg; cin >> arr_dbg;

                                    eid_type = rc_getID_data(eid,"id_type");//cout << "type = " << eid_type << endl; string arr_dbg; cin >> arr_dbg;
                                    if((atoi(rc_getID_data(eid,"id_dim").c_str())==id_args_count1 && eid_type.compare("#A")==0) || eid_type.compare("#&")==0)
                                    {
                                        switch(id_args_count1)
                                        {
                                            case 1: tmp[tmp_count] = "set_arr1 " + eid + " " + id_args[0][0] + " " + arr_store;
                                                    tmp_count++;
                                                    break;
                                            case 2: tmp[tmp_count] = "set_arr2 " + eid + " " + id_args[0][0] + " " + id_args[1][0] + " " + arr_store;
                                                    tmp_count++;
                                                    break;
                                            case 3: tmp[tmp_count] = "set_arr3 " + eid + " " + id_args[0][0] + " " + id_args[1][0] + " " + id_args[2][0] + " " + arr_store;
                                                    tmp_count++;
                                                    break;
                                        }
                                        return 1;
                                    }
                                    else if((atoi(rc_getID_data(eid,"id_dim").c_str())==id_args_count1 && eid_type.compare("$A")==0) || eid_type.compare("$&")==0)
                                    {
                                        if(s==0)
                                        {
                                            tmp[tmp_count] = "str$ s" + rc_intToString(s_index) + " " + arr_store;
                                            arr_store = "s" + rc_intToString(s_index);
                                            tmp_count++;
                                            s_index++;
                                            s++;
                                        }
                                        switch(id_args_count1)
                                        {
                                            case 1: tmp[tmp_count] = "set_arr1$ " + eid + " " + id_args[0][0] + " " + arr_store;
                                                    tmp_count++;
                                                    break;
                                            case 2: tmp[tmp_count] = "set_arr2$ " + eid + " " + id_args[0][0] + " " + id_args[1][0] + " " + arr_store;
                                                    tmp_count++;
                                                    break;
                                            case 3: tmp[tmp_count] = "set_arr3$ " + eid + " " + id_args[0][0] + " " + id_args[1][0] + " " + id_args[2][0] + " " + arr_store;
                                                    tmp_count++;
                                                    break;
                                        }
                                        return 1;
                                    }
                                    else
                                    {
                                        cout << "set array error\n";
                                        //cout << atoi(rc_getID_data(eid,"id_dim").c_str()) << "----" << id_args_count1 << " does not match dimension count\n";
                                        return 2;
                                    }
                                }
                            }
                        }

                        if(rc_tokens[i].compare("<sep>")==0)
                        {
                            id_args_count1++;
                            id_args_count2[id_args_count1] = 0;
                        }
                        else
                        {
                            id_args[id_args_count1][id_args_count2[id_args_count1]] = rc_tokens[i];
                            id_args_count2[id_args_count1]++;
                        }
                    }
                }
            }
        }

    }

    //cout << "RETURN ZERO\n";

    return 0;
}

string if_control[99];
int if_control_index[99];
int sel_control_index[99];

int rc_evalBlock(int eb_flag = 0)
{
    int l_tokens_count1 = 2;
    int l_tokens_count2[3];
    string l_tokens[3][99];
    string * hold_tokens;
    string eid="";
    unsigned int m = 0;
    unsigned int s = 0;

    if(eb_flag == 1)
        return 0;

    if(rc_tokens[0].compare("<for>")==0) // if next then it all variables within in this scope will be deleted
    {
        //cout << endl;
        //for(int a = 0; a < rc_tokens_count; a++)
            //cout << rc_tokens[a] << endl;
        //cout << endl;
        if(rc_tokens[1].substr(0,4).compare("<id>")==0)
        {
            relative_scope = relative_scope + "." + "!FOR<" + rc_intToString(current_loop) + ">";

            current_loop++;
            current_scope++;
            //i need to search for id in all scopes above the one i am in
            eid = rc_searchID(rc_tokens[1].substr(4));
            if(eid.compare("")==0)
            {
                rc_id[rc_id_count] = relative_scope + "." + rc_tokens[1].substr(4) + " # " + rc_intToString(nid_count) + " " + rc_intToString(current_scope) + " 0 " + rc_intToString(nid_count) + " 0 ";
                eid = relative_scope + "." + rc_tokens[1].substr(4);
                rc_id_count++;
                nid_count++;
                //rc_data_offset += 4;
            }

            if(rc_tokens[2].compare("<equal>")==0)
            {
                int i = 0;
                l_tokens_count2[0] = 0;
                int l_token_index = 0;
                for(i = 3; i < rc_tokens_count; i++)
                {
                    if(rc_tokens[i].compare("<to>")==0)  // mov for<0>.i m#    mov for<0>.end_con m#    mov for<0>.step (m# or 1)
                        break;
                    l_tokens[0][l_token_index] = rc_tokens[i];
                    l_tokens_count2[0]++;
                    l_token_index++;
                }
                if(rc_tokens[i].compare("<to>")==0 && (i+1) < rc_tokens_count)
                {
                    l_tokens_count2[1] = 0;
                    l_token_index = 0;
                    for(i += 1; i < rc_tokens_count; i++)
                    {
                        if(rc_tokens[i].compare("<step>")==0)
                            break;
                        l_tokens[1][l_token_index] = rc_tokens[i];
                        l_tokens_count2[1]++;
                        l_token_index++;
                    }
                    if(rc_tokens[i].compare("<step>")==0 && i < rc_tokens_count)
                    {
                        l_tokens_count2[2] = 0;
                        l_token_index = 0;
                        for(i += 1; i < rc_tokens_count; i++)
                        {
                            l_tokens[2][l_token_index] = rc_tokens[i];
                            l_tokens_count2[2]++;
                            l_token_index++;
                        }
                        l_tokens_count1 = 3;
                    }

                    for(i = 0; i < l_tokens_count1; i++)
                    {
                        for(int j = 0; j < l_tokens_count2[i]; j++)
                        {
                            rc_tokens[j] = l_tokens[i][j];
                        }
                        rc_tokens_count = l_tokens_count2[i];
                        if(rc_preParse(1)!=0)
                            return 2;
                        m = m_index;
                        s = s_index;
                        if(rc_mathParser(m,s)!=0)
                            return 2;
                        m = m_index;
                        tmp[tmp_count] = "mov f" + rc_intToString(f_index) + " " + rc_result;
                        tmp_count++;
                        l_tokens[i][0] = "f" + rc_intToString(f_index); //rc_result;
                        f_index++;
                    }


                    if(l_tokens_count1 != 3)
                    {
                        tmp[tmp_count] = "mov f" + rc_intToString(f_index) + " 1 ";
                        l_tokens[2][0] = "f"+rc_intToString(f_index);
                        f_index++;
                        tmp_count++;
                    }

                    tmp[tmp_count] = "mov " + eid + " " + l_tokens[0][0];
                    tmp_count++;

                    //test of new for loop
                    tmp[tmp_count] = "for " + eid + " " + l_tokens[1][0] + " " + l_tokens[2][0];
                    tmp_count++;

                    tmp[tmp_count] = "label [" + relative_scope + "]";
                    tmp_count++;

                    rc_id[rc_id_count] = relative_scope + " [LABEL] ";

                    if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
                        rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
                    else
                        rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

                    //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
                    rc_id_count++;

                    if(num_stack_size < 3)
                        num_stack_size = 3;

                    if( (m_index+3) > max_m)
                        max_m = m_index + 3;
                    rc_clearTokens();
                    return 1;
                    //end of new for loop

                    tmp[tmp_count] = "jmp !fwd![" + relative_scope + "_init]"; //have start_con point to eid
                    tmp_count++;
                    tmp[tmp_count] = "label [" + relative_scope + "]";
                    tmp_count++;

                    rc_id[rc_id_count] = relative_scope + " [LABEL] ";

                    if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
                        rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
                    else
                        rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

                    //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
                    rc_id_count++;

                    //tmp_count++;

                    //for(int i = 2; i >= 0; i--)
                    //{
                    //    tmp[tmp_count] = "pop " + l_tokens[i][0];
                    //    tmp_count++;
                    //}

                    tmp[tmp_count] = "add " + eid + " " + l_tokens[2][0];
                    tmp_count++;
                    tmp[tmp_count] = "label [" + relative_scope + "_init]";

                    rc_id[rc_id_count] = relative_scope + "_init [LABEL] ";

                    if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
                        rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
                    else
                        rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

                    //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
                    rc_id_count++;

                    tmp_count++;
                    tmp[tmp_count] = "mov m" + rc_intToString(m_index) + " " + eid; //30
                    tmp_count++;
                    tmp[tmp_count] = "mov m" + rc_intToString(m_index+1) + " " + l_tokens[1][0]; //0
                    tmp_count++;
                    tmp[tmp_count] = "abs_sub m" + rc_intToString(m_index) + " " + l_tokens[0][0]; //35
                    tmp_count++;
                    tmp[tmp_count] = "abs_sub m" + rc_intToString(m_index+1) + " " + l_tokens[0][0];
                    tmp_count++;
                    //tmp[tmp_count] = "sub m" + rc_intToString(m_index) + " " + l_tokens[0][0]; //35
                    //tmp_count++;
                    //tmp[tmp_count] = "sub m" + rc_intToString(m_index+1) + " " + l_tokens[0][0];
                    //tmp_count++;
                    //tmp[tmp_count] = "abs m" + rc_intToString(m_index);
                    //tmp_count++;
                    //tmp[tmp_count] = "abs m" + rc_intToString(m_index+1);
                    //tmp_count++;
                    tmp[tmp_count] = "cmp m" + rc_intToString(m_index) + " m" + rc_intToString(m_index+1);
                    tmp_count++;
                    tmp[tmp_count] = "jg !fwd![" + relative_scope + "_end]";
                    tmp_count++;

                    //for(int i = 0; i < 3; i++)
                    //{
                    //    tmp[tmp_count] = "push " + l_tokens[i][0];
                    //    tmp_count++;
                    //}

                    if(num_stack_size < 3)
                        num_stack_size = 3;

                    if( (m_index+3) > max_m)
                        max_m = m_index + 3;

                    rc_clearTokens();

                    return 1;

                }
                else
                    return 2; //did not find <to> or reached end of line
            }
        }
        else
        {
            cout << "expected ID\n";
            return 2;
        }
    }
    else if(rc_tokens[0].compare("<next>")==0)
    {
        if(current_scope<=0)
        {
            cout << "cannot end for loop in this scope" << endl;
            return 2;
        }
        //cout << "NEXT at " << relative_scope.substr(relative_scope.find_last_of("<")-3,3) << endl;
        if(relative_scope.substr(relative_scope.find_last_of("<")-3,3).compare("FOR")!=0)
        {
            cout << "cannot find start of FOR loop" << endl;
            return 2;
        }
        tmp[tmp_count] = "next";
        tmp_count++;

        tmp[tmp_count] = "label [" + relative_scope + "_end]";

        rc_id[rc_id_count] = relative_scope + "_end [LABEL] ";

        if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
            rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
        else
            rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

        //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
        rc_id_count++;

        tmp_count++;

        rc_clearTokens();
        relative_scope = relative_scope.substr(0, relative_scope.find_last_of("."));
        //current_loop--;
        current_scope--;
        if(f_index > max_f)
            max_f = f_index;
        f_index -= 3;
        return 1;
        //tmp[tmp_count] = "jmp [" + relative_scope + "]";
        //cout << "JMP to THIS = " << rc_getID_data(relative_scope,"id_address").c_str() << endl;
        //tmp_count++;

        if(f_index > max_f)
            max_f = f_index;

        f_index -= 3;
        tmp[tmp_count] = "label [" + relative_scope + "_end]";

        rc_id[rc_id_count] = relative_scope + "_end [LABEL] ";

        if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
            rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
        else
            rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

        //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
        rc_id_count++;

        tmp_count++;

        relative_scope = relative_scope.substr(0, relative_scope.find_last_of("."));
        current_loop--;
        current_scope--;
        //cout << "Scope = " << relative_scope << endl;

        rc_clearTokens();

        return 1;
    }
    else if(rc_tokens[0].compare("<while>")==0)
    {
        if(rc_tokens_count <= 1)
        {
            cout << "Expected expression in WHILE\n";
            return 2;
        }
        rc_tokens[0] = "";
        relative_scope += ".!while<" + rc_intToString(current_loop) + ">";
        current_loop++;
        current_scope++;

        wcon_start[wcon_index] = tmp_count;
        if(rc_preParse(1)!=0)
            return 2;
        m = m_index;
        s = s_index;
        if(rc_mathParser(m,s)!=0)
            return 2;
        wcon_end[wcon_index] = tmp_count;
        wcon_index++;

        tmp[tmp_count] = "cmp " + rc_result + " 0 ";
        tmp_count++;
        tmp[tmp_count] = "je !fwd![" + relative_scope + ".!end_loop]";
        tmp_count++;

        //new while loop
        tmp[tmp_count] = "while " + rc_result;
        tmp_count++;

        tmp[tmp_count] = "label [" + relative_scope + "]";

        rc_id[rc_id_count] = relative_scope + " [LABEL] ";

        if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
            rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
        else
            rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

        //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
        rc_id_count++;

        tmp_count++;

        rc_clearTokens();
        return 1;

        tmp[tmp_count] = "cmp " + rc_result + " 0";
        tmp_count++;
        tmp[tmp_count] = "je [" + relative_scope + ".!end_loop]";
        tmp_count++;
        //cout << "Scope = " << relative_scope << endl;

        rc_clearTokens();

        return 1;
    }
    else if(rc_tokens[0].compare("<wend>")==0)
    {
        if(current_scope<=0)
        {
            cout << "cannot end while loop in this scope" << endl;
            return 2;
        }
        if(relative_scope.substr(relative_scope.find_last_of("<")-5,5).compare("while")!=0)
        {
            cout << "cannot find start of while loop" << endl;
            return 2;
        }
        if(rc_tokens_count > 1)
        {
            cout << "too many arguments for WEND\n";
            return 2;
        }

        wcon_index--;

        for(int i = wcon_start[wcon_index]; i < wcon_end[wcon_index]; i++)
        {
            tmp[tmp_count] = tmp[i];
            tmp_count++;
        }

        tmp[tmp_count] = "wend [" + relative_scope + "]";
        tmp_count++;

        tmp[tmp_count] = "label [" + relative_scope + ".!end_loop]";

        rc_id[rc_id_count] = relative_scope + ".!end_loop [LABEL] ";

        if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
            rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
        else
            rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

        //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
        rc_id_count++;

        tmp_count++;

        relative_scope = relative_scope.substr(0, relative_scope.find_last_of("."));
        //current_loop--;
        current_scope--;

        rc_clearTokens();
        return 1;

        tmp[tmp_count] = "jmp [" + relative_scope + "]";
        tmp_count++;
        tmp[tmp_count] = "label [" + relative_scope + ".!end_loop]";

        rc_id[rc_id_count] = relative_scope + ".!end_loop [LABEL] ";

        if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
            rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
        else
            rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

        //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
        rc_id_count++;

        tmp_count++;
        relative_scope = relative_scope.substr(0, relative_scope.find_last_of("."));
        current_loop--;
        current_scope--;

        rc_clearTokens();

        return 1;
    }
    else if(rc_tokens[0].compare("<do>")==0)
    {
        if(rc_tokens_count > 1)
        {
            cout << "too many arguments for DO\n";
            return 2;
        }
        relative_scope += ".!do<" + rc_intToString(current_loop) + ">";
        current_loop++;
        current_scope++;

        tmp[tmp_count] = "do";
        tmp_count++;

        tmp[tmp_count] = "label [" + relative_scope + "]";

        rc_id[rc_id_count] = relative_scope + " [LABEL] ";

        if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
            rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
        else
            rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

        rc_id_count++;

        tmp_count++;

        rc_clearTokens();
        return 1;

        tmp[tmp_count] = "label [" + relative_scope + "]";

        rc_id[rc_id_count] = relative_scope + " [LABEL] ";

        if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
            rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
        else
            rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

        //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
        rc_id_count++;

        tmp_count++;

        rc_clearTokens();

        return 1;
    }
    else if(rc_tokens[0].compare("<loop>")==0)
    {
        if(current_scope<=0)
        {
            cout << "cannot end DO loop in this scope" << endl;
            return 2;
        }
        if(relative_scope.substr(relative_scope.find_last_of("<")-2,2).compare("do")!=0)
        {
            cout << "cannot find start of do loop" << endl;
            return 2;
        }
        if(rc_tokens_count == 1)
        {
            //new loop
            tmp[tmp_count] = "loop";
            tmp_count++;
            tmp[tmp_count] = "label [" + relative_scope + ".!end_loop]";
            tmp_count++;
            current_scope--;
            //current_loop--;
            relative_scope = relative_scope.substr(0, relative_scope.find_last_of("."));
            rc_clearTokens();
            return 1;

            tmp[tmp_count] = "jmp [" + relative_scope + "]";
            tmp_count++;
            tmp[tmp_count] = "label [" + relative_scope + ".!end_loop]";
            tmp_count++;
            current_loop--;
            current_scope--;
            relative_scope = relative_scope.substr(0, relative_scope.find_last_of("."));

            rc_clearTokens();

            return 1;
        }
        else
        {
            if(rc_tokens[1].compare("<until>")==0) //still needs tweeking, to do: need to check if rc_result is s# or m#
            {
                rc_tokens[0] = "";
                rc_tokens[1] = "";
                if(rc_preParse(1)!=0)
                    return 2;
                m = m_index;
                s = s_index;
                if(rc_mathParser(m,s)!=0)
                    return 2;
                m = m_index;
                tmp[tmp_count] = "loop_until " + rc_result;
                tmp_count++;
                tmp[tmp_count] = "label [" + relative_scope + ".!end_loop]";
                tmp_count++;
                relative_scope = relative_scope.substr(0, relative_scope.find_last_of("."));
                //current_loop--;
                current_scope--;
                rc_clearTokens();
                return 1;

                tmp[tmp_count] = "cmp " + rc_result + " 1";
                tmp_count++;
                tmp[tmp_count] = "jne [" + relative_scope + "]";
                tmp_count++;
                tmp[tmp_count] = "label [" + relative_scope + ".!end_loop]";
                tmp_count++;
                relative_scope = relative_scope.substr(0, relative_scope.find_last_of("."));
                current_loop--;
                current_scope--;

                rc_clearTokens();

                return 1;
            }
            else if(rc_tokens[1].compare("<while>")==0)
            {
                //cout << "found loop while bitch\n";
                rc_tokens[0] = "";
                rc_tokens[1] = "";
                if(rc_preParse(1)!=0)
                {
                    cout << "preparse error\n";
                    return 2;
                }
                for(int i = 0; i < rc_tokens_count; i++)
                    cout << "RC_TOKENS[" << i << "] = " << rc_tokens[i] << endl;
                if(rc_mathParser(m_index,s_index)!=0)
                {
                    cout << "mathParser error\n";
                    return 2;
                }
                m = m_index;
                tmp[tmp_count] = "loop_while " + rc_result;
                tmp_count++;
                tmp[tmp_count] = "label [" + relative_scope + ".!end_loop]";
                tmp_count++;
                relative_scope = relative_scope.substr(0, relative_scope.find_last_of("."));
                //current_loop--;
                current_scope--;
                rc_clearTokens();
                return 1;

                tmp[tmp_count] = "cmp m" + rc_intToString(m-1) + " 1";
                tmp_count++;
                tmp[tmp_count] = "je [" + relative_scope + "]";
                tmp_count++;
                tmp[tmp_count] = "label [" + relative_scope + ".!end_loop]";
                tmp_count++;
                relative_scope = relative_scope.substr(0, relative_scope.find_last_of("."));
                current_loop--;
                current_scope--;

                rc_clearTokens();

                return 1;
            }
            else
            {
                cout << "expected UNTIL or WHILE after LOOP\n";
                return 2;
            }
        }
    }
    else if(rc_tokens[0].compare("<if>")==0)
    {
        rc_tokens[0] = "";
        if(rc_tokens[rc_tokens_count-1].compare("<then>")!=0)
        {
            cout << "expected THEN statement\n";
            return 2;
        }
        rc_tokens[rc_tokens_count-1] = "";
        if(rc_preParse(1)!=0)
            return 2;
        if(rc_mathParser(m_index, s_index)==0)
        {
            if(rc_result.substr(0,1).compare("s")==0)
            {
                tmp[tmp_count] = "mov m" + rc_intToString(m_index) + " 0";
                rc_result = "m"+rc_intToString(m_index);
                tmp_count++;
                m_index++;
            }
            else if(rc_result.substr(0,1).compare("m")==0)
            {
                //already in number form
            }
            else
            {
                cout << "could not determine result in condition\n";
                return 2;
            }

            relative_scope += ".IF<" + rc_intToString(block_handle) + ">";
            current_scope++;

            tmp[tmp_count] = "cmp " + rc_result + " 0";
            tmp_count++;
            //tmp[tmp_count] = "je [" + relative_scope + "_Condition<" + rc_intToString(current_condition[block_handle]) + ">]";
            if_control_index[current_scope] = 0;
            tmp[tmp_count] = "je [" + relative_scope + "_Condition<" + rc_intToString(if_control_index[current_scope]) + ">]";
            //cout << "if: " << tmp[tmp_count] << endl;
            tmp_count++;
            //current_condition[block_handle]++;
            block_handle++;
            //cout << "block_handle is now " << block_handle << endl;
            //string dbg; cin >> dbg;
            //cout <<"IF START\n";
            return 1;
        }
        else
            return 2;

    }
    else if(rc_tokens[0].compare("<else>")==0)
    {
        if(current_scope<=0)
        {
            cout << "cannot continue if block in this scope" << endl;
            return 2;
        }

        if(relative_scope.substr(relative_scope.find_last_of("<")-2,2).compare("IF")!=0)
        {
            cout << "cannot find if block to continue" << endl;
            return 2;
        }
        if(block_handle<=0)
        {
            cout << "never started IF block\n";
            return 2;
        }

        if(rc_tokens[1].compare("<if>")==0)
        {
            rc_tokens[0] = "";
            rc_tokens[1] = "";
            if(rc_tokens[rc_tokens_count-1].compare("<then>")!=0)
            {
                cout << "expected THEN statement\n";
                return 2;
            }
            rc_tokens[rc_tokens_count-1] = "";
            tmp[tmp_count] = "jmp [" + relative_scope + "_end]";
            tmp_count++;
            //tmp[tmp_count] = "label [" + relative_scope + "_Condition<" + rc_intToString(current_condition[block_handle-1]) + ">]";
            tmp[tmp_count] = "label [" + relative_scope + "_Condition<" + rc_intToString(if_control_index[current_scope]) + ">]";
            //cout << "Else: " << tmp[tmp_count] << endl;

            //rc_id[rc_id_count] = relative_scope + "_Condition<" + rc_intToString(current_condition[block_handle-1]) + ">";
            rc_id[rc_id_count] = relative_scope + "_Condition<" + rc_intToString(if_control_index[current_scope]) + ">";
            if_control_index[current_scope]++;


            if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
                rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
            else
                rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

            //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
            rc_id_count++;

            tmp_count++;

            if(rc_preParse(1)!=0)
                return 2;
            if(rc_mathParser(m_index, s_index)==0)
            {
                if(rc_result.substr(0,1).compare("s")==0)
                {
                    tmp[tmp_count] = "mov m" + rc_intToString(m_index) + " 0";
                    rc_result = "m" + rc_intToString(m_index);
                    tmp_count++;
                    m_index++;
                }
                else if(rc_result.substr(0,1).compare("m")==0)
                {
                    //already in number form
                }
                else
                {
                    cout << "could not determine result in condition\n";
                    rc_clearTokens();
                    return 2;
                }



                tmp[tmp_count] = "cmp " + rc_result + " 0";
                tmp_count++;
                //tmp[tmp_count] = "je [" + relative_scope + "_Condition<" + rc_intToString(current_condition[block_handle-1]+1) + ">]";
                tmp[tmp_count] = "je [" + relative_scope + "_Condition<" + rc_intToString(if_control_index[current_scope]) + ">]";
                tmp_count++;
                current_condition[block_handle-1]++;
                rc_clearTokens();
                return 1;
            }
            else
            {
                rc_clearTokens();
                return 2; //rc_mathParser failed
            }
        }
        else if(rc_tokens_count==1)
        {
            tmp[tmp_count] = "jmp [" + relative_scope + "_end]";
            tmp_count++;
            //tmp[tmp_count] = "label [" + relative_scope + "_Condition<" + rc_intToString(current_condition[block_handle-1]) + ">]";
            tmp[tmp_count] = "label [" + relative_scope + "_Condition<" + rc_intToString(if_control_index[current_scope]) + ">]";

            //rc_id[rc_id_count] = relative_scope + "_Condition<" + rc_intToString(current_condition[block_handle-1]) + ">";
            rc_id[rc_id_count] = relative_scope + "_Condition<" + rc_intToString(if_control_index[current_scope]) + ">";
            if_control_index[current_scope]++;

            if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
                rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
            else
                rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

            //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
            rc_id_count++;

            tmp_count++;
            current_condition[block_handle-1]++;
            rc_clearTokens();
            return 1;
        }
    }
    else if(rc_tokens[0].compare("<elseif>")==0)
    {
        if(current_scope<=0)
        {
            cout << "cannot continue if block in this scope" << endl;
            return 2;
        }

        if(relative_scope.substr(relative_scope.find_last_of("<")-2,2).compare("IF")!=0)
        {
            cout << "cannot find if block to continue" << endl;
            return 2;
        }
        if(block_handle<=0)
        {
            cout << "never started IF block\n";
            return 2;
        }

        rc_tokens[0] = "";
        //rc_tokens[1] = "";
        if(rc_tokens[rc_tokens_count-1].compare("<then>")!=0)
        {
            cout << "expected THEN statement\n";
            return 2;
        }
        rc_tokens[rc_tokens_count-1] = "";
        tmp[tmp_count] = "jmp [" + relative_scope + "_end]";
        tmp_count++;
        //tmp[tmp_count] = "label [" + relative_scope + "_Condition<" + rc_intToString(current_condition[block_handle-1]) + ">]";
        tmp[tmp_count] = "label [" + relative_scope + "_Condition<" + rc_intToString(if_control_index[current_scope]) + ">]";
        //cout << "Else: " << tmp[tmp_count] << endl;

        //rc_id[rc_id_count] = relative_scope + "_Condition<" + rc_intToString(current_condition[block_handle-1]) + ">";
        rc_id[rc_id_count] = relative_scope + "_Condition<" + rc_intToString(if_control_index[current_scope]) + ">";
        if_control_index[current_scope]++;

        if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
            rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
        else
            rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

        //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
        rc_id_count++;

        tmp_count++;

        if(rc_preParse(1)!=0)
            return 2;
        if(rc_mathParser(m_index, s_index)==0)
        {
            if(rc_result.substr(0,1).compare("s")==0)
            {
                tmp[tmp_count] = "mov m" + rc_intToString(m_index) + " 0";
                rc_result = "m" + rc_intToString(m_index);
                tmp_count++;
                m_index++;
            }
            else if(rc_result.substr(0,1).compare("m")==0)
            {
                //already in number form
            }
            else
            {
                cout << "could not determine result in condition\n";
                rc_clearTokens();
                return 2;
            }

            tmp[tmp_count] = "cmp " + rc_result + " 0";
            tmp_count++;
            //tmp[tmp_count] = "je [" + relative_scope + "_Condition<" + rc_intToString(current_condition[block_handle-1]+1) + ">]";
            tmp[tmp_count] = "je [" + relative_scope + "_Condition<" + rc_intToString(if_control_index[current_scope]) + ">]";
            tmp_count++;
            current_condition[block_handle-1]++;
            rc_clearTokens();
            return 1;
        }
        else
        {
            rc_clearTokens();
            return 2; //rc_mathParser failed
        }

    }
    else if(rc_tokens[0].compare("<end>")==0)
    {
        if(rc_tokens_count == 1)
        {
            tmp[tmp_count] = "end 0";
            tmp_count++;
            rc_clearTokens();
            rc_tokens_count = 0;
            rc_clearTokens();
            return 1;
        }

        if(rc_tokens[1].compare("<if>")==0)
        {
            if(current_scope<=0)
            {
                cout << "Cannot end if block in this scope" << endl;
                return 2;
            }
            if(relative_scope.substr(relative_scope.find_last_of("<")-2,2).compare("IF")!=0)
            {
                cout << "Cannot close if block without starting it" << endl;
                return 2;
            }

            //tmp[tmp_count] = "label [" + relative_scope + "_Condition<0>]";
            //cout << "End If: " << tmp[tmp_count] << endl;

            //rc_id[rc_id_count] = relative_scope + "_Condition<0>";

            //if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
            //    rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
            //else
             //   rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

            //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
            //rc_id_count++;

            //tmp_count++;

            //tmp[tmp_count] = "label [" + relative_scope + "_Condition<" + rc_intToString(current_condition[block_handle-1]) + ">]";
            tmp[tmp_count] = "label [" + relative_scope + "_Condition<" + rc_intToString(if_control_index[current_scope]) + ">]";
            //cout << "End If: " << tmp[tmp_count] << endl;

            //rc_id[rc_id_count] = relative_scope + "_Condition<" + rc_intToString(current_condition[block_handle-1]) + ">";
            rc_id[rc_id_count] = relative_scope + "_Condition<" + rc_intToString(if_control_index[current_scope]) + ">";

            if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
                rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
            else
                rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

            //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
            rc_id_count++;

            tmp_count++;
            tmp[tmp_count] = "label [" + relative_scope + "_end]";

            rc_id[rc_id_count] = relative_scope + "_end";

            if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
                rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
            else
                rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

            //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
            rc_id_count++;

            tmp_count++;

            relative_scope = relative_scope.substr(0,relative_scope.find_last_of("."));
            current_scope--;
            //block_handle--;
            rc_clearTokens();
            return 1;
        }
        else if(rc_tokens[1].compare("<function>")==0)
        {
            if(current_scope<=0 || current_scope > 1)
            {
                cout << "Cannot end function in this scope" << endl;
                return 2;
            }
            if(rc_getID_data(relative_scope,"id_type").substr(1,1).compare("F")!=0)
            {
                cout << "No function was ever defined" << endl;
                return 2;
            }
            tmp[tmp_count] = "push 0";
            tmp_count++;
            tmp[tmp_count] = "return";
            tmp_count++;
            tmp[tmp_count] = ".CODE";
            tmp_count++;

            rc_segment_switch = RC_CODE_SEGMENT_SWITCH;

            relative_scope = relative_scope.substr(0, relative_scope.find_last_of("."));
            current_scope--;
            rc_clearTokens();
            return 1;
        }
        else if(rc_tokens[1].compare("<procedure>")==0)
        {
            if(current_scope<=0 || current_scope > 1)
            {
                cout << "Cannot end sub procedure in this scope" << endl;
                return 2;
            }
            if(rc_getID_data(relative_scope,"id_type").substr(1,1).compare("P")!=0)
            {
                cout << "No sub procedure was ever defined" << endl;
                return 2;
            }
            tmp[tmp_count] = "return";
            tmp_count++;
            tmp[tmp_count] = ".CODE";
            tmp_count++;

            rc_segment_switch = RC_CODE_SEGMENT_SWITCH;

            relative_scope = relative_scope.substr(0, relative_scope.find_last_of("."));
            current_scope--;
            rc_clearTokens();
            return 1;
        }
        else if(rc_tokens[1].compare("<select>")==0)
        {
            if(current_scope<=0)
            {
                cout << "Cannot close Select Block in this scope" << endl;
                return 2;
            }
            if(relative_scope.substr(relative_scope.find_last_of("<")-6,6).compare("select")!=0)
            {
                cout << "Cannot close Select Block without starting it" << endl;
                return 2;
            }
            tmp[tmp_count] = "label [" + relative_scope + "_Case<" + rc_intToString(sel_control_index[current_scope]) +">]";

            rc_id[rc_id_count] = relative_scope + "_Case<" + rc_intToString(sel_control_index[current_scope]) + ">";

            if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
                rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
            else
                rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

            //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
            rc_id_count++;

            tmp_count++;
            tmp[tmp_count] = "label [" + relative_scope + "_End]";

            rc_id[rc_id_count] = relative_scope + "_End";

            if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
                rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
            else
                rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

            //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
            rc_id_count++;

            tmp_count++;

            tmp[tmp_count] = "pop m0";
            tmp_count++;

            relative_scope = relative_scope.substr(0,relative_scope.find_last_of("."));
            select_result = select_result.substr(0, select_result.length()-1);
            //cout << "select result = " << select_result << endl;
            current_scope--;
            //block_handle--;
            rc_clearTokens();
            return 1;
        }
        else
        {
            //invalid argument for end
            cout << "Illegal argument for end" << endl;
            rc_clearTokens();
            return 2;
        }
    }
    else if(rc_tokens[0].compare("<exit>")==0)
    {
        if(current_scope <=0)
            return 2;
        if(rc_tokens_count != 2)
        {
            cout << "expected 1 argument \n";
            rc_clearTokens();
            return 2;
        }
        int exit_arg = 1;

        if(rc_tokens[1].compare("<for>")==0)
        {
            //string ex;
            string rs = relative_scope;
            string a = "";
            while(true)
            {
                a = rs.substr(rs.find_last_of(".")+1);
                if(a.substr(0,3).compare("!do")==0 || a.substr(0,6).compare("!while")==0)
                    exit_arg++;
                //cout << "EXIT: " << rs << "  ->  " << a << endl;
                //cin >> ex;
                if(a.substr(0,4).compare("!FOR") == 0)
                {
                    tmp[tmp_count] = "exit " + rc_intToString(exit_arg);
                    tmp_count++;
                    tmp[tmp_count] = "jmp !fwd![" + rs + "_end" + "]";
                    //cout << "::::::: " << tmp[tmp_count] << endl;
                    tmp_count++;
                    return 1;
                }
                if(a.compare("PROGRAM") == 0)
                    break;

                rs = rs.substr(0, rs.find_last_of("."));
            }
            cout << "Could not find FOR block to exit" << endl;
            return 2;
        }
        else if(rc_tokens[1].compare("<do>")==0)
        {
            //string ex;
            string rs = relative_scope;
            string a = "";
            while(true)
            {
                a = rs.substr(rs.find_last_of(".")+1);
                if(a.substr(0,4).compare("!FOR")==0 || a.substr(0,6).compare("!while")==0)
                    exit_arg++;
                //cout << "EXIT: " << rs << "  ->  " << a << endl;
                //cin >> ex;
                if(a.substr(0,3).compare("!do") == 0)
                {
                    tmp[tmp_count] = "exit " + rc_intToString(exit_arg);
                    tmp_count++;
                    tmp[tmp_count] = "jmp !fwd![" + rs + ".!end_loop" + "]";
                    //cout << "::::::: " << tmp[tmp_count] << endl;
                    tmp_count++;
                    return 1;
                }
                if(a.compare("PROGRAM") == 0)
                    break;

                rs = rs.substr(0, rs.find_last_of("."));
            }
            cout << "could not find DO block to exit" << endl;
            return 2;
        }
        else if(rc_tokens[1].compare("<while>")==0)
        {
            //string ex;
            string rs = relative_scope;
            string a = "";
            while(true)
            {
                a = rs.substr(rs.find_last_of(".")+1);
                if(a.substr(0,3).compare("!do")==0 || a.substr(0,4).compare("!FOR")==0)
                    exit_arg++;
                //cout << "EXIT: " << rs << "  ->  " << a << endl;
                //cin >> ex;
                if(a.substr(0,6).compare("!while") == 0)
                {
                    tmp[tmp_count] = "exit " + rc_intToString(exit_arg);
                    tmp_count++;
                    tmp[tmp_count] = "jmp !fwd![" + rs + ".!end_loop" + "]";
                    //cout << "WHILE::::::: " << tmp[tmp_count] << endl;
                    tmp_count++;
                    return 1;
                }
                if(a.compare("PROGRAM") == 0)
                    break;

                rs = rs.substr(0, rs.find_last_of("."));
            }
            cout << "could not find While block to exit" << endl;
            return 2;
        }
        else
        {
            //invalid argument for exit
            cout << "Illegal argument for exit" << endl;
            rc_clearTokens();
            return 2;
        }
    }
    else if(rc_tokens[0].compare("<select>")==0)
    {
        if(rc_tokens[1].compare("<case>")==0)
        {
            relative_scope += ".select<" + rc_intToString(block_handle) + ">";
            rc_tokens[0] = "";
            rc_tokens[1] = "";
            if(rc_preParse(1)!=0)
                return 2;
            if(rc_mathParser(m_index, s_index)!=0)
                return 2;
            tmp[tmp_count] = "push " + rc_result;
            tmp_count++;
            sel_control_index[current_scope+1] = 0;
            tmp[tmp_count] = "jmp [" + relative_scope + "_Case<" + rc_intToString(sel_control_index[current_scope+1]) + ">]";
            tmp_count++;
            select_result += rc_result.substr(0,1);
            m_index = 0;
            s_index = 0;
            current_scope++;
            block_handle++;
            rc_clearTokens();
        }
        else
        {
            cout << "Expected CASE followed by condition\n";
            return 2;
        }
    }
    else if(rc_tokens[0].compare("<case>")==0)
    {
        rc_tokens[0] = "";
        tmp[tmp_count] = "jmp [" + relative_scope + "_End]";
        tmp_count++;
        tmp[tmp_count] = "label [" + relative_scope + "_Case<" + rc_intToString(sel_control_index[current_scope]) + ">]";

        rc_id[rc_id_count] = relative_scope + "_Case<" + rc_intToString(sel_control_index[current_scope]) + ">";
        sel_control_index[current_scope]++;

        if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
            rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
        else
            rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

        //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
        rc_id_count++;

        tmp_count++;
        if(rc_preParse(1)!=0)
            return 2;
        if(rc_mathParser(m_index,s_index)!=0)
            return 2;
        if(select_result.substr(select_result.length()-1,1).compare("s")==0)
        {
            if(rc_result.substr(0,1).compare("s")!=0)
            {
                tmp[tmp_count] = "str$ s" + rc_intToString(s_index) + " " + rc_result;
                rc_result = "s"+rc_intToString(s_index);
                s_index++;
                tmp_count++;
            }
            tmp[tmp_count] = "pop s" + rc_intToString(s_index);
            tmp_count++;
            tmp[tmp_count] = "cmp$ s" + rc_intToString(s_index) + " " + rc_result;
            tmp_count++;
            tmp[tmp_count] = "push s" + rc_intToString(s_index);
            tmp_count++;
            tmp[tmp_count] = "jne [" + relative_scope + "_Case<" + rc_intToString(sel_control_index[current_scope]) + ">]";
            tmp_count++;
        }
        else if(select_result.substr(select_result.length()-1,1).compare("m")==0)
        {
            if(rc_result.substr(0,1).compare("m")!=0)
            {
                cout << "can't compare number to non-numerical expression\n";
                return 2;
            }
            tmp[tmp_count] = "pop m" + rc_intToString(m_index);
            tmp_count++;
            tmp[tmp_count] = "cmp m" + rc_intToString(m_index) + " " + rc_result;
            tmp_count++;
            tmp[tmp_count] = "push m" + rc_intToString(m_index);
            tmp_count++;
            tmp[tmp_count] = "jne [" + relative_scope + "_Case<" + rc_intToString(sel_control_index[current_scope]) + ">]";
            //cout << current_condition[block_handle-1] << endl; cout << tmp[tmp_count] << endl; string s = ""; cin >> s;
            tmp_count++;
        }
        current_condition[block_handle-1]++;
        //cout << block_handle << endl; string s= ""; cin >> s;
        rc_clearTokens();

    }
    else if(rc_tokens[0].compare("<default>")==0)
    {
        tmp[tmp_count] = "jmp [" + relative_scope + "_End]";
        tmp_count++;
        tmp[tmp_count] = "label [" + relative_scope + "_Case<" + rc_intToString(sel_control_index[current_scope]) + ">]";

        rc_id[rc_id_count] = relative_scope + "_Case<" + rc_intToString(sel_control_index[current_scope]) + ">";
        sel_control_index[current_scope]++;

        if(rc_segment_switch == RC_DATA_SEGMENT_SWITCH)
            rc_id[rc_id_count] += rc_intToString(rc_data_offset) + " 55 65 75 ";
        else
            rc_id[rc_id_count] += rc_intToString(rc_code_offset) + " 55 65 75 ";

        //cout << "LABEL created called #" <<rc_id[rc_id_count] << "#" << endl;
        rc_id_count++;

        tmp_count++;
        current_condition[block_handle-1]++;
        rc_clearTokens();
    }
    else if(rc_tokens[0].compare("<return>")==0)
    {
        //cout << "CPR 1\n";

        //EXIT LOOP START
            int exit_arg = 0;
            string rs = relative_scope;
            string a = "";
            while(true)
            {
                a = rs.substr(rs.find_last_of(".")+1);
                if(a.substr(0,3).compare("!do")==0 || a.substr(0,4).compare("!FOR")==0 || a.substr(0,6).compare("!while")==0)
                    exit_arg++;
                //cout << "EXIT: " << rs << "  ->  " << a << endl;
                //cin >> ex;
                if(a.substr(0,7).compare("PROGRAM") == 0)
                {
                    tmp[tmp_count] = "r_exit " + rc_intToString(exit_arg);
                    tmp_count++;
                    break;
                }

                rs = rs.substr(0, rs.find_last_of("."));
            }
        //EXIT LOOP END

        eid = rc_searchID(relative_scope.substr(relative_scope.find_last_of(".")+1));
        string tmp_relscope = relative_scope;
        //cout  << "CPR 2: " << eid << endl;
        string eid_type = rc_getID_data(eid,"id_type");
        while(eid_type.compare("$F")!=0 || eid_type.compare("#F")!=0 || eid_type.compare("$P")!=0 || eid_type.compare("#P")!=0)
        {
            //cout << "TMP_REL +1 = " << tmp_relscope.substr(tmp_relscope.find_last_of(".")+1) << endl;
            eid = rc_searchID(tmp_relscope.substr(tmp_relscope.find_last_of(".")+1));
            eid_type = rc_getID_data(eid, "id_type");
            if(eid_type.compare("$F")==0 || eid_type.compare("#F")==0 || eid_type.compare("$P")==0 || eid_type.compare("#P")==0)
                break;
            if(tmp_relscope.find_last_of(".") == string::npos)
            {
                cout << "can only return from within a function\n";
                //cout << "EID = " << eid << endl;
                //cout << "rel scope = " << relative_scope << endl;
                return 2;
            }
            //else
                //cout << "V: " << tmp_relscope << endl;
            tmp_relscope = tmp_relscope.substr(0, tmp_relscope.find_last_of("."));
        }
        //cout << "CPR 3\n";
        if(eid_type.substr(1,1).compare("P") == 0)
        {
            tmp[tmp_count] = "return";
            tmp_count++;
            rc_clearTokens();
            //cout << "Cannot return from sub procedure\n";
            return 0;
        }
        //cout << "CPR 4\n";
        rc_tokens[0] = "";
        if(rc_preParse(1)!=0)
            return 2;
        //cout << "4.1\n";
        if(rc_mathParser(m_index, s_index)!=0)
            return 2;
        //cout << "4.2\n";
        if(rc_result.substr(0,1).compare("m")==0)
        {
            tmp[tmp_count] = "push " + rc_result;
            tmp_count++;
        }
        else if(rc_result.substr(0,1).compare("s")==0)
        {
            tmp[tmp_count] = "push$ " + rc_result;
            tmp_count++;
        }
        //cout << "CPR 5\n";
        tmp[tmp_count] = "return";
        tmp_count++;
        rc_clearTokens();
    }
    else if(rc_tokens[0].compare("<delete>")==0)
    {

            //cout << "START\n";
        if(rc_tokens[1].substr(0,4).compare("<id>")==0 && rc_tokens_count == 2)
        {
            rc_tokens[1] = rc_searchID(rc_tokens[1].substr(4));
            if(rc_tokens[1].compare("")==0)
            {
                cout << "could not find ID to delete" << endl;
                return 2;
            }
            else
            {
                tmp[tmp_count] = "delete " + rc_tokens[1];
                tmp_count++;
                string id_type = rc_getID_data(rc_tokens[1],"id_type");
                //cout << "GET_ID_DATA = " << id_type << endl;
                if(id_type.compare("#")==0 || id_type.compare("#A")==0)
                    id_type = "#";
                else
                    id_type = "$";
                rc_setIDType(search_index, id_type);
                rc_clearTokens();
                //rc_id[search_index] = "";
            }
        }
    }
    else if(rc_tokens[0].compare("<print>")==0)
    {
        rc_tokens[0] = "<par>";//cout << "check1\n";
        rc_tokens[rc_tokens_count] = "</par>";
        rc_tokens_count++;
        if(rc_preParse(1)!=0)
            return 2;//cout << "check2\n";
        if(rc_mathParser(m_index, s_index)!=0)
            return 2;
        tmp[tmp_count] = "print " + rc_result;
        tmp_count++;
        rc_clearTokens();
    }

    //cout << "GOTCHA\n";


    return 0;
}

bool rc_isID(string s_line)
{
    for(int i = 0; i < rc_id_count; i++)
    {
        if(s_line.compare(rc_id[i].substr(0,s_line.length()))==0)
            return true;
    }
    return false;
}

int rc_getLabelAddress(string lbl)
{
    string l_addr = "";
    for(int i = 0; i < rc_label_count; i++)
    {
        if(lbl.compare(rc_label.at(i).substr(0,rc_label.at(i).find_first_of(" ")))==0)
        {
            l_addr = rc_label.at(i).substr(rc_label.at(i).find_first_of(" "));
            //cout << "LBB " << rc_label.at(i) << endl;
            //cout << "ADDRESS = " << l_addr << endl;
            return atoi(l_addr.c_str());
        }
        //else
          //  cout << "#" << lbl <<"# NOT LABEL = #" << rc_label.at(i).substr(0,rc_label.at(i).find_first_of(" ")) << "#" << endl;
    }
    return -1;
}

int rc_getLabelAddressFrom(string lbl, int start_index)
{
    string l_addr = "";
    for(int i = start_index; i < rc_label_count; i++)
    {
        if(lbl.compare(rc_label.at(i).substr(0,rc_label.at(i).find_first_of(" ")))==0)
        {
            l_addr = rc_label.at(i).substr(rc_label.at(i).find_first_of(" "));
            //cout << "LBB " << rc_label.at(i) << endl;
            //cout << "ADDRESS = " << l_addr << endl;
            return atoi(l_addr.c_str());
        }
        //else
          //  cout << "#" << lbl <<"# NOT LABEL = #" << rc_label.at(i).substr(0,rc_label.at(i).find_first_of(" ")) << "#" << endl;
    }
    return -1;
}

string rc_getParameter(string s_line, int pnum)
{
    int n = 0;
    string n_par = "";
    s_line = s_line.substr(s_line.find_first_of(" "));
    s_line = s_line.substr(s_line.find_first_not_of(" "))+" ";
    //cout << "SLINE #" << s_line << "#" << endl;
    for(int i = 0; i < s_line.length(); i++)
    {
        if(s_line.substr(i,1).compare(" ") == 0)
        {
            n++;
            if(n == pnum)
            {
                //cout << "yahtzee #" << s_line.substr(i) << "#" << endl;
                return n_par;
            }
            n_par = "";
        }
        else
            n_par += s_line.substr(i,1);
    }
    return "";
}

union u_vm_cmd
{
    uint16_t i;
    unsigned char data[sizeof(uint16_t)];
};

union u_vm_raw
{
    double f;
    unsigned char data[sizeof(double)];
};

union u_vm_arg
{
    uint64_t i;
    unsigned char data[sizeof(uint64_t)];
};

bool rc_isSystemID(string arg)
{
    if(arg.compare("%EQUAL_FLAG")==0 || arg.compare("%NOT_EQUAL_FLAG")==0 || arg.compare("%GREATER_FLAG")==0 ||
       arg.compare("%GREATER_EQUAL_FLAG")==0 || arg.compare("%LESS_FLAG")==0 || arg.compare("%LESS_EQUAL_FLAG")==0 ||
       arg.compare("%EXIT_CODE")==0 || arg.compare("%RETURN_ADDRESS")==0)
        return true;

    return false;
}

unsigned int rc_getSystemID(string arg)
{
    if(arg.compare("%EQUAL_FLAG")==0)
        return 1;
    else if(arg.compare("%NOT_EQUAL_FLAG")==0)
        return 2;
    else if(arg.compare("%GREATER_FLAG")==0)
        return 3;
    else if(arg.compare("%LESS_FLAG")==0)
        return 4;
    else if(arg.compare("%GREATER_EQUAL_FLAG")==0)
        return 5;
    else if(arg.compare("%LESS_EQUAL_FLAG")==0)
        return 6;
    else if(arg.compare("%EXIT_CODE")==0)
        return 7;
    else if(arg.compare("%RETURN_ADDRESS")==0)
        return 8;

    return -1;
}

int rc_pokeSegment(int seg_switch, unsigned char seg_byte)
{
    vector<unsigned char>::iterator it;
    if(seg_switch == RC_CODE_SEGMENT_SWITCH)
    {
        it = RC_CODE_SEGMENT.begin();
        RC_CODE_SEGMENT.insert(it+rc_code_offset, seg_byte);
        rc_code_offset++;
        return 0;
    }
    else if(seg_switch == RC_DATA_SEGMENT_SWITCH)
    {
        it = RC_DATA_SEGMENT.begin();
        RC_DATA_SEGMENT.insert(it+rc_data_offset, seg_byte);
        rc_data_offset++;
        return 0;
    }
    return 1;
}

int rc_writeByteCode()
{
    unsigned int tmp_to_label[tmp_count];
    int byte_count = 0;
    vector <unsigned char> vm_bytes(128);
    vector <unsigned char>::iterator vmi = vm_bytes.begin();
    string lbl = "";
    unsigned int seg[3];
    seg[0] = 0;
    seg[1] = 0;
    seg[2] = 0;
    unsigned int cs_code = 0;
    unsigned int cs_data = 1;
    unsigned int cs_index = 0;
    //get address of labels
    for(int i = 0; i < tmp_count; i++)
    {
        tmp_to_label[i] = rc_label_count;
        if(tmp[i].compare(".CODE")==0)
            cs_index = RC_CODE_SEGMENT_SWITCH;

        if(tmp[i].compare(".DATA")==0)
            cs_index = RC_DATA_SEGMENT_SWITCH;

        if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("mov")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("mov$")==0 ||
           tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("pwr")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("mul")==0 ||
           tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("div")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("mod")==0 ||
           tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("add")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("add$")==0 ||
           tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("sub")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("shl")==0 ||
           tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("shr")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("cmp")==0 ||
           tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("cmp$")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("and")==0 ||
           tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("or")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("str$")==0 ||
           tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("ptr")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("abs_sub")==0)
        {
            seg[cs_index] += 4 + (vm_data_size * 2);
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("push")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("push$")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("pop")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("pop$")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("abs")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("end")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("gosub")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("intern")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("while")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("wend")==0)
        {
            seg[cs_index] += 3 + vm_data_size;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("arr1")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("arr1$")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("set_arr1")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("set_arr1$")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("alloc")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("for")==0)
        {
            seg[cs_index] += 5 + (vm_data_size * 3);
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("arr2")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("arr2$")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("set_arr2")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("set_arr2$")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("dim")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("dim$")==0)
        {
            seg[cs_index] += 6 + (vm_data_size * 4);
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("arr3")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("arr3$")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("set_arr3")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("set_arr3$")==0)
        {
            seg[cs_index] += 7 + (vm_data_size * 5);
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("return")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("next")==0||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("do")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("loop")==0)
        {
            seg[cs_index] += 2;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jmp")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("je")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jne")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jg")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jge")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jl")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jle")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("loop_until")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("loop_while")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("print")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("not")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("exit")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("r_exit")==0)
        {
            seg[cs_index] += 3 + vm_data_size;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("label")==0)
        {
            lbl = tmp[i].substr(tmp[i].find_first_of("[")+1);
            lbl = lbl.substr(0, lbl.find_last_of("]"));
            //cout << "###" <<lbl << "###" << endl;
            vector<string>::iterator it = rc_label.begin();
            if(cs_index == RC_CODE_SEGMENT_SWITCH)
            rc_label.insert(it+rc_label_count, lbl + " " + rc_intToString(seg[cs_index]+rc_code_offset));
            else if(cs_index == RC_DATA_SEGMENT_SWITCH)
            rc_label.insert(it+rc_label_count, lbl + " " + rc_intToString(seg[cs_index]+rc_data_offset));
            else
            {
                cout << "Writing to non existent segment\n";
                return -1;
            }
            //cout << "{" << cs_index << "} " << lbl << ": CREATE LB = " << seg[cs_index]+rc_code_offset << endl;
            //cout << "label = " << rc_label.at(rc_label_count) << endl;
            rc_label_count++;
        }
        //cout << "LINE [" << i << "] = " << seg[cs_index] << endl;
    }
    //replace jmp and gosub labels with address
    for(int i = 0; i < tmp_count; i++)
    {
        if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jmp")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("je")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jne")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jg")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jge")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jl")==0 ||
                tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jle")==0 || tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("wend")==0)
        {
            //cout << "L_LINE = " << tmp[i] << endl;
            lbl = tmp[i].substr(tmp[i].find_first_of("[")+1);
            lbl = lbl.substr(0,lbl.find_first_of("]"));
            if(tmp[i].find("!fwd!")!=string::npos)
            {
                //cout << "encountered FWD: " << tmp_to_label[i] << endl; string minaj; cin >> minaj;
                tmp[i] = tmp[i].substr(0, tmp[i].find_first_of(" ")) + " " + rc_intToString(rc_getLabelAddressFrom(lbl, tmp_to_label[i]));
            }
            else
                tmp[i] = tmp[i].substr(0, tmp[i].find_first_of(" ")) + " " + rc_intToString(rc_getLabelAddress(lbl));
            //cout << "%%% " << lbl << "=" << rc_intToString(rc_getLabelAddress(lbl)) << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("gosub")==0)
        {
            lbl = tmp[i].substr(tmp[i].find_first_of("[")+1);
            lbl = lbl.substr(0,lbl.find_first_of("]"));
            tmp[i] = tmp[i].substr(0, tmp[i].find_first_of(" ")) + " " + rc_intToString(rc_getLabelAddress(lbl));
            //cout << "N_LINE = " << tmp[i] << endl;
        }
    }
    //replace readable instructions with byte code
    string v1 = "";
    string v2 = "";
    string v3 = "";
    string v4 = "";
    string v5 = "";
    string v6 = "";
    u_vm_cmd cmd;
    unsigned char atype[6];
    u_vm_arg arg[6];
    u_vm_raw raw[6];
    int vmi_size = 0;
    rc_code_offset = 0;
    //rc_data_offset = 0;
    for(int i = 0; i < tmp_count; i++)
    {

        if(tmp[i].compare(".CODE")==0)
            cs_index = RC_CODE_SEGMENT_SWITCH;

        if(tmp[i].compare(".DATA")==0)
            cs_index = RC_DATA_SEGMENT_SWITCH;

        if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("mov")==0)
        {
            cmd.i = 32;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(v1.substr(0,1).compare("f")==0)
            {
                atype[0] = 9;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(v2.substr(0,1).compare("f")==0)
            {
                atype[1] = 9;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)) || v2.substr(0,1).compare("-") == 0)
            {
                atype[1] = 0;
                raw[1].f = atof(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);
            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[1] != 0)
                {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
                }
                else
                {
                    //cout << (int)raw[1].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[1].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("mov$")==0)
        {
            cmd.i = 33;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 5;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("s")==0)
            {
                atype[0] = 3;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 5;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("s")==0)
            {
                atype[1] = 3;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(v2.substr(0,1).compare("@")==0)
            {
                atype[1] = 2;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[1] != 0)
                {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
                }
                else
                {
                    //cout << (int)raw[1].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[1].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("pwr")==0)
        {
            cmd.i = 34;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(v1.substr(0,1).compare("f")==0)
            {
                atype[0] = 9;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(v2.substr(0,1).compare("f")==0)
            {
                atype[1] = 9;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)) || v2.substr(0,1).compare("-") == 0)
            {
                atype[1] = 0;
                raw[1].f = atof(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[1] != 0)
                {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
                }
                else
                {
                    //cout << (int)raw[1].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[1].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("mul")==0)
        {
            cmd.i = 35;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(v1.substr(0,1).compare("f")==0)
            {
                atype[0] = 9;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(v2.substr(0,1).compare("f")==0)
            {
                atype[1] = 9;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 0;
                raw[1].f = atof(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[1] != 0)
                {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
                }
                else
                {
                    //cout << (int)raw[1].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[1].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("div")==0)
        {
            cmd.i = 36;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(v1.substr(0,1).compare("f")==0)
            {
                atype[0] = 9;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(v2.substr(0,1).compare("f")==0)
            {
                atype[1] = 9;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)) || v2.substr(0,1).compare("-") == 0)
            {
                atype[1] = 0;
                raw[1].f = atof(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[1] != 0)
                {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
                }
                else
                {
                    //cout << (int)raw[1].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[1].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("mod")==0)
        {
            cmd.i = 37;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(v1.substr(0,1).compare("f")==0)
            {
                atype[0] = 9;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(v2.substr(0,1).compare("f")==0)
            {
                atype[1] = 9;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)) || v2.substr(0,1).compare("-") == 0)
            {
                atype[1] = 0;
                raw[1].f = atof(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[1] != 0)
                {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
                }
                else
                {
                    //cout << (int)raw[1].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[1].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("add")==0)
        {
            cmd.i = 38;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(v1.substr(0,1).compare("f")==0)
            {
                atype[0] = 9;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(v2.substr(0,1).compare("f")==0)
            {
                atype[1] = 9;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)) || v2.substr(0,1).compare("-") == 0)
            {
                atype[1] = 0;
                raw[1].f = atof(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[1] != 0)
                {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
                }
                else
                {
                    //cout << (int)raw[1].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[1].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("add$")==0)
        {
            cmd.i = 39;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 5;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("s")==0)
            {
                atype[0] = 3;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 5;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("s")==0)
            {
                atype[1] = 3;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 2;
                arg[1].i = atoi(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("sub")==0)
        {
            cmd.i = 40;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(v1.substr(0,1).compare("f")==0)
            {
                atype[0] = 9;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(v2.substr(0,1).compare("f")==0)
            {
                atype[1] = 9;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)) || v2.substr(0,1).compare("-") == 0)
            {
                atype[1] = 0;
                raw[1].f = atof(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[1] != 0)
                {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
                }
                else
                {
                    //cout << (int)raw[1].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[1].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("shl")==0)
        {
            cmd.i = 41;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(v1.substr(0,1).compare("f")==0)
            {
                atype[0] = 9;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(v2.substr(0,1).compare("f")==0)
            {
                atype[1] = 9;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)) || v2.substr(0,1).compare("-") == 0)
            {
                atype[1] = 0;
                raw[1].f = atof(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[1] != 0)
                {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
                }
                else
                {
                    //cout << (int)raw[1].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[1].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("shr")==0)
        {
            cmd.i = 42;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(v1.substr(0,1).compare("f")==0)
            {
                atype[0] = 9;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(v2.substr(0,1).compare("f")==0)
            {
                atype[1] = 9;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)) || v2.substr(0,1).compare("-") == 0)
            {
                atype[1] = 0;
                raw[1].f = atof(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[1] != 0)
                {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
                }
                else
                {
                    //cout << (int)raw[1].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[1].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("cmp")==0)
        {
            cmd.i = 43;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(v1.substr(0,1).compare("f")==0)
            {
                atype[0] = 9;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(v2.substr(0,1).compare("f")==0)
            {
                atype[1] = 9;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)) || v2.substr(0,1).compare("-") == 0)
            {
                atype[1] = 0;
                raw[1].f = atof(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[1] != 0)
                {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
                }
                else
                {
                    //cout << (int)raw[1].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[1].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("cmp$")==0)
        {
            cmd.i = 44;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 5;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("s")==0)
            {
                atype[0] = 3;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 5;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("s")==0)
            {
                atype[1] = 3;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 2;
                arg[1].i = atoi(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("str$")==0)
        {
            cmd.i = 45;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 5;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("s")==0)
            {
                atype[0] = 3;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)) || v2.substr(0,1).compare("-") == 0)
            {
                atype[1] = 0;
                raw[1].f = atof(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[1] != 0)
                {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
                }
                else
                {
                    //cout << (int)raw[1].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[1].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("and")==0)
        {
            cmd.i = 46;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)) || v2.substr(0,1).compare("-") == 0)
            {
                atype[1] = 0;
                raw[1].f = atof(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[1] != 0)
                {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
                }
                else
                {
                    //cout << (int)raw[1].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[1].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("or")==0)
        {
            cmd.i = 47;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)) || v2.substr(0,1).compare("-") == 0)
            {
                atype[1] = 0;
                raw[1].f = atof(v2.substr(1).c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[1] != 0)
                {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
                }
                else
                {
                    //cout << (int)raw[1].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[1].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("push")==0)
        {
            cmd.i = 48;
            v1 = rc_getParameter(tmp[i], 1);

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else if(rc_isNumber(v1.substr(0,1)) || v2.substr(0,1).compare("-") == 0)
            {
                atype[0] = 0;
                raw[0].f = atof(v1.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("push$")==0)
        {
            cmd.i = 49;
            v1 = rc_getParameter(tmp[i], 1);

            if(rc_isID(v1))
            {
                atype[0] = 5;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("s")==0)
            {
                atype[0] = 3;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 2;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("pop")==0)
        {
            cmd.i = 50;
            v1 = rc_getParameter(tmp[i], 1);

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("pop$")==0)
        {
            cmd.i = 51;
            v1 = rc_getParameter(tmp[i], 1);

            if(rc_isID(v1))
            {
                atype[0] = 5;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("s")==0)
            {
                atype[0] = 3;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("gosub")==0)
        {
            cmd.i = 52;
            v1 = rc_getParameter(tmp[i], 1);

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 0;
                arg[0].i = atoi(v1.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("arr1")==0)
        {
            cmd.i = 53;
            v1 = rc_getParameter(tmp[i], 1); //ID
            v2 = rc_getParameter(tmp[i], 2); //Index
            v3 = rc_getParameter(tmp[i], 3); //Destination

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 0;
                arg[1].i = atoi(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(rc_isID(v3))
            {
                atype[2] = 4;
                arg[2].i = atoi(rc_getID_data(v3, "id_index").c_str());
            }
            else if(v3.substr(0,1).compare("m")==0)
            {
                atype[2] = 1;
                arg[2].i = atoi(v3.substr(1).c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v3 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }

            //cout << (int)atype[2] << " data:";
            rc_pokeSegment(cs_index, atype[2]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[2].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[2].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("arr2")==0)
        {
            cmd.i = 54;
            v1 = rc_getParameter(tmp[i], 1); //ID
            v2 = rc_getParameter(tmp[i], 2); //Index 1
            v3 = rc_getParameter(tmp[i], 3); //Index 2
            v4 = rc_getParameter(tmp[i], 4); //Destination

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 0;
                arg[1].i = atoi(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(v3.substr(0,1).compare("m")==0)
            {
                atype[2] = 1;
                arg[2].i = atoi(v3.substr(1).c_str());
            }
            else if(rc_isSystemID(v3))
            {
                atype[2] = 6;
                arg[2].i = rc_getSystemID(v3);
            }
            else if(rc_isNumber(v3.substr(0,1)))
            {
                atype[2] = 0;
                arg[2].i = atoi(v3.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v3 << "\n";
                //return 1;
            }

            if(rc_isID(v4))
            {
                atype[3] = 4;
                arg[3].i = atoi(rc_getID_data(v4, "id_index").c_str());
            }
            else if(v4.substr(0,1).compare("m")==0)
            {
                atype[3] = 1;
                arg[3].i = atoi(v4.substr(1).c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v4 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }

            //cout << (int)atype[2] << " data:";
            rc_pokeSegment(cs_index, atype[2]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[2].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[2].data[a]);
            }

            //cout << (int)atype[3] << " data:";
            rc_pokeSegment(cs_index, atype[3]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[3].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[3].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("arr3")==0)
        {
            cmd.i = 55;
            v1 = rc_getParameter(tmp[i], 1); //ID
            v2 = rc_getParameter(tmp[i], 2); //Index 1
            v3 = rc_getParameter(tmp[i], 3); //Index 2
            v4 = rc_getParameter(tmp[i], 4); //Index 3
            v5 = rc_getParameter(tmp[i], 5); //Destination

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 0;
                arg[1].i = atoi(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(v3.substr(0,1).compare("m")==0)
            {
                atype[2] = 1;
                arg[2].i = atoi(v3.substr(1).c_str());
            }
            else if(rc_isSystemID(v3))
            {
                atype[2] = 6;
                arg[2].i = rc_getSystemID(v3);
            }
            else if(rc_isNumber(v3.substr(0,1)))
            {
                atype[2] = 0;
                arg[2].i = atoi(v3.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v3 << "\n";
                //return 1;
            }

            if(v4.substr(0,1).compare("m")==0)
            {
                atype[3] = 1;
                arg[3].i = atoi(v4.substr(1).c_str());
            }
            else if(rc_isSystemID(v4))
            {
                atype[3] = 6;
                arg[3].i = rc_getSystemID(v4);
            }
            else if(rc_isNumber(v4.substr(0,1)))
            {
                atype[3] = 0;
                arg[3].i = atoi(v4.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v4 << "\n";
                //return 1;
            }

            if(rc_isID(v5))
            {
                atype[4] = 4;
                arg[4].i = atoi(rc_getID_data(v5, "id_index").c_str());
            }
            else if(v5.substr(0,1).compare("m")==0)
            {
                atype[4] = 1;
                arg[4].i = atoi(v5.substr(1).c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v5 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }

            //cout << (int)atype[2] << " data:";
            rc_pokeSegment(cs_index, atype[2]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[2].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[2].data[a]);
            }

            //cout << (int)atype[3] << " data:";
            rc_pokeSegment(cs_index, atype[3]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[3].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[3].data[a]);
            }

            //cout << (int)atype[4] << " data:";
            rc_pokeSegment(cs_index, atype[4]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[4].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[4].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("arr1$")==0)
        {
            cmd.i = 56;
            v1 = rc_getParameter(tmp[i], 1); //ID
            v2 = rc_getParameter(tmp[i], 2); //Index
            v3 = rc_getParameter(tmp[i], 3); //Destination

            if(rc_isID(v1))
            {
                atype[0] = 5;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 0;
                arg[1].i = atoi(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(rc_isID(v3))
            {
                atype[2] = 5;
                arg[2].i = atoi(rc_getID_data(v3, "id_index").c_str());
            }
            else if(v3.substr(0,1).compare("s")==0)
            {
                atype[2] = 3;
                arg[2].i = atoi(v3.substr(1).c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v3 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }

            //cout << (int)atype[2] << " data:";
            rc_pokeSegment(cs_index, atype[2]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[2].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[2].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("arr2$")==0)
        {
            cmd.i = 57;
            v1 = rc_getParameter(tmp[i], 1); //ID
            v2 = rc_getParameter(tmp[i], 2); //Index 1
            v3 = rc_getParameter(tmp[i], 3); //Index 2
            v4 = rc_getParameter(tmp[i], 4); //Destination

            if(rc_isID(v1))
            {
                atype[0] = 5;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 0;
                arg[1].i = atoi(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(v3.substr(0,1).compare("m")==0)
            {
                atype[2] = 1;
                arg[2].i = atoi(v3.substr(1).c_str());
            }
            else if(rc_isSystemID(v3))
            {
                atype[2] = 6;
                arg[2].i = rc_getSystemID(v3);
            }
            else if(rc_isNumber(v3.substr(0,1)))
            {
                atype[2] = 0;
                arg[2].i = atoi(v3.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v3 << "\n";
                //return 1;
            }

            if(rc_isID(v4))
            {
                atype[3] = 5;
                arg[3].i = atoi(rc_getID_data(v4, "id_index").c_str());
            }
            else if(v4.substr(0,1).compare("s")==0)
            {
                atype[3] = 3;
                arg[3].i = atoi(v4.substr(1).c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v4 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }

            //cout << (int)atype[2] << " data:";
            rc_pokeSegment(cs_index, atype[2]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[2].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[2].data[a]);
            }

            //cout << (int)atype[3] << " data:";
            rc_pokeSegment(cs_index, atype[3]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[3].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[3].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("arr3$")==0)
        {
            cmd.i = 58;
            v1 = rc_getParameter(tmp[i], 1); //ID
            v2 = rc_getParameter(tmp[i], 2); //Index 1
            v3 = rc_getParameter(tmp[i], 3); //Index 2
            v4 = rc_getParameter(tmp[i], 4); //Index 3
            v5 = rc_getParameter(tmp[i], 5); //Destination

            if(rc_isID(v1))
            {
                atype[0] = 5;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 0;
                arg[1].i = atoi(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(v3.substr(0,1).compare("m")==0)
            {
                atype[2] = 1;
                arg[2].i = atoi(v3.substr(1).c_str());
            }
            else if(rc_isSystemID(v3))
            {
                atype[2] = 6;
                arg[2].i = rc_getSystemID(v3);
            }
            else if(rc_isNumber(v3.substr(0,1)))
            {
                atype[2] = 0;
                arg[2].i = atoi(v3.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v3 << "\n";
                //return 1;
            }

            if(v4.substr(0,1).compare("m")==0)
            {
                atype[3] = 1;
                arg[3].i = atoi(v4.substr(1).c_str());
            }
            else if(rc_isSystemID(v4))
            {
                atype[3] = 6;
                arg[3].i = rc_getSystemID(v4);
            }
            else if(rc_isNumber(v4.substr(0,1)))
            {
                atype[3] = 0;
                arg[3].i = atoi(v4.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v4 << "\n";
                //return 1;
            }

            if(rc_isID(v5))
            {
                atype[4] = 5;
                arg[4].i = atoi(rc_getID_data(v5, "id_index").c_str());
            }
            else if(v5.substr(0,1).compare("s")==0)
            {
                atype[4] = 3;
                arg[4].i = atoi(v5.substr(1).c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v5 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }

            //cout << (int)atype[2] << " data:";
            rc_pokeSegment(cs_index, atype[2]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[2].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[2].data[a]);
            }

            //cout << (int)atype[3] << " data:";
            rc_pokeSegment(cs_index, atype[3]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[3].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[3].data[a]);
            }

            //cout << (int)atype[4] << " data:";
            rc_pokeSegment(cs_index, atype[4]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[4].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[4].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("alloc")==0)
        {
            cmd.i = 59;
            v1 = rc_getParameter(tmp[i], 1); //Size
            v2 = rc_getParameter(tmp[i], 2); //Type
            v3 = rc_getParameter(tmp[i], 3); //Index

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 0;
                arg[0].i = atoi(v1.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v1 << "\n";
                //return 1;
            }

            if(v2.compare("#")==0)
            {
                atype[1] = 0;
                arg[1].i = 0;
            }
            else if(v2.compare("$")==0)
            {
                atype[1] = 0;
                arg[1].i = 1;
            }
            else
            {
                cout << "Invalid source argument: " << v2 << "\n";
            }

            if(rc_isID(v3.substr(1)))
            {
                atype[2] = 4;
                arg[2].i = atoi(rc_getID_data(v3.substr(1), "id_index").c_str());
            }
            else if(v3.substr(1,1).compare("m")==0)
            {
                atype[2] = 1;
                arg[2].i = atoi(v3.substr(2).c_str());
            }
            else if(rc_isSystemID(v3.substr(1)))
            {
                atype[2] = 6;
                arg[2].i = rc_getSystemID(v3.substr(1));
            }
            else if(rc_isNumber(v3.substr(1,1)))
            {
                atype[2] = 0;
                arg[2].i = atoi(v3.substr(1).c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v3.substr(1) << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }

            //cout << (int)atype[2] << " data:";
            rc_pokeSegment(cs_index, atype[2]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[2].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[2].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("set_arr1")==0)
        {
            cmd.i = 62;
            v1 = rc_getParameter(tmp[i], 1); //ID
            v2 = rc_getParameter(tmp[i], 2); //Index
            v3 = rc_getParameter(tmp[i], 3); //Value

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 0;
                arg[1].i = atoi(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(rc_isID(v3))
            {
                atype[2] = 4;
                arg[2].i = atoi(rc_getID_data(v3, "id_index").c_str());
            }
            else if(v3.substr(0,1).compare("m")==0)
            {
                atype[2] = 1;
                arg[2].i = atoi(v3.substr(1).c_str());
            }
            else if(rc_isSystemID(v3))
            {
                atype[2] = 6;
                arg[2].i = rc_getSystemID(v3);
            }
            else if(rc_isNumber(v3.substr(0,1)) || v2.substr(0,1).compare("-") == 0)
            {
                atype[2] = 0;
                raw[2].f = atof(v3.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v3 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }

            //cout << (int)atype[2] << " data:";
            rc_pokeSegment(cs_index, atype[2]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[2] != 0)
                {
                    //cout << (int)arg[2].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[2].data[a]);
                }
                else
                {
                    //cout << (int) raw[2].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[2].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("set_arr2")==0)
        {
            cmd.i = 63;
            v1 = rc_getParameter(tmp[i], 1); //ID
            v2 = rc_getParameter(tmp[i], 2); //Index 1
            v3 = rc_getParameter(tmp[i], 3); //Index 2
            v4 = rc_getParameter(tmp[i], 4); //Value

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 0;
                arg[1].i = atoi(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(rc_isID(v3))
            {
                atype[2] = 4;
                arg[2].i = atoi(rc_getID_data(v3, "id_index").c_str());
            }
            else if(v3.substr(0,1).compare("m")==0)
            {
                atype[2] = 1;
                arg[2].i = atoi(v3.substr(1).c_str());
            }
            else if(rc_isSystemID(v3))
            {
                atype[2] = 6;
                arg[2].i = rc_getSystemID(v3);
            }
            else if(rc_isNumber(v3.substr(0,1)))
            {
                atype[2] = 0;
                arg[2].i = atoi(v3.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v3 << "\n";
                //return 1;
            }

            if(rc_isID(v4))
            {
                atype[3] = 4;
                arg[3].i = atoi(rc_getID_data(v4, "id_index").c_str());
            }
            else if(v4.substr(0,1).compare("m")==0)
            {
                atype[3] = 1;
                arg[3].i = atoi(v4.substr(1).c_str());
            }
            else if(rc_isSystemID(v4))
            {
                atype[3] = 6;
                arg[3].i = rc_getSystemID(v4);
            }
            else if(rc_isNumber(v4.substr(0,1)) || v2.substr(0,1).compare("-") == 0)
            {
                atype[3] = 0;
                raw[3].f = atof(v4.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v4 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }

            //cout << (int)atype[2] << " data:";
            rc_pokeSegment(cs_index, atype[2]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[2].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[2].data[a]);
            }

            //cout << (int)atype[3] << " data:";
            rc_pokeSegment(cs_index, atype[3]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[3] != 0)
                {
                    //cout << (int)arg[3].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[3].data[a]);
                }
                else
                {
                    //cout << (int) raw[3].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[3].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("set_arr3")==0)
        {
            cmd.i = 64;
            v1 = rc_getParameter(tmp[i], 1); //ID
            v2 = rc_getParameter(tmp[i], 2); //Index 1
            v3 = rc_getParameter(tmp[i], 3); //Index 2
            v4 = rc_getParameter(tmp[i], 4); //Index 3
            v5 = rc_getParameter(tmp[i], 5); //Value

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 0;
                arg[1].i = atoi(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(rc_isID(v3))
            {
                atype[2] = 4;
                arg[2].i = atoi(rc_getID_data(v3, "id_index").c_str());
            }
            else if(v3.substr(0,1).compare("m")==0)
            {
                atype[2] = 1;
                arg[2].i = atoi(v3.substr(1).c_str());
            }
            else if(rc_isSystemID(v3))
            {
                atype[2] = 6;
                arg[2].i = rc_getSystemID(v3);
            }
            else if(rc_isNumber(v3.substr(0,1)))
            {
                atype[2] = 0;
                arg[2].i = atoi(v3.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v3 << "\n";
                //return 1;
            }

            if(rc_isID(v4))
            {
                atype[3] = 4;
                arg[3].i = atoi(rc_getID_data(v4, "id_index").c_str());
            }
            else if(v4.substr(0,1).compare("m")==0)
            {
                atype[3] = 1;
                arg[3].i = atoi(v4.substr(1).c_str());
            }
            else if(rc_isSystemID(v4))
            {
                atype[3] = 6;
                arg[3].i = rc_getSystemID(v4);
            }
            else if(rc_isNumber(v4.substr(0,1)))
            {
                atype[3] = 0;
                arg[3].i = atoi(v4.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v4 << "\n";
                //return 1;
            }

            if(rc_isID(v5))
            {
                atype[4] = 4;
                arg[4].i = atoi(rc_getID_data(v5, "id_index").c_str());
            }
            else if(v5.substr(0,1).compare("m")==0)
            {
                atype[4] = 1;
                arg[4].i = atoi(v5.substr(1).c_str());
            }
            else if(rc_isSystemID(v5))
            {
                atype[4] = 6;
                arg[4].i = rc_getSystemID(v5);
            }
            else if(rc_isNumber(v5.substr(0,1)) || v2.substr(0,1).compare("-") == 0)
            {
                atype[4] = 0;
                raw[4].f = atof(v5.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v5 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }

            //cout << (int)atype[2] << " data:";
            rc_pokeSegment(cs_index, atype[2]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[2].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[2].data[a]);
            }

            //cout << (int)atype[3] << " data:";
            rc_pokeSegment(cs_index, atype[3]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[3].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[3].data[a]);
            }

            //cout << (int)atype[4] << " data:";
            rc_pokeSegment(cs_index, atype[4]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[4] != 0)
                {
                    //cout << (int)arg[4].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[4].data[a]);
                }
                else
                {
                    //cout << (int) raw[4].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[4].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("set_arr1$")==0)
        {
            cmd.i = 65;
            v1 = rc_getParameter(tmp[i], 1); //ID
            v2 = rc_getParameter(tmp[i], 2); //Index
            v3 = rc_getParameter(tmp[i], 3); //Value

            if(rc_isID(v1))
            {
                atype[0] = 5;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 0;
                arg[1].i = atoi(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(rc_isID(v3))
            {
                atype[2] = 5;
                arg[2].i = atoi(rc_getID_data(v3, "id_index").c_str());
            }
            else if(v3.substr(0,1).compare("s")==0)
            {
                atype[2] = 3;
                arg[2].i = atoi(v3.substr(1).c_str());
            }
            else if(rc_isSystemID(v3))
            {
                atype[2] = 6;
                arg[2].i = rc_getSystemID(v3);
            }
            else
            {
                cout << "Invalid source argument: v3 = "<< v3 << "   i = " << i << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }

            //cout << (int)atype[2] << " data:";
            rc_pokeSegment(cs_index, atype[2]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[2] != 0)
                {
                    //cout << (int)arg[2].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[2].data[a]);
                }
                else
                {
                    //cout << (int) raw[2].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[2].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("set_arr2$")==0)
        {
            cmd.i = 66;
            v1 = rc_getParameter(tmp[i], 1); //ID
            v2 = rc_getParameter(tmp[i], 2); //Index 1
            v3 = rc_getParameter(tmp[i], 3); //Index 2
            v4 = rc_getParameter(tmp[i], 4); //Value

            if(rc_isID(v1))
            {
                atype[0] = 5;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 0;
                arg[1].i = atoi(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(rc_isID(v3))
            {
                atype[2] = 4;
                arg[2].i = atoi(rc_getID_data(v3, "id_index").c_str());
            }
            else if(v3.substr(0,1).compare("m")==0)
            {
                atype[2] = 1;
                arg[2].i = atoi(v3.substr(1).c_str());
            }
            else if(rc_isSystemID(v3))
            {
                atype[2] = 6;
                arg[2].i = rc_getSystemID(v3);
            }
            else if(rc_isNumber(v3.substr(0,1)))
            {
                atype[2] = 0;
                arg[2].i = atoi(v3.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v3 << "\n";
                //return 1;
            }

            if(rc_isID(v4))
            {
                atype[3] = 5;
                arg[3].i = atoi(rc_getID_data(v4, "id_index").c_str());
            }
            else if(v4.substr(0,1).compare("s")==0)
            {
                atype[3] = 3;
                arg[3].i = atoi(v4.substr(1).c_str());
            }
            else if(rc_isSystemID(v4))
            {
                atype[3] = 6;
                arg[3].i = rc_getSystemID(v4);
            }
            else
            {
                //cout << "Invalid source argument: "<< v4 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }

            //cout << (int)atype[2] << " data:";
            rc_pokeSegment(cs_index, atype[2]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[2].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[2].data[a]);
            }

            //cout << (int)atype[3] << " data:";
            rc_pokeSegment(cs_index, atype[3]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[3] != 0)
                {
                    //cout << (int)arg[3].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[3].data[a]);
                }
                else
                {
                    //cout << (int) raw[3].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[3].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("set_arr3$")==0)
        {
            cmd.i = 67;
            v1 = rc_getParameter(tmp[i], 1); //ID
            v2 = rc_getParameter(tmp[i], 2); //Index 1
            v3 = rc_getParameter(tmp[i], 3); //Index 2
            v4 = rc_getParameter(tmp[i], 4); //Index 3
            v5 = rc_getParameter(tmp[i], 5); //Value

            if(rc_isID(v1))
            {
                atype[0] = 5;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 0;
                arg[1].i = atoi(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(rc_isID(v3))
            {
                atype[2] = 4;
                arg[2].i = atoi(rc_getID_data(v3, "id_index").c_str());
            }
            else if(v3.substr(0,1).compare("m")==0)
            {
                atype[2] = 1;
                arg[2].i = atoi(v3.substr(1).c_str());
            }
            else if(rc_isSystemID(v3))
            {
                atype[2] = 6;
                arg[2].i = rc_getSystemID(v3);
            }
            else if(rc_isNumber(v3.substr(0,1)))
            {
                atype[2] = 0;
                arg[2].i = atoi(v3.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v3 << "\n";
                //return 1;
            }

            if(rc_isID(v4))
            {
                atype[3] = 4;
                arg[3].i = atoi(rc_getID_data(v4, "id_index").c_str());
            }
            else if(v4.substr(0,1).compare("m")==0)
            {
                atype[3] = 1;
                arg[3].i = atoi(v4.substr(1).c_str());
            }
            else if(rc_isSystemID(v4))
            {
                atype[3] = 6;
                arg[3].i = rc_getSystemID(v4);
            }
            else if(rc_isNumber(v4.substr(0,1)))
            {
                atype[3] = 0;
                arg[3].i = atoi(v4.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v4 << "\n";
                //return 1;
            }

            if(rc_isID(v5))
            {
                atype[4] = 5;
                arg[4].i = atoi(rc_getID_data(v5, "id_index").c_str());
            }
            else if(v5.substr(0,1).compare("s")==0)
            {
                atype[4] = 3;
                arg[4].i = atoi(v5.substr(1).c_str());
            }
            else if(rc_isSystemID(v5))
            {
                atype[4] = 6;
                arg[4].i = rc_getSystemID(v5);
            }
            else
            {
                cout << "Invalid source argument: "<< v5 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }

            //cout << (int)atype[2] << " data:";
            rc_pokeSegment(cs_index, atype[2]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[2].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[2].data[a]);
            }

            //cout << (int)atype[3] << " data:";
            rc_pokeSegment(cs_index, atype[2]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[3].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[3].data[a]);
            }

            //cout << (int)atype[4] << " data:";
            rc_pokeSegment(cs_index, atype[4]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[4] != 0)
                {
                    //cout << (int)arg[4].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[4].data[a]);
                }
                else
                {
                    //cout << (int) raw[4].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[4].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jmp")==0)
        {
            cmd.i = 68;
            v1 = rc_getParameter(tmp[i], 1); //address

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 0;
                arg[0].i = atoi(v1.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("je")==0)
        {
            cmd.i = 69;
            v1 = rc_getParameter(tmp[i], 1); //address

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 0;
                arg[0].i = atoi(v1.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jne")==0)
        {
            cmd.i = 70;
            v1 = rc_getParameter(tmp[i], 1); //address

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 0;
                arg[0].i = atoi(v1.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;// << "JNE = " << rc_code_offset << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jl")==0)
        {
            cmd.i = 71;
            v1 = rc_getParameter(tmp[i], 1); //address

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 0;
                arg[0].i = atoi(v1.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jg")==0)
        {
            cmd.i = 72;
            v1 = rc_getParameter(tmp[i], 1); //address

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 0;
                arg[0].i = atoi(v1.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jle")==0)
        {
            cmd.i = 73;
            v1 = rc_getParameter(tmp[i], 1); //address

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 0;
                arg[0].i = atoi(v1.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("jge")==0)
        {
            cmd.i = 74;
            v1 = rc_getParameter(tmp[i], 1); //address

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 0;
                arg[0].i = atoi(v1.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("abs")==0)
        {
            cmd.i = 75;
            v1 = rc_getParameter(tmp[i], 1); //address

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(v1.substr(0,1).compare("f")==0)
            {
                atype[0] = 9;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("end")==0)
        {
            cmd.i = 76;
            v1 = rc_getParameter(tmp[i], 1); //address

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 0;
                arg[0].i = atoi(v1.c_str());
            }
            else
            {
                //cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("return")==0)
        {
            cmd.i = 77;

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " " << endl;
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("xor")==0)
        {
            cmd.i = 78;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 0;
                raw[1].f = atof(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[1] != 0)
                {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
                }
                else
                {
                    //cout << (int)raw[1].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[1].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("not")==0)
        {
            cmd.i = 79;
            v1 = rc_getParameter(tmp[i], 1); //address

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 0;
                arg[0].i = atoi(v1.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("dim")==0)
        {
            cmd.i = 80;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            v3 = rc_getParameter(tmp[i], 3);
            v4 = rc_getParameter(tmp[i], 4);

            if(rc_isID(v1))
            {
                atype[0] = 0;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 0;
                arg[0].i = atoi(v1.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v1 << "\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 0;
                arg[1].i = atoi(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(rc_isID(v3))
            {
                atype[2] = 4;
                arg[2].i = atoi(rc_getID_data(v3, "id_index").c_str());
            }
            else if(v3.substr(0,1).compare("m")==0)
            {
                atype[2] = 1;
                arg[2].i = atoi(v3.substr(1).c_str());
            }
            else if(rc_isSystemID(v3))
            {
                atype[2] = 6;
                arg[2].i = rc_getSystemID(v3);
            }
            else if(rc_isNumber(v3.substr(0,1)))
            {
                atype[2] = 0;
                arg[2].i = atoi(v3.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v3 << "\n";
                //return 1;
            }

            if(rc_isID(v4))
            {
                atype[3] = 4;
                arg[3].i = atoi(rc_getID_data(v4, "id_index").c_str());
            }
            else if(v4.substr(0,1).compare("m")==0)
            {
                atype[3] = 1;
                arg[3].i = atoi(v4.substr(1).c_str());
            }
            else if(rc_isSystemID(v4))
            {
                atype[3] = 6;
                arg[3].i = rc_getSystemID(v4);
            }
            else if(rc_isNumber(v4.substr(0,1)))
            {
                atype[3] = 0;
                arg[3].i = atoi(v3.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v4 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }

            //cout << (int)atype[2] << " data:";
            rc_pokeSegment(cs_index, atype[2]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[2].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[2].data[a]);
            }

            //cout << (int)atype[3] << " data:";
            rc_pokeSegment(cs_index, atype[3]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[3].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[3].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("dim$")==0)
        {
            cmd.i = 81;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            v3 = rc_getParameter(tmp[i], 3);
            v4 = rc_getParameter(tmp[i], 4);

            if(rc_isID(v1))
            {
                atype[0] = 0;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 0;
                arg[0].i = atoi(v1.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v1 << "\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)))
            {
                atype[1] = 0;
                arg[1].i = atoi(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(rc_isID(v3))
            {
                atype[2] = 4;
                arg[2].i = atoi(rc_getID_data(v3, "id_index").c_str());
            }
            else if(v3.substr(0,1).compare("m")==0)
            {
                atype[2] = 1;
                arg[2].i = atoi(v3.substr(1).c_str());
            }
            else if(rc_isSystemID(v3))
            {
                atype[2] = 6;
                arg[2].i = rc_getSystemID(v3);
            }
            else if(rc_isNumber(v3.substr(0,1)))
            {
                atype[2] = 0;
                arg[2].i = atoi(v3.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v3 << "\n";
                //return 1;
            }

            if(rc_isID(v4))
            {
                atype[3] = 4;
                arg[3].i = atoi(rc_getID_data(v4, "id_index").c_str());
            }
            else if(v4.substr(0,1).compare("m")==0)
            {
                atype[3] = 1;
                arg[3].i = atoi(v4.substr(1).c_str());
            }
            else if(rc_isSystemID(v4))
            {
                atype[3] = 6;
                arg[3].i = rc_getSystemID(v4);
            }
            else if(rc_isNumber(v4.substr(0,1)))
            {
                atype[3] = 0;
                arg[3].i = atoi(v3.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v4 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }

            //cout << (int)atype[2] << " data:";
            rc_pokeSegment(cs_index, atype[2]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[2].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[2].data[a]);
            }

            //cout << (int)atype[3] << " data:";
            rc_pokeSegment(cs_index, atype[3]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[3].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[3].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("intern")==0)
        {
            cmd.i = 82;
            v1 = rc_getParameter(tmp[i], 1);

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 0;
                arg[0].i = atoi(v1.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("delete")==0)
        {
            cmd.i = 83;
            v1 = rc_getParameter(tmp[i], 1);


            if(rc_isID(v1) && rc_getID_data(v1,"id_type").substr(0,1).compare("#")==0)
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(rc_isID(v1) && rc_getID_data(v1,"id_type").substr(0,1).compare("$")==0)
            {
                atype[0] = 5;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else
            {
                cout << "Invalid Source Argument" << endl;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("ptr")==0)
        {
            cmd.i = 84;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);

            if(rc_isID(v1) && rc_getID_data(v1,"id_type").compare("#&")==0)
            {
                atype[0] = 7;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(rc_isID(v1) && rc_getID_data(v1,"id_type").compare("$&")==0)
            {
                atype[0] = 8;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else
            {
                cout << "Invalid Source Argument" << endl;
            }

            if(rc_isID(v2) && rc_getID_data(v2,"id_type").substr(0,1).compare("#")==0)
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(rc_isID(v2) && rc_getID_data(v2,"id_type").substr(0,1).compare("$")==0)
            {
                atype[1] = 5;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else
            {
                cout << "Invalid Source Argument" << endl;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("abs_sub")==0)
        {
            cmd.i = 85;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(v1.substr(0,1).compare("f")==0)
            {
                atype[0] = 9;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(rc_isID(v2))
            {
                atype[1] = 4;
                arg[1].i = atoi(rc_getID_data(v2, "id_index").c_str());
            }
            else if(v2.substr(0,1).compare("m")==0)
            {
                atype[1] = 1;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(v2.substr(0,1).compare("f")==0)
            {
                atype[1] = 9;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else if(rc_isSystemID(v2))
            {
                atype[1] = 6;
                arg[1].i = rc_getSystemID(v2);
            }
            else if(rc_isNumber(v2.substr(0,1)) || v2.substr(0,1).compare("-") == 0)
            {
                atype[1] = 0;
                raw[1].f = atof(v2.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[1] != 0)
                {
                    //cout << (int)arg[1].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[1].data[a]);
                }
                else
                {
                    //cout << (int)raw[1].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[1].data[a]);
                }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("for")==0)
        {
            cmd.i = 86;
            v1 = rc_getParameter(tmp[i], 1);
            v2 = rc_getParameter(tmp[i], 2);
            v3 = rc_getParameter(tmp[i], 3);

            if(rc_isID(v1))
            {
                //cout << "DEBUG: " << rc_getID_data(v1,"id_index") << endl;
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else
            {
                cout << "Invalid destination argument\n";
                //return 1;
            }

            if(v2.substr(0,1).compare("f")==0)
            {
                atype[1] = 9;
                arg[1].i = atoi(v2.substr(1).c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v2 << "\n";
                //return 1;
            }

            if(v3.substr(0,1).compare("f")==0)
            {
                atype[2] = 9;
                arg[2].i = atoi(v3.substr(1).c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v3 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                if(atype[0] != 0)
                {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
                }
                else
                {
                    //cout << (int)raw[0].data[a] << " ";
                    rc_pokeSegment(cs_index, raw[0].data[a]);
                }
            }

            //cout << (int)atype[1] << " data:";
            rc_pokeSegment(cs_index, atype[1]);

            for(int a = 0; a < vm_data_size; a++)
            {
                //cout << (int)arg[1].data[a] << " ";
                rc_pokeSegment(cs_index, arg[1].data[a]);
            }

            //cout << (int)atype[2] << " data:";
            rc_pokeSegment(cs_index, atype[2]);

            for(int a = 0; a < vm_data_size; a++)
            {
                //cout << (int)arg[2].data[a] << " ";
                rc_pokeSegment(cs_index, arg[2].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("next")==0)
        {
            cmd.i = 87;

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("while")==0)
        {
            cmd.i = 88;
            v1 = rc_getParameter(tmp[i], 1);

            if(rc_isID(v1))
            {
                atype[0] = 4;
                arg[0].i = atoi(rc_getID_data(v1, "id_index").c_str());
            }
            else if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(rc_isSystemID(v1))
            {
                atype[0] = 6;
                arg[0].i = rc_getSystemID(v1);
            }
            else if(rc_isNumber(v1.substr(0,1)) || v1.substr(0,1).compare("-") == 0)
            {
                atype[0] = 0;
                raw[0].f = atof(v1.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v1 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    if(atype[0] != 0)
                    {
                        //cout << (int)arg[0].data[a] << " ";
                        rc_pokeSegment(cs_index, arg[0].data[a]);
                    }
                    else
                    {
                        //cout << (int)raw[0].data[a] << " ";
                        rc_pokeSegment(cs_index, raw[0].data[a]);
                    }
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("wend")==0)
        {
            cmd.i = 89;
            v1 = rc_getParameter(tmp[i], 1);

            if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 0;
                arg[0].i = atoi(v1.c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v1 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                    //cout << (int)arg[0].data[a] << " ";
                    rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("do")==0)
        {
            cmd.i = 90;

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("loop")==0)
        {
            cmd.i = 91;

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("loop_until")==0)
        {
            cmd.i = 92;
            v1 = rc_getParameter(tmp[i], 1);

            if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v1 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                //cout << (int)arg[0].data[a] << " ";
                rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("loop_while")==0)
        {
            cmd.i = 93;
            v1 = rc_getParameter(tmp[i], 1);

            if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v1 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                //cout << (int)arg[0].data[a] << " ";
                rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("exit")==0)
        {
            //cout << "exit cmd\n";
            cmd.i = 94;
            v1 = rc_getParameter(tmp[i], 1);

            if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 0;
                arg[0].i = atoi(v1.c_str());
            }
            else
            {
                cout << "exit expected raw number as argument\n";
                return 2;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                //cout << (int)arg[0].data[a] << " ";
                rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("print")==0)
        {
            cmd.i = 95;
            v1 = rc_getParameter(tmp[i], 1);

            if(v1.substr(0,1).compare("m")==0)
            {
                atype[0] = 1;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else if(v1.substr(0,1).compare("s")==0)
            {
                atype[0] = 3;
                arg[0].i = atoi(v1.substr(1).c_str());
            }
            else
            {
                cout << "Invalid source argument: "<< v1 << "\n";
                //return 1;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                //cout << (int)arg[0].data[a] << " ";
                rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
        else if(tmp[i].substr(0,tmp[i].find_first_of(" ")).compare("r_exit")==0)
        {
            //cout << "r_exit cmd\n";
            cmd.i = 96;
            v1 = rc_getParameter(tmp[i], 1);

            if(rc_isNumber(v1.substr(0,1)))
            {
                atype[0] = 0;
                arg[0].i = atoi(v1.c_str());
            }
            else
            {
                cout << "exit expected raw number as argument\n";
                return 2;
            }

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << (int)atype[0] << " data:";
            rc_pokeSegment(cs_index, atype[0]);

            for(int a = 0; a < vm_data_size; a++)
            {
                //cout << (int)arg[0].data[a] << " ";
                rc_pokeSegment(cs_index, arg[0].data[a]);
            }
            //cout << endl;
        }
    }

    return 0;
}

fstream pp_file;
string pp_str = "rc_file_copy.bas";

int rc_init()
{
    //cout << "init\n";
    string s_line = "";

    pp_file.open(pp_str.c_str(), ios::out | ios::trunc);
    //cout << "file open\n";

    while(!rc_file.eof())
    {
        getline(rc_file, s_line);
        pp_file << s_line << endl;
    }
    pp_file.close();
}

int rc_preprocessor(string src_path)
{
    fstream pp_tmp;
    fstream pp_tmp2;
    pp_tmp.open("tmp.bas", fstream::in | fstream::out | fstream::trunc);
    //pp_tmp2.open("tmp2.bas", ios::out | ios::in);
    int a_status = 0;
    string s_line = "";
    int resolved = 0;
    unsigned long pp_pos = 0;
    string inc_file = "";
    int line_num = 1;
    int inc_line = 1;
    end_of_line_count = 0;
    bool inc_flag = false;
    bool inc_p = false;

    pp_file.open(pp_str.c_str(), ios::in);

    while(!pp_file.eof())
    {
        getline(pp_file, s_line);
        if(s_line.compare("#INC_START")==0)
        {
            //cout << line_num << " START @ " << inc_line << endl;
            inc_flag = true;
            pp_tmp << s_line << endl;
            //inc_line++;
            continue;
        }
        else if(s_line.compare("#INC_END")==0)
        {
            //cout << line_num << " END @ " << inc_line << endl;
            inc_flag = false;
            line_num++;
            end_of_line[end_of_line_count] = inc_line;
            end_of_line_count++;
            inc_line++;
            pp_tmp << s_line << endl;
            continue;
        }
        else if(s_line.compare("#SKIP")==0)
        {
            //cout << line_num << " END @ " << inc_line << endl;
            //inc_line++;
            pp_tmp << s_line << endl;
            continue;
        }
        a_status = rc_tokenizer(s_line);
        if(a_status == 0)
        {
            if(rc_tokens[0].compare("<include>") == 0)
            {
                //for(int i = 0; i < rc_tokens_count; i++)
                //    cout << rc_tokens[i] << endl;
                //cout << endl;


                resolved = 1;
                if(rc_tokens_count != 2)
                {
                    pp_file.close();
                    pp_tmp.close();
                    cout << "Expect 1 Argument and Found " << endl;
                    return 2;
                }
                if(rc_tokens[1].substr(0,8).compare("<string>")!=0)
                {
                    pp_file.close();
                    pp_tmp.close();
                    cout << "Expected String in Argument and Found " << endl;
                    return 2;
                }
                inc_file = rc_tokens[1].substr(8);
                pp_tmp2.open(inc_file.c_str(), fstream::in);
                if(!pp_tmp2.is_open())
                {
                    #ifdef RC_WINDOWS
                        inc_file = src_path + "\\" + inc_file;
                    #else
                        inc_file = src_path + "/" + inc_file;
                    #endif // RC_WINDOWS
                    pp_tmp2.open(inc_file.c_str(), fstream::in);
                }
                if(!pp_tmp2.is_open())
                {
                    cout << "Could not open FILE: " << inc_file << endl;
                    return 2;
                }

                //Note to Me: some bullshit to make include keep line numbers, old method doesn't work
                if(!inc_flag)
                    pp_tmp << "#INC_START" << endl;

                while(!pp_tmp2.eof())
                {
                    getline(pp_tmp2,s_line);
                    pp_tmp << s_line << endl;
                    inc_line++;
                }
                s_line = "";
                pp_tmp2.close();

                if(!inc_flag)
                    pp_tmp << "#INC_END" << endl;
                else
                    pp_tmp << "#SKIP" << endl;

                inc_p = true;
                rc_clearTokens();
                rc_tokens_count = 0;
            }
            if(!inc_flag)
                line_num++;
        }
        else
        {
            cout << "Syntax Error at line " << line_num << endl;
            return 2;
        }

        if(!inc_flag)
        {
            end_of_line[end_of_line_count] = inc_line;
            end_of_line_count++;
        }

        if(rc_tokens_count > 0)
        {
            pp_tmp << s_line << endl;
            inc_line++;
        }
        else if(inc_p)
        {
            inc_p = false;
        }
        else
        {
            pp_tmp << endl;
            inc_line++;
        }

        rc_clearTokens();
    }
    pp_file.close();
    pp_tmp.seekg(0);
    pp_file.open(pp_str.c_str(), ios::out | ios::trunc);
    while(!pp_tmp.eof())
    {
        getline(pp_tmp,s_line);
        pp_file << s_line << endl;
    }
    pp_file.close();
    pp_tmp.close();

    return resolved;
}

int rc_checkTokens()
{
    int p = 0;
    string t_id = "";
    string ut_id = "";
    string eid = "";
    string eid_type = "";
    for(int i = 0; i < rc_tokens_count; i++)
    {
        if(rc_tokens[i].length() >= 4)
        {
            if(rc_tokens[i].substr(0,4).compare("<id>")==0)
            {
                eid = rc_tokens[i].substr(4);
                for(int n = 0; n < eid.length(); n++)
                {
                    if(rc_isSpecialCharacter(eid.substr(n,1)))
                    {
                        return 2;
                    }
                }
                if(rc_tokens[i+1].compare("<par>")==0)
                {
                    eid = rc_searchID(eid);
                    eid_type = rc_getID_data(eid,"id_type");
                    if(eid_type.compare("")!=0)
                        if(eid_type.substr(1,1).compare("A")==0)
                        {
                            cout << "Invalid Syntax for Array\n";
                            return 2;
                        }
                }
                else if(rc_tokens[i+1].compare("<square>")==0)
                {
                    eid = rc_searchID(eid);
                    eid_type = rc_getID_data(eid,"id_type");
                    if(eid_type.compare("")!=0)
                        if(eid_type.substr(1,1).compare("F")==0 || eid_type.substr(1,1).compare("P")==0)
                        {
                            cout << "Invalid Syntax for Function/Sub Routine\n";
                            return 2;
                        }
                }
            }
        }
    }
    return 0;
}

int rc_analyze(string s_line)
{
    if(s_line.compare("")==0)
        return 0;
    m_index = 0;
    s_index = 0;
    int a_status = rc_tokenizer(s_line);
    int tcount = rc_tokens_count;
    if(a_status==0)
    {
        a_status = rc_checkTokens();
        if(a_status != 0)
        {
            cout << "Illegal Identifier Name\n";
            return 2;
        }

        a_status = rc_evalBlock();
        if(a_status==0)
        {
            //cout << "=current scope = " << current_scope << endl;
            a_status = rc_evalID();
            //cout << "?rc_evalID current scope = " << current_scope << endl;
            //cout << "a_status evalID = " << a_status << endl << endl;
            if(a_status==0)
            {
                //cout << "direct math\n";
                if(rc_preParse(1)!=2)
                {
                    if(rc_mathParser(m_index,s_index)!=2)
                    {
                        rc_tokens_count = tcount;
                        return 0;
                    }
                    else
                        return 2;
                }
                else
                    return 2;
            }
            else if(a_status == 2)
            {
                //cout << "ABC\n";
                return 2;
            }
            if(m_index > max_m)
                max_m = m_index;
            if(s_index > max_s)
                max_s = s_index;
        }
        else if(a_status == 2)
        {
            cout << "could not evaluate ID\n";
            return 3;
        }

    }
    else if(a_status==2)
    {
        cout << "tokenizer malfunctioned\n";
        return 4;
    }
    rc_tokens_count = tcount;
    return 0;
}

int rc_initFunctions()
{
    rc_id[0] = "PROGRAM.FPRINT #F 0 0 1 1 0 "; //id   type   offset   scope   dimensions   index/total_ids   overload
    rc_id[1] = "PROGRAM.FPRINT.S$ $ 1 1 0 0 0 "; sid_count++;
    rc_id[2] = "PROGRAM.NUMCOMMANDS #F 2 0 0 0 0 ";
    rc_id[3] = "PROGRAM.ARRAYDIM #F 3 0 1 2 0 ";
    rc_id[4] = "PROGRAM.ARRAYDIM.ARR_ID #& 4 1 0 0 ~ "; nid_count++;
    rc_id[5] = "PROGRAM.ARRAYDIM.ARR_ID$ $& 5 1 0 1 1 "; sid_count++;
    rc_id[6] = "PROGRAM.ARRAYSIZE #F 6 0 2 3 0 ";
    rc_id[7] = "PROGRAM.ARRAYSIZE.ARR_ID #& 7 1 0 1 ~ "; nid_count++;
    rc_id[8] = "PROGRAM.ARRAYSIZE.ARR_ID$ $& 8 1 0 2 1 "; sid_count++;
    rc_id[9] = "PROGRAM.ARRAYSIZE.ARR_DIM # 9 1 0 2 0 "; nid_count++;
    rc_id[10] = "PROGRAM.ASC #F 10 0 1 1 0 ";
    rc_id[11] = "PROGRAM.ASC.C$ $ 11 1 0 3 0 "; sid_count++;
    rc_id[12] = "PROGRAM.CHR$ $F 12 0 1 1 0 ";
    rc_id[13] = "PROGRAM.CHR$.N # 13 1 0 3 0 "; nid_count++; //this is nid 3
    rc_id[14] = "PROGRAM.INSERT$ $F 14 0 3 3 0 ";
    rc_id[15] = "PROGRAM.INSERT$.SRC$ $ 15 1 0 4 0 "; sid_count++; //this is str 4
    rc_id[16] = "PROGRAM.INSERT$.TGT$ $ 16 1 0 5 0 "; sid_count++; //str 5
    rc_id[17] = "PROGRAM.INSERT$.POS # 17 1 0 4 0 "; nid_count++; //nid 4
    rc_id[18] = "PROGRAM.INSTR #F 18 0 2 2 0 ";
    rc_id[19] = "PROGRAM.INSTR.STRING$ $ 19 1 0 6 0 "; sid_count++; //str 6
    rc_id[20] = "PROGRAM.INSTR.SUBSTR$ $ 20 1 0 7 0 "; sid_count++; //str 7
    rc_id[21] = "PROGRAM.LCASE$ $F 21 0 1 1 0 ";
    rc_id[22] = "PROGRAM.LCASE$.STRING$ $ 22 1 0 8 0 "; sid_count++; //str 8
    rc_id[23] = "PROGRAM.LEFT$ $F 23 0 2 2 0 ";
    rc_id[24] = "PROGRAM.LEFT$.STRING$ $ 24 1 0 9 0 "; sid_count++; //str 9
    rc_id[25] = "PROGRAM.LEFT$.N # 25 1 0 5 0 "; nid_count++; //nid 5
    rc_id[26] = "PROGRAM.LENGTH #F 26 0 1 1 0 ";
    rc_id[27] = "PROGRAM.LENGTH.STRING$ $ 27 1 0 10 0 "; sid_count++; //str 10
    rc_id[28] = "PROGRAM.LTRIM$ $F 28 0 1 1 0 ";
    rc_id[29] = "PROGRAM.LTRIM$.STRING$ $ 29 1 0 11 0 "; sid_count++; //str 11
    rc_id[30] = "PROGRAM.MID$ $F 30 0 3 3 0 ";
    rc_id[31] = "PROGRAM.MID$.STRING$ $ 31 1 0 12 0 "; sid_count++; //str 12
    rc_id[32] = "PROGRAM.MID$.START # 32 1 0 6 0 "; nid_count++; //nid 6
    rc_id[33] = "PROGRAM.MID$.N # 33 1 0 7 0 "; nid_count++; //nid 7
    rc_id[34] = "PROGRAM.REPLACE$ $F 34 0 3 3 0 ";
    rc_id[35] = "PROGRAM.REPLACE$.SRC$ $ 35 1 0 13 0 "; sid_count++; //str 13
    rc_id[36] = "PROGRAM.REPLACE$.RPC$ $ 36 1 0 14 0 "; sid_count++; //str 14
    rc_id[37] = "PROGRAM.REPLACE$.POS # 37 1 0 8 0 "; nid_count++; //nid 8
    rc_id[38] = "PROGRAM.REPLACESUBSTR$ $F 38 0 3 3 0 ";
    rc_id[39] = "PROGRAM.REPLACESUBSTR$.SRC$ $ 39 1 0 15 0 "; sid_count++; //str 15
    rc_id[40] = "PROGRAM.REPLACESUBSTR$.TGT$ $ 40 1 0 16 0 "; sid_count++; //str 16
    rc_id[41] = "PROGRAM.REPLACESUBSTR$.RPC$ $ 41 1 0 17 0 "; sid_count++; //str 17
    rc_id[42] = "PROGRAM.REVERSE$ $F 42 0 1 1 0 ";
    rc_id[43] = "PROGRAM.REVERSE$.STRING$ $ 43 1 0 18 0 "; sid_count++; //str 18
    rc_id[44] = "PROGRAM.RIGHT$ $F 44 0 2 2 0 ";
    rc_id[45] = "PROGRAM.RIGHT$.STRING$ $ 45 1 0 19 0 "; sid_count++; //str 19
    rc_id[46] = "PROGRAM.RIGHT$.N # 46 1 0 9 0 "; nid_count++; //nid 9
    rc_id[47] = "PROGRAM.RTRIM$ $F 47 0 1 1 0 ";
    rc_id[48] = "PROGRAM.RTRIM$.STRING$ $ 48 1 0 20 0 "; sid_count++; //str 20
    rc_id[49] = "PROGRAM.FILLSTRING$ $F 49 0 2 2 0 ";
    rc_id[50] = "PROGRAM.FILLSTRING$.STRING$ $ 50 1 0 21 0 "; sid_count++; //str 21
    rc_id[51] = "PROGRAM.FILLSTRING$.N # 51 1 0 10 0 "; nid_count++; //nid 10
    rc_id[52] = "PROGRAM.STR$ $F 52 0 1 1 0 ";
    rc_id[53] = "PROGRAM.STR$.VALUE # 53 1 0 11 0 "; nid_count++; //nid 11
    rc_id[54] = "PROGRAM.TALLY #F 54 0 2 2 0 ";
    rc_id[55] = "PROGRAM.TALLY.STRING$ $ 55 1 0 22 0 "; sid_count++; //str 22
    rc_id[56] = "PROGRAM.TALLY.SUBSTR$ $ 56 1 0 23 0 "; sid_count++; //str 23
    rc_id[57] = "PROGRAM.TRIM$ $F 57 0 1 1 0 ";
    rc_id[58] = "PROGRAM.TRIM$.STRING$ $ 58 1 0 24 0 "; sid_count++; //str 24
    rc_id[59] = "PROGRAM.UCASE$ $F 59 0 1 1 0 ";
    rc_id[60] = "PROGRAM.UCASE$.STRING$ $ 60 1 0 25 0 "; sid_count++; //str 25
    rc_id[61] = "PROGRAM.VAL #F 61 0 1 1 0 ";
    rc_id[62] = "PROGRAM.VAL.STRING$ $ 62 1 0 26 0 "; sid_count++; //str 26
    rc_id[63] = "PROGRAM.ABS #F 63 0 1 1 0 ";
    rc_id[64] = "PROGRAM.ABS.N # 64 1 0 12 0 "; nid_count++; //nid 12
    rc_id[65] = "PROGRAM.ACOS #F 65 0 1 1 0 ";
    rc_id[66] = "PROGRAM.ACOS.N # 66 1 0 13 0 "; nid_count++; //nid 13
    rc_id[67] = "PROGRAM.ANDBIT #F 67 0 2 2 0 ";
    rc_id[68] = "PROGRAM.ANDBIT.A # 68 1 0 14 0 "; nid_count++; //nid 14
    rc_id[69] = "PROGRAM.ANDBIT.B # 69 1 0 15 0 "; nid_count++; //nid 15
    rc_id[70] = "PROGRAM.ASIN #F 70 0 1 1 0 ";
    rc_id[71] = "PROGRAM.ASIN.N # 71 1 0 16 0 "; nid_count++; //nid 16
    rc_id[72] = "PROGRAM.ATAN #F 72 0 1 1 0 ";
    rc_id[73] = "PROGRAM.ATAN.N # 73 1 0 17 0 "; nid_count++; //nid 17
    rc_id[74] = "PROGRAM.BIN$ $F 74 0 1 1 0 ";
    rc_id[75] = "PROGRAM.BIN.N # 75 1 0 18 0 "; nid_count++; //nid 18
    rc_id[76] = "PROGRAM.COS #F 76 0 1 1 0 ";
    rc_id[77] = "PROGRAM.COS.ANGLE # 77 1 0 19 0 "; nid_count++; //nid 19
    rc_id[78] = "PROGRAM.EXP #F 78 0 1 1 0 ";
    rc_id[79] = "PROGRAM.EXP.N # 79 1 0 20 0 "; nid_count++; //nid 20
    rc_id[80] = "PROGRAM.FRAC #F 80 0 1 1 0 ";
    rc_id[81] = "PROGRAM.FRAC.N # 81 1 0 21 0 "; nid_count++; //nid 21
    rc_id[82] = "PROGRAM.HEX$ $F 82 0 1 1 0 ";
    rc_id[83] = "PROGRAM.HEX$.VALUE # 83 1 0 22 0 "; nid_count++; //nid 22
    rc_id[84] = "PROGRAM.INT #F 84 0 1 1 0 ";
    rc_id[85] = "PROGRAM.INT.N # 85 1 0 23 0 "; nid_count++; //nid 23
    rc_id[86] = "PROGRAM.LOG #F 86 0 1 1 0 ";
    rc_id[87] = "PROGRAM.LOG.N # 87 1 0 24 0 "; nid_count++; //nid 24
    rc_id[88] = "PROGRAM.MAX #F 88 0 2 2 0 ";
    rc_id[89] = "PROGRAM.MAX.A # 89 1 0 25 0 "; nid_count++; //nid 25
    rc_id[90] = "PROGRAM.MAX.B # 90 1 0 26 0 "; nid_count++; //nid 26
    rc_id[91] = "PROGRAM.MIN #F 91 0 2 2 0 ";
    rc_id[92] = "PROGRAM.MIN.A # 92 1 0 27 0 "; nid_count++; //nid 27
    rc_id[93] = "PROGRAM.MIN.B # 93 1 0 28 0 "; nid_count++; //nid 28
    rc_id[94] = "PROGRAM.ORBIT #F 94 0 2 2 0 ";
    rc_id[95] = "PROGRAM.ORBIT.A # 95 1 0 29 0 "; nid_count++; //nid 29
    rc_id[96] = "PROGRAM.ORBIT.B # 96 1 0 30 0 "; nid_count++; //nid 30
    rc_id[97] = "PROGRAM.RANDOMIZE #F 97 0 1 1 0 ";
    rc_id[98] = "PROGRAM.RANDOMIZE.N # 98 1 0 31 0 "; nid_count++; //nid 31
    rc_id[99] = "PROGRAM.RAND #F 99 0 1 1 0 ";
    rc_id[100] = "PROGRAM.RAND.N # 100 1 0 32 0 "; nid_count++; //nid 32
    rc_id[101] = "PROGRAM.ROUND #F 101 0 1 1 0 ";
    rc_id[102] = "PROGRAM.ROUND.N # 102 1 0 33 0 "; nid_count++; //nid 33
    rc_id[103] = "PROGRAM.SIGN #F 103 0 1 1 0 ";
    rc_id[104] = "PROGRAM.SIGN.N # 104 1 0 34 0 "; nid_count++; //nid 34
    rc_id[105] = "PROGRAM.SIN #F 105 0 1 1 0 ";
    rc_id[106] = "PROGRAM.SIN.ANGLE # 106 1 0 35 0 "; nid_count++; //nid 35
    rc_id[107] = "PROGRAM.SQRT #F 107 0 1 1 0 ";
    rc_id[108] = "PROGRAM.SQRT.N # 108 1 0 36 0 "; nid_count++; //nid 36
    rc_id[109] = "PROGRAM.TAN #F 109 0 1 1 0 ";
    rc_id[110] = "PROGRAM.TAN.ANGLE # 110 1 0 37 0 "; nid_count++; //nid 37
    rc_id[111] = "PROGRAM.XORBIT #F 111 0 2 2 0 ";
    rc_id[112] = "PROGRAM.XORBIT.A # 112 1 0 38 0 "; nid_count++; //nid 38
    rc_id[113] = "PROGRAM.XORBIT.B # 113 1 0 39 0 "; nid_count++; //nid 39
    rc_id[114] = "PROGRAM.FILEOPEN #F 114 0 3 3 0 ";
    rc_id[115] = "PROGRAM.FILEOPEN.STREAM # 115 1 0 40 0 "; nid_count++; //nid 40
    rc_id[116] = "PROGRAM.FILEOPEN.FILE$ $ 116 1 0 27 0 "; sid_count++; //str 27
    rc_id[117] = "PROGRAM.FILEOPEN.IO_MODE # 117 1 0 41 0 "; nid_count++; //nid 41
    rc_id[118] = "PROGRAM.FILECLOSE #F 118 0 1 1 0 ";
    rc_id[119] = "PROGRAM.FILECLOSE.STREAM # 119 1 0 42 0 "; nid_count++; //nid 42
    rc_id[120] = "PROGRAM.READBYTE #F 120 0 1 1 0 ";
    rc_id[121] = "PROGRAM.READBYTE.STREAM # 121 1 0 43 0 "; nid_count++; //nid 43
    rc_id[122] = "PROGRAM.WRITEBYTE #F 122 0 2 2 0 ";
    rc_id[123] = "PROGRAM.WRITEBYTE.STREAM # 123 1 0 44 0 "; nid_count++; //nid 44
    rc_id[124] = "PROGRAM.WRITEBYTE.BYTE # 124 1 0 45 0 "; nid_count++; //nid 45
    rc_id[125] = "PROGRAM.READLINE$ $F 125 0 1 1 0 ";
    rc_id[126] = "PROGRAM.READLINE$.STREAM # 126 1 0 46 0 "; nid_count++; //nid 46
    rc_id[127] = "PROGRAM.WRITELINE #F 127 0 2 2 0 ";
    rc_id[128] = "PROGRAM.WRITELINE.STREAM # 128 1 0 47 0 "; nid_count++; //nid 47
    rc_id[129] = "PROGRAM.WRITELINE.LINE$ $ 129 1 0 28 0 "; sid_count++; //str 28
    rc_id[130] = "PROGRAM.COPYFILE #F 130 0 2 2 0 ";
    rc_id[131] = "PROGRAM.COPYFILE.SRC$ $ 131 1 0 29 0 "; sid_count++; //str 29
    rc_id[132] = "PROGRAM.COPYFILE.DEST$ $ 132 1 0 30 0 "; sid_count++; //str 30
    rc_id[133] = "PROGRAM.REMOVEFILE #F 133 0 1 1 0 ";
    rc_id[134] = "PROGRAM.REMOVEFILE.FILE$ $ 134 1 0 31 0 "; sid_count++; //str 31
    rc_id[135] = "PROGRAM.FILEEXISTS #F 135 0 1 1 0 ";
    rc_id[136] = "PROGRAM.FILEEXISTS.FILE$ $ 136 1 0 32 0 "; sid_count++; //str 32
    rc_id[137] = "PROGRAM.MOVEFILE #F 137 0 2 2 0 ";
    rc_id[138] = "PROGRAM.MOVEFILE.SRC$ $ 138 1 0 33 0 "; sid_count++; //str 33
    rc_id[139] = "PROGRAM.MOVEFILE.DEST$ $ 139 1 0 34 0 "; sid_count++; //str 34
    rc_id[140] = "PROGRAM.RENAMEFILE #F 140 0 2 2 0 ";
    rc_id[141] = "PROGRAM.RENAMEFILE.FILE$ $ 141 1 0 35 0 "; sid_count++; //str 35
    rc_id[142] = "PROGRAM.RENAMEFILE.RENAME$ $ 142 1 0 36 0 "; sid_count++; //str 36
    rc_id[143] = "PROGRAM.FILELENGTH #F 143 0 1 1 0 ";
    rc_id[144] = "PROGRAM.FILELENGTH.FILE$ $ 144 1 0 37 0 "; sid_count++; //str 37
    rc_id[145] = "PROGRAM.TELL #F 145 0 1 1 0 ";
    rc_id[146] = "PROGRAM.TELL.STREAM # 146 1 0 48 0 "; nid_count++; //nid 48
    rc_id[147] = "PROGRAM.SEEK #F 147 0 2 2 0 ";
    rc_id[148] = "PROGRAM.SEEK.STREAM # 148 1 0 49 0 "; nid_count++; //nid 49
    rc_id[149] = "PROGRAM.SEEK.POSITION # 149 1 0 50 0 "; nid_count++; //nid 50
    rc_id[150] = "PROGRAM.EOF #F 150 0 1 1 0 ";
    rc_id[151] = "PROGRAM.EOF.STREAM # 151 1 0 51 0 "; nid_count++; //nid 51
    rc_id[152] = "PROGRAM.FREEFILE #F 152 0 0 0 0 ";
    rc_id[153] = "PROGRAM.CHANGEDIR #F 153 0 1 1 0 ";
    rc_id[154] = "PROGRAM.CHANGEDIR.PATH$ $ 154 1 0 38 0 "; sid_count++; //str 38
    rc_id[155] = "PROGRAM.DIREXISTS #F 155 0 1 1 0 ";
    rc_id[156] = "PROGRAM.DIREXISTS.PATH$ $ 156 1 0 39 0 "; sid_count++; //str 39
    rc_id[157] = "PROGRAM.DIRFIRST$ $F 157 0 0 0 0 ";
    rc_id[158] = "PROGRAM.DIR$ $F 158 0 0 0 0 ";
    rc_id[159] = "PROGRAM.DIRNEXT$ $F 159 0 0 0 0 ";
    rc_id[160] = "PROGRAM.MAKEDIR #F 160 0 1 1 0 ";
    rc_id[161] = "PROGRAM.MAKEDIR.PATH$ $ 161 1 0 40 0 "; sid_count++; //str 40
    rc_id[162] = "PROGRAM.REMOVEDIR #F 162 0 1 1 0 ";
    rc_id[163] = "PROGRAM.REMOVEDIR.PATH$ $ 163 1 0 41 0 "; sid_count++; //str 41
    rc_id[164] = "PROGRAM.DATE$ $F 164 0 0 0 0 ";
    rc_id[165] = "PROGRAM.TICKS #F 165 0 0 0 0 ";
    rc_id[166] = "PROGRAM.TIME$ $F 166 0 0 0 0 ";
    rc_id[167] = "PROGRAM.TIMER #F 167 0 0 0 0 ";
    rc_id[168] = "PROGRAM.WAIT #F 168 0 1 1 0 ";
    rc_id[169] = "PROGRAM.WAIT.MSEC # 169 1 0 52 0 "; nid_count++; //nid 52
    rc_id[170] = "PROGRAM.INPUT$ $F 170 0 1 1 0 ";
    rc_id[171] = "PROGRAM.INPUT$.PROMPT $ 171 1 0 42 0 "; sid_count++; //str 42
    rc_id[172] = "PROGRAM.WINDOWOPEN #P 0 1 7 0 0 ";
    rc_id[173] = "PROGRAM.WINDOWOPEN.WIN_NUM # 0 1 0 53 ";
    rc_id[174] = "PROGRAM.WINDOWOPEN.TITLE$ $ 0 1 0 43 ";
    rc_id[175] = "PROGRAM.WINDOWOPEN.X # 0 1 0 54 ";
    rc_id[176] = "PROGRAM.WINDOWOPEN.Y # 0 1 0 55 ";
    rc_id[177] = "PROGRAM.WINDOWOPEN.W # 0 1 0 56 ";
    rc_id[178] = "PROGRAM.WINDOWOPEN.H # 0 1 0 57 ";
    rc_id[179] = "PROGRAM.WINDOWOPEN.MODE # 0 1 0 58 ";
    rc_id[180] = "PROGRAM.WINDOWCLOSE #P 0 1 1 0 0 ";
    rc_id[181] = "PROGRAM.WINDOWCLOSE.WIN_NUM # 0 1 0 59 ";
    rc_id[182] = "PROGRAM.RAISEWINDOW #P 0 1 0 0 0 ";
    rc_id[183] = "PROGRAM.WINDOW #P 0 1 1 0 0 ";
    rc_id[184] = "PROGRAM.WINDOW.WIN_NUM # 0 1 0 60 ";
    rc_id[185] = "PROGRAM.UPDATE #P 0 1 0 0 0 ";
    rc_id[186] = "PROGRAM.SHOWWINDOW #P 0 1 0 0 0 ";
    rc_id[187] = "PROGRAM.HIDEWINDOW #P 0 1 0 0 0 ";
    rc_id[188] = "PROGRAM.SETWINDOWTITLE #P 0 1 1 0 0 ";
    rc_id[189] = "PROGRAM.SETWINDOWTITLE.TITLE$ $ 0 1 0 44 ";
    rc_id[190] = "PROGRAM.WINDOWTITLE$ $F 0 1 0 0 0 ";
    rc_id[191] = "PROGRAM.SETWINDOWPOSITION #P 0 1 2 0 0 ";
    rc_id[192] = "PROGRAM.SETWINDOWPOSITION.X # 0 1 0 61 ";
    rc_id[193] = "PROGRAM.SETWINDOWPOSITION.Y # 0 1 0 62 ";
    rc_id[194] = "PROGRAM.GETWINDOWPOSITION #P 0 1 2 0 0 ";
    rc_id[195] = "PROGRAM.GETWINDOWPOSITION.X #& 0 1 0 63 ";
    rc_id[196] = "PROGRAM.GETWINDOWPOSITION.Y #& 0 1 0 64 ";
    rc_id[197] = "PROGRAM.SETWINDOWSIZE #P 0 1 2 0 0 ";
    rc_id[198] = "PROGRAM.SETWINDOWSIZE.W # 0 1 0 65 ";
    rc_id[199] = "PROGRAM.SETWINDOWSIZE.H # 0 1 0 66 ";
    rc_id[200] = "PROGRAM.GETWINDOWSIZE #P 0 1 2 0 0 ";
    rc_id[201] = "PROGRAM.GETWINDOWSIZE.W #& 0 1 0 67 ";
    rc_id[202] = "PROGRAM.GETWINDOWSIZE.H #& 0 1 0 68 ";
    rc_id[203] = "PROGRAM.SETWINDOWMINSIZE #P 0 1 2 0 0 ";
    rc_id[204] = "PROGRAM.SETWINDOWMINSIZE.W # 0 1 0 69 ";
    rc_id[205] = "PROGRAM.SETWINDOWMINSIZE.H # 0 1 0 70 ";
    rc_id[206] = "PROGRAM.SETWINDOWMAXSIZE #P 0 1 2 0 0 ";
    rc_id[207] = "PROGRAM.SETWINDOWMAXSIZE.W # 0 1 0 71 ";
    rc_id[208] = "PROGRAM.SETWINDOWMAXSIZE.H # 0 1 0 72 ";
    rc_id[209] = "PROGRAM.GETWINDOWMINSIZE #P 0 1 2 0 0 ";
    rc_id[210] = "PROGRAM.GETWINDOWMINSIZE.W #& 0 1 0 73 ";
    rc_id[211] = "PROGRAM.GETWINDOWMINSIZE.H #& 0 1 0 74 ";
    rc_id[212] = "PROGRAM.GETWINDOWMAXSIZE #P 0 1 2 0 0 ";
    rc_id[213] = "PROGRAM.GETWINDOWMAXSIZE.W #& 0 1 0 75 ";
    rc_id[214] = "PROGRAM.GETWINDOWMAXSIZE.H #& 0 1 0 76 ";
    rc_id[215] = "PROGRAM.WINDOWISFULLSCREEN #F 0 1 0 0 0 ";
    rc_id[216] = "PROGRAM.WINDOWISVISIBLE #F 0 1 0 0 0 ";
    rc_id[217] = "PROGRAM.WINDOWISBORDERED #F 0 1 0 0 0 ";
    rc_id[218] = "PROGRAM.WINDOWISRESIZABLE #F 0 1 0 0 0 ";
    rc_id[219] = "PROGRAM.WINDOWISMINIMIZED #F 0 1 0 0 0 ";
    rc_id[220] = "PROGRAM.WINDOWISMAXIMIZED #F 0 1 0 0 0 ";
    rc_id[221] = "PROGRAM.SETWINDOWFULLSCREEN #P 0 1 1 0 0 ";
    rc_id[222] = "PROGRAM.SETWINDOWFULLSCREEN.FLAG # 0 1 0 77 ";
    rc_id[223] = "PROGRAM.MAXIMIZEWINDOW #P 0 1 0 0 0 ";
    rc_id[224] = "PROGRAM.MINIMIZEWINDOW #P 0 1 0 0 0 ";
    rc_id[225] = "PROGRAM.SETWINDOWBORDER #P 0 1 1 0 0 ";
    rc_id[226] = "PROGRAM.SETWINDOWBORDER.FLAG # 0 1 0 78 ";
    rc_id[227] = "PROGRAM.CANVASOPEN #P 0 1 8 0 0 ";
    rc_id[228] = "PROGRAM.CANVASOPEN.S_NUM # 0 1 0 79 ";
    rc_id[229] = "PROGRAM.CANVASOPEN.W # 0 1 0 80 ";
    rc_id[230] = "PROGRAM.CANVASOPEN.H # 0 1 0 81 ";
    rc_id[231] = "PROGRAM.CANVASOPEN.VPX # 0 1 0 82 ";
    rc_id[232] = "PROGRAM.CANVASOPEN.VPY # 0 1 0 83 ";
    rc_id[233] = "PROGRAM.CANVASOPEN.VPW # 0 1 0 84 ";
    rc_id[234] = "PROGRAM.CANVASOPEN.VPH # 0 1 0 85 ";
    rc_id[235] = "PROGRAM.CANVASOPEN.FLAG # 0 1 0 86 ";
    rc_id[236] = "PROGRAM.CANVASCLOSE #P 0 1 1 0 0 ";
    rc_id[237] = "PROGRAM.CANVASCLOSE.S_NUM # 0 1 0 87 ";
    rc_id[238] = "PROGRAM.SETCANVASVISIBLE #P 0 1 2 0 0 ";
    rc_id[239] = "PROGRAM.SETCANVASVISIBLE.S_NUM # 0 1 0 88 ";
    rc_id[240] = "PROGRAM.SETCANVASVISIBLE.FLAG # 0 1 0 89 ";
    rc_id[241] = "PROGRAM.CANVASISVISIBLE #F 0 1 1 0 0 ";
    rc_id[242] = "PROGRAM.CANVASISVISIBLE.S_NUM # 0 1 0 90 ";
    rc_id[243] = "PROGRAM.SETCANVASVIEWPORT #P 0 1 4 0 0 ";
    rc_id[244] = "PROGRAM.SETCANVASVIEWPORT.X # 0 1 0 91 ";
    rc_id[245] = "PROGRAM.SETCANVASVIEWPORT.Y # 0 1 0 92 ";
    rc_id[246] = "PROGRAM.SETCANVASVIEWPORT.W # 0 1 0 93 ";
    rc_id[247] = "PROGRAM.SETCANVASVIEWPORT.H # 0 1 0 94 ";
    rc_id[248] = "PROGRAM.GETCANVASVIEWPORT #P 0 1 4 0 0 ";
    rc_id[249] = "PROGRAM.GETCANVASVIEWPORT.X #& 0 1 0 95 ";
    rc_id[250] = "PROGRAM.GETCANVASVIEWPORT.Y #& 0 1 0 96 ";
    rc_id[251] = "PROGRAM.GETCANVASVIEWPORT.W #& 0 1 0 97 ";
    rc_id[252] = "PROGRAM.GETCANVASVIEWPORT.H #& 0 1 0 98 ";
    rc_id[253] = "PROGRAM.CANVAS #P 0 1 1 0 0 ";
    rc_id[254] = "PROGRAM.CANVAS.S_NUM # 0 1 0 99 ";
    rc_id[255] = "PROGRAM.SETCANVASOFFSET #P 0 1 2 0 0 ";
    rc_id[256] = "PROGRAM.SETCANVASOFFSET.X # 0 1 0 100 ";
    rc_id[257] = "PROGRAM.SETCANVASOFFSET.Y # 0 1 0 101 ";
    rc_id[258] = "PROGRAM.GETCANVASOFFSET #P 0 1 2 0 0 ";
    rc_id[259] = "PROGRAM.GETCANVASOFFSET.X #& 0 1 0 102 ";
    rc_id[260] = "PROGRAM.GETCANVASOFFSET.Y #& 0 1 0 103 ";
    rc_id[261] = "PROGRAM.GETCANVASSIZE #P 0 1 2 0 0 ";
    rc_id[262] = "PROGRAM.GETCANVASSIZE.W #& 0 1 0 104 ";
    rc_id[263] = "PROGRAM.GETCANVASSIZE.H #& 0 1 0 105 ";
    rc_id[264] = "PROGRAM.CLEARCANVAS #P 0 1 0 0 0 ";
    rc_id[265] = "PROGRAM.SETCANVASALPHA #P 0 1 1 0 0 ";
    rc_id[266] = "PROGRAM.SETCANVASALPHA.ALPHA # 0 1 0 106 ";
    rc_id[267] = "PROGRAM.COPYCANVAS #P 0 1 8 0 0 ";
    rc_id[268] = "PROGRAM.COPYCANVAS.SRC_SCREEN # 0 1 0 107 ";
    rc_id[269] = "PROGRAM.COPYCANVAS.X # 0 1 0 108 ";
    rc_id[270] = "PROGRAM.COPYCANVAS.Y # 0 1 0 109 ";
    rc_id[271] = "PROGRAM.COPYCANVAS.W # 0 1 0 110 ";
    rc_id[272] = "PROGRAM.COPYCANVAS.H # 0 1 0 111 ";
    rc_id[273] = "PROGRAM.COPYCANVAS.DST_SCREEN # 0 1 0 112 ";
    rc_id[274] = "PROGRAM.COPYCANVAS.DST_X # 0 1 0 113 ";
    rc_id[275] = "PROGRAM.COPYCANVAS.DST_Y # 0 1 0 114 ";
    rc_id[276] = "PROGRAM.CLONECANVAS #P 0 1 2 0 0 ";
    rc_id[277] = "PROGRAM.CLONECANVAS.SRC_SCREEN # 0 1 0 115 ";
    rc_id[278] = "PROGRAM.CLONECANVAS.DST_SCREEN # 0 1 0 116 ";
    rc_id[279] = "PROGRAM.SETCANVASZ #P 0 1 1 0 0 ";
    rc_id[280] = "PROGRAM.SETCANVASZ.Z # 0 1 0 117 ";
    rc_id[281] = "PROGRAM.BOX #P 0 1 4 0 0 ";
    rc_id[282] = "PROGRAM.BOX.X1 # 0 1 0 118 ";
    rc_id[283] = "PROGRAM.BOX.Y1 # 0 1 0 119 ";
    rc_id[284] = "PROGRAM.BOX.X2 # 0 1 0 120 ";
    rc_id[285] = "PROGRAM.BOX.Y2 # 0 1 0 121 ";
    rc_id[286] = "PROGRAM.BOXFILL #P 0 1 4 0 0 ";
    rc_id[287] = "PROGRAM.BOXFILL.X1 # 0 1 0 122 ";
    rc_id[288] = "PROGRAM.BOXFILL.Y1 # 0 1 0 123 ";
    rc_id[289] = "PROGRAM.BOXFILL.X2 # 0 1 0 124 ";
    rc_id[290] = "PROGRAM.BOXFILL.Y2 # 0 1 0 125 ";
    rc_id[291] = "PROGRAM.CIRCLE #P 0 1 3 0 0 ";
    rc_id[292] = "PROGRAM.CIRCLE.X # 0 1 0 126 ";
    rc_id[293] = "PROGRAM.CIRCLE.Y # 0 1 0 127 ";
    rc_id[294] = "PROGRAM.CIRCLE.RADIUS # 0 1 0 128 ";
    rc_id[295] = "PROGRAM.CIRCLEFILL #P 0 1 3 0 0 ";
    rc_id[296] = "PROGRAM.CIRCLEFILL.X # 0 1 0 129 ";
    rc_id[297] = "PROGRAM.CIRCLEFILL.Y # 0 1 0 130 ";
    rc_id[298] = "PROGRAM.CIRCLEFILL.RADIUS # 0 1 0 131 ";
    rc_id[299] = "PROGRAM.ELLIPSE #P 0 1 4 0 0 ";
    rc_id[300] = "PROGRAM.ELLIPSE.X # 0 1 0 132 ";
    rc_id[301] = "PROGRAM.ELLIPSE.Y # 0 1 0 133 ";
    rc_id[302] = "PROGRAM.ELLIPSE.RADIUS_X # 0 1 0 134 ";
    rc_id[303] = "PROGRAM.ELLIPSE.RADIUS_Y # 0 1 0 135 ";
    rc_id[304] = "PROGRAM.ELLIPSEFILL #P 0 1 4 0 0 ";
    rc_id[305] = "PROGRAM.ELLIPSEFILL.X # 0 1 0 136 ";
    rc_id[306] = "PROGRAM.ELLIPSEFILL.Y # 0 1 0 137 ";
    rc_id[307] = "PROGRAM.ELLIPSEFILL.RADIUS_X # 0 1 0 138 ";
    rc_id[308] = "PROGRAM.ELLIPSEFILL.RADIUS_Y # 0 1 0 139 ";
    rc_id[309] = "PROGRAM.PIXEL #F 0 1 2 0 0 ";
    rc_id[310] = "PROGRAM.PIXEL.X # 0 1 0 140 ";
    rc_id[311] = "PROGRAM.PIXEL.Y # 0 1 0 141 ";
    rc_id[312] = "PROGRAM.SETCOLOR #P 0 1 1 0 0 ";
    rc_id[313] = "PROGRAM.SETCOLOR.COLOR # 0 1 0 142 ";
    rc_id[314] = "PROGRAM.LINE #P 0 1 4 0 0 ";
    rc_id[315] = "PROGRAM.LINE.X1 # 0 1 0 143 ";
    rc_id[316] = "PROGRAM.LINE.Y1 # 0 1 0 144 ";
    rc_id[317] = "PROGRAM.LINE.X2 # 0 1 0 145 ";
    rc_id[318] = "PROGRAM.LINE.Y2 # 0 1 0 146 ";
    rc_id[319] = "PROGRAM.FLOODFILL #P 0 1 2 0 0 ";
    rc_id[320] = "PROGRAM.FLOODFILL.X # 0 1 0 147 ";
    rc_id[321] = "PROGRAM.FLOODFILL.Y # 0 1 0 148 ";
    rc_id[322] = "PROGRAM.RGB #F 0 1 3 0 0 ";
    rc_id[323] = "PROGRAM.RGB.R # 0 1 0 149 ";
    rc_id[324] = "PROGRAM.RGB.G # 0 1 0 150 ";
    rc_id[325] = "PROGRAM.RGB.B # 0 1 0 151 ";
    rc_id[326] = "PROGRAM.RGBA #F 0 1 4 0 0 ";
    rc_id[327] = "PROGRAM.RGBA.R # 0 1 0 152 ";
    rc_id[328] = "PROGRAM.RGBA.G # 0 1 0 153 ";
    rc_id[329] = "PROGRAM.RGBA.B # 0 1 0 154 ";
    rc_id[330] = "PROGRAM.RGBA.A # 0 1 0 155 ";
    rc_id[331] = "PROGRAM.PSET #P 0 1 2 0 0 ";
    rc_id[332] = "PROGRAM.PSET.X # 0 1 0 156 ";
    rc_id[333] = "PROGRAM.PSET.Y # 0 1 0 157 ";
    rc_id[334] = "PROGRAM.CREATEIMAGE #P 0 1 4 0 0 ";
    rc_id[335] = "PROGRAM.CREATEIMAGE.SLOT # 0 1 0 158 ";
    rc_id[336] = "PROGRAM.CREATEIMAGE.W # 0 1 0 159 ";
    rc_id[337] = "PROGRAM.CREATEIMAGE.H # 0 1 0 160 ";
    rc_id[338] = "PROGRAM.CREATEIMAGE.PDATA #& 0 1 0 161 ";
    rc_id[339] = "PROGRAM.CREATEIMAGE_EX #P 0 1 5 0 0 ";
    rc_id[340] = "PROGRAM.CREATEIMAGE_EX.SLOT # 0 1 0 162 ";
    rc_id[341] = "PROGRAM.CREATEIMAGE_EX.W # 0 1 0 163 ";
    rc_id[342] = "PROGRAM.CREATEIMAGE_EX.H # 0 1 0 164 ";
    rc_id[343] = "PROGRAM.CREATEIMAGE_EX.PDATA #& 0 1 0 165 ";
    rc_id[344] = "PROGRAM.CREATEIMAGE_EX.COLKEY # 0 1 0 166 ";
    rc_id[345] = "PROGRAM.LOADIMAGE #P 0 1 2 0 0 ";
    rc_id[346] = "PROGRAM.LOADIMAGE.SLOT # 0 1 0 167 ";
    rc_id[347] = "PROGRAM.LOADIMAGE.IMG_FILE$ $ 0 1 0 45 ";
    rc_id[348] = "PROGRAM.COLORKEY #P 0 1 2 0 0 ";
    rc_id[349] = "PROGRAM.COLORKEY.SLOT # 0 1 0 168 ";
    rc_id[350] = "PROGRAM.COLORKEY.COLOR # 0 1 0 169 ";
    rc_id[351] = "PROGRAM.COPYIMAGE #P 0 1 2 0 0 ";
    rc_id[352] = "PROGRAM.COPYIMAGE.SRC_SLOT # 0 1 0 170 ";
    rc_id[353] = "PROGRAM.COPYIMAGE.DST_SLOT # 0 1 0 171 ";
    rc_id[354] = "PROGRAM.DELETEIMAGE #P 0 1 1 0 0 ";
    rc_id[355] = "PROGRAM.DELETEIMAGE.SLOT # 0 1 0 172 ";
    rc_id[356] = "PROGRAM.SETIMAGEALPHA #P 0 1 2 0 0 ";
    rc_id[357] = "PROGRAM.SETIMAGEALPHA.SLOT # 0 1 0 173 ";
    rc_id[358] = "PROGRAM.SETIMAGEALPHA.ALPHA # 0 1 0 174 ";
    rc_id[359] = "PROGRAM.GETIMAGEALPHA #P 0 1 2 0 0 ";
    rc_id[360] = "PROGRAM.GETIMAGEALPHA.SLOT # 0 1 0 175 ";
    rc_id[361] = "PROGRAM.GETIMAGEALPHA.ALPHA #& 0 1 0 176 ";
    rc_id[362] = "PROGRAM.GETIMAGESIZE #P 0 1 3 0 0 ";
    rc_id[363] = "PROGRAM.GETIMAGESIZE.SLOT # 0 1 0 177 ";
    rc_id[364] = "PROGRAM.GETIMAGESIZE.W #& 0 1 0 178 ";
    rc_id[365] = "PROGRAM.GETIMAGESIZE.H #& 0 1 0 179 ";
    rc_id[366] = "PROGRAM.FLIPIMAGE #P 0 1 3 0 0 ";
    rc_id[367] = "PROGRAM.FLIPIMAGE.SLOT # 0 1 0 180 ";
    rc_id[368] = "PROGRAM.FLIPIMAGE.FLIP_H # 0 1 0 181 ";
    rc_id[369] = "PROGRAM.FLIPIMAGE.FLIP_V # 0 1 0 182 ";
    rc_id[370] = "PROGRAM.DRAWIMAGE #P 0 1 3 0 0 ";
    rc_id[371] = "PROGRAM.DRAWIMAGE.SLOT # 0 1 0 183 ";
    rc_id[372] = "PROGRAM.DRAWIMAGE.X # 0 1 0 184 ";
    rc_id[373] = "PROGRAM.DRAWIMAGE.Y # 0 1 0 185 ";
    rc_id[374] = "PROGRAM.DRAWIMAGE_BLIT #P 0 1 7 0 0 ";
    rc_id[375] = "PROGRAM.DRAWIMAGE_BLIT.SLOT # 0 1 0 186 ";
    rc_id[376] = "PROGRAM.DRAWIMAGE_BLIT.X # 0 1 0 187 ";
    rc_id[377] = "PROGRAM.DRAWIMAGE_BLIT.Y # 0 1 0 188 ";
    rc_id[378] = "PROGRAM.DRAWIMAGE_BLIT.SRC_X # 0 1 0 189 ";
    rc_id[379] = "PROGRAM.DRAWIMAGE_BLIT.SRC_Y # 0 1 0 190 ";
    rc_id[380] = "PROGRAM.DRAWIMAGE_BLIT.SRC_W # 0 1 0 191 ";
    rc_id[381] = "PROGRAM.DRAWIMAGE_BLIT.SRC_H # 0 1 0 192 ";
    rc_id[382] = "PROGRAM.DRAWIMAGE_ROTATE #P 0 1 4 0 0 ";
    rc_id[383] = "PROGRAM.DRAWIMAGE_ROTATE.SLOT # 0 1 0 193 ";
    rc_id[384] = "PROGRAM.DRAWIMAGE_ROTATE.X # 0 1 0 194 ";
    rc_id[385] = "PROGRAM.DRAWIMAGE_ROTATE.Y # 0 1 0 195 ";
    rc_id[386] = "PROGRAM.DRAWIMAGE_ROTATE.ANGLE # 0 1 0 196 ";
    rc_id[387] = "PROGRAM.DRAWIMAGE_ROTATE_EX #P 0 1 8 0 0 ";
    rc_id[388] = "PROGRAM.DRAWIMAGE_ROTATE_EX.SLOT # 0 1 0 197 ";
    rc_id[389] = "PROGRAM.DRAWIMAGE_ROTATE_EX.X # 0 1 0 198 ";
    rc_id[390] = "PROGRAM.DRAWIMAGE_ROTATE_EX.Y # 0 1 0 199 ";
    rc_id[391] = "PROGRAM.DRAWIMAGE_ROTATE_EX.SRC_X # 0 1 0 200 ";
    rc_id[392] = "PROGRAM.DRAWIMAGE_ROTATE_EX.SRC_Y # 0 1 0 201 ";
    rc_id[393] = "PROGRAM.DRAWIMAGE_ROTATE_EX.SRC_W # 0 1 0 202 ";
    rc_id[394] = "PROGRAM.DRAWIMAGE_ROTATE_EX.SRC_H # 0 1 0 203 ";
    rc_id[395] = "PROGRAM.DRAWIMAGE_ROTATE_EX.ANGLE # 0 1 0 204 ";
    rc_id[396] = "PROGRAM.DRAWIMAGE_ZOOM #P 0 1 5 0 0 ";
    rc_id[397] = "PROGRAM.DRAWIMAGE_ZOOM.SLOT # 0 1 0 205 ";
    rc_id[398] = "PROGRAM.DRAWIMAGE_ZOOM.X # 0 1 0 206 ";
    rc_id[399] = "PROGRAM.DRAWIMAGE_ZOOM.Y # 0 1 0 207 ";
    rc_id[400] = "PROGRAM.DRAWIMAGE_ZOOM.ZOOM_X # 0 1 0 208 ";
    rc_id[401] = "PROGRAM.DRAWIMAGE_ZOOM.ZOOM_Y # 0 1 0 209 ";
    rc_id[402] = "PROGRAM.DRAWIMAGE_ZOOM_EX #P 0 1 9 0 0 ";
    rc_id[403] = "PROGRAM.DRAWIMAGE_ZOOM_EX.SLOT # 0 1 0 210 ";
    rc_id[404] = "PROGRAM.DRAWIMAGE_ZOOM_EX.X # 0 1 0 211 ";
    rc_id[405] = "PROGRAM.DRAWIMAGE_ZOOM_EX.Y # 0 1 0 212 ";
    rc_id[406] = "PROGRAM.DRAWIMAGE_ZOOM_EX.SRC_X # 0 1 0 213 ";
    rc_id[407] = "PROGRAM.DRAWIMAGE_ZOOM_EX.SRC_Y # 0 1 0 214 ";
    rc_id[408] = "PROGRAM.DRAWIMAGE_ZOOM_EX.SRC_W # 0 1 0 215 ";
    rc_id[409] = "PROGRAM.DRAWIMAGE_ZOOM_EX.SRC_H # 0 1 0 216 ";
    rc_id[410] = "PROGRAM.DRAWIMAGE_ZOOM_EX.ZOOM_X # 0 1 0 217 ";
    rc_id[411] = "PROGRAM.DRAWIMAGE_ZOOM_EX.ZOOM_Y # 0 1 0 218 ";
    rc_id[412] = "PROGRAM.DRAWIMAGE_ROTOZOOM #P 0 1 6 0 0 ";
    rc_id[413] = "PROGRAM.DRAWIMAGE_ROTOZOOM.SLOT # 0 1 0 219 ";
    rc_id[414] = "PROGRAM.DRAWIMAGE_ROTOZOOM.X # 0 1 0 220 ";
    rc_id[415] = "PROGRAM.DRAWIMAGE_ROTOZOOM.Y # 0 1 0 221 ";
    rc_id[416] = "PROGRAM.DRAWIMAGE_ROTOZOOM.ANGLE # 0 1 0 222 ";
    rc_id[417] = "PROGRAM.DRAWIMAGE_ROTOZOOM.ZOOM_X # 0 1 0 223 ";
    rc_id[418] = "PROGRAM.DRAWIMAGE_ROTOZOOM.ZOOM_Y # 0 1 0 224 ";
    rc_id[419] = "PROGRAM.DRAWIMAGE_ROTOZOOM_EX #P 0 1 10 0 0 ";
    rc_id[420] = "PROGRAM.DRAWIMAGE_ROTOZOOM_EX.SLOT # 0 1 0 225 ";
    rc_id[421] = "PROGRAM.DRAWIMAGE_ROTOZOOM_EX.X # 0 1 0 226 ";
    rc_id[422] = "PROGRAM.DRAWIMAGE_ROTOZOOM_EX.Y # 0 1 0 227 ";
    rc_id[423] = "PROGRAM.DRAWIMAGE_ROTOZOOM_EX.SRC_X # 0 1 0 228 ";
    rc_id[424] = "PROGRAM.DRAWIMAGE_ROTOZOOM_EX.SRC_Y # 0 1 0 229 ";
    rc_id[425] = "PROGRAM.DRAWIMAGE_ROTOZOOM_EX.SRC_W # 0 1 0 230 ";
    rc_id[426] = "PROGRAM.DRAWIMAGE_ROTOZOOM_EX.SRC_H # 0 1 0 231 ";
    rc_id[427] = "PROGRAM.DRAWIMAGE_ROTOZOOM_EX.ANGLE # 0 1 0 232 ";
    rc_id[428] = "PROGRAM.DRAWIMAGE_ROTOZOOM_EX.ZOOM_X # 0 1 0 233 ";
    rc_id[429] = "PROGRAM.DRAWIMAGE_ROTOZOOM_EX.ZOOM_Y # 0 1 0 234 ";
    rc_id[430] = "PROGRAM.DRAWIMAGE_BLIT_EX #P 0 1 9 0 0 ";
    rc_id[431] = "PROGRAM.DRAWIMAGE_BLIT_EX.SLOT # 0 1 0 235 ";
    rc_id[432] = "PROGRAM.DRAWIMAGE_BLIT_EX.POS_X # 0 1 0 236 ";
    rc_id[433] = "PROGRAM.DRAWIMAGE_BLIT_EX.POS_Y # 0 1 0 237 ";
    rc_id[434] = "PROGRAM.DRAWIMAGE_BLIT_EX.POS_W # 0 1 0 238 ";
    rc_id[435] = "PROGRAM.DRAWIMAGE_BLIT_EX.POS_H # 0 1 0 239 ";
    rc_id[436] = "PROGRAM.DRAWIMAGE_BLIT_EX.SRC_X # 0 1 0 240 ";
    rc_id[437] = "PROGRAM.DRAWIMAGE_BLIT_EX.SRC_Y # 0 1 0 241 ";
    rc_id[438] = "PROGRAM.DRAWIMAGE_BLIT_EX.SRC_W # 0 1 0 242 ";
    rc_id[439] = "PROGRAM.DRAWIMAGE_BLIT_EX.SRC_H # 0 1 0 243 ";
    rc_id[440] = "PROGRAM.GETCURSOR #P 0 1 2 0 0 ";
    rc_id[441] = "PROGRAM.GETCURSOR.X #& 0 1 0 244 ";
    rc_id[442] = "PROGRAM.GETCURSOR.Y #& 0 1 0 245 ";
    rc_id[443] = "PROGRAM.PRINTS #P 0 1 1 0 0 ";
    rc_id[444] = "PROGRAM.PRINTS.TXT$ $ 0 1 0 46 ";
    rc_id[445] = "PROGRAM.INPUTS$ $F 0 1 1 0 0 ";
    rc_id[446] = "PROGRAM.INPUTS$.PROMPT$ $ 0 1 0 47 ";
    rc_id[447] = "PROGRAM.LOCATE #P 0 1 2 0 0 ";
    rc_id[448] = "PROGRAM.LOCATE.X # 0 1 0 246 ";
    rc_id[449] = "PROGRAM.LOCATE.Y # 0 1 0 247 ";
    rc_id[450] = "PROGRAM.FONTISLOADED #F 0 1 1 0 0 ";
    rc_id[451] = "PROGRAM.FONTISLOADED.F_SLOT # 0 1 0 248 ";
    rc_id[452] = "PROGRAM.FONT #P 0 1 1 0 0 ";
    rc_id[453] = "PROGRAM.FONT.F_SLOT # 0 1 0 249 ";
    rc_id[454] = "PROGRAM.LOADFONT #P 0 1 3 0 0 ";
    rc_id[455] = "PROGRAM.LOADFONT.F_SLOT # 0 1 0 250 ";
    rc_id[456] = "PROGRAM.LOADFONT.FNT_FILE$ $ 0 1 0 48 ";
    rc_id[457] = "PROGRAM.LOADFONT.SIZE # 0 1 0 251 ";
    rc_id[458] = "PROGRAM.DELETEFONT #P 0 1 1 0 0 ";
    rc_id[459] = "PROGRAM.DELETEFONT.F_SLOT # 0 1 0 252 ";
    rc_id[460] = "PROGRAM.SETFONTSTYLE #P 0 1 2 0 0 ";
    rc_id[461] = "PROGRAM.SETFONTSTYLE.F_SLOT # 0 1 0 253 ";
    rc_id[462] = "PROGRAM.SETFONTSTYLE.STYLE # 0 1 0 254 ";
    rc_id[463] = "PROGRAM.DRAWTEXT #P 0 1 3 0 0 ";
    rc_id[464] = "PROGRAM.DRAWTEXT.TXT$ $ 0 1 0 49 ";
    rc_id[465] = "PROGRAM.DRAWTEXT.X # 0 1 0 255 ";
    rc_id[466] = "PROGRAM.DRAWTEXT.Y # 0 1 0 256 ";
    rc_id[467] = "PROGRAM.DRAWTEXT_SHADED #P 0 1 5 0 0 ";
    rc_id[468] = "PROGRAM.DRAWTEXT_SHADED.TXT$ $ 0 1 0 50 ";
    rc_id[469] = "PROGRAM.DRAWTEXT_SHADED.X # 0 1 0 257 ";
    rc_id[470] = "PROGRAM.DRAWTEXT_SHADED.Y # 0 1 0 258 ";
    rc_id[471] = "PROGRAM.DRAWTEXT_SHADED.FG_COLOR # 0 1 0 259 ";
    rc_id[472] = "PROGRAM.DRAWTEXT_SHADED.BG_COLOR # 0 1 0 260 ";
    rc_id[473] = "PROGRAM.DRAWTEXT_BLENDED #P 0 1 3 0 0 ";
    rc_id[474] = "PROGRAM.DRAWTEXT_BLENDED.TXT$ $ 0 1 0 51 ";
    rc_id[475] = "PROGRAM.DRAWTEXT_BLENDED.X # 0 1 0 261 ";
    rc_id[476] = "PROGRAM.DRAWTEXT_BLENDED.Y # 0 1 0 262 ";
    rc_id[477] = "PROGRAM.RENDERTEXT #P 0 1 2 0 0 ";
    rc_id[478] = "PROGRAM.RENDERTEXT.SLOT # 0 1 0 263 ";
    rc_id[479] = "PROGRAM.RENDERTEXT.TXT$ $ 0 1 0 52 ";
    rc_id[480] = "PROGRAM.GETTEXTSIZE #P 0 1 4 0 0 ";
    rc_id[481] = "PROGRAM.GETTEXTSIZE.F_SLOT # 0 1 0 264 ";
    rc_id[482] = "PROGRAM.GETTEXTSIZE.TXT$ $ 0 1 0 53 ";
    rc_id[483] = "PROGRAM.GETTEXTSIZE.W #& 0 1 0 265 ";
    rc_id[484] = "PROGRAM.GETTEXTSIZE.H #& 0 1 0 266 ";
    rc_id[485] = "PROGRAM.INKEY #F 0 1 0 0 0 ";
    rc_id[486] = "PROGRAM.KEY #F 0 1 1 0 0 ";
    rc_id[487] = "PROGRAM.KEY.KEY_CODE # 0 1 0 267 ";
    rc_id[488] = "PROGRAM.WAITKEY #P 0 1 0 0 0 ";
    rc_id[489] = "PROGRAM.HIDEMOUSE #P 0 1 0 0 0 ";
    rc_id[490] = "PROGRAM.SHOWMOUSE #P 0 1 0 0 0 ";
    rc_id[491] = "PROGRAM.MOUSEISVISIBLE #F 0 1 0 0 0 ";
    rc_id[492] = "PROGRAM.GETMOUSE #P 0 1 5 0 0 ";
    rc_id[493] = "PROGRAM.GETMOUSE.X #& 0 1 0 268 ";
    rc_id[494] = "PROGRAM.GETMOUSE.Y #& 0 1 0 269 ";
    rc_id[495] = "PROGRAM.GETMOUSE.MB1 #& 0 1 0 270 ";
    rc_id[496] = "PROGRAM.GETMOUSE.MB2 #& 0 1 0 271 ";
    rc_id[497] = "PROGRAM.GETMOUSE.MB3 #& 0 1 0 272 ";
    rc_id[498] = "PROGRAM.GETMOUSEWHEEL #P 0 1 2 0 0 ";
    rc_id[499] = "PROGRAM.GETMOUSEWHEEL.X_AXIS #& 0 1 0 273 ";
    rc_id[500] = "PROGRAM.GETMOUSEWHEEL.Y_AXIS #& 0 1 0 274 ";
    rc_id[501] = "PROGRAM.IMAGEEXISTS #F 0 1 1 0 0 ";
    rc_id[502] = "PROGRAM.IMAGEEXISTS.SLOT # 0 1 0 275 ";
    rc_id[503] = "PROGRAM.NUMJOYSTICKS #F 0 1 0 0 0 ";
    rc_id[504] = "PROGRAM.JOYAXIS #F 0 1 2 0 0 ";
    rc_id[505] = "PROGRAM.JOYAXIS.JOY_NUM # 0 1 0 276 ";
    rc_id[506] = "PROGRAM.JOYAXIS.AXIS # 0 1 0 277 ";
    rc_id[507] = "PROGRAM.JOYBUTTON #F 0 1 2 0 0 ";
    rc_id[508] = "PROGRAM.JOYBUTTON.JOY_NUM # 0 1 0 278 ";
    rc_id[509] = "PROGRAM.JOYBUTTON.JOY_BUTTON # 0 1 0 279 ";
    rc_id[510] = "PROGRAM.JOYNAME$ $F 0 1 1 0 0 ";
    rc_id[511] = "PROGRAM.JOYNAME$.JOY_NUM # 0 1 0 280 ";
    rc_id[512] = "PROGRAM.LOADSOUND #P 0 1 2 0 0 ";
    rc_id[513] = "PROGRAM.LOADSOUND.SLOT # 0 1 0 281 ";
    rc_id[514] = "PROGRAM.LOADSOUND.SOUND_FILE$ $ 0 1 0 54 ";
    rc_id[515] = "PROGRAM.LOADMUSIC #P 0 1 1 0 0 ";
    rc_id[516] = "PROGRAM.LOADMUSIC.MUSIC_FILE$ $ 0 1 0 55 ";
    rc_id[517] = "PROGRAM.PLAYSOUND #P 0 1 3 0 0 ";
    rc_id[518] = "PROGRAM.PLAYSOUND.SLOT # 0 1 0 282 ";
    rc_id[519] = "PROGRAM.PLAYSOUND.CHANNEL # 0 1 0 283 ";
    rc_id[520] = "PROGRAM.PLAYSOUND.LOOPS # 0 1 0 284 ";
    rc_id[521] = "PROGRAM.PLAYSOUNDTIMED #P 0 1 4 0 0 ";
    rc_id[522] = "PROGRAM.PLAYSOUNDTIMED.SLOT # 0 1 0 285 ";
    rc_id[523] = "PROGRAM.PLAYSOUNDTIMED.CHANNEL # 0 1 0 286 ";
    rc_id[524] = "PROGRAM.PLAYSOUNDTIMED.LOOPS # 0 1 0 287 ";
    rc_id[525] = "PROGRAM.PLAYSOUNDTIMED.MS # 0 1 0 288 ";
    rc_id[526] = "PROGRAM.PLAYMUSIC #P 0 1 1 0 0 ";
    rc_id[527] = "PROGRAM.PLAYMUSIC.LOOPS # 0 1 0 289 ";
    rc_id[528] = "PROGRAM.PAUSESOUND #P 0 1 1 0 0 ";
    rc_id[529] = "PROGRAM.PAUSESOUND.CHANNEL # 0 1 0 290 ";
    rc_id[530] = "PROGRAM.RESUMESOUND #P 0 1 1 0 0 ";
    rc_id[531] = "PROGRAM.RESUMESOUND.CHANNEL # 0 1 0 291 ";
    rc_id[532] = "PROGRAM.PAUSEMUSIC #P 0 1 0 0 0 ";
    rc_id[533] = "PROGRAM.RESUMEMUSIC #P 0 1 0 0 0 ";
    rc_id[534] = "PROGRAM.DELETESOUND #P 0 1 1 0 0 ";
    rc_id[535] = "PROGRAM.DELETESOUND.SLOT # 0 1 0 292 ";
    rc_id[536] = "PROGRAM.FADEMUSICIN #P 0 1 2 0 0 ";
    rc_id[537] = "PROGRAM.FADEMUSICIN.FADE_TIME # 0 1 0 293 ";
    rc_id[538] = "PROGRAM.FADEMUSICIN.LOOPS # 0 1 0 294 ";
    rc_id[539] = "PROGRAM.FADEMUSICOUT #P 0 1 1 0 0 ";
    rc_id[540] = "PROGRAM.FADEMUSICOUT.FADE_TIME # 0 1 0 295 ";
    rc_id[541] = "PROGRAM.MUSICEXISTS #F 0 1 0 0 0 ";
    rc_id[542] = "PROGRAM.SETMUSICVOLUME #P 0 1 1 0 0 ";
    rc_id[543] = "PROGRAM.SETMUSICVOLUME.VOLUME # 0 1 0 296 ";
    rc_id[544] = "PROGRAM.MUSICVOLUME #F 0 1 0 0 0 ";
    rc_id[545] = "PROGRAM.SETMUSICPOSITION #P 0 1 1 0 0 ";
    rc_id[546] = "PROGRAM.SETMUSICPOSITION.MUSIC_POS # 0 1 0 297 ";
    rc_id[547] = "PROGRAM.MUSICPOSITION #F 0 1 0 0 0 ";
    rc_id[548] = "PROGRAM.REWINDMUSIC #P 0 1 0 0 0 ";
    rc_id[549] = "PROGRAM.SETSOUNDCHANNELS #P 0 1 1 0 0 ";
    rc_id[550] = "PROGRAM.SETSOUNDCHANNELS.MAX_CHANNELS # 0 1 0 298 ";
    rc_id[551] = "PROGRAM.NUMSOUNDCHANNELS #F 0 1 0 0 0 ";
    rc_id[552] = "PROGRAM.SOUNDISENABLED #F 0 1 0 0 0 ";
    rc_id[553] = "PROGRAM.SOUNDEXISTS #F 0 1 1 0 0 ";
    rc_id[554] = "PROGRAM.SOUNDEXISTS.SLOT # 0 1 0 299 ";
    rc_id[555] = "PROGRAM.SETCHANNELVOLUME #P 0 1 2 0 0 ";
    rc_id[556] = "PROGRAM.SETCHANNELVOLUME.CHANNEL # 0 1 0 300 ";
    rc_id[557] = "PROGRAM.SETCHANNELVOLUME.VOLUME # 0 1 0 301 ";
    rc_id[558] = "PROGRAM.CHANNELVOLUME #F 0 1 1 0 0 ";
    rc_id[559] = "PROGRAM.CHANNELVOLUME.CHANNEL # 0 1 0 302 ";
    rc_id[560] = "PROGRAM.SETSOUNDVOLUME #P 0 1 2 0 0 ";
    rc_id[561] = "PROGRAM.SETSOUNDVOLUME.SLOT # 0 1 0 303 ";
    rc_id[562] = "PROGRAM.SETSOUNDVOLUME.VOLUME # 0 1 0 304 ";
    rc_id[563] = "PROGRAM.SOUNDVOLUME #F 0 1 1 0 0 ";
    rc_id[564] = "PROGRAM.SOUNDVOLUME.SLOT # 0 1 0 305 ";
    rc_id[565] = "PROGRAM.STOPMUSIC #P 0 1 0 0 0 ";
    rc_id[566] = "PROGRAM.STOPSOUND #P 0 1 1 0 0 ";
    rc_id[567] = "PROGRAM.STOPSOUND.CHANNEL # 0 1 0 306 ";
    rc_id[568] = "PROGRAM.TCP_SOCKETOPEN #P 0 1 3 0 0 ";
    rc_id[569] = "PROGRAM.TCP_SOCKETOPEN.SOCKET # 0 1 0 307 ";
    rc_id[570] = "PROGRAM.TCP_SOCKETOPEN.HOST$ $ 0 1 0 56 ";
    rc_id[571] = "PROGRAM.TCP_SOCKETOPEN.PORT # 0 1 0 308 ";
    rc_id[572] = "PROGRAM.TCP_SOCKETCLOSE #P 0 1 1 0 0 ";
    rc_id[573] = "PROGRAM.TCP_SOCKETCLOSE.SOCKET # 0 1 0 309 ";
    rc_id[574] = "PROGRAM.TCP_REMOTEHOST #F 0 1 1 0 0 ";
    rc_id[575] = "PROGRAM.TCP_REMOTEHOST.SOCKET # 0 1 0 310 ";
    rc_id[576] = "PROGRAM.TCP_REMOTEPORT #F 0 1 1 0 0 ";
    rc_id[577] = "PROGRAM.TCP_REMOTEPORT.SOCKET # 0 1 0 311 ";
    rc_id[578] = "PROGRAM.TCP_GETDATA #F 0 1 3 0 0 ";
    rc_id[579] = "PROGRAM.TCP_GETDATA.SOCKET # 0 1 0 312 ";
    rc_id[580] = "PROGRAM.TCP_GETDATA.DST$ $& 0 1 0 57 ";
    rc_id[581] = "PROGRAM.TCP_GETDATA.NUMBYTES # 0 1 0 313 ";
    rc_id[582] = "PROGRAM.TCP_SENDDATA #P 0 1 2 0 0 ";
    rc_id[583] = "PROGRAM.TCP_SENDDATA.SOCKET # 0 1 0 314 ";
    rc_id[584] = "PROGRAM.TCP_SENDDATA.S_DATA$ $ 0 1 0 58 ";
    rc_id[585] = "PROGRAM.TCP_ACCEPTSOCKET #F 0 1 2 0 0 ";
    rc_id[586] = "PROGRAM.TCP_ACCEPTSOCKET.SOCKET_SERVER # 0 1 0 315 ";
    rc_id[587] = "PROGRAM.TCP_ACCEPTSOCKET.SOCKET_CLIENT # 0 1 0 316 ";
    rc_id[588] = "PROGRAM.UDP_SOCKETOPEN #F 0 1 2 0 0 ";
    rc_id[589] = "PROGRAM.UDP_SOCKETOPEN.SOCKET # 0 1 0 317 ";
    rc_id[590] = "PROGRAM.UDP_SOCKETOPEN.PORT # 0 1 0 318 ";
    rc_id[591] = "PROGRAM.UDP_GETDATA #F 0 1 4 0 0 ";
    rc_id[592] = "PROGRAM.UDP_GETDATA.SOCKET # 0 1 0 319 ";
    rc_id[593] = "PROGRAM.UDP_GETDATA.DST$ $& 0 1 0 59 ";
    rc_id[594] = "PROGRAM.UDP_GETDATA.HOST$ $& 0 1 0 60 ";
    rc_id[595] = "PROGRAM.UDP_GETDATA.PORT #& 0 1 0 320 ";
    rc_id[596] = "PROGRAM.UDP_LENGTH #F 0 1 0 0 0 ";
    rc_id[597] = "PROGRAM.UDP_MAXLENGTH #F 0 1 0 0 0 ";
    rc_id[598] = "PROGRAM.UDP_REMOTEHOST$ $F 0 1 1 0 0 ";
    rc_id[599] = "PROGRAM.UDP_REMOTEHOST$.SOCKET # 0 1 0 321 ";
    rc_id[600] = "PROGRAM.UDP_REMOTEPORT #F 0 1 1 0 0 ";
    rc_id[601] = "PROGRAM.UDP_REMOTEPORT.SOCKET # 0 1 0 322 ";
    rc_id[602] = "PROGRAM.UDP_SOCKETCLOSE #P 0 1 1 0 0 ";
    rc_id[603] = "PROGRAM.UDP_SOCKETCLOSE.SOCKET # 0 1 0 323 ";
    rc_id[604] = "PROGRAM.UDP_SENDDATA #F 0 1 4 0 0 ";
    rc_id[605] = "PROGRAM.UDP_SENDDATA.SOCKET # 0 1 0 324 ";
    rc_id[606] = "PROGRAM.UDP_SENDDATA.S_DATA$ $ 0 1 0 61 ";
    rc_id[607] = "PROGRAM.UDP_SENDDATA.HOST$ $ 0 1 0 62 ";
    rc_id[608] = "PROGRAM.UDP_SENDDATA.PORT # 0 1 0 325 ";
    rc_id[609] = "PROGRAM.TOUCHPRESSURE #F 0 1 0 0 0 ";
    rc_id[610] = "PROGRAM.GETTOUCH #P 0 1 5 0 0 ";
    rc_id[611] = "PROGRAM.GETTOUCH.STATUS #& 0 1 0 326 ";
    rc_id[612] = "PROGRAM.GETTOUCH.X #& 0 1 0 327 ";
    rc_id[613] = "PROGRAM.GETTOUCH.Y #& 0 1 0 328 ";
    rc_id[614] = "PROGRAM.GETTOUCH.DX #& 0 1 0 329 ";
    rc_id[615] = "PROGRAM.GETTOUCH.DY #& 0 1 0 330 ";
    rc_id[616] = "PROGRAM.GETMULTITOUCH #P 0 1 6 0 0 ";
    rc_id[617] = "PROGRAM.GETMULTITOUCH.STATUS #& 0 1 0 331 ";
    rc_id[618] = "PROGRAM.GETMULTITOUCH.X #& 0 1 0 332 ";
    rc_id[619] = "PROGRAM.GETMULTITOUCH.Y #& 0 1 0 333 ";
    rc_id[620] = "PROGRAM.GETMULTITOUCH.NUMFINGERS #& 0 1 0 334 ";
    rc_id[621] = "PROGRAM.GETMULTITOUCH.DIST #& 0 1 0 335 ";
    rc_id[622] = "PROGRAM.GETMULTITOUCH.THETA #& 0 1 0 336 ";
    rc_id[623] = "PROGRAM.ZONEINPUT$ $F 0 1 4 0 0 ";
    rc_id[624] = "PROGRAM.ZONEINPUT$.X # 0 1 0 337 ";
    rc_id[625] = "PROGRAM.ZONEINPUT$.Y # 0 1 0 338 ";
    rc_id[626] = "PROGRAM.ZONEINPUT$.W # 0 1 0 339 ";
    rc_id[627] = "PROGRAM.ZONEINPUT$.H # 0 1 0 340 ";
    rc_id[628] = "PROGRAM.READINPUT_START #P 0 1 0 0 0 ";
    rc_id[629] = "PROGRAM.READINPUT_STOP #P 0 1 0 0 0 ";
    rc_id[630] = "PROGRAM.READINPUT_TEXT$ $F 0 1 0 0 0 ";
    rc_id[631] = "PROGRAM.CANVASCLIP #P 0 1 5 0 0 ";
    rc_id[632] = "PROGRAM.CANVASCLIP.SLOT # 0 1 0 341 ";
    rc_id[633] = "PROGRAM.CANVASCLIP.SRC_X # 0 1 0 342 ";
    rc_id[634] = "PROGRAM.CANVASCLIP.SRC_Y # 0 1 0 343 ";
    rc_id[635] = "PROGRAM.CANVASCLIP.SRC_W # 0 1 0 344 ";
    rc_id[636] = "PROGRAM.CANVASCLIP.SRC_H # 0 1 0 345 ";
    rc_id[637] = "PROGRAM.WINDOWCLIP #P 0 1 5 0 0 ";
    rc_id[638] = "PROGRAM.WINDOWCLIP.SLOT # 0 1 0 346 ";
    rc_id[639] = "PROGRAM.WINDOWCLIP.SRC_X # 0 1 0 347 ";
    rc_id[640] = "PROGRAM.WINDOWCLIP.SRC_Y # 0 1 0 348 ";
    rc_id[641] = "PROGRAM.WINDOWCLIP.SRC_W # 0 1 0 349 ";
    rc_id[642] = "PROGRAM.WINDOWCLIP.SRC_H # 0 1 0 350 ";
    rc_id[643] = "PROGRAM.LOCALIP$ $F 0 1 0 0 0 ";
    rc_id[644] = "PROGRAM.READINPUT_SETTEXT #P 0 1 1 0 0 ";
    rc_id[645] = "PROGRAM.READINPUT_SETTEXT.TXT$ $ 0 1 0 63 ";
    rc_id[646] = "PROGRAM.GETTOUCHFINGER #P 0 1 4 0 0 ";
    rc_id[647] = "PROGRAM.GETTOUCHFINGER.FINGER # 0 1 0 351 ";
    rc_id[648] = "PROGRAM.GETTOUCHFINGER.X #& 0 1 0 352 ";
    rc_id[649] = "PROGRAM.GETTOUCHFINGER.Y #& 0 1 0 353 ";
    rc_id[650] = "PROGRAM.GETTOUCHFINGER.PRESSURE #& 0 1 0 354 ";
    rc_id[651] = "PROGRAM.NUMFINGERS #F 0 1 0 0 0 ";
    rc_id[652] = "PROGRAM.LOADIMAGE_EX #P 0 1 3 0 0 ";
    rc_id[653] = "PROGRAM.LOADIMAGE_EX.SLOT # 0 1 0 355 ";
    rc_id[654] = "PROGRAM.LOADIMAGE_EX.IMG_FILE$ $ 0 1 0 64 ";
    rc_id[655] = "PROGRAM.LOADIMAGE_EX.COLKEY # 0 1 0 356 ";
    rc_id[656] = "PROGRAM.RECT #P 0 1 4 0 0 ";
    rc_id[657] = "PROGRAM.RECT.X # 0 1 0 357 ";
    rc_id[658] = "PROGRAM.RECT.Y # 0 1 0 358 ";
    rc_id[659] = "PROGRAM.RECT.W # 0 1 0 359 ";
    rc_id[660] = "PROGRAM.RECT.H # 0 1 0 360 ";
    rc_id[661] = "PROGRAM.RECTFILL #P 0 1 4 0 0 ";
    rc_id[662] = "PROGRAM.RECTFILL.X # 0 1 0 361 ";
    rc_id[663] = "PROGRAM.RECTFILL.Y # 0 1 0 362 ";
    rc_id[664] = "PROGRAM.RECTFILL.W # 0 1 0 363 ";
    rc_id[665] = "PROGRAM.RECTFILL.H # 0 1 0 364 ";
    rc_id[666] = "PROGRAM.OS$ $F 0 1 0 0 0 ";
    rc_id[667] = "PROGRAM.NUMJOYBUTTONS #F 0 1 1 0 0 ";
    rc_id[668] = "PROGRAM.NUMJOYBUTTONS.JOY_NUM # 0 1 0 365 ";
    rc_id[669] = "PROGRAM.NUMJOYAXES #F 0 1 1 0 0 ";
    rc_id[670] = "PROGRAM.NUMJOYAXES.JOY_NUM # 0 1 0 366 ";
    rc_id[671] = "PROGRAM.LOADVIDEO #P 0 1 1 0 0 ";
    rc_id[672] = "PROGRAM.LOADVIDEO.VID_FILE$ $ 0 1 0 65 ";
    rc_id[673] = "PROGRAM.PLAYVIDEO #P 0 1 1 0 0 ";
    rc_id[674] = "PROGRAM.PLAYVIDEO.VLOOPS # 0 1 0 367 ";
    rc_id[675] = "PROGRAM.PAUSEVIDEO #P 0 1 0 0 0 ";
    rc_id[676] = "PROGRAM.STOPVIDEO #P 0 1 0 0 0 ";
    rc_id[677] = "PROGRAM.SETVIDEOPOSITION #P 0 1 1 0 0 ";
    rc_id[678] = "PROGRAM.SETVIDEOPOSITION.VPOS # 0 1 0 368 ";
    rc_id[679] = "PROGRAM.RESUMEVIDEO #P 0 1 0 0 0 ";
    rc_id[680] = "PROGRAM.VIDEOPOSITION #F 0 1 0 0 0 ";
    rc_id[681] = "PROGRAM.DELETEVIDEO #P 0 1 0 0 0 ";
    rc_id[682] = "PROGRAM.DELETEMUSIC #P 0 1 0 0 0 ";
    rc_id[683] = "PROGRAM.SYSTEM #F 0 1 1 0 0 ";
    rc_id[684] = "PROGRAM.SYSTEM.SYS_CMD$ $ 0 1 0 66 ";
    rc_id[685] = "PROGRAM.VIDEOISPLAYING #F 0 1 0 0 0 ";
    rc_id[686] = "PROGRAM.ROUNDRECT #P 0 1 5 0 0 ";
    rc_id[687] = "PROGRAM.ROUNDRECT.X # 0 1 0 369 ";
    rc_id[688] = "PROGRAM.ROUNDRECT.Y # 0 1 0 370 ";
    rc_id[689] = "PROGRAM.ROUNDRECT.W # 0 1 0 371 ";
    rc_id[690] = "PROGRAM.ROUNDRECT.H # 0 1 0 372 ";
    rc_id[691] = "PROGRAM.ROUNDRECT.R # 0 1 0 373 ";
    rc_id[692] = "PROGRAM.ROUNDRECTFILL #P 0 1 5 0 0 ";
    rc_id[693] = "PROGRAM.ROUNDRECTFILL.X # 0 1 0 374 ";
    rc_id[694] = "PROGRAM.ROUNDRECTFILL.Y # 0 1 0 375 ";
    rc_id[695] = "PROGRAM.ROUNDRECTFILL.W # 0 1 0 376 ";
    rc_id[696] = "PROGRAM.ROUNDRECTFILL.H # 0 1 0 377 ";
    rc_id[697] = "PROGRAM.ROUNDRECTFILL.R # 0 1 0 378 ";
    rc_id[698] = "PROGRAM.VIDEOEND #F 0 1 0 0 0 ";
    rc_id[699] = "PROGRAM.GETVIDEOSTATS #F 0 1 5 0 0 ";
    rc_id[700] = "PROGRAM.GETVIDEOSTATS.VFILE$ $ 0 1 0 67 ";
    rc_id[701] = "PROGRAM.GETVIDEOSTATS.VLENGTH #& 0 1 0 379 ";
    rc_id[702] = "PROGRAM.GETVIDEOSTATS.VFPS #& 0 1 0 380 ";
    rc_id[703] = "PROGRAM.GETVIDEOSTATS.W #& 0 1 0 381 ";
    rc_id[704] = "PROGRAM.GETVIDEOSTATS.H #& 0 1 0 382 ";
    rc_id[705] = "PROGRAM.SETVIDEODRAWRECT #P 0 1 4 0 0 ";
    rc_id[706] = "PROGRAM.SETVIDEODRAWRECT.X # 0 1 0 383 ";
    rc_id[707] = "PROGRAM.SETVIDEODRAWRECT.Y # 0 1 0 384 ";
    rc_id[708] = "PROGRAM.SETVIDEODRAWRECT.W # 0 1 0 385 ";
    rc_id[709] = "PROGRAM.SETVIDEODRAWRECT.H # 0 1 0 386 ";
    rc_id[710] = "PROGRAM.GETVIDEODRAWRECT #P 0 1 4 0 0 ";
    rc_id[711] = "PROGRAM.GETVIDEODRAWRECT.X #& 0 1 0 387 ";
    rc_id[712] = "PROGRAM.GETVIDEODRAWRECT.Y #& 0 1 0 388 ";
    rc_id[713] = "PROGRAM.GETVIDEODRAWRECT.W #& 0 1 0 389 ";
    rc_id[714] = "PROGRAM.GETVIDEODRAWRECT.H #& 0 1 0 390 ";
    rc_id[715] = "PROGRAM.GETVIDEOSIZE #P 0 1 2 0 0 ";
    rc_id[716] = "PROGRAM.GETVIDEOSIZE.W #& 0 1 0 391 ";
    rc_id[717] = "PROGRAM.GETVIDEOSIZE.H #& 0 1 0 392 ";
    rc_id[718] = "PROGRAM.VIDEOEXISTS #F 0 1 0 0 0 ";
    rc_id[719] = "PROGRAM.SETVIDEOALPHA #P 0 1 1 0 0 ";
    rc_id[720] = "PROGRAM.SETVIDEOALPHA.V_ALPHA # 0 1 0 393 ";
    rc_id[721] = "PROGRAM.CREATESOUND #P 0 1 4 0 0 ";
    rc_id[722] = "PROGRAM.CREATESOUND.SLOT # 0 1 0 394 ";
    rc_id[723] = "PROGRAM.CREATESOUND.ABUF #& 0 1 0 395 ";
    rc_id[724] = "PROGRAM.CREATESOUND.ALEN # 0 1 0 396 ";
    rc_id[725] = "PROGRAM.CREATESOUND.AVOL # 0 1 0 397 ";
    rc_id[726] = "PROGRAM.COMMAND$ $F 0 1 1 0 0 ";
    rc_id[727] = "PROGRAM.COMMAND$.ARG # 0 1 0 398 ";
    rc_id[728] = "PROGRAM.STR_F$ $F 0 1 1 0 0 ";
    rc_id[729] = "PROGRAM.STR_F$.N # 0 1 0 399 ";
    rc_id[730] = "PROGRAM.STR_S$ $F 0 1 1 0 0 ";
    rc_id[731] = "PROGRAM.STR_S$.N # 0 1 0 400 ";
    rc_id[732] = "PROGRAM.CLS #P 0 1 0 0 0 ";
    rc_id[733] = "PROGRAM.ENV$ $F 0 1 1 0 0 ";
    rc_id[734] = "PROGRAM.ENV$.V$ $ 0 1 0 68 ";
    rc_id[735] = "PROGRAM.SETENV #F 0 1 1 0 0 ";
    rc_id[736] = "PROGRAM.SETENV.V$ $ 0 1 0 69 ";
    rc_id[737] = "PROGRAM.PREFPATH$ $F 0 1 2 0 0 ";
    rc_id[738] = "PROGRAM.PREFPATH$.ORG_NAME$ $ 0 1 0 70 ";
    rc_id[739] = "PROGRAM.PREFPATH$.APP_NAME$ $ 0 1 0 71 ";
    rc_id[740] = "PROGRAM.NUMJOYHATS #F 0 1 1 0 0 ";
    rc_id[741] = "PROGRAM.NUMJOYHATS.JOY_NUM # 0 1 0 401 ";
    rc_id[742] = "PROGRAM.JOYHAT #F 0 1 2 0 0 ";
    rc_id[743] = "PROGRAM.JOYHAT.JOY_NUM # 0 1 0 402 ";
    rc_id[744] = "PROGRAM.JOYHAT.HAT # 0 1 0 403 ";
    rc_id[745] = "PROGRAM.NUMJOYTRACKBALLS #F 0 1 1 0 0 ";
    rc_id[746] = "PROGRAM.NUMJOYTRACKBALLS.JOY_NUM # 0 1 0 404 ";
    rc_id[747] = "PROGRAM.GETJOYTRACKBALL #P 0 1 4 0 0 ";
    rc_id[748] = "PROGRAM.GETJOYTRACKBALL.JOY_NUM # 0 1 0 405 ";
    rc_id[749] = "PROGRAM.GETJOYTRACKBALL.BALL # 0 1 0 406 ";
    rc_id[750] = "PROGRAM.GETJOYTRACKBALL.DX #& 0 1 0 407 ";
    rc_id[751] = "PROGRAM.GETJOYTRACKBALL.DY #& 0 1 0 408 ";
    rc_id[752] = "PROGRAM.WINDOWHASINPUTFOCUS #F 0 1 1 0 0 ";
    rc_id[753] = "PROGRAM.WINDOWHASINPUTFOCUS.WIN # 0 1 0 409 ";
    rc_id[754] = "PROGRAM.WINDOWHASMOUSEFOCUS #F 0 1 1 0 0 ";
    rc_id[755] = "PROGRAM.WINDOWHASMOUSEFOCUS.WIN # 0 1 0 410 ";

    nid_count = 411;
    sid_count = 72;
    rc_id_count = 756;

    //nid_count = 53;
    //sid_count = 43;
    //rc_id_count = 172;

    string id = "";
    for(int i = 0; i < rc_id_count; i++)
    {
        id = rc_id[i].substr(0,rc_id[i].find_first_of(" "));
        if(rc_getID_data(id,"id_type").compare("#&")==0)
        {
            //cout << "byref# = " << id << endl;
            nid_byref_index[nid_byref_count] = atoi(rc_getID_data(id,"id_index").c_str());
            nid_byref_count++;
        }
        else if(rc_getID_data(id,"id_type").compare("$&")==0)
        {
            //cout << "byref$ = " << id << endl;
            sid_byref_index[sid_byref_count] = atoi(rc_getID_data(id,"id_index").c_str());
            sid_byref_count++;
        }
    }

    return 0;
}

int rc_getMainSourceLine(int line_num)
{
    for(int i = 0; i < end_of_line_count; i++)
    {
        if(line_num <= end_of_line[i])
            return i+1;
    }
    return line_num;
}

int main(int argc, char * argv[])
{
//    rc_file.open("/home/n00b/Desktop/rc_lib.bas", fstream::out);
//    for(int i = 0; i < rc_keywords_count; i++)
//        rc_file << StringToLower(rc_keyword[i]) << " ";
//    rc_file.close();
//    return 0;
    //cout << "START\n";
    rc_initFunctions();

    //cout << "CP1\n";

    string s_line="";
    rc_segment_switch = RC_CODE_SEGMENT_SWITCH;
    tmp[tmp_count] = ".CODE";
    tmp_count++;

    string in_file = "";
    //cin >> in_file;

    if(argc > 1)
        in_file = argv[1];
    //else
     //   in_file = "test.bas";

    //cout << "CP2\n";

    rc_file.open(in_file.c_str(), fstream::in);
    //rc_file.open("C:\\Users\\Recademics\\Desktop\\New Folder\\demo\\rc_test.bas", fstream::in);
    //rc_file.open("/home/n00b/Desktop/rc_lib.bas", fstream::in);

    if(!rc_file.is_open())
    {
        cout << "file not found\n";
        #ifdef RC_WINDOWS
            system("PAUSE");
        #endif // RC_WINDOWS
        exit(EXIT_FAILURE);
        return 0;
    }

    //cout << "RC Basic file open\n";

    int analyze_status = 0;
    int line_number = 1;
    current_scope = 0;
    int pre_process_status = 0;

    #ifdef RC_WINDOWS
        string rc_src_path = in_file.substr(0, in_file.find_last_of("\\"));
    #else
        char * p = realpath(in_file.c_str(), NULL);
        string rc_src_path = (string)p;
        rc_src_path = rc_src_path.substr(0, rc_src_path.find_last_of("/"));
        free(p);
    #endif

    rc_init();
    //cout << "CP4\n";
    int pp_cycle = 0;
    int pp_status = rc_preprocessor(rc_src_path);
    //cout << "CP5\n";

    while(pp_status!=0)
    {
        if(pp_status==2)
        {
            //cout << "Preproccessor failed to compile at line " << src_line_num << "\n";
            #ifdef RC_WINDOWS
                system("PAUSE");
            #endif // RC_WINDOWS
            exit(EXIT_FAILURE);
            return 2;
        }
        src_line_num++;
        pp_status = rc_preprocessor(rc_src_path);
        //cout << "PRE PROCCESS: #" << pp_cycle << endl;
        //cin >> s_line;
        //pp_cycle++;
    }

    rc_file.close();

//    for(int i = 0; i < end_of_line_count; i++)
//        cout << i << " -> " << end_of_line[i] << endl;

    //cout << "RC Basic file close\n";

    rc_file.open(pp_str.c_str(), fstream::in);

    line_number = 1;//src_line_num;

    int tmp_size = 10000;

    int inc_i = 0;


    while(true)
    {
        if(tmp_count > (tmp_size-5000))
        {
            tmp_size += 5000;
            tmp.resize(tmp_size);
        }
        //cout << "Enter a Command: ";
        //cout << "LINE #" << line_number << endl;
        rc_tokens_count = 0;
        //getline(cin, s_line);
        s_line = "";
        getline(rc_file, s_line);
        if(s_line.compare("#INC_START")==0 || s_line.compare("#INC_END")==0 || s_line.compare("#SKIP")==0)
        {
            if(s_line.compare("#INC_END")==0)
            {
                line_number++;
            }
            continue;
        }
        //s_line = " " + s_line;
        //cout << endl << line_number << " code_line = " << s_line << " ->                    " << s_line.length() << endl;
        //s_line = s_line.substr(s_line.find_first_not_of(" ")-1);
        analyze_status = rc_analyze(s_line);
        if(analyze_status!=0)
        {
            cout << "Error on line: " << s_line << endl;
            //cout << "Compiler Stopped with status " << analyze_status << " at line " << line_number << "\n";
            cout << "Compiler stopped at line " << rc_getMainSourceLine(line_number) << endl;
            cout << line_number << endl;
            #ifdef RC_WINDOWS
                system("PAUSE");
            #endif
            exit(EXIT_FAILURE);
            return 2;
        }
        if(rc_file.eof())
        {
            //cout << "end of file reached\n";
            if(current_scope!=0)
            {
                cout << "Compiler Error: Cannot end program in current scope" << endl;
                #ifdef RC_WINDOWS
                    system("PAUSE");
                #endif // RC_WINDOWS
                exit(EXIT_FAILURE);
                return 2;
            }
            break;
        }
        //cout << "ere\n\n\n\n";
        //cout << "rctc = " << rc_tokens_count << endl;
        rc_clearTokens();
        line_number++;
    }

    rc_file.close();

    tmp[tmp_count] = "end 0 ";
    tmp_count++;

    rc_writeByteCode();

    remove("rc_file_copy.bas");
    remove("tmp.bas");

    //for(int i = 0; i < tmp_count; i++)
    //    cout << "TMP[" << rc_intToString(i) <<"] = " << tmp[i] << endl;

    //system("pause");

    //string rdbg; cin >> rdbg;

    //cout << endl << endl;

    string filter_id = "";
    string filter_type = "";
    string def_id = "";

//    fstream def_file;
//    //def_file.open("C:\\Users\\Recademics\\Desktop\\New folder\\demo\\def_file.txt", ios::out);
//    def_file.open("/home/n00b/Desktop/def_file.txt", ios::out);
//
//    for(int i = 0; i < rc_id_count; i++)
//    {
//        filter_id = rc_id[i].substr(0,rc_id[i].find_first_of(" "));
//        filter_type = rc_getID_data(filter_id, "id_type");
//        if(filter_type.compare("#")==0 || filter_type.compare("$")==0 ||
//           filter_type.compare("#&")==0 || filter_type.compare("$&")==0)
//        {
//            def_id = filter_id.substr(filter_id.find_first_of(".")+1);
//            for(int n = 0; n < def_id.length(); n++)
//            {
//                if(def_id.substr(n,1).compare(".")==0)
//                {
//                    def_id = def_id.substr(0,n) + "_" + def_id.substr(n+1);
//                }
//                if(def_id.substr(n,1).compare("$")==0)
//                {
//                    def_id = def_id.substr(0,n) + "_STR" + def_id.substr(n+1);
//                }
//            }
//            def_file << "#define " << def_id << " " << rc_getID_data(filter_id, "id_index") << endl;
//        }
//        else
//        {
//            //cout << "bull\n";
//        }
//        //cout << filter_id << "  --->  " << rc_getID_data(filter_id, "id_type") << endl;
//        //def_file << "rc_id[" << i << "] = \"" << rc_id[i] << "\";" << endl;
//    }
//
//    def_file.close();

    //cout << endl << endl;
    s_line = "";

    //cout << nid_byref_count << endl;

    //for(int i = 0; i < nid_byref_count; i++)
      //  cout << "NID BYREF #" << nid_byref_index[i] << endl;

    //for(int i = 0; i < sid_byref_count; i++)
      //  cout << "SID BYREF #" << sid_byref_index[i] << endl;

    //cout << endl << endl;

    string out_file = in_file;
    out_file = out_file.substr(0, out_file.find_first_of(".")) + ".cbc";
    //cout << out_file << endl;
    //return 0;

    rc_file.open(out_file.c_str(), fstream::out | fstream::binary | fstream::trunc);
    u_index seg_size;

    seg_size.i = rc_code_offset;

    //cout << seg_size.i << endl;

    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    seg_size.i = rc_data_offset;

    //cout << seg_size.i << endl;

    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    seg_size.i = nid_byref_count;

    //cout << seg_size.i << endl;

    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    seg_size.i = sid_byref_count;

    //cout << seg_size.i << endl;

    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    for(int i = 0; i < nid_byref_count; i++)
    {
        //cout << "BYREF -> " << nid_byref_index[i] << endl;
        seg_size.i = nid_byref_index[i];

        //cout << seg_size.i << endl;

        for(int n = 0; n < sizeof(seg_size.i); n++)
            rc_file.put(seg_size.data[n]);
    }
    //string bs;cin>>bs;

    for(int i = 0; i < sid_byref_count; i++)
    {
        seg_size.i = sid_byref_index[i];

        //cout << seg_size.i << endl;

        for(int n = 0; n < sizeof(seg_size.i); n++)
            rc_file.put(seg_size.data[n]);
    }

    seg_size.i = nid_count;
    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    seg_size.i = sid_count;
    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    seg_size.i = max_m;
    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    seg_size.i = max_s;
    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    seg_size.i = max_f;
    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    seg_size.i = num_stack_size+32;

    //cout << seg_size.i << endl;

    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    seg_size.i = str_stack_size+32;
    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    for(int i = 0; i < rc_code_offset; i++)
        rc_file.put(RC_CODE_SEGMENT.at(i));

    for(int i = 0; i < rc_data_offset; i++)
        rc_file.put(RC_DATA_SEGMENT.at(i));

    rc_file.close();

    cout << "Compiled " << out_file << " Successfully" << endl;

    s_line = "q";

    while(s_line.compare("q")!=0)
    {
        cout << "DEBUG: ";
        getline(cin, s_line);
        if(s_line.compare(".code")==0)
        {
            for(int i = 0; i < rc_code_offset; i++)
                cout << "CS[" << i << "]:" << (int)RC_CODE_SEGMENT[i] << endl;
        }
        else if(s_line.compare(".data")==0)
        {
            for(int i = 0; i < rc_data_offset; i++)
                cout << "DS[" << i << "]:" << (int)RC_DATA_SEGMENT[i] << endl;
        }
        else if(s_line.compare("max_m")==0)
            cout << "RC_MAX_M = " << max_m << endl;
        else if(s_line.compare("max_s")==0)
            cout << "RC_MAX_S = " << max_s << endl;
        else if(s_line.compare("num_stack_size")==0)
            cout << "NUM_STACK_SIZE = " << num_stack_size << endl;
        else if(s_line.compare("str_stack_size")==0)
            cout << "STR_STACK_SIZE = " << str_stack_size << endl;
        else if(s_line.compare("nid")==0)
            cout << "#NID = " << nid_count << endl;
        else if(s_line.compare("sid")==0)
            cout << "#SID = " << sid_count << endl;
        else if(s_line.compare("intern_id")==0)
        {
            string id = "";
            for(int i = 0; i < rc_id_count; i++)
            {
                id = rc_id[i].substr(0, rc_id[i].find_first_of(" "));
                if(rc_getID_data(id,"id_type").compare("#F")!=0 && rc_getID_data(id,"id_type").compare("$F")!=0)
                    cout << id << " -> " << rc_getID_data(id,"id_type") << " -> " << rc_getID_data(id,"id_index") << endl;
            }
        }
        else if(s_line.compare("intern_id#")==0)
        {
            string id = "";
            for(int i = 0; i < rc_id_count; i++)
            {
                id = rc_id[i].substr(0, rc_id[i].find_first_of(" "));
                if(rc_getID_data(id,"id_type").compare("#")==0 && rc_getID_data(id,"id_type").compare("#A")!=0)
                    cout << id << " -> " << rc_getID_data(id,"id_type") << " -> " << rc_getID_data(id,"id_index") << endl;
            }
        }
        else if(s_line.compare("intern_id$")==0)
        {
            string id = "";
            for(int i = 0; i < rc_id_count; i++)
            {
                id = rc_id[i].substr(0, rc_id[i].find_first_of(" "));
                if(rc_getID_data(id,"id_type").compare("$")==0 && rc_getID_data(id,"id_type").compare("$A")!=0)
                    cout << id << " -> " << rc_getID_data(id,"id_type") << " -> " << rc_getID_data(id,"id_index") << endl;
            }
        }
    }

    return 0;
}
