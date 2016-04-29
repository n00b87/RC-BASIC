#include <jni.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <stack>
#include <bitset>
#include <sys/stat.h>   //file system stuff
#include <sys/types.h>  //file system stuff
#include <unistd.h>   //file system stuff
#include <sys/param.h>
#include <dirent.h>
#include <algorithm>
#include "rc_media.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "native-activity", __VA_ARGS__))

#define RC_EQUAL_FLAG 1
#define RC_NOT_EQUAL_FLAG 2
#define RC_GREATER_FLAG 3
#define RC_LESS_FLAG 4
#define RC_GREATER_EQUAL_FLAG 5
#define RC_LESS_EQUAL_FLAG 6
#define RC_EXIT_CODE 7
#define RC_RETURN_ADDRESS 8

#define RC_MAX_FILES 32

#define RC_FILE_IO_TEXT_INPUT 1
#define RC_FILE_IO_TEXT_OUTPUT 2
#define RC_FILE_IO_TEXT_APPEND 3
#define RC_FILE_IO_TEXT_INPUT_UPDATE 4
#define RC_FILE_IO_TEXT_OUTPUT_UPDATE 5
#define RC_FILE_IO_TEXT_APPEND_UPDATE 6
#define RC_FILE_IO_BINARY_INPUT 7
#define RC_FILE_IO_BINARY_OUTPUT 8
#define RC_FILE_IO_BINARY_APPEND 9
#define RC_FILE_IO_BINARY_INPUT_UPDATE 10
#define RC_FILE_IO_BINARY_OUTPUT_UPDATE 11
#define RC_FILE_IO_BINARY_APPEND_UPDATE 12

#define FPRINT_S_STR 0
#define ARRAYDIMENSIONS_ARR_ID 0
#define ARRAYDIMENSIONS_ARR_ID_STR 1
#define ARRAYSIZE_ARR_ID 1
#define ARRAYSIZE_ARR_ID_STR 2
#define ARRAYSIZE_ARR_DIM 2
#define ASC_C_STR 3
#define CHR_STR_N 3
#define INSERT_STR_SRC_STR 4
#define INSERT_STR_TGT_STR 5
#define INSERT_STR_POS 4
#define INSTR_STRING_STR 6
#define INSTR_SUBSTR_STR 7
#define LCASE_STR_STRING_STR 8
#define LEFT_STR_STRING_STR 9
#define LEFT_STR_N 5
#define LENGTH_STRING_STR 10
#define LTRIM_STR_STRING_STR 11
#define MID_STR_STRING_STR 12
#define MID_STR_START 6
#define MID_STR_N 7
#define REPLACE_STR_SRC_STR 13
#define REPLACE_STR_RPC_STR 14
#define REPLACE_STR_POS 8
#define REPLACESUBSTR_STR_SRC_STR 15
#define REPLACESUBSTR_STR_TGT_STR 16
#define REPLACESUBSTR_STR_RPC_STR 17
#define REVERSE_STR_STRING_STR 18
#define RIGHT_STR_STRING_STR 19
#define RIGHT_STR_N 9
#define RTRIM_STR_STRING_STR 20
#define FILLSTRING_STR_STRING_STR 21
#define FILLSTRING_STR_N 10
#define STR_STR_VALUE 11
#define TALLY_STRING_STR 22
#define TALLY_SUBSTR_STR 23
#define TRIM_STR_STRING_STR 24
#define UCASE_STR_STRING_STR 25
#define VAL_STRING_STR 26
#define ABS_N 12
#define ACOS_N 13
#define ANDBIT_A 14
#define ANDBIT_B 15
#define ASIN_N 16
#define ATAN_N 17
#define BIN_N 18
#define COS_ANGLE 19
#define EXP_N 20
#define FRAC_N 21
#define HEX_STR_VALUE 22
#define INT_N 23
#define LOG_N 24
#define MAX_A 25
#define MAX_B 26
#define MIN_A 27
#define MIN_B 28
#define ORBIT_A 29
#define ORBIT_B 30
#define RANDOMIZE_N 31
#define RAND_N 32
#define ROUND_N 33
#define SIGN_N 34
#define SIN_ANGLE 35
#define SQR_N 36
#define TAN_ANGLE 37
#define XORBIT_A 38
#define XORBIT_B 39
#define FILEOPEN_STREAM 40
#define FILEOPEN_FILE_STR 27
#define FILEOPEN_IO_MODE 41
#define FILECLOSE_STREAM 42
#define READBYTE_STREAM 43
#define WRITEBYTE_STREAM 44
#define WRITEBYTE_BYTE 45
#define READLINE_STR_STREAM 46
#define WRITELINE_STREAM 47
#define WRITELINE_LINE_STR 28
#define COPYFILE_SRC_STR 29
#define COPYFILE_DEST_STR 30
#define REMOVEFILE_FILE_STR 31
#define FILEEXISTS_FILE_STR 32
#define MOVEFILE_SRC_STR 33
#define MOVEFILE_DEST_STR 34
#define RENAMEFILE_FILE_STR 35
#define RENAMEFILE_RENAME_STR 36
#define FILELENGTH_FILE_STR 37
#define TELL_STREAM 48
#define SEEK_STREAM 49
#define SEEK_POSITION 50
#define EOF_STREAM 51
#define CHANGEDIR_PATH_STR 38
#define DIREXISTS_PATH_STR 39
#define MAKEDIR_PATH_STR 40
#define REMOVEDIR_PATH_STR 41
#define WAIT_MSEC 52
#define INPUT_STR_PROMPT 42
#define WINDOWOPEN_WIN_NUM 53
#define WINDOWOPEN_TITLE_STR 43
#define WINDOWOPEN_X 54
#define WINDOWOPEN_Y 55
#define WINDOWOPEN_W 56
#define WINDOWOPEN_H 57
#define WINDOWOPEN_MODE 58
#define WINDOWCLOSE_WIN_NUM 59
#define WINDOW_WIN_NUM 60
#define SETWINDOWTITLE_TITLE_STR 44
#define SETWINDOWPOSITION_X 61
#define SETWINDOWPOSITION_Y 62
#define GETWINDOWPOSITION_X 63
#define GETWINDOWPOSITION_Y 64
#define SETWINDOWSIZE_W 65
#define SETWINDOWSIZE_H 66
#define GETWINDOWSIZE_W 67
#define GETWINDOWSIZE_H 68
#define SETWINDOWMINSIZE_W 69
#define SETWINDOWMINSIZE_H 70
#define SETWINDOWMAXSIZE_W 71
#define SETWINDOWMAXSIZE_H 72
#define GETWINDOWMINSIZE_W 73
#define GETWINDOWMINSIZE_H 74
#define GETWINDOWMAXSIZE_W 75
#define GETWINDOWMAXSIZE_H 76
#define SETWINDOWFULLSCREEN_FLAG 77
#define SETWINDOWBORDER_FLAG 78
#define CANVASOPEN_S_NUM 79
#define CANVASOPEN_W 80
#define CANVASOPEN_H 81
#define CANVASOPEN_VPX 82
#define CANVASOPEN_VPY 83
#define CANVASOPEN_VPW 84
#define CANVASOPEN_VPH 85
#define CANVASOPEN_FLAG 86
#define CANVASCLOSE_S_NUM 87
#define SETCANVASVISIBLE_S_NUM 88
#define SETCANVASVISIBLE_FLAG 89
#define CANVASISVISIBLE_S_NUM 90
#define SETCANVASVIEWPORT_X 91
#define SETCANVASVIEWPORT_Y 92
#define SETCANVASVIEWPORT_W 93
#define SETCANVASVIEWPORT_H 94
#define GETCANVASVIEWPORT_X 95
#define GETCANVASVIEWPORT_Y 96
#define GETCANVASVIEWPORT_W 97
#define GETCANVASVIEWPORT_H 98
#define CANVAS_S_NUM 99
#define SETCANVASOFFSET_X 100
#define SETCANVASOFFSET_Y 101
#define GETCANVASOFFSET_X 102
#define GETCANVASOFFSET_Y 103
#define GETCANVASSIZE_W 104
#define GETCANVASSIZE_H 105
#define SETCANVASALPHA_ALPHA 106
#define COPYCANVAS_SRC_SCREEN 107
#define COPYCANVAS_X 108
#define COPYCANVAS_Y 109
#define COPYCANVAS_W 110
#define COPYCANVAS_H 111
#define COPYCANVAS_DST_SCREEN 112
#define COPYCANVAS_DST_X 113
#define COPYCANVAS_DST_Y 114
#define CLONECANVAS_SRC_SCREEN 115
#define CLONECANVAS_DST_SCREEN 116
#define SETCANVASZ_Z 117
#define BOX_X1 118
#define BOX_Y1 119
#define BOX_X2 120
#define BOX_Y2 121
#define BOXFILL_X1 122
#define BOXFILL_Y1 123
#define BOXFILL_X2 124
#define BOXFILL_Y2 125
#define CIRCLE_X 126
#define CIRCLE_Y 127
#define CIRCLE_RADIUS 128
#define CIRCLEFILL_X 129
#define CIRCLEFILL_Y 130
#define CIRCLEFILL_RADIUS 131
#define ELLIPSE_X 132
#define ELLIPSE_Y 133
#define ELLIPSE_RADIUS_X 134
#define ELLIPSE_RADIUS_Y 135
#define ELLIPSEFILL_X 136
#define ELLIPSEFILL_Y 137
#define ELLIPSEFILL_RADIUS_X 138
#define ELLIPSEFILL_RADIUS_Y 139
#define GETPIXEL_X 140
#define GETPIXEL_Y 141
#define SETCOLOR_COLOR 142
#define LINE_X1 143
#define LINE_Y1 144
#define LINE_X2 145
#define LINE_Y2 146
#define FLOODFILL_X 147
#define FLOODFILL_Y 148
#define RGB_R 149
#define RGB_G 150
#define RGB_B 151
#define RGBA_R 152
#define RGBA_G 153
#define RGBA_B 154
#define RGBA_A 155
#define PIXEL_X 156
#define PIXEL_Y 157
#define CREATEIMAGE_SLOT 158
#define CREATEIMAGE_W 159
#define CREATEIMAGE_H 160
#define CREATEIMAGE_PDATA 161
#define CREATEIMAGE_EX_SLOT 162
#define CREATEIMAGE_EX_W 163
#define CREATEIMAGE_EX_H 164
#define CREATEIMAGE_EX_PDATA 165
#define CREATEIMAGE_EX_COLKEY 166
#define LOADIMAGE_SLOT 167
#define LOADIMAGE_IMG_FILE_STR 45
#define COLORKEY_SLOT 168
#define COLORKEY_COLOR 169
#define COPYIMAGE_SRC_SLOT 170
#define COPYIMAGE_DST_SLOT 171
#define DELETEIMAGE_SLOT 172
#define SETIMAGEALPHA_SLOT 173
#define SETIMAGEALPHA_ALPHA 174
#define GETIMAGEALPHA_SLOT 175
#define GETIMAGEALPHA_ALPHA 176
#define GETIMAGESIZE_SLOT 177
#define GETIMAGESIZE_W 178
#define GETIMAGESIZE_H 179
#define FLIPIMAGE_SLOT 180
#define FLIPIMAGE_FLIP_H 181
#define FLIPIMAGE_FLIP_V 182
#define DRAWIMAGE_SLOT 183
#define DRAWIMAGE_X 184
#define DRAWIMAGE_Y 185
#define DRAWIMAGE_CLIP_SLOT 186
#define DRAWIMAGE_CLIP_X 187
#define DRAWIMAGE_CLIP_Y 188
#define DRAWIMAGE_CLIP_SRC_X 189
#define DRAWIMAGE_CLIP_SRC_Y 190
#define DRAWIMAGE_CLIP_SRC_W 191
#define DRAWIMAGE_CLIP_SRC_H 192
#define DRAWIMAGE_ROTATE_SLOT 193
#define DRAWIMAGE_ROTATE_X 194
#define DRAWIMAGE_ROTATE_Y 195
#define DRAWIMAGE_ROTATE_ANGLE 196
#define DRAWIMAGE_ROTATE_EX_SLOT 197
#define DRAWIMAGE_ROTATE_EX_X 198
#define DRAWIMAGE_ROTATE_EX_Y 199
#define DRAWIMAGE_ROTATE_EX_SRC_X 200
#define DRAWIMAGE_ROTATE_EX_SRC_Y 201
#define DRAWIMAGE_ROTATE_EX_SRC_W 202
#define DRAWIMAGE_ROTATE_EX_SRC_H 203
#define DRAWIMAGE_ROTATE_EX_ANGLE 204
#define DRAWIMAGE_ZOOM_SLOT 205
#define DRAWIMAGE_ZOOM_X 206
#define DRAWIMAGE_ZOOM_Y 207
#define DRAWIMAGE_ZOOM_ZOOM_X 208
#define DRAWIMAGE_ZOOM_ZOOM_Y 209
#define DRAWIMAGE_ZOOM_EX_SLOT 210
#define DRAWIMAGE_ZOOM_EX_X 211
#define DRAWIMAGE_ZOOM_EX_Y 212
#define DRAWIMAGE_ZOOM_EX_SRC_X 213
#define DRAWIMAGE_ZOOM_EX_SRC_Y 214
#define DRAWIMAGE_ZOOM_EX_SRC_W 215
#define DRAWIMAGE_ZOOM_EX_SRC_H 216
#define DRAWIMAGE_ZOOM_EX_ZOOM_X 217
#define DRAWIMAGE_ZOOM_EX_ZOOM_Y 218
#define DRAWIMAGE_ROTOZOOM_SLOT 219
#define DRAWIMAGE_ROTOZOOM_X 220
#define DRAWIMAGE_ROTOZOOM_Y 221
#define DRAWIMAGE_ROTOZOOM_ANGLE 222
#define DRAWIMAGE_ROTOZOOM_ZOOM_X 223
#define DRAWIMAGE_ROTOZOOM_ZOOM_Y 224
#define DRAWIMAGE_ROTOZOOM_EX_SLOT 225
#define DRAWIMAGE_ROTOZOOM_EX_X 226
#define DRAWIMAGE_ROTOZOOM_EX_Y 227
#define DRAWIMAGE_ROTOZOOM_EX_SRC_X 228
#define DRAWIMAGE_ROTOZOOM_EX_SRC_Y 229
#define DRAWIMAGE_ROTOZOOM_EX_SRC_W 230
#define DRAWIMAGE_ROTOZOOM_EX_SRC_H 231
#define DRAWIMAGE_ROTOZOOM_EX_ANGLE 232
#define DRAWIMAGE_ROTOZOOM_EX_ZOOM_X 233
#define DRAWIMAGE_ROTOZOOM_EX_ZOOM_Y 234
#define DRAWIMAGE_BLIT_SLOT 235
#define DRAWIMAGE_BLIT_POS_X 236
#define DRAWIMAGE_BLIT_POS_Y 237
#define DRAWIMAGE_BLIT_POS_W 238
#define DRAWIMAGE_BLIT_POS_H 239
#define DRAWIMAGE_BLIT_SRC_X 240
#define DRAWIMAGE_BLIT_SRC_Y 241
#define DRAWIMAGE_BLIT_SRC_W 242
#define DRAWIMAGE_BLIT_SRC_H 243
#define GETCURSOR_X 244
#define GETCURSOR_Y 245
#define PRINTS_TXT_STR 46
#define INPUTS_STR_PROMPT_STR 47
#define LOCATE_X 246
#define LOCATE_Y 247
#define FONTISLOADED_F_SLOT 248
#define FONT_F_SLOT 249
#define LOADFONT_F_SLOT 250
#define LOADFONT_FNT_FILE_STR 48
#define LOADFONT_SIZE 251
#define DELETEFONT_F_SLOT 252
#define SETFONTSTYLE_F_SLOT 253
#define SETFONTSTYLE_STYLE 254
#define DRAWTEXT_TXT_STR 49
#define DRAWTEXT_X 255
#define DRAWTEXT_Y 256
#define DRAWTEXT_SHADED_TXT_STR 50
#define DRAWTEXT_SHADED_X 257
#define DRAWTEXT_SHADED_Y 258
#define DRAWTEXT_SHADED_FG_COLOR 259
#define DRAWTEXT_SHADED_BG_COLOR 260
#define DRAWTEXT_BLENDED_TXT_STR 51
#define DRAWTEXT_BLENDED_X 261
#define DRAWTEXT_BLENDED_Y 262
#define RENDERTEXT_SLOT 263
#define RENDERTEXT_TXT_STR 52
#define GETTEXTSIZE_F_SLOT 264
#define GETTEXTSIZE_TXT_STR 53
#define GETTEXTSIZE_W 265
#define GETTEXTSIZE_H 266
#define KEY_KEY_CODE 267
#define GETMOUSE_X 268
#define GETMOUSE_Y 269
#define GETMOUSE_MB1 270
#define GETMOUSE_MB2 271
#define GETMOUSE_MB3 272
#define GETMOUSEWHEEL_X_AXIS 273
#define GETMOUSEWHEEL_Y_AXIS 274
#define IMAGEEXISTS_SLOT 275
#define JOYAXIS_JOY_NUM 276
#define JOYAXIS_AXIS 277
#define JOYBUTTON_JOY_NUM 278
#define JOYBUTTON_JOY_BUTTON 279
#define JOYNAME_STR_JOY_NUM 280
#define LOADSOUND_SLOT 281
#define LOADSOUND_SOUND_FILE_STR 54
#define LOADMUSIC_MUSIC_FILE_STR 55
#define PLAYSOUND_SLOT 282
#define PLAYSOUND_CHANNEL 283
#define PLAYSOUND_LOOPS 284
#define PLAYSOUNDTIMED_SLOT 285
#define PLAYSOUNDTIMED_CHANNEL 286
#define PLAYSOUNDTIMED_LOOPS 287
#define PLAYSOUNDTIMED_MS 288
#define PLAYMUSIC_LOOPS 289
#define PAUSESOUND_CHANNEL 290
#define RESUMESOUND_CHANNEL 291
#define DELETESOUND_SLOT 292
#define FADEMUSICIN_FADE_TIME 293
#define FADEMUSICIN_LOOPS 294
#define FADEMUSICOUT_FADE_TIME 295
#define SETMUSICVOLUME_VOLUME 296
#define SETMUSICPOSITION_MUSIC_POS 297
#define SETSOUNDCHANNELS_MAX_CHANNELS 298
#define SOUNDEXISTS_SLOT 299
#define SETCHANNELVOLUME_CHANNEL 300
#define SETCHANNELVOLUME_VOLUME 301
#define CHANNELVOLUME_CHANNEL 302
#define SETSOUNDVOLUME_SLOT 303
#define SETSOUNDVOLUME_VOLUME 304
#define SOUNDVOLUME_SLOT 305
#define STOPSOUND_CHANNEL 306
#define TCP_SOCKETOPEN_SOCKET 307
#define TCP_SOCKETOPEN_HOST_STR 56
#define TCP_SOCKETOPEN_PORT 308
#define TCP_SOCKETCLOSE_SOCKET 309
#define TCP_REMOTEHOST_SOCKET 310
#define TCP_REMOTEPORT_SOCKET 311
#define TCP_GETDATA_SOCKET 312
#define TCP_GETDATA_DST_STR 57
#define TCP_GETDATA_NUMBYTES 313
#define TCP_SENDDATA_SOCKET 314
#define TCP_SENDDATA_S_DATA_STR 58
#define TCP_ACCEPTSOCKET_SOCKET_SERVER 315
#define TCP_ACCEPTSOCKET_SOCKET_CLIENT 316
#define UDP_SOCKETOPEN_SOCKET 317
#define UDP_SOCKETOPEN_PORT 318
#define UDP_GETDATA_SOCKET 319
#define UDP_GETDATA_DST_STR 59
#define UDP_GETDATA_HOST_STR 60
#define UDP_GETDATA_PORT 320
#define UDP_REMOTEHOST_STR_SOCKET 321
#define UDP_REMOTEPORT_SOCKET 322
#define UDP_SOCKETCLOSE_SOCKET 323
#define UDP_SENDDATA_SOCKET 324
#define UDP_SENDDATA_S_DATA_STR 61
#define UDP_SENDDATA_HOST_STR 62
#define UDP_SENDDATA_PORT 325
#define GETTOUCH_STATUS 326
#define GETTOUCH_X 327
#define GETTOUCH_Y 328
#define GETTOUCH_DX 329
#define GETTOUCH_DY 330
#define GETMULTITOUCH_STATUS 331
#define GETMULTITOUCH_X 332
#define GETMULTITOUCH_Y 333
#define GETMULTITOUCH_NUMFINGERS 334
#define GETMULTITOUCH_DIST 335
#define GETMULTITOUCH_THETA 336
#define ZONEINPUT_STR_X 337
#define ZONEINPUT_STR_Y 338
#define ZONEINPUT_STR_W 339
#define ZONEINPUT_STR_H 340
#define CANVASCLIP_SLOT 341
#define CANVASCLIP_SRC_X 342
#define CANVASCLIP_SRC_Y 343
#define CANVASCLIP_SRC_W 344
#define CANVASCLIP_SRC_H 345
#define WINDOWCLIP_SLOT 346
#define WINDOWCLIP_SRC_X 347
#define WINDOWCLIP_SRC_Y 348
#define WINDOWCLIP_SRC_W 349
#define WINDOWCLIP_SRC_H 350
#define READINPUT_SETTEXT_TXT_STR 63
#define GETTOUCHFINGER_FINGER 351
#define GETTOUCHFINGER_X 352
#define GETTOUCHFINGER_Y 353
#define GETTOUCHFINGER_PRESSURE 354
#define LOADIMAGE_EX_SLOT 355
#define LOADIMAGE_EX_IMG_FILE_STR 64
#define LOADIMAGE_EX_COLKEY 356
#define RECT_X 357
#define RECT_Y 358
#define RECT_W 359
#define RECT_H 360
#define RECTFILL_X 361
#define RECTFILL_Y 362
#define RECTFILL_W 363
#define RECTFILL_H 364
#define NUMJOYBUTTONS_JOY_NUM 365
#define NUMJOYAXES_JOY_NUM 366
#define LOADVIDEO_VID_FILE_STR 65
#define PLAYVIDEO_VLOOPS 367
#define SETVIDEOPOSITION_VPOS 368
#define SYSTEM_SYS_CMD_STR 66
#define ROUNDRECT_X 369
#define ROUNDRECT_Y 370
#define ROUNDRECT_W 371
#define ROUNDRECT_H 372
#define ROUNDRECT_R 373
#define ROUNDRECTFILL_X 374
#define ROUNDRECTFILL_Y 375
#define ROUNDRECTFILL_W 376
#define ROUNDRECTFILL_H 377
#define ROUNDRECTFILL_R 378
#define GETVIDEOSTATS_VFILE_STR 67
#define GETVIDEOSTATS_VLENGTH 379
#define GETVIDEOSTATS_VFPS 380
#define GETVIDEOSTATS_W 381
#define GETVIDEOSTATS_H 382
#define SETVIDEODRAWRECT_X 383
#define SETVIDEODRAWRECT_Y 384
#define SETVIDEODRAWRECT_W 385
#define SETVIDEODRAWRECT_H 386
#define GETVIDEODRAWRECT_X 387
#define GETVIDEODRAWRECT_Y 388
#define GETVIDEODRAWRECT_W 389
#define GETVIDEODRAWRECT_H 390
#define GETVIDEOSIZE_W 391
#define GETVIDEOSIZE_H 392
#define SETVIDEOALPHA_V_ALPHA 393
#define CREATESOUND_SLOT 394
#define CREATESOUND_ABUF 395
#define CREATESOUND_ALEN 396
#define CREATESOUND_AVOL 397
#define COMMAND_STR_ARG 398
#define STR_F_STR_N 399
#define STR_S_STR_N 400
#define ENV_STR_V_STR 68
#define SETENV_V_STR 69
#define PREFPATH_STR_ORG_NAME_STR 70
#define PREFPATH_STR_APP_NAME_STR 71

#define RC_LOOP_BREAK 50

using namespace std;

Uint64 rc_code_segment_size = 0;
Uint64 rc_data_segment_size = 0;
Uint64 rc_nid_size = 0;
Uint64 rc_sid_size = 0;
Uint64 rc_max_m = 0;
Uint64 rc_max_s = 0;
Uint64 rc_max_f = 0;
Uint64 rc_num_stack_size = 0;
Uint64 rc_str_stack_size = 0;

unsigned char * RC_SEGMENT[2];

double * rc_vm_m;
string * rc_vm_s;
double * rc_vm_f;

double ** rc_nid;
string ** rc_sid;

Uint64 * rc_nid_w;
Uint64 * rc_nid_h;
Uint64 * rc_nid_d;

Uint64 * rc_sid_w;
Uint64 * rc_sid_h;
Uint64 * rc_sid_d;

Uint64 * rc_byref_nid;
Uint64 * rc_byref_sid;
Uint64 rc_byref_nid_count = 0;
Uint64 rc_byref_sid_count = 0;

double * rc_num_stack;
string * rc_str_stack;

SDL_RWops * rc_fstream[16];

stack<Uint64> rtn_address_stack;
stack<double> rc_vm_stack;

Uint64 rc_num_stack_index = 0;
Uint64 rc_str_stack_index = 0;

const int RC_CODE_SEGMENT = 0;
const int RC_DATA_SEGMENT = 1;

Uint64 RC_CURRENT_SEGMENT = 0;
Uint64 RC_CURRENT_ADDRESS = 0;

int RC_VM_ACTIVE = 1;

unsigned int rc_vm_system_id[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

unsigned char RC_PTR_TYPE = 0;

struct dirent *rc_entry;
DIR *rc_dir;
string rc_dir_path = "";

bool is_wend = false;
bool loop_exit = false;
int exit_arg = 0;

unsigned int w_condition = 0;

int vm_data_size = 8;

bool rc_vm_isClean = false;

int rc_cmd_count = 0;
string * rc_cmd_args;

bool rc_eof[RC_MAX_FILES];

union rc_int
{
    int64_t i;
    char data[sizeof(int64_t)];
};

union rc_double
{
    double f;
    char data[sizeof(double)];
};

union rc_uint
{
    Uint64 ui;
    char data[sizeof(Uint64)];
};

union rc_ushort
{
    Uint16 s;
    char data[sizeof(Uint16)];
};

inline char rc_segment_getByte(int64_t rcs_segment, Uint64 rcs_addr)
{
    return RC_SEGMENT[rcs_segment][rcs_addr];
}

inline unsigned char rc_segment_getUByte(int64_t rcs_segment, Uint64 rcs_addr)
{
    return RC_SEGMENT[rcs_segment][rcs_addr];
}

inline double rc_segment_getdouble(int64_t rcs_segment, Uint64 rcs_addr)
{
    rc_double rf;
    for(int i = 0; i < sizeof(double); i++)
        rf.data[i] = RC_SEGMENT[rcs_segment][rcs_addr+i];
    return rf.f;
}

inline int64_t rc_segment_getInt(int64_t rcs_segment, Uint64 rcs_addr)
{
    rc_int ri;
    for(int i = 0; i < sizeof(int64_t); i++)
        ri.data[i] = RC_SEGMENT[rcs_segment][rcs_addr+i];
    return ri.i;
}

inline Uint64 rc_segment_getUInt(int64_t rcs_segment, Uint64 rcs_addr)
{
    rc_uint ru;
    for(int i = 0; i < sizeof(Uint64); i++)
        ru.data[i] = RC_SEGMENT[rcs_segment][rcs_addr+i];
    return ru.ui;
}

inline Uint16 rc_segment_getUShort(int64_t rcs_segment, Uint64 rcs_addr)
{
    rc_ushort rs;
    for(int i = 0; i < sizeof(Uint16); i++)
        rs.data[i] = RC_SEGMENT[rcs_segment][rcs_addr+i];
    return rs.s;
}

inline string rc_segment_getString(unsigned int rcs_addr)
{
    unsigned int i = rcs_addr;
    string s = "";
    while(RC_SEGMENT[RC_DATA_SEGMENT][i] != 0)
    {
        s = s + (char)RC_SEGMENT[RC_DATA_SEGMENT][i];
        i++;
    }
    return s;
}

string rc_vm_getSysID_Name(unsigned int sys_id)
{
    switch(sys_id)
    {
        case 1: return "EQUAL_FLAG";
        case 2: return "NOT_EQUAL_FLAG";
        case 3: return "GREATER_FLAG";
        case 4: return "LESS_FLAG";
        case 5: return "GREATER_EQUAL_FLAG";
        case 6: return "LESS_EQUAL_FLAG";
        case 7: return "EXIT_CODE";
        case 8: return "RETURN_ADDRESS";
    }
    return "";
}

inline void rc_vm_setReturnAddress(Uint64 ra)
{
    rtn_address_stack.push(ra);
    //cout << "RTN = " << rtn_address_stack.size() << endl;
}

inline void rc_vm_return()
{
    unsigned int ra_size = rtn_address_stack.size();
    if(ra_size > 0)
    {
        RC_CURRENT_ADDRESS = rtn_address_stack.top();
        rtn_address_stack.pop();
        if(ra_size == 1)
            RC_CURRENT_SEGMENT = RC_CODE_SEGMENT;
    }
    else
        RC_CURRENT_ADDRESS += 2;
}

inline void rc_vm_setSysFlags(double a, double b)
{
    rc_vm_system_id[1] = (a == b);
    rc_vm_system_id[2] = (a != b);
    rc_vm_system_id[3] = (a > b);
    rc_vm_system_id[4] = (a < b);
    rc_vm_system_id[5] = (a >= b);
    rc_vm_system_id[6] = (a <= b);
}

inline void rc_vm_setSysFlags(string a, string b)
{
    rc_vm_system_id[1] = !(a.compare(b));
    rc_vm_system_id[2] = !rc_vm_system_id[1];
    //cout << "\n" << a << " NOT " << b << " = " << rc_vm_system_id[2] << endl;
}

inline void rc_vm_mov()
{
    //cout << "<mov>\n";
    double raw_num;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = raw_num;
                    //cout << "#M[" << i_val1 << "] = " << raw_num << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val2];
                    //cout << "#M[" << i_val1 << "] = #M[" << i_val2 << "]\n";
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_nid[i_val2][0];
                    //cout << "#M[" << i_val1 << "] = NUM_ID[" << i_val2 << "]\n";
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_system_id[i_val2];
                    //cout << "#M[" << i_val1 << "] = %" << rc_vm_getSysID_Name(i_val2) << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_f[i_val2];
                    //cout << "#M[" << i_val1 << "] = #M[" << i_val2 << "]\n";
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = raw_num;
                    //cout << "NUM_ID[" << i_val1 << "] = " << raw_num << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_vm_m[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = #M[" << i_val2 << "]\n";
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val2][0];
                    //cout << "NUM_ID[" << i_val1 << "] = NUM_ID[" << i_val2 << "]\n";
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_vm_system_id[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = %" << rc_vm_getSysID_Name(i_val2) << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_vm_f[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = #M[" << i_val2 << "]\n";
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 9:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = raw_num;
                    //cout << "#M[" << i_val1 << "] = " << raw_num << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_m[i_val2];
                    //cout << "#M[" << i_val1 << "] = #M[" << i_val2 << "]\n";
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_nid[i_val2][0];
                    //cout << "#M[" << i_val1 << "] = NUM_ID[" << i_val2 << "]\n";
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_system_id[i_val2];
                    //cout << "#M[" << i_val1 << "] = %" << rc_vm_getSysID_Name(i_val2) << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val2];
                    //cout << "#M[" << i_val1 << "] = #M[" << i_val2 << "]\n";
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_mov_str()
{
    string raw_string;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 3:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 2:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_s[i_val1] = rc_segment_getString(i_val2);
                    //cout << "#S[" << i_val1 << "] = @" << i_val2 << " = " << rc_vm_s[i_val1] << endl;
                    return;
                case 3:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_s[i_val1] = rc_vm_s[i_val2];
                    //cout << "#S[" << i_val1 << "] = #S[" << i_val2 << "]\n";
                    return;
                case 5:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_s[i_val1] = rc_sid[i_val2][0];
                    //cout << "#S[" << i_val1 << "] = STR_ID[" << i_val2 << "]\n";
                    return;

                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 5:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 2:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_sid[i_val1][0] = rc_segment_getString(i_val2);
                    //cout << "STR_ID[" << i_val1 << "] = " << rc_vm_s[i_val1] << endl;
                    return;
                case 3:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_sid[i_val1][0] = rc_vm_s[i_val2];
                    //cout << "STR_ID[" << i_val1 << "] = #S[" << i_val2 << "]\n";
                    return;
                case 5:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_sid[i_val1][0] = rc_sid[i_val2][0];
                    //cout << "STR_ID[" << i_val1 << "] = STR_ID[" << i_val2 << "]\n";
                    return;

                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }

        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_pwr()
{
    double raw_num;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = pow(rc_vm_m[i_val1], raw_num);
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = pow(rc_vm_m[i_val1], rc_vm_m[i_val2]);
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = pow(rc_vm_m[i_val1], rc_nid[i_val2][0]);
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = SDL_pow(rc_vm_m[i_val1], rc_vm_system_id[i_val2]);
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = pow(rc_vm_m[i_val1], rc_vm_f[i_val2]);
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = pow(rc_nid[i_val1][0], raw_num);
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = pow(rc_nid[i_val1][0], rc_vm_m[i_val2]);
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = pow(rc_nid[i_val1][0], rc_nid[i_val2][0]);
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = SDL_pow(rc_nid[i_val1][0], rc_vm_system_id[i_val2]);
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = pow(rc_nid[i_val1][0], rc_vm_f[i_val2]);
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 9:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = pow(rc_vm_f[i_val1], raw_num);
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = pow(rc_vm_f[i_val1], rc_vm_m[i_val2]);
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = pow(rc_vm_f[i_val1], rc_nid[i_val2][0]);
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = SDL_pow(rc_vm_f[i_val1], rc_vm_system_id[i_val2]);
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = pow(rc_vm_f[i_val1], rc_vm_f[i_val2]);
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_mul()
{
    double raw_num;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] * raw_num;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] * rc_vm_m[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] * rc_nid[i_val2][0];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] * rc_vm_system_id[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] * rc_vm_f[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] * raw_num;
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] * rc_vm_m[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] * rc_nid[i_val2][0];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] * rc_vm_system_id[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] * rc_vm_f[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 9:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] * raw_num;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] * rc_vm_m[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] * rc_nid[i_val2][0];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] * rc_vm_system_id[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] * rc_vm_f[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_div()
{
    double raw_num;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] / raw_num;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] / rc_vm_m[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] / rc_nid[i_val2][0];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] / rc_vm_system_id[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] / rc_vm_f[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] / raw_num;
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] / rc_vm_m[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] / rc_nid[i_val2][0];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] / rc_vm_system_id[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] / rc_vm_f[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 9:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] / raw_num;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] / rc_vm_m[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] / rc_nid[i_val2][0];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] / rc_vm_system_id[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] / rc_vm_f[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_mod()
{
    double raw_num;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = (int)rc_vm_m[i_val1] % (int)raw_num;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = (int)rc_vm_m[i_val1] % (int)rc_vm_m[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = (int)rc_vm_m[i_val1] % (int)rc_nid[i_val2][0];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = (int)rc_vm_m[i_val1] % (int)rc_vm_system_id[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = (int)rc_vm_m[i_val1] % (int)rc_vm_f[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = (int)rc_nid[i_val1][0] % (int)raw_num;
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = (int)rc_nid[i_val1][0] % (int)rc_vm_m[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = (int)rc_nid[i_val1][0] % (int)rc_nid[i_val2][0];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = (int)rc_nid[i_val1][0] % (int)rc_vm_system_id[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = (int)rc_nid[i_val1][0] % (int)rc_vm_f[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 9:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = (int)rc_vm_f[i_val1] % (int)raw_num;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = (int)rc_vm_f[i_val1] % (int)rc_vm_m[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = (int)rc_vm_f[i_val1] % (int)rc_nid[i_val2][0];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = (int)rc_vm_f[i_val1] % (int)rc_vm_system_id[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = (int)rc_vm_f[i_val1] % (int)rc_vm_f[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_add()
{
    //cout << "<add>\n";
    double raw_num;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] + raw_num;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] + rc_vm_m[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] + rc_nid[i_val2][0];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] + rc_vm_system_id[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] + rc_vm_f[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] + raw_num;
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] + rc_vm_m[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] + rc_nid[i_val2][0];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] + rc_vm_system_id[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] + rc_vm_f[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 9:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] + raw_num;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] + rc_vm_m[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] + rc_nid[i_val2][0];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] + rc_vm_system_id[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] + rc_vm_f[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_add_str()
{
    //cout << "add_str\n";
    double raw_num;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 3:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 2:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_s[i_val1] = rc_vm_s[i_val1] + rc_segment_getString(i_val2);
                    //cout << "#S[" << i_val1 << "] = " << rc_vm_s[i_val1] << endl;
                    return;
                case 3:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_s[i_val1] = rc_vm_s[i_val1] + rc_vm_s[i_val2];
                    //cout << "#S[" << i_val1 << "] = " << rc_vm_s[i_val1] << endl;
                    return;
                case 5:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_s[i_val1] = rc_vm_s[i_val1] + rc_sid[i_val2][0];
                    //cout << "#S[" << i_val1 << "] = " << rc_vm_s[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 5:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 2:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_sid[i_val1][0] = rc_sid[i_val1][0] + rc_segment_getString(i_val2);
                    //cout << "#STR_ID[" << i_val1 << "] = " << rc_sid[i_val1][0] << endl;
                    return;
                case 3:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_sid[i_val1][0] = rc_sid[i_val1][0] + rc_vm_s[i_val2];
                    //cout << "#STR_ID[" << i_val1 << "] = " << rc_sid[i_val1][0] << endl;
                    return;
                case 5:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_sid[i_val1][0] = rc_sid[i_val1][0] + rc_sid[i_val2][0];
                    //cout << "#STR_ID[" << i_val1 << "] = " << rc_sid[i_val1][0] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_sub()
{
    //cout << "<sub>\n";
    double raw_num;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] - raw_num;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] - rc_vm_m[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] - rc_nid[i_val2][0];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] - rc_vm_system_id[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] - rc_vm_f[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] - raw_num;
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] - rc_vm_m[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] - rc_nid[i_val2][0];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] - rc_vm_system_id[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] - rc_vm_f[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 9:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] - raw_num;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] - rc_vm_m[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] - rc_nid[i_val2][0];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] - rc_vm_system_id[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] - rc_vm_f[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_abs_sub()
{
    //cout << "<sub>\n";
    double raw_num;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] - raw_num;
                    if(rc_vm_m[i_val1] < 0)
                        rc_vm_m[i_val1] *= -1;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] - rc_vm_m[i_val2];
                    if(rc_vm_m[i_val1] < 0)
                        rc_vm_m[i_val1] *= -1;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] - rc_nid[i_val2][0];
                    if(rc_vm_m[i_val1] < 0)
                        rc_vm_m[i_val1] *= -1;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] - rc_vm_system_id[i_val2];
                    if(rc_vm_m[i_val1] < 0)
                        rc_vm_m[i_val1] *= -1;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] - rc_vm_f[i_val2];
                    if(rc_vm_m[i_val1] < 0)
                        rc_vm_m[i_val1] *= -1;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] - raw_num;
                    if(rc_vm_m[i_val1] < 0)
                        rc_vm_m[i_val1] *= -1;
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] - rc_vm_m[i_val2];
                    if(rc_vm_m[i_val1] < 0)
                        rc_vm_m[i_val1] *= -1;
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] - rc_nid[i_val2][0];
                    if(rc_vm_m[i_val1] < 0)
                        rc_vm_m[i_val1] *= -1;
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] - rc_vm_system_id[i_val2];
                    if(rc_vm_m[i_val1] < 0)
                        rc_vm_m[i_val1] *= -1;
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] - rc_vm_f[i_val2];
                    if(rc_vm_m[i_val1] < 0)
                        rc_vm_m[i_val1] *= -1;
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 9:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] - raw_num;
                    if(rc_vm_m[i_val1] < 0)
                        rc_vm_m[i_val1] *= -1;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] - rc_vm_m[i_val2];
                    if(rc_vm_m[i_val1] < 0)
                        rc_vm_m[i_val1] *= -1;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] - rc_nid[i_val2][0];
                    if(rc_vm_m[i_val1] < 0)
                        rc_vm_m[i_val1] *= -1;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] - rc_vm_system_id[i_val2];
                    if(rc_vm_m[i_val1] < 0)
                        rc_vm_m[i_val1] *= -1;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_f[i_val1] = rc_vm_f[i_val1] - rc_vm_f[i_val2];
                    if(rc_vm_m[i_val1] < 0)
                        rc_vm_m[i_val1] *= -1;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_shl()
{
    double raw_num;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = (int)rc_vm_m[i_val1] << (int)raw_num;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = (int)rc_vm_m[i_val1] << (int)rc_vm_m[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = (int)rc_vm_m[i_val1] << (int)rc_nid[i_val2][0];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = (int)rc_vm_m[i_val1] << (int)rc_vm_system_id[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = (int)rc_nid[i_val1][0] << (int)raw_num;
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = (int)rc_nid[i_val1][0] << (int)rc_vm_m[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = (int)rc_nid[i_val1][0] << (int)rc_nid[i_val2][0];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = (int)rc_nid[i_val1][0] << (int)rc_vm_system_id[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_shr()
{
    double raw_num;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = (int)rc_vm_m[i_val1] >> (int)raw_num;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = (int)rc_vm_m[i_val1] >> (int)rc_vm_m[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = (int)rc_vm_m[i_val1] >> (int)rc_nid[i_val2][0];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = (int)rc_vm_m[i_val1] >> (int)rc_vm_system_id[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = (int)rc_nid[i_val1][0] >> (int)raw_num;
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = (int)rc_nid[i_val1][0] >> (int)rc_vm_m[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = (int)rc_nid[i_val1][0] >> (int)rc_nid[i_val2][0];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = (int)rc_nid[i_val1][0] >> (int)rc_vm_system_id[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

int rc_debug_outputFlags()
{
    cout << "EQUAL_FLAG = " << rc_vm_system_id[1] << endl;
    cout << "NOT_EQUAL_FLAG = " << rc_vm_system_id[2] << endl;
    cout << "GREATER_FLAG = " << rc_vm_system_id[3] << endl;
    cout << "LESS_FLAG = " << rc_vm_system_id[4] << endl;
    cout << "GREATER_EQUAL_FLAG = " << rc_vm_system_id[5] << endl;
    cout << "LESS_EQUAL_FLAG = " << rc_vm_system_id[6] << endl;
    return 0;
}

inline void rc_vm_cmp()
{
    //cout << "<cmp>\n";
    double raw_num;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_vm_m[i_val1], raw_num);
                    //rc_debug_outputFlags();
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_vm_m[i_val1], rc_vm_m[i_val2]);
                    //rc_debug_outputFlags();
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_vm_m[i_val1], rc_nid[i_val2][0]);
                    //rc_debug_outputFlags();
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_vm_m[i_val1], rc_vm_system_id[i_val2]);
                    //rc_debug_outputFlags();
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_vm_m[i_val1], rc_vm_f[i_val2]);
                    //rc_debug_outputFlags();
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_nid[i_val1][0], raw_num);
                    //rc_debug_outputFlags();
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_nid[i_val1][0], rc_vm_m[i_val2]);
                    //rc_debug_outputFlags();
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_nid[i_val1][0], rc_nid[i_val2][0]);
                    //rc_debug_outputFlags();
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_nid[i_val1][0], rc_vm_system_id[i_val2]);
                    //rc_debug_outputFlags();
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_nid[i_val1][0], rc_vm_f[i_val2]);
                    //rc_debug_outputFlags();
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 9:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_vm_f[i_val1], raw_num);
                    //rc_debug_outputFlags();
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_vm_f[i_val1], rc_vm_m[i_val2]);
                    //rc_debug_outputFlags();
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_vm_f[i_val1], rc_nid[i_val2][0]);
                    //rc_debug_outputFlags();
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_vm_f[i_val1], rc_vm_system_id[i_val2]);
                    //rc_debug_outputFlags();
                    return;
                case 9:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_vm_f[i_val1], rc_vm_f[i_val2]);
                    //rc_debug_outputFlags();
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_cmp_str()
{
    //cout << "<cmp_str>\n";
    double raw_num;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 3:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 2:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_vm_s[i_val1], rc_segment_getString(i_val2));
                    //rc_debug_outputFlags();
                    return;
                case 3:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_vm_s[i_val1], rc_vm_s[i_val2]);
                    //rc_debug_outputFlags();
                    return;
                case 5:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_vm_s[i_val1], rc_sid[i_val2][0]);
                    //rc_debug_outputFlags();
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 5:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 2:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_sid[i_val1][0], rc_segment_getString(i_val2));
                    //rc_debug_outputFlags();
                    return;
                case 3:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_sid[i_val1][0], rc_vm_s[i_val2]);
                    //rc_debug_outputFlags();
                    return;
                case 5:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_setSysFlags(rc_sid[i_val1][0], rc_sid[i_val2][0]);
                    //rc_debug_outputFlags();
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline string rc_doubleToString(double a)
{
    stringstream ss;
    string str = "";
    ss << fixed << a;
    str = ss.str();
    for(int i = str.length()-1; i >= 0; i--)
    {
        if(str.substr(i,1).compare("0")!=0)
        {
            str = str.substr(0,i+1);
            if(str.substr(str.length()-1,1).compare(".")==0)
                return str.substr(0,str.length()-1);
            else
                return str;
        }
    }
    return str;
}

inline void rc_vm_convertToStr()
{
    double raw_num;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 3:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_s[i_val1] = rc_doubleToString(raw_num);
                    //cout << "#S[" << i_val1 << "] = " << rc_vm_s[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_s[i_val1] = rc_doubleToString(rc_vm_m[i_val2]);
                    //cout << "#S[" << i_val1 << "] = " << rc_vm_s[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_s[i_val1] = rc_doubleToString(rc_nid[i_val2][0]);
                    //cout << "#S[" << i_val1 << "] = " << rc_vm_s[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_s[i_val1] = rc_doubleToString(rc_vm_system_id[i_val2]);
                    //cout << "#S[" << i_val1 << "] = " << rc_vm_s[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 5:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_sid[i_val1][0] = rc_doubleToString(raw_num);
                    //cout << "#STR_ID[" << i_val1 << "] = " << rc_sid[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_sid[i_val1][0] = rc_doubleToString(rc_vm_m[i_val2]);
                    //cout << "#STR_ID[" << i_val1 << "] = " << rc_sid[i_val1][0] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_sid[i_val1][0] = rc_doubleToString(rc_nid[i_val2][0]);
                    //cout << "#STR_ID[" << i_val1 << "] = " << rc_sid[i_val1][0] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_sid[i_val1][0] = rc_doubleToString(rc_vm_system_id[i_val2]);
                    //cout << "#STR_ID[" << i_val1 << "] = " << rc_sid[i_val1][0] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_and()
{
    double raw_num;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] && raw_num;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] && rc_vm_m[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] && rc_nid[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] && rc_vm_system_id[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] && raw_num;
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] && rc_vm_m[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] && rc_nid[i_val2][0];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] && rc_vm_system_id[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_or()
{
    double raw_num;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] || raw_num;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] || rc_vm_m[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] || rc_nid[i_val2][0];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = rc_vm_m[i_val1] || rc_vm_system_id[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] || raw_num;
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] || rc_vm_m[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] || rc_nid[i_val2][0];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = rc_nid[i_val1][0] || rc_vm_system_id[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_xor()
{
    double raw_num;
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    char arg_type2 = 0;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = (int) rc_vm_m[i_val1] ^ (int)raw_num;
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = (int)rc_vm_m[i_val1] ^ (int)rc_vm_m[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = (int)rc_vm_m[i_val1] ^ (int)rc_nid[i_val2][0];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_vm_m[i_val1] = (int)rc_vm_m[i_val1] ^ rc_vm_system_id[i_val2];
                    //cout << "#M[" << i_val1 << "] = " << rc_vm_m[i_val1] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type2)
            {
                case 0:
                    raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = (int)rc_nid[i_val1][0] ^ (int)raw_num;
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = (int)rc_nid[i_val1][0] ^ (int)rc_vm_m[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = (int)rc_nid[i_val1][0] ^ (int)rc_nid[i_val2][0];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1] << endl;
                    return;
                case 6:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    rc_nid[i_val1][0] = (int)rc_nid[i_val1][0] ^ rc_vm_system_id[i_val2];
                    //cout << "NUM_ID[" << i_val1 << "] = " << rc_nid[i_val1][0] << endl;
                    return;
                default:
                    RC_VM_ACTIVE = 0;
                    return;
            }
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_push()
{
    //cout << "<push>\n";
    double raw_num;
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 0:
            raw_num = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            rc_num_stack[rc_num_stack_index] = raw_num;
            rc_num_stack_index++;
            return;
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            rc_num_stack[rc_num_stack_index] = rc_vm_m[i_val1];
            rc_num_stack_index++;
            return;
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            rc_num_stack[rc_num_stack_index] = rc_nid[i_val1][0];
            rc_num_stack_index++;
            return;
        case 6:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            rc_num_stack[rc_num_stack_index] = rc_vm_system_id[i_val1];
            rc_num_stack_index++;
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_push_str()
{
    //cout << "test push\n";
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 2:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            rc_str_stack[rc_str_stack_index] = rc_segment_getString(i_val1);
            rc_str_stack_index++;
            return;
        case 3:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            rc_str_stack[rc_str_stack_index] = rc_vm_s[i_val1]; //if(i_val1 >= 6) cout << "test s6\n";
            rc_str_stack_index++;
            return;
        case 5:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            rc_str_stack[rc_str_stack_index] = rc_sid[i_val1][0];
            rc_str_stack_index++;
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_pop()
{
    //cout << "<pop>\n";
    double raw_num;
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;

            if(rc_num_stack_index > 0)
            rc_num_stack_index--;

            rc_vm_m[i_val1] = rc_num_stack[rc_num_stack_index];
            //cout << "pop m" << i_val1 << endl;
            return;
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;

            if(rc_num_stack_index > 0)
            rc_num_stack_index--;

            rc_nid[i_val1][0] = rc_num_stack[rc_num_stack_index];
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_pop_str()
{
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 3:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;

            if(rc_str_stack_index > 0)
                rc_str_stack_index--;

            rc_vm_s[i_val1] = rc_str_stack[rc_str_stack_index];
            return;
        case 5:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;

            if(rc_str_stack_index > 0)
                rc_str_stack_index--;

            rc_sid[i_val1][0] = rc_str_stack[rc_str_stack_index];
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_gosub()
{
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 0:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;

            RC_CURRENT_SEGMENT = RC_DATA_SEGMENT;
            rc_vm_setReturnAddress(RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS = i_val1;

            return;
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;

            RC_CURRENT_SEGMENT = RC_DATA_SEGMENT;
            rc_vm_setReturnAddress(RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS = rc_vm_m[i_val1];

            return;
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;

            RC_CURRENT_SEGMENT = RC_DATA_SEGMENT;
            rc_vm_setReturnAddress(RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS = rc_nid[i_val1][0];

            return;
        case 6:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;

            RC_CURRENT_SEGMENT = RC_DATA_SEGMENT;
            rc_vm_setReturnAddress(RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS = rc_vm_system_id[i_val1];

            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_arr1()
{
    //cout << "<arr1>\n";
    unsigned int i_val1;
    unsigned int i_val2;
    unsigned int i_val3;
    char arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type)
            {
                case 0:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
                    RC_CURRENT_ADDRESS++;
                    switch(arg_type)
                    {
                        case 1:
                            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                            RC_CURRENT_ADDRESS += vm_data_size;
                            rc_vm_m[i_val3] = rc_nid[i_val1][i_val2];
                            return;
                        case 4:
                            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                            RC_CURRENT_ADDRESS += vm_data_size;
                            rc_nid[i_val3][0] = rc_nid[i_val1][i_val2];
                            return;
                    }

                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
                    RC_CURRENT_ADDRESS++;
                    switch(arg_type)
                    {
                        case 1:
                            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                            RC_CURRENT_ADDRESS += vm_data_size;
                            rc_vm_m[i_val3] = rc_nid[i_val1][(unsigned int)rc_vm_m[i_val2]];
                            return;
                        case 4:
                            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                            RC_CURRENT_ADDRESS += vm_data_size;
                            rc_nid[i_val3][0] = rc_nid[i_val1][(unsigned int)rc_vm_m[i_val2]];
                            return;
                    }

                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
                    RC_CURRENT_ADDRESS++;
                    switch(arg_type)
                    {
                        case 1:
                            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                            RC_CURRENT_ADDRESS += vm_data_size;
                            rc_vm_m[i_val3] = rc_nid[i_val1][(unsigned int)rc_nid[i_val2][0]];
                            return;
                        case 4:
                            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                            RC_CURRENT_ADDRESS += vm_data_size;
                            rc_nid[i_val3][0] = rc_nid[i_val1][(unsigned int)rc_nid[i_val2][0]];
                            return;
                    }

            }

        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_arr2()
{
    unsigned int i_val1; //arr id
    unsigned int i_val2; //index 1
    unsigned int i_val3; //index 2
    unsigned int i_val4; //destination
    unsigned int arr_index; //index in rc_nid
    char arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val2 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val2 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val3 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val3 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 1:
            i_val4 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            rc_vm_m[i_val4] = rc_nid[i_val1][(unsigned int)((rc_nid_h[i_val1] * i_val2) + i_val3)];
            //cout << "~X[" << i_val2 << ", " << i_val3 << "] = " << rc_vm_m[i_val4] << endl;
            return;
        case 4:
            i_val2 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            rc_nid[i_val4][0] = rc_nid[i_val1][(unsigned int)((rc_nid_h[i_val1] * i_val2) + i_val3)];
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_arr3()
{
    unsigned int i_val1; //arr id
    unsigned int i_val2; //index 1
    unsigned int i_val3; //index 2
    unsigned int i_val4; //index 3
    unsigned int i_val5; //destination
    unsigned int arr_index; //index in rc_nid
    char arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val2 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val2 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val3 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val3 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val4 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val4 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val4 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 1:
            i_val5 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            rc_vm_m[i_val5] = rc_nid[i_val1][(i_val2 * (rc_nid_h[i_val1] * rc_nid_d[i_val1])) + (i_val3 * rc_nid_d[i_val1]) + i_val4];
            return;
        case 4:
            i_val5 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            rc_nid[i_val5][0] = rc_nid[i_val1][(i_val2 * (rc_nid_h[i_val1] * rc_nid_d[i_val1])) + (i_val3 * rc_nid_d[i_val1]) + i_val4];
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_arr1_str()
{
    unsigned int i_val1;
    unsigned int i_val2;
    unsigned int i_val3;
    char arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 5:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
            RC_CURRENT_ADDRESS++;
            switch(arg_type)
            {
                case 0:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
                    RC_CURRENT_ADDRESS++;
                    switch(arg_type)
                    {
                        case 3:
                            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                            RC_CURRENT_ADDRESS += vm_data_size;
                            rc_vm_s[i_val3] = rc_sid[i_val1][i_val2];
                            return;
                        case 5:
                            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                            RC_CURRENT_ADDRESS += vm_data_size;
                            rc_sid[i_val3][0] = rc_sid[i_val1][i_val2];
                            return;
                    }

                case 1:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
                    RC_CURRENT_ADDRESS++;
                    switch(arg_type)
                    {
                        case 3:
                            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                            RC_CURRENT_ADDRESS += vm_data_size;
                            rc_vm_s[i_val3] = rc_sid[i_val1][(unsigned int)rc_vm_m[i_val2]];
                            return;
                        case 5:
                            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                            RC_CURRENT_ADDRESS += vm_data_size;
                            rc_sid[i_val3][0] = rc_sid[i_val1][(unsigned int)rc_vm_m[i_val2]];
                            return;
                    }

                case 4:
                    i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                    RC_CURRENT_ADDRESS += vm_data_size;
                    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
                    RC_CURRENT_ADDRESS++;
                    switch(arg_type)
                    {
                        case 3:
                            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                            RC_CURRENT_ADDRESS += vm_data_size;
                            rc_vm_s[i_val3] = rc_sid[i_val1][(unsigned int)rc_nid[i_val2][0]];
                            return;
                        case 5:
                            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
                            RC_CURRENT_ADDRESS += vm_data_size;
                            rc_sid[i_val3][0] = rc_sid[i_val1][(unsigned int)rc_nid[i_val2][0]];
                            return;
                    }

            }

        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_arr2_str()
{
    unsigned int i_val1; //arr id
    unsigned int i_val2; //index 1
    unsigned int i_val3; //index 2
    unsigned int i_val4; //destination
    //unsigned int arr_index; //index in rc_nid
    char arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 5:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val2 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val2 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val3 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val3 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 3:
            i_val4 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            rc_vm_s[i_val4] = rc_sid[i_val1][(unsigned int)(rc_sid_h[i_val1] * i_val2) + i_val3];
            return;
        case 5:
            i_val2 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            rc_sid[i_val4][0] = rc_sid[i_val1][(unsigned int)(rc_sid_h[i_val1] * i_val2) + i_val3];
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_arr3_str()
{
    unsigned int i_val1; //arr id
    unsigned int i_val2; //index 1
    unsigned int i_val3; //index 2
    unsigned int i_val4; //index 3
    unsigned int i_val5; //destination
    unsigned int arr_index; //index in rc_nid
    char arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 5:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val2 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val2 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val3 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val3 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val4 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val4 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val4 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 3:
            i_val5 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            rc_vm_s[i_val5] = rc_sid[i_val1][(i_val2 * (rc_sid_h[i_val1] * rc_sid_d[i_val1])) + (i_val3 * rc_sid_d[i_val1]) + i_val4];
            return;
        case 5:
            i_val5 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            rc_sid[i_val5][0] = rc_sid[i_val1][(i_val2 * (rc_sid_h[i_val1] * rc_sid_d[i_val1])) + (i_val3 * rc_sid_d[i_val1]) + i_val4];
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_alloc()
{
    //cout << "<alloc>\n";
    unsigned int i_val1; //size
    unsigned int i_val2; //data type
    unsigned int i_val3; //rc_nid or rc_sid index

    char arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;

    switch(arg_type)
    {
        case 0:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val1 = (unsigned int) rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val1 = (unsigned int) rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val2 = (unsigned int) rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val2 = (unsigned int) rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val3 = (unsigned int) rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val3 = (unsigned int) rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    switch(i_val2)
    {
        case 0:
            delete[] rc_nid[i_val3];
            rc_nid[i_val3] = new double[i_val1];
            SDL_memset(rc_nid[i_val3], 0, sizeof(double)*i_val1);
            return;
        case 1:
            //delete[] rc_sid[i_val3];
            rc_sid[i_val3] = new string[i_val1];
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }

    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_setArr1()
{
    //cout << "<setArr1>\n";
    unsigned int i_val1; //arr id
    unsigned int i_val2; //index 1
    unsigned int i_val3; //value index
    double raw_num; //value raw number
    //unsigned int arr_index; //index in rc_nid
    char arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val2 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val2 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            rc_nid[i_val1][i_val2] = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            return;
        case 1:
            rc_nid[i_val1][i_val2] = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            return;
        case 4:
            rc_nid[i_val1][i_val2] = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_setArr2()
{
    //cout << "SET_ARR2" << endl;
    //SDL_Delay(3000);
    unsigned int i_val1; //arr id
    unsigned int i_val2; //index 1
    unsigned int i_val3; //index 2
    unsigned int i_val4; //value index
    //double raw_num; //value raw number
    //unsigned int arr_index; //index in rc_nid
    char arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val2 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val2 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val3 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val3 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    //cout << "AT = " << (int)arg_type << endl;
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            rc_nid[i_val1][(rc_nid_h[i_val1] * i_val2) + i_val3] = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            //cout << "1X[" << i_val2 << ", " << i_val3 << "] = [" << (rc_nid_w[i_val1] * i_val2) + i_val3 << "] = " << rc_nid[i_val1][(rc_nid_w[i_val1] * i_val2) + i_val3] << endl;
            //SDL_Delay(3000);
            return;
            //return 1;
        case 1:
            rc_nid[i_val1][(rc_nid_h[i_val1] * i_val2) + i_val3] = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            //cout << "2X[" << i_val2 << ", " << i_val3 << "] = [" << (rc_nid_w[i_val1] * i_val2) + i_val3 << "] = " << rc_nid[i_val1][(rc_nid_w[i_val1] * i_val2) + i_val3] << endl;
            //SDL_Delay(3000);
            return;
            //return 2;
        case 4:
            rc_nid[i_val1][(rc_nid_h[i_val1] * i_val2) + i_val3] = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            //cout << "3X[" << i_val2 << ", " << i_val3 << "] = [" << (rc_nid_w[i_val1] * i_val2) + i_val3 << "] = " << rc_nid[i_val1][(rc_nid_w[i_val1] * i_val2) + i_val3] << endl;
            //SDL_Delay(3000);
            return;
            //return 3;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }

    //cout << "X[" << i_val2 << ", " << i_val3 << "] = [" << (rc_nid_w[i_val1] * i_val2) + i_val3 << "] = " << rc_nid[i_val1][(rc_nid_w[i_val1] * i_val2) + i_val3] << endl;
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_setArr3()
{
    unsigned int i_val1; //arr id
    unsigned int i_val2; //index 1
    unsigned int i_val3; //index 2
    unsigned int i_val4; //value index
    double raw_num; //value raw number
    //unsigned int arr_index; //index in rc_nid
    char arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val2 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val2 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val3 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val3 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val4 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val4 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val4 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            rc_nid[i_val1][(i_val2 * (rc_nid_h[i_val1] * rc_nid_d[i_val1])) + (i_val3 * rc_nid_d[i_val1]) + i_val4] = rc_segment_getdouble(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            return;
        case 1:
            rc_nid[i_val1][(i_val2 * (rc_nid_h[i_val1] * rc_nid_d[i_val1])) + (i_val3 * rc_nid_d[i_val1]) + i_val4] = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            return;
        case 4:
            rc_nid[i_val1][(i_val2 * (rc_nid_h[i_val1] * rc_nid_d[i_val1])) + (i_val3 * rc_nid_d[i_val1]) + i_val4] = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }

    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_setArr1_str()
{
    //cout << "<setArr1>\n";
    unsigned int i_val1; //arr id
    unsigned int i_val2; //index 1
    unsigned int i_val3; //value index
    //double raw_num; //value raw number
    //unsigned int arr_index; //index in rc_nid
    char arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 5:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }

    //cout << "SetArray$ CP 1\n";

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val2 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val2 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    //cout << "SetArray$ CP 2\n";

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    //cout << "arg_type set: " << (int)arg_type << endl;
    switch(arg_type)
    {
        case 2:
            rc_sid[i_val1][i_val2] = rc_segment_getString(rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS));
            RC_CURRENT_ADDRESS += vm_data_size;
            return;
        case 3:
            //cout << "IVAL1 = " << i_val1 << endl << "IVAL2 = " << i_val2 << endl;
            rc_sid[i_val1][i_val2] = rc_vm_s[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            return;
        case 5:
            rc_sid[i_val1][i_val2] = rc_sid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }

    //cout << "SetArray$ CP 3\n";

    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_setArr2_str()
{
    unsigned int i_val1; //arr id
    unsigned int i_val2; //index 1
    unsigned int i_val3; //index 2
    unsigned int i_val4; //value index
    //double raw_num; //value raw number
    //unsigned int arr_index; //index in rc_nid
    char arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 5:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val2 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val2 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val3 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val3 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 2:
            rc_sid[i_val1][(rc_sid_w[i_val1] * i_val2) + i_val3] = rc_segment_getString(rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS));
            RC_CURRENT_ADDRESS += vm_data_size;
            return;
            //return 1;
        case 3:
            //cout << "address = " << (rc_sid_h[i_val1] * i_val2) + i_val3 << endl;
            rc_sid[i_val1][(rc_sid_h[i_val1] * i_val2) + i_val3] = rc_vm_s[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            return;
            //return 2;
        case 5:
            rc_sid[i_val1][(rc_sid_h[i_val1] * i_val2) + i_val3] = rc_sid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            return;
            //return 3;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }

    //cout << "X[" << i_val2 << ", " << i_val3 << "] = [" << (rc_nid_w[i_val1] * i_val2) + i_val3 << "] = " << rc_nid[i_val1][(rc_nid_w[i_val1] * i_val2) + i_val3] << endl;
    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_setArr3_str()
{
    unsigned int i_val1; //arr id
    unsigned int i_val2; //index 1
    unsigned int i_val3; //index 2
    unsigned int i_val4; //value index
    //double raw_num; //value raw number
    //unsigned int arr_index; //index in rc_nid
    char arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 5:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val2 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val2 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val3 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val3 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 0:
            i_val4 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val4 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val4 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type)
    {
        case 2:
            rc_sid[i_val1][(i_val2 * (rc_sid_h[i_val1] * rc_sid_d[i_val1])) + (i_val3 * rc_sid_d[i_val1]) + i_val4] = rc_segment_getString(rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS));
            RC_CURRENT_ADDRESS += vm_data_size;
            return;
        case 3:
            rc_sid[i_val1][(i_val2 * (rc_sid_h[i_val1] * rc_sid_d[i_val1])) + (i_val3 * rc_sid_d[i_val1]) + i_val4] = rc_vm_s[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            return;
        case 5:
            rc_sid[i_val1][(i_val2 * (rc_sid_h[i_val1] * rc_sid_d[i_val1])) + (i_val3 * rc_sid_d[i_val1]) + i_val4] = rc_sid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }

    RC_VM_ACTIVE = 0;
    return;
}

inline void rc_vm_jmp()
{
    //cout << "<JMP>\n";
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 0:
            RC_CURRENT_ADDRESS = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            return;
        case 1:
            RC_CURRENT_ADDRESS = (unsigned int) rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            return;
        case 4:
            RC_CURRENT_ADDRESS = (unsigned int) rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            return;
        case 6:
            RC_CURRENT_ADDRESS = rc_vm_system_id[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
}

inline void rc_vm_je()
{
    //cout << "<JE>\n";
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 0:
            if(rc_vm_system_id[RC_EQUAL_FLAG])
            RC_CURRENT_ADDRESS = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 1:
            if(rc_vm_system_id[RC_EQUAL_FLAG])
            RC_CURRENT_ADDRESS = (unsigned int) rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 4:
            if(rc_vm_system_id[RC_EQUAL_FLAG])
            RC_CURRENT_ADDRESS = (unsigned int) rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 6:
            if(rc_vm_system_id[RC_EQUAL_FLAG])
            RC_CURRENT_ADDRESS = rc_vm_system_id[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
}

inline void rc_vm_jne()
{
    //cout << "<JNE>\n";
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 0:
            if(rc_vm_system_id[RC_NOT_EQUAL_FLAG])
            RC_CURRENT_ADDRESS = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 1:
            if(rc_vm_system_id[RC_NOT_EQUAL_FLAG])
            RC_CURRENT_ADDRESS = (unsigned int) rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 4:
            if(rc_vm_system_id[RC_NOT_EQUAL_FLAG])
            RC_CURRENT_ADDRESS = (unsigned int) rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 6:
            if(rc_vm_system_id[RC_NOT_EQUAL_FLAG])
            RC_CURRENT_ADDRESS = rc_vm_system_id[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
}

inline void rc_vm_jg()
{
    //cout << "<jg>\n";
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 0:
            if(rc_vm_system_id[RC_GREATER_FLAG])
            RC_CURRENT_ADDRESS = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 1:
            if(rc_vm_system_id[RC_GREATER_FLAG])
            RC_CURRENT_ADDRESS = (unsigned int) rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 4:
            if(rc_vm_system_id[RC_GREATER_FLAG])
            RC_CURRENT_ADDRESS = (unsigned int) rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 6:
            if(rc_vm_system_id[RC_GREATER_FLAG])
            RC_CURRENT_ADDRESS = rc_vm_system_id[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
}

inline void rc_vm_jl()
{
    //cout << "<JL>\n";
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 0:
            if(rc_vm_system_id[RC_LESS_FLAG])
            RC_CURRENT_ADDRESS = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 1:
            if(rc_vm_system_id[RC_LESS_FLAG])
            RC_CURRENT_ADDRESS = (unsigned int) rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 4:
            if(rc_vm_system_id[RC_LESS_FLAG])
            RC_CURRENT_ADDRESS = (unsigned int) rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 6:
            if(rc_vm_system_id[RC_LESS_FLAG])
            RC_CURRENT_ADDRESS = rc_vm_system_id[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
}

inline void rc_vm_jge()
{
    //cout << "<JGE>\n";
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 0:
            if(rc_vm_system_id[RC_GREATER_EQUAL_FLAG])
            RC_CURRENT_ADDRESS = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 1:
            if(rc_vm_system_id[RC_GREATER_EQUAL_FLAG])
            RC_CURRENT_ADDRESS = (unsigned int) rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 4:
            if(rc_vm_system_id[RC_GREATER_EQUAL_FLAG])
            RC_CURRENT_ADDRESS = (unsigned int) rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 6:
            if(rc_vm_system_id[RC_GREATER_EQUAL_FLAG])
            RC_CURRENT_ADDRESS = rc_vm_system_id[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
}

inline void rc_vm_jle()
{
    //cout << "<JLE>\n";
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 0:
            if(rc_vm_system_id[RC_LESS_EQUAL_FLAG])
            RC_CURRENT_ADDRESS = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 1:
            if(rc_vm_system_id[RC_LESS_EQUAL_FLAG])
            RC_CURRENT_ADDRESS = (unsigned int) rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 4:
            if(rc_vm_system_id[RC_LESS_EQUAL_FLAG])
            RC_CURRENT_ADDRESS = (unsigned int) rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        case 6:
            if(rc_vm_system_id[RC_LESS_EQUAL_FLAG])
            RC_CURRENT_ADDRESS = rc_vm_system_id[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            else
                RC_CURRENT_ADDRESS+=vm_data_size;
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
}

inline void rc_vm_abs()
{
    //cout << "<ABS>\n";
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            if(rc_vm_m[i_val1] < 0)
                rc_vm_m[i_val1] = rc_vm_m[i_val1] * -1;
            //rc_vm_m[i_val1] = abs(rc_vm_m[i_val1]);
            return;
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            if(rc_nid[i_val1][0] < 0)
                rc_nid[i_val1][0] * -1;
            //rc_nid[i_val1][0] = abs(rc_nid[i_val1][0]);
            return;
        case 9:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            if(rc_vm_f[i_val1] < 0)
                rc_vm_f[i_val1] = rc_vm_f[i_val1] * -1;
            //rc_vm_f[i_val1] = abs(rc_vm_f[i_val1]);
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
}

inline void rc_vm_not()
{
    //cout << "<NOT>\n";
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;

            rc_vm_m[i_val1] = not(int)rc_vm_m[i_val1];
            return;
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;

            rc_nid[i_val1][0] = not(int)rc_nid[i_val1][0];
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
}

inline void rc_vm_end()
{
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 0:
            //cout << "end instruction\n";
            i_val1 = rc_segment_getInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;

            rc_vm_system_id[RC_EXIT_CODE] = i_val1;
            RC_VM_ACTIVE = 0;
            return;
        case 1:
            //cout << "end instruction\n";
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;

            rc_vm_system_id[RC_EXIT_CODE] = rc_vm_m[i_val1];
            RC_VM_ACTIVE = 0;
            return;
        case 4:
            //cout << "end instruction\n";
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;

            rc_vm_system_id[RC_EXIT_CODE] = rc_nid[i_val1][0];
            RC_VM_ACTIVE = 0;
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
}

inline void rc_vm_dim()
{
    unsigned int i_val1; //arr id
    int dt = 0;
    //unsigned int arr_index; //index in rc_nid
    char arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;

    switch(arg_type)
    {
        case 0:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val1 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val1 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;

    switch(arg_type)
    {
        case 0:
            rc_nid_w[i_val1] = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            rc_nid_w[i_val1] = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            rc_nid_w[i_val1] = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;

    switch(arg_type)
    {
        case 0:
            rc_nid_h[i_val1] = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            rc_nid_h[i_val1] = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            rc_nid_h[i_val1] = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;

    switch(arg_type)
    {
        case 0:
            rc_nid_d[i_val1] = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            rc_nid_d[i_val1] = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            rc_nid_d[i_val1] = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    //return 1;
}

inline void rc_vm_dim_str()
{
    unsigned int i_val1; //arr id
    int dt = 0;
    //unsigned int arr_index; //index in rc_nid
    char arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;

    switch(arg_type)
    {
        case 0:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val1 = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            i_val1 = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;

    switch(arg_type)
    {
        case 0:
            rc_sid_w[i_val1] = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            rc_sid_w[i_val1] = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            rc_sid_w[i_val1] = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;

    switch(arg_type)
    {
        case 0:
            rc_sid_h[i_val1] = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            rc_sid_h[i_val1] = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            rc_sid_h[i_val1] = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    arg_type = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;

    switch(arg_type)
    {
        case 0:
            rc_sid_d[i_val1] = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            rc_sid_d[i_val1] = rc_vm_m[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 4:
            rc_sid_d[i_val1] = rc_nid[rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS)][0];
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
    }

    //return 1;
}

inline void rc_vm_print()
{
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    string m_str = "";
    stringstream ss;
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 1:
            //cout << "end of prog\n";
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            cout << rc_doubleToString(rc_vm_m[i_val1]) <<endl;
            return;
        case 3:
            //cout << "end of prog\n";
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            cout << rc_vm_s[i_val1] << endl;
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
}

inline int rc_intern_arrayDimensions()
{
    rc_nid[ARRAYDIMENSIONS_ARR_ID] = NULL;
    rc_sid[ARRAYDIMENSIONS_ARR_ID_STR] = NULL;
    if(RC_PTR_TYPE == 0)
    {
        if(rc_nid_w[ARRAYDIMENSIONS_ARR_ID]>0)
        {
            if(rc_nid_h[ARRAYDIMENSIONS_ARR_ID]>0)
            {
                if(rc_nid_d[ARRAYDIMENSIONS_ARR_ID]>0)
                {
                    return 3;
                }
                return 2;
            }
            return 1;
        }
    }
    else if(RC_PTR_TYPE == 1)
    {
        if(rc_sid_w[ARRAYDIMENSIONS_ARR_ID_STR]>0)
        {
            if(rc_sid_h[ARRAYDIMENSIONS_ARR_ID_STR]>0)
            {
                if(rc_sid_d[ARRAYDIMENSIONS_ARR_ID_STR]>0)
                {
                    return 3;
                }
                return 2;
            }
            return 1;
        }
    }
    return 0;
}

inline unsigned int rc_intern_arraySize()
{
    rc_nid[ARRAYSIZE_ARR_ID] = NULL;
    rc_sid[ARRAYSIZE_ARR_ID_STR] = NULL;
    if(RC_PTR_TYPE == 0)
    {
        switch((int)rc_nid[ARRAYSIZE_ARR_DIM][0])
        {
            case 1:
                return rc_nid_w[ARRAYSIZE_ARR_ID];
            case 2:
                return rc_nid_h[ARRAYSIZE_ARR_ID];
            case 3:
                return rc_nid_d[ARRAYSIZE_ARR_ID];
        }
    }
    else if(RC_PTR_TYPE == 1)
    {
        switch((int)rc_nid[ARRAYSIZE_ARR_DIM][0])
        {
            case 1:
                return rc_sid_w[ARRAYSIZE_ARR_ID_STR];
            case 2:
                return rc_sid_h[ARRAYSIZE_ARR_ID_STR];
            case 3:
                return rc_sid_d[ARRAYSIZE_ARR_ID_STR];
        }
    }
    return 0;
}

inline int rc_intern_asc()
{
    return (int)rc_sid[ASC_C_STR][0][0];
}

inline string rc_intern_chr()
{
    string s = "";
    s += (char) rc_nid[CHR_STR_N][0];
    return s;
}

inline string rc_intern_insert()
{
    return rc_sid[INSERT_STR_SRC_STR][0].substr(0, rc_nid[INSERT_STR_POS][0]) + rc_sid[INSERT_STR_TGT_STR][0] + rc_sid[INSERT_STR_SRC_STR][0].substr(rc_nid[INSERT_STR_POS][0]);
}

inline int rc_intern_instr()
{
    //cout << "Cant find " << rc_sid[INSTR_SUBSTR][0] << " in " << rc_sid[INSTR_STR][0] << endl;
    return rc_sid[INSTR_STRING_STR][0].find(rc_sid[INSTR_SUBSTR_STR][0]);
}

inline string rc_intern_lcase()
{
   for(unsigned int i=0;i<rc_sid[LCASE_STR_STRING_STR][0].length();i++)
   {
      rc_sid[LCASE_STR_STRING_STR][0][i] = tolower(rc_sid[LCASE_STR_STRING_STR][0][i]);
   }
   return rc_sid[LCASE_STR_STRING_STR][0];
}

inline string rc_intern_left()
{
    return rc_sid[LEFT_STR_STRING_STR][0].substr(0,(int)rc_nid[LEFT_STR_N][0]);
}

inline int rc_intern_length()
{
    return rc_sid[LENGTH_STRING_STR][0].length();
}

inline string rc_intern_ltrim()
{
    return rc_sid[LTRIM_STR_STRING_STR][0].substr(rc_sid[LTRIM_STR_STRING_STR][0].find_first_not_of(" "));
}

inline string rc_intern_mid()
{
    return rc_sid[MID_STR_STRING_STR][0].substr((int)rc_nid[MID_STR_START][0], (int)rc_nid[MID_STR_N][0]);
}

inline string rc_intern_replace()
{
    string src = rc_sid[REPLACE_STR_SRC_STR][0];
    string rpc = rc_sid[REPLACE_STR_RPC_STR][0];
    unsigned int pos = rc_nid[REPLACE_STR_POS][0];
    int rpc_i = 0;
    string n_str = src.substr(0,pos);
    for(int i = pos; i < src.length(); i++)
    {
        if(rpc_i < rpc.length())
            n_str += rpc.substr(rpc_i,1);
        else
            break;
        rpc_i++;
    }
    if((pos+rpc_i) < src.length())
        n_str += src.substr(pos+rpc_i);
    return n_str;
}

inline string rc_intern_replaceSubstr()
{
    unsigned int found = 0;
    string src = rc_sid[REPLACESUBSTR_STR_SRC_STR][0];
    string tgt = rc_sid[REPLACESUBSTR_STR_TGT_STR][0];
    string rpc = rc_sid[REPLACESUBSTR_STR_RPC_STR][0];
    found = src.find(tgt);
    while( ((int)found) != -1)
    {
        src = src.substr(0,found) + rpc + src.substr(found + tgt.length());
        found = src.find(tgt,found+1);
    }
    return src;
}

inline string rc_intern_reverse()
{
    string n_str = "";
    for(int i = rc_sid[REVERSE_STR_STRING_STR][0].length()-1; i >= 0; i--)
        n_str += rc_sid[REVERSE_STR_STRING_STR][0][i];
    return n_str;
}

inline string rc_intern_right()
{
    return rc_sid[RIGHT_STR_STRING_STR][0].substr(rc_sid[RIGHT_STR_STRING_STR][0].length()-rc_nid[RIGHT_STR_N][0]);
}

inline string rc_intern_rtrim()
{
    int i = 0;
    for(i = rc_sid[RTRIM_STR_STRING_STR][0].length()-1; i >= 0; i--)
        if(rc_sid[RTRIM_STR_STRING_STR][0].substr(i,1).compare(" ") != 0)
            break;
    return rc_sid[RTRIM_STR_STRING_STR][0].substr(0,i+1);
}

inline string rc_intern_fillstring()
{
    string f = "";
    for(int i = 0; i < rc_nid[FILLSTRING_STR_N][0]; i++)
        f += rc_sid[FILLSTRING_STR_STRING_STR][0];
    return f;
}

inline string rc_intern_str()
{
    stringstream ss;
    ss << fixed << rc_nid[STR_STR_VALUE][0];
    int d = ss.str().find_first_of(".");
    string s1 = ss.str().substr(0, d);
    string s2 = ss.str().substr(d);
    int n = 0;
    for(int i = s2.length()-1; i >= 0; i--)
    {
        if(s2.substr(i,1).compare("0")!=0 && s2.substr(i,1).compare(".")!=0)
        {
            n = i+1;
            break;
        }
    }
    string s_rtn = s1;
    if(n!=0)
        s_rtn += s2.substr(0,n);
    return s_rtn;
}

inline string rc_intern_str_f()
{
    stringstream ss;
    ss << fixed << rc_nid[STR_F_STR_N][0];
    return ss.str();
}

inline string rc_intern_str_s()
{
    stringstream ss;
    ss << scientific << rc_nid[STR_S_STR_N][0];
    return ss.str();
}

inline unsigned int rc_intern_tally()
{
    unsigned int found = 0;
    string t_str = rc_sid[TALLY_STRING_STR][0];
    string t_substr = rc_sid[TALLY_SUBSTR_STR][0];
    found = t_str.find(t_substr);
    unsigned int tally_count = 0;
    while( ((int)found) != -1)
    {
        tally_count++;
        found = t_str.find(t_substr,found+1);
    }
    return tally_count;
}

inline string rc_intern_trim()
{
    int i = 0;
    for(i = rc_sid[TRIM_STR_STRING_STR][0].length()-1; i >= 0; i--)
        if(rc_sid[TRIM_STR_STRING_STR][0].substr(i,1).compare(" ") != 0)
            break;
    rc_sid[TRIM_STR_STRING_STR][0] = rc_sid[TRIM_STR_STRING_STR][0].substr(0,i+1);
    return rc_sid[TRIM_STR_STRING_STR][0].substr(rc_sid[TRIM_STR_STRING_STR][0].find_first_not_of(" "));
}

inline string rc_intern_ucase()
{
    for(unsigned int i=0;i<rc_sid[UCASE_STR_STRING_STR][0].length();i++)
    {
        rc_sid[UCASE_STR_STRING_STR][0][i] = toupper(rc_sid[UCASE_STR_STRING_STR][0][i]);
    }
   return rc_sid[UCASE_STR_STRING_STR][0];
}

inline double rc_intern_val()
{
    return atof(rc_sid[VAL_STRING_STR][0].c_str());
}

inline double rc_intern_abs()
{
    if(rc_nid[ABS_N][0] < 0)
        return (rc_nid[ABS_N][0] * -1);
    else
        return rc_nid[ABS_N][0];
}

inline double rc_intern_aCos()
{
    return acos(rc_nid[ACOS_N][0]);
}

inline double rc_intern_andBit()
{
    return ((int)rc_nid[ANDBIT_A][0] & (int)rc_nid[ANDBIT_B][0]);
}

inline double rc_intern_aSin()
{
    return asin(rc_nid[ASIN_N][0]);
}

inline double rc_intern_aTan()
{
    return atan(rc_nid[ATAN_N][0]);
}

string Convert(unsigned int val)
{
   unsigned int mask = 1 << (sizeof(int) * 8 - 1);
   string binstr = "";

   for(int i = 0; i < sizeof(int) * 8; i++)
   {
      if( (val & mask) == 0 )
         //cout << '0' ;
           binstr += "0";
      else
         //cout << '1' ;
           binstr += "1";

      mask  >>= 1;
   }
   return binstr;
}

inline string rc_intern_bin()
{
    //string bin_str = bitset<32>((int)rc_nid[BIN_N][0]).to_string();
    //bin_str = bin_str.substr(bin_str.find_first_not_of("0"));
    string binstr = Convert(rc_nid[BIN_N][0]);// bin_str;
    return binstr.substr(binstr.find_first_not_of("0"));
}

inline double rc_intern_cos()
{
    return cos(rc_nid[COS_ANGLE][0]);
}

inline double rc_intern_exp()
{
    return exp(rc_nid[EXP_N][0]);
}

inline double rc_intern_frac()
{
    return rc_nid[FRAC_N][0] - floor(rc_nid[FRAC_N][0]);
}

inline string rc_intern_hex()
{
    stringstream ss;
    ss << hex << (int)rc_nid[HEX_STR_VALUE][0];
    return ss.str();
}

inline int rc_intern_int()
{
    return (int)rc_nid[INT_N][0];
}

inline double rc_intern_log()
{
    return log(rc_nid[LOG_N][0]);
}

inline double rc_intern_max()
{
    if(rc_nid[MAX_A][0] > rc_nid[MAX_B][0])
        return rc_nid[MAX_A][0];
    else
        return rc_nid[MAX_B][0];
}

inline double rc_intern_min()
{
    if(rc_nid[MIN_A][0] < rc_nid[MIN_B][0])
        return rc_nid[MIN_A][0];
    else
        return rc_nid[MIN_B][0];
}

inline double rc_intern_orBit()
{
    return ((int)rc_nid[ORBIT_A][0] | (int)rc_nid[ORBIT_B][0]);
}

inline int rc_intern_randomize()
{
    srand(rc_nid[RANDOMIZE_N][0]);
    return 0;
}

inline double rc_intern_rand()
{
    return rand() % (int)rc_nid[RAND_N][0];
}

inline double rc_intern_round()
{
    return round(rc_nid[ROUND_N][0]);
}

inline int rc_intern_sign()
{
    if(rc_nid[SIGN_N][0] == 0)
        return 0;
    else if(rc_nid[SIGN_N][0] > 0)
        return 1;
    else
        return -1;
}

inline double rc_intern_sin()
{
    return sin(rc_nid[SIN_ANGLE][0]);
}

inline double rc_intern_sqr()
{
    return sqrt(rc_nid[SQR_N][0]);
}

inline double rc_intern_tan()
{
    return tan(rc_nid[TAN_ANGLE][0]);
}

inline double rc_intern_xorbit()
{
    return ((int)rc_nid[XORBIT_A][0] ^ (int)rc_nid[XORBIT_B][0]);
}

inline int rc_intern_fileOpen()
{
    int fo_stream = (int)rc_nid[FILEOPEN_STREAM][0];
    string fo_file = rc_sid[FILEOPEN_FILE_STR][0];
    int fo_mode = rc_nid[FILEOPEN_IO_MODE][0];
    if(fo_stream >= 0 && fo_stream < RC_MAX_FILES)
    {
        if(rc_fstream[fo_stream] != NULL)
            return 0;

        rc_eof[(int)rc_nid[FILEOPEN_STREAM][0]] = false;

        switch(fo_mode)
        {
            case RC_FILE_IO_TEXT_INPUT:
                //rc_fstream[fo_stream].open(fo_file.c_str(), fstream::in);
                rc_fstream[fo_stream] = SDL_RWFromFile(fo_file.c_str(), "r");
                //cout << "RC_DEBUG_TEXT_INPUT" << endl;
                break;
            case RC_FILE_IO_TEXT_OUTPUT:
                rc_fstream[fo_stream] = SDL_RWFromFile(fo_file.c_str(),"w");
                break;
            case RC_FILE_IO_TEXT_APPEND:
                rc_fstream[fo_stream] = SDL_RWFromFile(fo_file.c_str(),"a");
                break;
            case RC_FILE_IO_TEXT_INPUT_UPDATE:
                rc_fstream[fo_stream] = SDL_RWFromFile(fo_file.c_str(),"r+");
                break;
            case RC_FILE_IO_TEXT_OUTPUT_UPDATE:
                rc_fstream[fo_stream] = SDL_RWFromFile(fo_file.c_str(),"w+");
                break;
            case RC_FILE_IO_TEXT_APPEND_UPDATE:
                rc_fstream[fo_stream] = SDL_RWFromFile(fo_file.c_str(),"a+");
                break;
            case RC_FILE_IO_BINARY_INPUT:
                rc_fstream[fo_stream] = SDL_RWFromFile(fo_file.c_str(),"rb");
                break;
            case RC_FILE_IO_BINARY_OUTPUT:
                rc_fstream[fo_stream] = SDL_RWFromFile(fo_file.c_str(),"wb");
                break;
            case RC_FILE_IO_BINARY_APPEND:
                rc_fstream[fo_stream] = SDL_RWFromFile(fo_file.c_str(),"ab");
                break;
            case RC_FILE_IO_BINARY_INPUT_UPDATE:
                rc_fstream[fo_stream] = SDL_RWFromFile(fo_file.c_str(),"rb+");
                break;
            case RC_FILE_IO_BINARY_OUTPUT_UPDATE:
                rc_fstream[fo_stream] = SDL_RWFromFile(fo_file.c_str(),"wb+");
                break;
            case RC_FILE_IO_BINARY_APPEND_UPDATE:
                rc_fstream[fo_stream] = SDL_RWFromFile(fo_file.c_str(),"ab+");
                break;
        }
        if(rc_fstream[fo_stream] == NULL)
        {
            //cout << "RC_DEBUG: FILE NOT OPEN" << endl;
            return 0;
        }
    }
    return 1;
}

inline int rc_intern_fileClose()
{
    rc_eof[(int)rc_nid[FILECLOSE_STREAM][0]] = false;
    int fc_stream = (int)rc_nid[FILECLOSE_STREAM][0];
    if(rc_fstream[fc_stream] != NULL)
    {
        SDL_RWclose(rc_fstream[fc_stream]);
        rc_fstream[fc_stream] = NULL;
        return 1;
    }
    else
        return 0;
}

inline int rc_intern_fileReadByte()
{
    unsigned char buf;
    unsigned char rb = SDL_RWread(rc_fstream[(int)rc_nid[READBYTE_STREAM][0]], &buf, 1, 1);
    return (int)rb;
}

inline int rc_intern_fileWriteByte()
{
    char wb = (char)rc_nid[WRITEBYTE_BYTE][0];
    SDL_RWwrite(rc_fstream[(int)rc_nid[WRITEBYTE_STREAM][0]], &wb, 1, 1);
    return 1;
}

inline string rc_intern_fileReadLine()
{
    string rline = "";
    unsigned char buf[1];
    if(SDL_RWread(rc_fstream[(int)rc_nid[READLINE_STR_STREAM][0]], buf, 1, 1)==0)
    {
        rc_eof[(int)rc_nid[READLINE_STR_STREAM][0]] = true;
        return "";
    }
    while(buf[0]!='\0' && buf[0]!='\n' && buf[0]!='\r')
    {
        rline.append(1,buf[0]);
        if(SDL_RWread(rc_fstream[(int)rc_nid[READLINE_STR_STREAM][0]], buf, 1, 1)==0)
        {
            rc_eof[(int)rc_nid[READLINE_STR_STREAM][0]] = true;
            break;
        }
    }
    if(buf[0]=='\r')
        SDL_RWread(rc_fstream[(int)rc_nid[READLINE_STR_STREAM][0]], buf, 1, 1);
    return rline;
}

inline int rc_intern_fileWriteLine()
{
    //cout << "DEBUG: WRITELINE" << endl;
    if(rc_fstream[(int)rc_nid[WRITELINE_STREAM][0]]!=NULL)
        SDL_RWwrite(rc_fstream[(int)rc_nid[WRITELINE_STREAM][0]], rc_sid[WRITELINE_LINE_STR][0].c_str(), 1, rc_sid[WRITELINE_LINE_STR][0].length());
    //cout << "WRITELINE_END" << endl;
    return 1;
}

inline int rc_intern_fileCopy()
{
    std::ifstream  src(rc_sid[COPYFILE_SRC_STR][0].c_str(), std::ios::binary);
    std::ofstream  dst(rc_sid[COPYFILE_DEST_STR][0].c_str(), std::ios::binary);
    dst << src.rdbuf();
    src.close();
    dst.close();
    return 1;
}

inline int rc_intern_fileDelete()
{
    if(remove(rc_sid[REMOVEFILE_FILE_STR][0].c_str())==0)
        return 1;
    else
        return 0;
}

inline int rc_intern_fileExist()
{
    std::ifstream infile(rc_sid[FILEEXISTS_FILE_STR][0].c_str());
    bool fx = infile.good();
    infile.close();
    return (int)fx;
}

inline int rc_intern_fileMove()
{
    int fm = rename(rc_sid[MOVEFILE_SRC_STR][0].c_str(), rc_sid[MOVEFILE_DEST_STR][0].c_str());
    if(fm == 0)
        return 1;
    else
        return 0;
}

inline int rc_intern_fileRename()
{
    int fm = rename(rc_sid[RENAMEFILE_FILE_STR][0].c_str(), rc_sid[RENAMEFILE_RENAME_STR][0].c_str());
    if(fm == 0)
        return 1;
    else
        return 0;
}

inline unsigned long rc_intern_fileLength()
{
    //struct stat st;
    SDL_RWops * fl_file = SDL_RWFromFile(rc_sid[FILELENGTH_FILE_STR][0].c_str(), "r");
    int fl_size = SDL_RWsize(fl_file);
    SDL_RWclose(fl_file);
    return fl_size;
}

inline long rc_intern_fileTell()
{
    return SDL_RWtell(rc_fstream[TELL_STREAM]);
}

inline unsigned long rc_intern_fileSeek()
{
    return SDL_RWseek(rc_fstream[SEEK_STREAM],rc_nid[SEEK_POSITION][0],RW_SEEK_SET);
}

inline int rc_intern_eof()
{
    //Uint32 current_pos = SDL_RWtell(rc_fstream[(int)rc_nid[EOF_STREAM][0]]);
    //Uint32 current_pos = SDL_RWseek(rc_fstream[(int)rc_nid[EOF_STREAM][0],0,RW_SEEK_CUR);
    //Uint32 file_eof = SDL_RWseek(rc_fstream[(int)rc_nid[EOF_STREAM][0]],0,RW_SEEK_END);
    //SDL_RWseek(rc_fstream[(int)rc_nid[EOF_STREAM][0]],0,current_pos);
    return rc_eof[(int)rc_nid[EOF_STREAM][0]];
}

inline int rc_intern_freeFile()
{
    for(int i = 1; i <= 16; i++)
        if(rc_fstream[i] == NULL)
        return i;
    return 0;
}

inline int rc_intern_dirChange()
{
    if(chdir(rc_sid[CHANGEDIR_PATH_STR][0].c_str())!=0)
    {
        //cout << "Error: Could not change directory\n";
        return 2;
    }
    rc_dir_path = rc_sid[CHANGEDIR_PATH_STR][0];
    return 0;
}

inline int rc_intern_dirExist()
{
    struct stat info;

    if(stat( rc_sid[DIREXISTS_PATH_STR][0].c_str(), &info ) != 0)
        return 0;
    else if(info.st_mode & S_IFDIR)
        return 1;
    else
        return 0;
}

inline string rc_intern_dirFirst ()
{
    rc_dir = opendir (rc_dir_path.c_str());
    //string s = "";
    //__android_log_print(ANDROID_LOG_ERROR, "RC_DEBUG_DIRFIRST", "%s", rc_dir_path.c_str());

    if ((rc_entry = readdir (rc_dir)) != NULL)
    {
        //cout << "ERROR: " << rc_entry->d_name;
        return rc_entry->d_name;
    }
    return "";
}

string getcwd_str()
{
    char *buffer = new char[MAXPATHLEN];
    getcwd(buffer,MAXPATHLEN);
    if(buffer != NULL)
    {
        string ret(buffer);
        delete[] buffer;
        return ret;
    }
    else
    {
        return string();
    }
}

inline string rc_intern_dir()
{
    string d = getcwd_str();
    //__android_log_print(ANDROID_LOG_ERROR, "RC_DEBUG_DIR", "%s", SDL_GetPrefPath("rcbasic","lucky"));
    if(d.compare("")==0)
    {
        //cout << "Could not get current directory" << endl;
        return "";
    }
    rc_dir_path = d;
    return d;
}

inline string rc_intern_dirNext()
{
    if( (rc_entry = readdir(rc_dir))!=NULL)
        return rc_entry->d_name;
    return "";
}

inline int rc_intern_dirCreate()
{
    if(mkdir(rc_sid[MAKEDIR_PATH_STR][0].c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)!=0)
    {
        //cout << "ERROR: Could not make directory" << endl;
        return 0;
    }
    return 1;
}

inline int rc_intern_dirDelete()
{
    if(rmdir(rc_sid[REMOVEDIR_PATH_STR][0].c_str())!=0)
    {
        //cout << "ERROR: Could not delete directory" << endl;
        return 0;
    }
    return 1;
}

inline string rc_intern_date()
{
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    stringstream d;
    if( now->tm_mon+1 < 10)
        d << "0" << now->tm_mon + 1 << "-";
    else
        d << now->tm_mon+1 << "-";
    if(now->tm_mday < 10)
        d << "0" << now->tm_mday << "-";
    else
        d<< now->tm_mday << "-";
    d << (now->tm_year + 1900);
    return d.str();
}

inline unsigned long rc_intern_ticks()
{
    return SDL_GetTicks();
}

inline string rc_intern_time()
{
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    stringstream d;
    if( now->tm_hour < 10)
        d << "0" << now->tm_hour << ":";
    else
        d << now->tm_hour << ":";

    if(now->tm_min < 10)
        d << "0" << now->tm_min << ":";
    else
        d<< now->tm_min << ":";

    if(now->tm_sec < 10)
        d << "0" << now->tm_sec;
    else
        d << now->tm_sec;
    return d.str();
}

inline double rc_intern_timer()
{
    return clock() / (double)(CLOCKS_PER_SEC / 1000);
}

inline int rc_intern_wait()
{
    clock_t end_t = clock() + (((unsigned long)rc_nid[WAIT_MSEC][0]/1000)*CLOCKS_PER_SEC);
    //cout << (unsigned long)r << endl;
    //string s; cin >> s;
    while(clock() < end_t)
    {
        //cout << clock() << endl;
    }
    return 0;
}

inline void rc_intern_system(string rc_sys_cmd)
{
    system(rc_sys_cmd.c_str());
}

int rc_getNSize(int nid)
{
    if(rc_nid_w[nid]>0)
    {
        if(rc_nid_h[nid]>0)
        {
            if(rc_nid_d[nid]>0)
            {
                return rc_nid_w[nid]*rc_nid_h[nid]*rc_nid_d[nid];
            }
            return rc_nid_w[nid]*rc_nid_h[nid];
        }
        return rc_nid_w[nid];
    }
    return 0;
}

inline string rc_intern_OS()
{
    return "ANDROID";
}

inline string rc_intern_command(int num)
{
    if(num < rc_cmd_count)
    {
        return rc_cmd_args[num];
    }
    return "";
}

inline double rc_intern_numCommands()
{
    return (double)rc_cmd_count;
}

inline string rc_intern_env()
{
    const char * c = getenv(rc_sid[ENV_STR_V_STR][0].c_str());
    if(c != NULL)
        return (string) c;
    return "";
}

inline int rc_intern_setEnv()
{
    return putenv(rc_sid[SETENV_V_STR][0].c_str());
}

inline string rc_intern_prefPath()
{
    return (string) SDL_GetPrefPath(rc_sid[PREFPATH_STR_ORG_NAME_STR][0].c_str(), rc_sid[PREFPATH_STR_APP_NAME_STR][0].c_str());
}

inline void rc_vm_intern()
{
    //cout << "<intern>";
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 0:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            break;
        case 1:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            i_val1 = (unsigned int)rc_vm_m[i_val1];
            break;
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            i_val1 = (unsigned int)rc_nid[i_val1][0];
            break;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    //cout << i_val1 << endl;
    switch(i_val1)
    {
        case 0:
            cout << fixed << rc_sid[FPRINT_S_STR][0];
            rc_num_stack[rc_num_stack_index] = 0;
            rc_num_stack_index++;
            break;
        case 1:
            cout << rc_sid[INPUT_STR_PROMPT][0];
            getline(cin,rc_str_stack[rc_str_stack_index]);
            rc_str_stack_index++;
            break;
        case 3:
            rc_num_stack[rc_num_stack_index] = rc_intern_arrayDimensions();
            rc_num_stack_index++;
            break;
        case 4:
            rc_num_stack[rc_num_stack_index] = rc_intern_arraySize();
            rc_num_stack_index++;
            break;
        case 5:
            rc_num_stack[rc_num_stack_index] = rc_intern_asc();
            rc_num_stack_index++;
            break;
        case 6:
            rc_str_stack[rc_str_stack_index] = rc_intern_chr();
            rc_str_stack_index++;
            break;
        case 7:
            rc_str_stack[rc_str_stack_index] = rc_intern_insert();
            rc_str_stack_index++;
            break;
        case 8:
            rc_num_stack[rc_num_stack_index] = rc_intern_instr();
            rc_num_stack_index++;
            break;
        case 9:
            rc_str_stack[rc_str_stack_index] = rc_intern_lcase();
            rc_str_stack_index++;
            break;
        case 10:
            rc_str_stack[rc_str_stack_index] = rc_intern_left();
            rc_str_stack_index++;
            break;
        case 11:
            rc_num_stack[rc_num_stack_index] = rc_intern_length();
            rc_num_stack_index++;
            break;
        case 12:
            rc_str_stack[rc_str_stack_index] = rc_intern_ltrim();
            rc_str_stack_index++;
            break;
        case 13:
            rc_str_stack[rc_str_stack_index] = rc_intern_mid();
            rc_str_stack_index++;
            break;
        case 14:
            rc_str_stack[rc_str_stack_index] = rc_intern_replace();
            rc_str_stack_index++;
            break;
        case 15:
            rc_str_stack[rc_str_stack_index] = rc_intern_replaceSubstr();
            rc_str_stack_index++;
            break;
        case 16:
            rc_str_stack[rc_str_stack_index] = rc_intern_reverse();
            rc_str_stack_index++;
            break;
        case 17:
            rc_str_stack[rc_str_stack_index] = rc_intern_right();
            rc_str_stack_index++;
            break;
        case 18:
            rc_str_stack[rc_str_stack_index] = rc_intern_rtrim();
            rc_str_stack_index++;
            break;
        case 19:
            rc_str_stack[rc_str_stack_index] = rc_intern_fillstring();
            rc_str_stack_index++;
            break;
        case 20:
            rc_str_stack[rc_str_stack_index] = rc_intern_str();
            rc_str_stack_index++;
            break;
        case 21:
            rc_num_stack[rc_num_stack_index] = rc_intern_tally();
            rc_num_stack_index++;
            break;
        case 22:
            rc_str_stack[rc_str_stack_index] = rc_intern_trim();
            rc_str_stack_index++;
            break;
        case 23:
            rc_str_stack[rc_str_stack_index] = rc_intern_ucase();
            rc_str_stack_index++;
            break;
        case 24:
            rc_num_stack[rc_num_stack_index] = rc_intern_val();
            rc_num_stack_index++;
            break;
        case 25:
            rc_num_stack[rc_num_stack_index] = rc_intern_abs();
            rc_num_stack_index++;
            break;
        case 26:
            rc_num_stack[rc_num_stack_index] = rc_intern_aCos();
            rc_num_stack_index++;
            break;
        case 27:
            rc_num_stack[rc_num_stack_index] = rc_intern_andBit();
            rc_num_stack_index++;
            break;
        case 28:
            rc_num_stack[rc_num_stack_index] = rc_intern_aSin();
            rc_num_stack_index++;
            break;
        case 29:
            rc_num_stack[rc_num_stack_index] = rc_intern_aTan();
            rc_num_stack_index++;
            break;
        case 30:
            rc_str_stack[rc_str_stack_index] = rc_intern_bin();
            rc_str_stack_index++;
            break;
        case 31:
            rc_num_stack[rc_num_stack_index] = rc_intern_cos();
            rc_num_stack_index++;
            break;
        case 32:
            rc_num_stack[rc_num_stack_index] = rc_intern_exp();
            rc_num_stack_index++;
            break;
        case 33:
            rc_num_stack[rc_num_stack_index] = rc_intern_frac();
            rc_num_stack_index++;
            break;
        case 34:
            rc_str_stack[rc_str_stack_index] = rc_intern_hex();
            rc_str_stack_index++;
            break;
        case 35:
            rc_num_stack[rc_num_stack_index] = rc_intern_int();
            rc_num_stack_index++;
            break;
        case 36:
            rc_num_stack[rc_num_stack_index] = rc_intern_log();
            rc_num_stack_index++;
            break;
        case 37:
            rc_num_stack[rc_num_stack_index] = rc_intern_max();
            rc_num_stack_index++;
            break;
        case 38:
            rc_num_stack[rc_num_stack_index] = rc_intern_min();
            rc_num_stack_index++;
            break;
        case 39:
            rc_num_stack[rc_num_stack_index] = rc_intern_orBit();
            rc_num_stack_index++;
            break;
        case 40:
            rc_num_stack[rc_num_stack_index] = rc_intern_randomize();
            rc_num_stack_index++;
            break;
        case 41:
            rc_num_stack[rc_num_stack_index] = rc_intern_rand();
            rc_num_stack_index++;
            break;
        case 42:
            rc_num_stack[rc_num_stack_index] = rc_intern_round();
            rc_num_stack_index++;
            break;
        case 43:
            rc_num_stack[rc_num_stack_index] = rc_intern_sign();
            rc_num_stack_index++;
            break;
        case 44:
            rc_num_stack[rc_num_stack_index] = rc_intern_sin();
            rc_num_stack_index++;
            break;
        case 45:
            rc_num_stack[rc_num_stack_index] = rc_intern_sqr();
            rc_num_stack_index++;
            break;
        case 46:
            rc_num_stack[rc_num_stack_index] = rc_intern_tan();
            rc_num_stack_index++;
            break;
        case 47:
            rc_num_stack[rc_num_stack_index] = rc_intern_xorbit();
            rc_num_stack_index++;
            break;
        case 48:
            rc_num_stack[rc_num_stack_index] = rc_intern_fileOpen();
            rc_num_stack_index++;
            break;
        case 49:
            rc_num_stack[rc_num_stack_index] = rc_intern_fileClose();
            rc_num_stack_index++;
            break;
        case 50:
            rc_num_stack[rc_num_stack_index] = rc_intern_fileReadByte();
            rc_num_stack_index++;
            break;
        case 51:
            rc_num_stack[rc_num_stack_index] = rc_intern_fileWriteByte();
            rc_num_stack_index++;
            break;
        case 52:
            rc_str_stack[rc_str_stack_index] = rc_intern_fileReadLine();
            rc_str_stack_index++;
            break;
        case 53:
            rc_num_stack[rc_num_stack_index] = rc_intern_fileWriteLine();
            rc_num_stack_index++;
            break;
        case 54:
            rc_num_stack[rc_num_stack_index] = rc_intern_fileCopy();
            rc_num_stack_index++;
            break;
        case 55:
            rc_num_stack[rc_num_stack_index] = rc_intern_fileDelete();
            rc_num_stack_index++;
            break;
        case 56:
            rc_num_stack[rc_num_stack_index] = rc_intern_fileExist();
            rc_num_stack_index++;
            break;
        case 57:
            rc_num_stack[rc_num_stack_index] = rc_intern_fileMove();
            rc_num_stack_index++;
            break;
        case 58:
            rc_num_stack[rc_num_stack_index] = rc_intern_fileRename();
            rc_num_stack_index++;
            break;
        case 59:
            rc_num_stack[rc_num_stack_index] = rc_intern_fileLength();
            rc_num_stack_index++;
            break;
        case 60:
            rc_num_stack[rc_num_stack_index] = rc_intern_fileTell();
            rc_num_stack_index++;
            break;
        case 61:
            rc_num_stack[rc_num_stack_index] = rc_intern_fileSeek();
            rc_num_stack_index++;
            break;
        case 62:
            rc_num_stack[rc_num_stack_index] = rc_intern_eof();
            rc_num_stack_index++;
            break;
        case 63:
            rc_num_stack[rc_num_stack_index] = rc_intern_freeFile();
            rc_num_stack_index++;
            break;
        case 64:
            rc_num_stack[rc_num_stack_index] = rc_intern_dirChange();
            rc_num_stack_index++;
            break;
        case 65:
            rc_num_stack[rc_num_stack_index] = rc_intern_dirExist();
            rc_num_stack_index++;
            break;
        case 66:
            rc_str_stack[rc_str_stack_index] = rc_intern_dirFirst();
            rc_str_stack_index++;
            break;
        case 67:
            rc_str_stack[rc_str_stack_index] = rc_intern_dir();
            rc_str_stack_index++;
            break;
        case 68:
            rc_str_stack[rc_str_stack_index] = rc_intern_dirNext();
            rc_str_stack_index++;
            break;
        case 69:
            rc_num_stack[rc_num_stack_index] = rc_intern_dirCreate();
            rc_num_stack_index++;
            break;
        case 70:
            rc_num_stack[rc_num_stack_index] = rc_intern_dirDelete();
            rc_num_stack_index++;
            break;
        case 71:
            rc_str_stack[rc_str_stack_index] = rc_intern_date();
            rc_str_stack_index++;
            break;
        case 72:
            rc_num_stack[rc_num_stack_index] = rc_intern_ticks();
            rc_num_stack_index++;
            break;
        case 73:
            rc_str_stack[rc_str_stack_index] = rc_intern_time();
            rc_str_stack_index++;
            break;
        case 74:
            rc_num_stack[rc_num_stack_index] = rc_intern_timer();
            rc_num_stack_index++;
            break;
        case 75:
            rc_media_wait(rc_nid[WAIT_MSEC][0]);
            //cout << "\nWAITING\n";
            rc_num_stack[rc_num_stack_index] = 0;//rc_intern_wait();
            rc_num_stack_index++;
            break;
        case 76:
            //cout << "open window\n";
            rc_media_openWindow_hw((int)rc_nid[WINDOWOPEN_WIN_NUM][0],rc_sid[WINDOWOPEN_TITLE_STR][0],(int)rc_nid[WINDOWOPEN_X][0],(int)rc_nid[WINDOWOPEN_Y][0],(int)rc_nid[WINDOWOPEN_W][0],(int)rc_nid[WINDOWOPEN_H][0],(int)rc_nid[WINDOWOPEN_MODE][0]);
            break;
        case 77:
            //cout << "close window\n";
            rc_media_closeWindow_hw((int)rc_nid[WINDOWCLOSE_WIN_NUM][0]);
            break;
        case 78:
            rc_media_raiseWindow(rc_active_window);
            break;
        case 79:
            rc_media_setActiveWindow((int)rc_nid[WINDOW_WIN_NUM][0]);
            break;
        case 80:
            rc_media_updateWindow_hw();
            break;
        case 81:
            rc_media_showWindow(rc_active_window);
            break;
        case 82:
            rc_media_hideWindow(rc_active_window);
            break;
        case 83:
            rc_media_setWindowTitle(rc_active_window,rc_sid[SETWINDOWTITLE_TITLE_STR][0]);
            break;
        case 84:
            rc_str_stack[rc_str_stack_index] = rc_media_getWindowTitle(rc_active_window);
            rc_str_stack_index++;
            break;
        case 85:
            rc_media_setWindowPosition(rc_active_window, rc_nid[SETWINDOWPOSITION_X][0],rc_nid[SETWINDOWPOSITION_Y][0]);
            break;
        case 86:
            rc_media_getWindowPosition(rc_active_window,&rc_nid[GETWINDOWPOSITION_X][0],&rc_nid[GETWINDOWPOSITION_Y][0]);
            break;
        case 87:
            rc_media_setWindowSize(rc_active_window,rc_nid[SETWINDOWSIZE_W][0],rc_nid[SETWINDOWSIZE_H][0]);
            break;
        case 88:
            rc_media_getWindowSize(rc_active_window,&rc_nid[GETWINDOWSIZE_W][0],&rc_nid[GETWINDOWSIZE_H][0]);
            break;
        case 89:
            rc_media_setWindowMinSize(rc_active_window,rc_nid[SETWINDOWMINSIZE_W][0],rc_nid[SETWINDOWMINSIZE_H][0]);
            break;
        case 90:
            rc_media_getWindowMinSize(rc_active_window,&rc_nid[GETWINDOWMINSIZE_W][0],&rc_nid[GETWINDOWMINSIZE_H][0]);
            break;
        case 91:
            rc_media_setWindowMaxSize(rc_active_window,rc_nid[SETWINDOWMAXSIZE_W][0],rc_nid[SETWINDOWMAXSIZE_H][0]);
            break;
        case 92:
            rc_media_getWindowMaxSize(rc_active_window,&rc_nid[GETWINDOWMAXSIZE_W][0],&rc_nid[GETWINDOWMAXSIZE_H][0]);
            break;
        case 93:
            rc_num_stack[rc_num_stack_index] = rc_media_windowIsFullscreen(rc_active_window);
            rc_num_stack_index++;
            break;
        case 94:
            rc_num_stack[rc_num_stack_index] = rc_media_windowIsVisible(rc_active_window);
            rc_num_stack_index++;
            break;
        case 95:
            rc_num_stack[rc_num_stack_index] = rc_media_windowIsBordered(rc_active_window);
            rc_num_stack_index++;
            break;
        case 96:
            rc_num_stack[rc_num_stack_index] = rc_media_windowIsResizable(rc_active_window);
            rc_num_stack_index++;
            break;
        case 97:
            rc_num_stack[rc_num_stack_index] = rc_media_windowIsMinimized(rc_active_window);
            rc_num_stack_index++;
            break;
        case 98:
            rc_num_stack[rc_num_stack_index] = rc_media_windowIsMaximized(rc_active_window);
            rc_num_stack_index++;
            break;
        case 99:
            rc_media_setWindowFullscreen(rc_active_window, rc_nid[SETWINDOWFULLSCREEN_FLAG][0]);
            break;
        case 100:
            rc_media_maximizeWindow(rc_active_window);
            break;
        case 101:
            rc_media_minimizeWindow(rc_active_window);
            break;
        case 102:
            rc_media_setWindowBordered(rc_active_window, (SDL_bool)rc_nid[SETWINDOWBORDER_FLAG][0]);
            break;
        case 104:
            rc_media_openScreen_hw(rc_nid[CANVASOPEN_S_NUM][0],rc_nid[CANVASOPEN_W][0],rc_nid[CANVASOPEN_H][0],rc_nid[CANVASOPEN_VPX][0],rc_nid[CANVASOPEN_VPY][0],rc_nid[CANVASOPEN_VPW][0],rc_nid[CANVASOPEN_VPH][0],rc_nid[CANVASOPEN_FLAG][0]);
            break;
        case 105:
            rc_media_closeScreen_hw(rc_nid[CANVASCLOSE_S_NUM][0]);
            break;
        case 106:
            rc_media_setScreenViewport(rc_nid[SETCANVASVIEWPORT_X][0],rc_nid[SETCANVASVIEWPORT_Y][0],rc_nid[SETCANVASVIEWPORT_W][0],rc_nid[SETCANVASVIEWPORT_H][0]);
            break;
        case 107:
            rc_media_getScreenViewport(&rc_nid[GETCANVASVIEWPORT_X][0],&rc_nid[GETCANVASVIEWPORT_Y][0],&rc_nid[GETCANVASVIEWPORT_W][0],&rc_nid[GETCANVASVIEWPORT_H][0]);
            break;
        case 108:
            rc_media_screen_hw(rc_nid[CANVAS_S_NUM][0]);
            break;
        case 109:
            rc_media_screenOffset((int)rc_nid[SETCANVASOFFSET_X][0],(int)rc_nid[SETCANVASOFFSET_Y][0]);
            break;
        case 110:
            rc_media_getScreenOffset(&rc_nid[GETCANVASOFFSET_X][0],&rc_nid[GETCANVASOFFSET_Y][0]);
            break;
        case 111:
            rc_media_getScreenSize(&rc_nid[GETCANVASSIZE_W][0],&rc_nid[GETCANVASSIZE_H][0]);
            break;
        case 112:
            //cout << "clearing screen\n";
            rc_media_clearScreen_hw();
            break;
        case 113:
            rc_media_setScreenAlpha_hw((Uint8)rc_nid[SETCANVASALPHA_ALPHA][0]);
            break;
        case 114:
            rc_media_copyScreen_hw(rc_nid[COPYCANVAS_SRC_SCREEN][0],rc_nid[COPYCANVAS_X][0],rc_nid[COPYCANVAS_Y][0],rc_nid[COPYCANVAS_W][0],rc_nid[COPYCANVAS_H][0],rc_nid[COPYCANVAS_DST_SCREEN][0],rc_nid[COPYCANVAS_DST_X][0],rc_nid[COPYCANVAS_DST_Y][0]);
            break;
        case 115:
            rc_media_cloneScreen_hw(rc_nid[CLONECANVAS_SRC_SCREEN][0],rc_nid[CLONECANVAS_DST_SCREEN][0]);
            break;
        case 116:
            rc_media_setScreenZ(rc_nid[SETCANVASZ_Z][0]);
            break;
        case 117:
            rc_media_box_hw((int)rc_nid[BOX_X1][0],(int)rc_nid[BOX_Y1][0],(int)rc_nid[BOX_X2][0],(int)rc_nid[BOX_Y2][0]);
            break;
        case 118:
            rc_media_boxFill_hw((int)rc_nid[BOXFILL_X1][0],(int)rc_nid[BOXFILL_Y1][0],(int)rc_nid[BOXFILL_X2][0],(int)rc_nid[BOXFILL_Y2][0]);
            break;
        case 119:
            rc_media_circle_hw(rc_nid[CIRCLE_X][0],rc_nid[CIRCLE_Y][0],rc_nid[CIRCLE_RADIUS][0]);
            break;
        case 120:
            rc_media_circleFill_hw(rc_nid[CIRCLEFILL_X][0],rc_nid[CIRCLEFILL_Y][0],rc_nid[CIRCLEFILL_RADIUS][0]);
            break;
        case 121:
            rc_media_ellipse_hw(rc_nid[ELLIPSE_X][0],rc_nid[ELLIPSE_Y][0],rc_nid[ELLIPSE_RADIUS_X][0],rc_nid[ELLIPSE_RADIUS_Y][0]);
            break;
        case 122:
            rc_media_ellipseFill_hw(rc_nid[ELLIPSEFILL_X][0],rc_nid[ELLIPSEFILL_Y][0],rc_nid[ELLIPSEFILL_RADIUS_X][0],rc_nid[ELLIPSEFILL_RADIUS_Y][0]);
            break;
        case 123:
            rc_num_stack[rc_num_stack_index] = rc_media_getPixel_hw(rc_nid[GETPIXEL_X][0],rc_nid[GETPIXEL_Y][0]);
            rc_num_stack_index++;
            break;
        case 124:
            rc_media_ink((Uint32)rc_nid[SETCOLOR_COLOR][0]);
            break;
        case 125:
            rc_media_line_hw((int)rc_nid[LINE_X1][0],(int)rc_nid[LINE_Y1][0],(int)rc_nid[LINE_X2][0],(int)rc_nid[LINE_Y2][0]);
            break;
        case 126:
            rc_media_floodFill_hw(rc_nid[FLOODFILL_X][0],rc_nid[FLOODFILL_Y][0]);
            break;
        case 127:
            rc_num_stack[rc_num_stack_index] = rc_media_rgb((Uint8)rc_nid[RGB_R][0],(Uint8)rc_nid[RGB_G][0],(Uint8)rc_nid[RGB_B][0]);
            //cout << "stack entry = " << (Uint32)rc_num_stack[rc_num_stack_index] << endl;
            rc_num_stack_index++;
            break;
        case 128:
            rc_num_stack[rc_num_stack_index] = rc_media_rgba((Uint8)rc_nid[RGBA_R][0],(Uint8)rc_nid[RGBA_G][0],(Uint8)rc_nid[RGBA_B][0],(Uint8)rc_nid[RGBA_A][0]);
            rc_num_stack_index++;
            break;
        case 129:
            rc_media_drawPixel_hw(rc_nid[PIXEL_X][0],rc_nid[PIXEL_Y][0]);
            break;
        case 130:
            rc_media_loadImage_hw(rc_nid[LOADIMAGE_SLOT][0],rc_sid[LOADIMAGE_IMG_FILE_STR][0]);
            break;
        case 131:
            rc_media_colorKey_hw(rc_nid[COLORKEY_SLOT][0],rc_nid[COLORKEY_COLOR][0]);
            break;
        case 132:
            rc_media_copyImage_hw(rc_nid[COPYIMAGE_SRC_SLOT][0],rc_nid[COPYIMAGE_DST_SLOT][0]);
            break;
        case 133:
            rc_media_deleteImage_hw(rc_nid[DELETEIMAGE_SLOT][0]);
            break;
        case 134:
            rc_media_setImageAlpha_hw(rc_nid[SETIMAGEALPHA_SLOT][0],(Uint8)rc_nid[SETIMAGEALPHA_ALPHA][0]);
            break;
        case 135:
            rc_media_getImageSize_hw(rc_nid[GETIMAGESIZE_SLOT][0],&rc_nid[GETIMAGESIZE_W][0],&rc_nid[GETIMAGESIZE_H][0]);
            break;
        case 136:
            rc_media_MirrorImage_hw(rc_nid[FLIPIMAGE_SLOT][0],rc_nid[FLIPIMAGE_FLIP_H][0],rc_nid[FLIPIMAGE_FLIP_V][0]);
            break;
        case 137:
            rc_media_drawImage_hw(rc_nid[DRAWIMAGE_SLOT][0],rc_nid[DRAWIMAGE_X][0],rc_nid[DRAWIMAGE_Y][0]);
            break;
        case 138:
            rc_media_rotateImage_hw(rc_nid[DRAWIMAGE_ROTATE_SLOT][0],rc_nid[DRAWIMAGE_ROTATE_X][0],rc_nid[DRAWIMAGE_ROTATE_Y][0],rc_nid[DRAWIMAGE_ROTATE_ANGLE][0]);
            break;
        case 139:
            rc_media_rotateImageEX_hw(rc_nid[DRAWIMAGE_ROTATE_EX_SLOT][0],rc_nid[DRAWIMAGE_ROTATE_EX_X][0],rc_nid[DRAWIMAGE_ROTATE_EX_Y][0], rc_nid[DRAWIMAGE_ROTATE_EX_SRC_X][0],
                                      rc_nid[DRAWIMAGE_ROTATE_EX_SRC_Y][0],rc_nid[DRAWIMAGE_ROTATE_EX_SRC_W][0],rc_nid[DRAWIMAGE_ROTATE_EX_SRC_H][0],rc_nid[DRAWIMAGE_ROTATE_EX_ANGLE][0]);
            break;
        case 140:
            rc_media_zoomImage_hw(rc_nid[DRAWIMAGE_ZOOM_SLOT][0],rc_nid[DRAWIMAGE_ZOOM_X][0],rc_nid[DRAWIMAGE_ZOOM_Y][0],rc_nid[DRAWIMAGE_ZOOM_ZOOM_X][0],rc_nid[DRAWIMAGE_ZOOM_ZOOM_Y][0]);
            break;
        case 141:
            rc_media_zoomImageEX_hw(rc_nid[DRAWIMAGE_ZOOM_EX_SLOT][0],rc_nid[DRAWIMAGE_ZOOM_EX_X][0],rc_nid[DRAWIMAGE_ZOOM_EX_Y][0],rc_nid[DRAWIMAGE_ZOOM_EX_SRC_X][0],rc_nid[DRAWIMAGE_ZOOM_EX_SRC_Y][0],
                                    rc_nid[DRAWIMAGE_ZOOM_EX_SRC_W][0],rc_nid[DRAWIMAGE_ZOOM_EX_SRC_H][0],rc_nid[DRAWIMAGE_ZOOM_EX_ZOOM_X][0],rc_nid[DRAWIMAGE_ZOOM_EX_ZOOM_Y][0]);
            break;
        case 142:
            rc_media_rotozoomImage_hw(rc_nid[DRAWIMAGE_ROTOZOOM_SLOT][0],rc_nid[DRAWIMAGE_ROTOZOOM_X][0],rc_nid[DRAWIMAGE_ROTOZOOM_Y][0],
                                      rc_nid[DRAWIMAGE_ROTOZOOM_ANGLE][0],rc_nid[DRAWIMAGE_ROTOZOOM_ZOOM_X][0],rc_nid[DRAWIMAGE_ROTOZOOM_ZOOM_Y][0]);
            break;
        case 143:
            rc_media_rotozoomImageEX_hw(rc_nid[DRAWIMAGE_ROTOZOOM_EX_SLOT][0],rc_nid[DRAWIMAGE_ROTOZOOM_EX_X][0],rc_nid[DRAWIMAGE_ROTOZOOM_EX_Y][0],rc_nid[DRAWIMAGE_ROTOZOOM_EX_SRC_X][0],rc_nid[DRAWIMAGE_ROTOZOOM_EX_SRC_Y][0],
                                        rc_nid[DRAWIMAGE_ROTOZOOM_EX_SRC_W][0],rc_nid[DRAWIMAGE_ROTOZOOM_EX_SRC_H][0],rc_nid[DRAWIMAGE_ROTOZOOM_EX_ANGLE][0],rc_nid[DRAWIMAGE_ROTOZOOM_EX_ZOOM_X][0],rc_nid[DRAWIMAGE_ROTOZOOM_EX_ZOOM_Y][0]);
            break;
        case 144:
            //drawImage_Clip()
            //cout << "DrawImage_Clip" << endl;
            rc_media_drawImage_Clip_hw(rc_nid[DRAWIMAGE_CLIP_SLOT][0], rc_nid[DRAWIMAGE_CLIP_X][0], rc_nid[DRAWIMAGE_CLIP_Y][0], rc_nid[DRAWIMAGE_CLIP_SRC_X][0], rc_nid[DRAWIMAGE_CLIP_SRC_Y][0],
                                       rc_nid[DRAWIMAGE_CLIP_SRC_W][0], rc_nid[DRAWIMAGE_CLIP_SRC_H][0]);
            break;
        case 145:
            //drawImage_Blit()
            //cout << "DrawImage_Blit" << endl;
            rc_media_drawImage_Blit_hw(rc_nid[DRAWIMAGE_BLIT_SLOT][0], rc_nid[DRAWIMAGE_BLIT_POS_X][0], rc_nid[DRAWIMAGE_BLIT_POS_Y][0], rc_nid[DRAWIMAGE_BLIT_POS_W][0], rc_nid[DRAWIMAGE_BLIT_POS_H][0],
                                       rc_nid[DRAWIMAGE_BLIT_SRC_X][0], rc_nid[DRAWIMAGE_BLIT_SRC_Y][0], rc_nid[DRAWIMAGE_BLIT_SRC_W][0], rc_nid[DRAWIMAGE_BLIT_SRC_H][0]);
            break;
        case 146:
            //getImageAlpha()
            //cout << "GetImageAlpha" << endl;
            rc_media_getImageAlpha_hw(rc_nid[GETIMAGEALPHA_SLOT][0], &rc_nid[GETIMAGEALPHA_ALPHA][0]);
            break;
        case 160:
            rc_media_getCursor(&rc_nid[GETCURSOR_X][0],&rc_nid[GETCURSOR_Y][0]);
            break;
        case 161:
            rc_media_showCursor();
            break;
        case 162:
            rc_media_hideCursor();
            break;
        case 163:
            //cout << "rc_prints -> " << rc_sid[PRINTS_TXT_STR][0] << endl;
            rc_media_printS_hw(rc_sid[PRINTS_TXT_STR][0]);
            break;
        case 164:
            //cout << "testing inputS\n";
            rc_str_stack[rc_str_stack_index] = rc_media_inputS_hw(rc_sid[INPUTS_STR_PROMPT_STR][0]);
            rc_str_stack_index++;
            break;
        case 165:
            rc_media_locate((int)rc_nid[LOCATE_X][0],(int)rc_nid[LOCATE_Y][0]);
            break;
        case 168:
            rc_media_drawText_hw(rc_sid[DRAWTEXT_TXT_STR][0],(int)rc_nid[DRAWTEXT_X][0],(int)rc_nid[DRAWTEXT_Y][0]);
            break;
        case 169:
            rc_num_stack[rc_num_stack_index] = rc_media_inkey();
            rc_num_stack_index++;
            break;
        case 170:
            rc_num_stack[rc_num_stack_index] = rc_media_key((int)rc_nid[KEY_KEY_CODE][0]);
            rc_num_stack_index++;
            break;
        case 171:
            rc_media_waitKey();
            break;
        case 172:
            rc_media_hideMouse();
            break;
        case 173:
            rc_media_showMouse();
            break;
        case 174:
            rc_num_stack[rc_num_stack_index] = rc_media_mouseIsVisible();
            rc_num_stack_index++;
            break;
        case 175:
            //cout << "getmouse\n";
            rc_media_getMouse(&rc_nid[GETMOUSE_X][0],&rc_nid[GETMOUSE_Y][0],&rc_nid[GETMOUSE_MB1][0],
                              &rc_nid[GETMOUSE_MB2][0],&rc_nid[GETMOUSE_MB3][0]);
            break;
        case 176:
            rc_media_getMouseWheel(&rc_nid[GETMOUSEWHEEL_X_AXIS][0],&rc_nid[GETMOUSEWHEEL_Y_AXIS][0]);
            break;
        case 177:
            rc_num_stack[rc_num_stack_index] = rc_media_imageExist_hw(rc_nid[IMAGEEXISTS_SLOT][0]);
            rc_num_stack_index++;
            break;
        case 178:
            rc_num_stack[rc_num_stack_index] = rc_media_numJoysticks();
            rc_num_stack_index++;
            break;
        case 179:
            rc_num_stack[rc_num_stack_index] = rc_media_joyAxis(rc_nid[JOYAXIS_JOY_NUM][0], rc_nid[JOYAXIS_AXIS][0]);
            rc_num_stack_index++;
            break;
        case 180:
            rc_num_stack[rc_num_stack_index] = rc_media_joyButton(rc_nid[JOYBUTTON_JOY_NUM][0], rc_nid[JOYBUTTON_JOY_BUTTON][0]);
            rc_num_stack_index++;
            break;
        case 181:
            rc_str_stack[rc_str_stack_index] = rc_media_joystickName(rc_nid[JOYNAME_STR_JOY_NUM][0]);
            rc_str_stack_index++;
            break;
        case 182:
            rc_media_loadSound(rc_nid[LOADSOUND_SLOT][0], rc_sid[LOADSOUND_SOUND_FILE_STR][0]);
            break;
        case 183:
            rc_media_loadMusic(rc_sid[LOADMUSIC_MUSIC_FILE_STR][0]);
            break;
        case 184:
            rc_media_playSound(rc_nid[PLAYSOUND_SLOT][0],rc_nid[PLAYSOUND_CHANNEL][0],rc_nid[PLAYSOUND_LOOPS][0]);
            break;
        case 185:
            rc_media_playSoundTimed(rc_nid[PLAYSOUNDTIMED_SLOT][0], rc_nid[PLAYSOUNDTIMED_CHANNEL][0], rc_nid[PLAYSOUNDTIMED_LOOPS][0], rc_nid[PLAYSOUNDTIMED_MS][0]);
            break;
        case 186:
            rc_media_playMusic(rc_nid[PLAYMUSIC_LOOPS][0]);
            break;
        case 187:
            rc_media_pauseSound(rc_nid[PAUSESOUND_CHANNEL][0]);
            break;
        case 188:
            rc_media_resumeSound(rc_nid[RESUMESOUND_CHANNEL][0]);
            break;
        case 189:
            rc_media_pauseMusic();
            break;
        case 190:
            rc_media_resumeMusic();
            break;
        case 191:
            rc_media_deleteSound(rc_nid[DELETESOUND_SLOT][0]);
            break;
        case 192:
            rc_media_fadeMusicIn(rc_nid[FADEMUSICIN_FADE_TIME][0], rc_nid[FADEMUSICIN_LOOPS][0]);
            break;
        case 193:
            rc_media_fadeMusicOut(rc_nid[FADEMUSICOUT_FADE_TIME][0]);
            break;
        case 194:
            rc_num_stack[rc_num_stack_index] = rc_media_musicExists();
            rc_num_stack_index++;
            break;
        case 195:
            rc_media_setMusicVolume(rc_nid[SETMUSICVOLUME_VOLUME][0]);
            break;
        case 196:
            rc_num_stack[rc_num_stack_index] = rc_media_getMusicVolume();
            rc_num_stack_index++;
            break;
        case 197:
            rc_media_setMusicPosition(rc_nid[SETMUSICPOSITION_MUSIC_POS][0]);
            break;
        case 198:
            rc_num_stack[rc_num_stack_index] = rc_media_getMusicPosition();
            rc_num_stack_index++;
            break;
        case 199:
            rc_media_rewindMusic();
            break;
        case 200:
            rc_media_setSoundChannels(rc_nid[SETSOUNDCHANNELS_MAX_CHANNELS][0]);
            break;
        case 201:
            rc_num_stack[rc_num_stack_index] = rc_media_soundIsEnabled();
            rc_num_stack_index++;
            break;
        case 202:
            rc_num_stack[rc_num_stack_index] = rc_media_soundExists(rc_nid[SOUNDEXISTS_SLOT][0]);
            rc_num_stack_index++;
            break;
        case 203:
            rc_media_setChannelVolume(rc_nid[SETCHANNELVOLUME_CHANNEL][0], rc_nid[SETCHANNELVOLUME_VOLUME][0]);
            break;
        case 204:
            rc_num_stack[rc_num_stack_index] = rc_media_getChannelVolume(rc_nid[CHANNELVOLUME_CHANNEL][0]);
            rc_num_stack_index++;
            break;
        case 205:
            rc_media_setSoundVolume(rc_nid[SETSOUNDVOLUME_SLOT][0], rc_nid[SETSOUNDVOLUME_VOLUME][0]);
            break;
        case 206:
            rc_num_stack[rc_num_stack_index] = rc_media_getSoundVolume(rc_nid[SOUNDVOLUME_SLOT][0]);
            rc_num_stack_index++;
            break;
        case 207:
            rc_media_stopMusic();
            break;
        case 208:
            rc_media_stopSound(rc_nid[STOPSOUND_CHANNEL][0]);
            break;
        case 209:
            rc_net_tcp_openSocket(rc_nid[TCP_SOCKETOPEN_SOCKET][0], rc_sid[TCP_SOCKETOPEN_HOST_STR][0], rc_nid[TCP_SOCKETOPEN_PORT][0]);
            break;
        case 210:
            rc_net_tcp_closeSocket(rc_nid[TCP_SOCKETCLOSE_SOCKET][0]);
            break;
        case 211:
            rc_num_stack[rc_num_stack_index] = rc_net_tcp_remoteHost(rc_nid[TCP_REMOTEHOST_SOCKET][0]);
            rc_num_stack_index++;
            break;
        case 212:
            rc_num_stack[rc_num_stack_index] = rc_net_tcp_remotePort(rc_nid[TCP_REMOTEPORT_SOCKET][0]);
            rc_num_stack_index++;
            break;
        case 213:
            rc_net_tcp_getData_str(rc_nid[TCP_GETDATA_SOCKET][0], &rc_sid[TCP_GETDATA_DST_STR][0], rc_nid[TCP_GETDATA_NUMBYTES][0]);
            break;
        case 214:
            //cout << "Sending: " << rc_sid[TCP_SENDDATA_S_DATA_STR][0] << endl;
            //cout << "";
            rc_net_tcp_sendData(rc_nid[TCP_SENDDATA_SOCKET][0], rc_sid[TCP_SENDDATA_S_DATA_STR][0].c_str(), rc_sid[TCP_SENDDATA_S_DATA_STR][0].length());
            break;
        case 215:
            //cout << "\n\nHUGE BUG\n\n";
            rc_num_stack[rc_num_stack_index] = rc_net_tcp_acceptSocket(rc_nid[TCP_ACCEPTSOCKET_SOCKET_SERVER][0], rc_nid[TCP_ACCEPTSOCKET_SOCKET_CLIENT][0]);
            rc_num_stack_index++;
            break;
        case 216:
            rc_net_udp_openSocket(rc_nid[UDP_SOCKETOPEN_SOCKET][0], rc_nid[UDP_SOCKETOPEN_PORT][0]);
            break;
        case 217:
            rc_num_stack[rc_num_stack_index] = rc_net_udp_readStream(rc_nid[UDP_GETDATA_SOCKET][0],&rc_sid[UDP_GETDATA_DST_STR][0],&rc_sid[UDP_GETDATA_HOST_STR][0],&rc_nid[UDP_GETDATA_PORT][0]);
            rc_num_stack_index++;
            break;
        case 219:
            rc_num_stack[rc_num_stack_index] = rc_net_udp_len();
            rc_num_stack_index++;
            break;
        case 222:
            rc_num_stack[rc_num_stack_index] = rc_net_udp_maxlen();
            rc_num_stack_index++;
            break;
        case 223:
            rc_str_stack[rc_str_stack_index] = rc_net_udp_getRemoteHost(rc_nid[UDP_REMOTEHOST_STR_SOCKET][0]);
            rc_str_stack_index++;
            break;
        case 224:
            rc_num_stack[rc_num_stack_index] = rc_net_udp_getRemotePort(rc_nid[UDP_REMOTEPORT_SOCKET][0]);
            rc_num_stack_index++;
            break;
        case 225:
            rc_net_udp_closeSocket(rc_nid[UDP_SOCKETCLOSE_SOCKET][0]);
            break;
        case 226:
            rc_num_stack[rc_num_stack_index] = rc_net_udp_sendData(rc_nid[UDP_SENDDATA_SOCKET][0], rc_sid[UDP_SENDDATA_HOST_STR][0], rc_nid[UDP_SENDDATA_PORT][0], rc_sid[UDP_SENDDATA_S_DATA_STR][0]);
            rc_num_stack_index++;
            break;
        case 227:
            rc_num_stack[rc_num_stack_index] = rc_media_touchPressure();
            rc_num_stack_index++;
            break;
        case 228:
            rc_media_getTouch(&rc_nid[GETTOUCH_STATUS][0],&rc_nid[GETTOUCH_X][0],&rc_nid[GETTOUCH_Y][0],&rc_nid[GETTOUCH_DX][0],&rc_nid[GETTOUCH_DY][0]);
            break;
        case 229:
            rc_media_getMultiTouch(&rc_nid[GETMULTITOUCH_STATUS][0],&rc_nid[GETMULTITOUCH_X][0],&rc_nid[GETMULTITOUCH_Y][0],&rc_nid[GETMULTITOUCH_NUMFINGERS][0],&rc_nid[GETMULTITOUCH_DIST][0],&rc_nid[GETMULTITOUCH_THETA][0]);
            break;
        case 230:
            rc_str_stack[rc_str_stack_index] = rc_media_ZoneInputS_hw(rc_nid[ZONEINPUT_STR_X][0], rc_nid[ZONEINPUT_STR_Y][0], rc_nid[ZONEINPUT_STR_W][0], rc_nid[ZONEINPUT_STR_H][0]);
            rc_str_stack_index++;
            break;
        case 233:
            rc_media_ReadInput_Start();
            break;
        case 234:
            rc_media_ReadInput_Stop();
            break;
        case 235:
            rc_media_getScreenClip_hw(rc_nid[CANVASCLIP_SLOT][0], rc_nid[CANVASCLIP_SRC_X][0], rc_nid[CANVASCLIP_SRC_Y][0], rc_nid[CANVASCLIP_SRC_W][0], rc_nid[CANVASCLIP_SRC_H][0]);
            break;
        case 236:
            rc_media_getWindowClip_hw(rc_nid[WINDOWCLIP_SLOT][0], rc_nid[WINDOWCLIP_SRC_X][0], rc_nid[WINDOWCLIP_SRC_Y][0], rc_nid[WINDOWCLIP_SRC_W][0], rc_nid[WINDOWCLIP_SRC_H][0]);
            break;
        case 237:
            //cout << "GetRenderedText" << endl;
            //getRenderedText()
            rc_media_GetRenderedText_hw((int)rc_nid[RENDERTEXT_SLOT][0], rc_sid[RENDERTEXT_TXT_STR][0]);
            break;
        case 238:
            rc_str_stack[rc_str_stack_index] = rc_media_ReadInput_Text();
            rc_str_stack_index++;
            break;
        case 240:
            //cout << "SetCanvasVisible" << endl;
            rc_media_setScreenVisible(rc_nid[SETCANVASVISIBLE_S_NUM][0], rc_nid[SETCANVASVISIBLE_FLAG][0]);
            break;
        case 241:
            //function
            //cout << "CanvasIsVisible" << endl;
            rc_num_stack[rc_num_stack_index] = rc_media_screenIsVisible(rc_nid[CANVASISVISIBLE_S_NUM][0]);
            rc_num_stack_index++;
            break;
        case 242:
            //cout << "CreateImage" << endl;
            rc_media_createImage_hw(rc_nid[CREATEIMAGE_SLOT][0],rc_nid[CREATEIMAGE_W][0],rc_nid[CREATEIMAGE_H][0],&rc_nid[CREATEIMAGE_PDATA][0],rc_getNSize(CREATEIMAGE_PDATA));
            break;
        case 243:
            //cout << "CreateImage_Ex" << endl;
            rc_media_createImage_Ex_hw(rc_nid[CREATEIMAGE_EX_SLOT][0],rc_nid[CREATEIMAGE_EX_W][0],rc_nid[CREATEIMAGE_EX_H][0],&rc_nid[CREATEIMAGE_EX_PDATA][0],rc_nid[CREATEIMAGE_EX_COLKEY][0],rc_getNSize(CREATEIMAGE_EX_PDATA));
            break;
        case 244:
            //function
            //cout << "FontIsLoaded" << endl;
            rc_num_stack[rc_num_stack_index] = rc_media_fontIsLoaded(rc_nid[FONTISLOADED_F_SLOT][0]);
            rc_num_stack_index++;
            break;
        case 245:
            //cout << "Font" << endl;
            rc_media_font(rc_nid[FONT_F_SLOT][0]);
            break;
        case 246:
            //cout << "LoadFont" << endl;
            rc_media_LoadFont(rc_nid[LOADFONT_F_SLOT][0],rc_sid[LOADFONT_FNT_FILE_STR][0],rc_nid[LOADFONT_SIZE][0]);
            break;
        case 247:
            //cout << "DeleteFont" << endl;
            rc_media_DeleteFont(rc_nid[DELETEFONT_F_SLOT][0]);
            break;
        case 248:
            rc_media_setFontStyle(rc_nid[SETFONTSTYLE_F_SLOT][0],rc_nid[SETFONTSTYLE_STYLE][0]);
            break;
        case 249:
            rc_media_drawText_shaded_hw(rc_sid[DRAWTEXT_SHADED_TXT_STR][0],rc_nid[DRAWTEXT_SHADED_X][0],rc_nid[DRAWTEXT_SHADED_Y][0],rc_nid[DRAWTEXT_SHADED_FG_COLOR][0],rc_nid[DRAWTEXT_SHADED_BG_COLOR][0]);
            break;
        case 250:
            //cout << "Blend" << endl;
            rc_media_drawText_blended_hw(rc_sid[DRAWTEXT_BLENDED_TXT_STR][0],rc_nid[DRAWTEXT_BLENDED_X][0],rc_nid[DRAWTEXT_BLENDED_Y][0]);
            break;
        case 251:
            //cout << "GetTextSize" << endl;
            rc_media_getTextSize(rc_nid[GETTEXTSIZE_F_SLOT][0], rc_sid[GETTEXTSIZE_TXT_STR][0], &rc_nid[GETTEXTSIZE_W][0], &rc_nid[GETTEXTSIZE_H][0]);
            break;
        case 252:
            //function
            //cout << "SoundChannels" << endl;
            rc_num_stack[rc_num_stack_index] = rc_media_getSoundChannels();
            rc_num_stack_index++;
            break;
        case 253:
            rc_str_stack[rc_str_stack_index] = rc_net_myLocalIP();
            rc_str_stack_index++;
            break;
        case 254:
            rc_media_ReadInput_SetText(rc_sid[READINPUT_SETTEXT_TXT_STR][0]);
            break;
        case 255:
            //GetTouchFinger
            rc_media_getTouchFinger((int)rc_nid[GETTOUCHFINGER_FINGER][0],&rc_nid[GETTOUCHFINGER_X][0],&rc_nid[GETTOUCHFINGER_Y][0],&rc_nid[GETTOUCHFINGER_PRESSURE][0]);
            break;
        case 256:
            //NumFingers
            rc_num_stack[rc_num_stack_index] = rc_media_numFingers();
            rc_num_stack_index++;
            break;
        case 257:
            rc_media_loadImage_ex_hw(rc_nid[LOADIMAGE_EX_SLOT][0], rc_sid[LOADIMAGE_EX_IMG_FILE_STR][0], rc_nid[LOADIMAGE_EX_COLKEY][0]);
            break;
        case 258:
            rc_media_rectangle(rc_nid[RECT_X][0],rc_nid[RECT_Y][0],rc_nid[RECT_W][0],rc_nid[RECT_H][0]);
            break;
        case 259:
            rc_media_rectangleFill(rc_nid[RECTFILL_X][0],rc_nid[RECTFILL_Y][0],rc_nid[RECTFILL_W][0],rc_nid[RECTFILL_H][0]);
            break;
        case 260:
            //OS$
            rc_str_stack[rc_str_stack_index] = rc_intern_OS();
            rc_str_stack_index++;
            break;
        case 261:
            rc_num_stack[rc_num_stack_index] = rc_media_numJoyButtons(rc_nid[NUMJOYBUTTONS_JOY_NUM][0]);
            rc_num_stack_index++;
            break;
        case 262:
            rc_num_stack[rc_num_stack_index] = rc_media_numJoyAxes(rc_nid[NUMJOYAXES_JOY_NUM][0]);
            rc_num_stack_index++;
            break;
        case 263:
            rc_media_loadVideo(rc_sid[LOADVIDEO_VID_FILE_STR][0]);
            break;
        case 264:
            rc_media_playVideo(rc_nid[PLAYVIDEO_VLOOPS][0]);
            break;
        case 265:
            rc_media_pauseVideo();
            break;
        case 266:
            rc_media_stopVideo();
            break;
        case 267:
            rc_media_setVideoPosition(rc_nid[SETVIDEOPOSITION_VPOS][0]);
            break;
        case 269:
            rc_media_resumeVideo();
            break;
        case 270:
            rc_num_stack[rc_num_stack_index] = rc_media_videoPosition();
            rc_num_stack_index++;
            break;
        case 272:
            rc_media_deleteVideo();
            break;
        case 273:
            rc_media_deleteMusic();
            break;
        case 274:
            rc_intern_system(rc_sid[SYSTEM_SYS_CMD_STR][0]);
            break;
        case 275:
            rc_num_stack[rc_num_stack_index] = rc_media_videoIsPlaying();
            rc_num_stack_index++;
            break;
        case 276:
            rc_media_roundRect(rc_nid[ROUNDRECT_X][0],rc_nid[ROUNDRECT_Y][0],rc_nid[ROUNDRECT_W][0],rc_nid[ROUNDRECT_H][0],rc_nid[ROUNDRECT_R][0]);
            break;
        case 277:
            rc_media_roundRectFill(rc_nid[ROUNDRECTFILL_X][0],rc_nid[ROUNDRECTFILL_Y][0],rc_nid[ROUNDRECTFILL_W][0],rc_nid[ROUNDRECTFILL_H][0],rc_nid[ROUNDRECTFILL_R][0]);
            break;
        case 278:
            rc_num_stack[rc_num_stack_index] = rc_media_videoEnd();
            rc_num_stack_index++;
            break;
        case 279:
            rc_num_stack[rc_num_stack_index] = rc_media_getVideoStats(rc_sid[GETVIDEOSTATS_VFILE_STR][0],&rc_nid[GETVIDEOSTATS_VLENGTH][0],&rc_nid[GETVIDEOSTATS_VFPS][0],&rc_nid[GETVIDEOSTATS_W][0],&rc_nid[GETVIDEOSTATS_H][0]);
            break;
        case 280:
            rc_media_setVideoDrawRect(rc_nid[SETVIDEODRAWRECT_X][0],rc_nid[SETVIDEODRAWRECT_Y][0],rc_nid[SETVIDEODRAWRECT_W][0],rc_nid[SETVIDEODRAWRECT_H][0]);
            break;
        case 281:
            rc_media_getVideoDrawRect(&rc_nid[GETVIDEODRAWRECT_X][0],&rc_nid[GETVIDEODRAWRECT_Y][0],&rc_nid[GETVIDEODRAWRECT_W][0],&rc_nid[GETVIDEODRAWRECT_H][0]);
            break;
        case 282:
            rc_media_getVideoSize(&rc_nid[GETVIDEOSIZE_W][0],&rc_nid[GETVIDEOSIZE_H][0]);
            break;
        case 283:
            rc_num_stack[rc_num_stack_index] = rc_media_videoExists();
            rc_num_stack_index++;
            break;
        case 284:
            rc_media_setVideoAlpha(rc_nid[SETVIDEOALPHA_V_ALPHA][0]);
            break;
        case 285:
            rc_media_createSound((int)rc_nid[CREATESOUND_SLOT][0],&rc_nid[CREATESOUND_ABUF][0],(Uint32)rc_nid[CREATESOUND_ALEN][0],(Uint8)rc_nid[CREATESOUND_AVOL][0]);
            break;
        case 286:
            //command$
            rc_str_stack[rc_str_stack_index] = rc_intern_command((int)rc_nid[COMMAND_STR_ARG][0]);
            rc_str_stack_index++;
            break;
        case 287:
            //NumCommands
            rc_num_stack[rc_num_stack_index] = rc_intern_numCommands();
            rc_num_stack_index++;
            break;
        case 288:
            rc_str_stack[rc_str_stack_index] = rc_intern_str_f();
            rc_str_stack_index++;
            break;
        case 289:
            rc_str_stack[rc_str_stack_index] = rc_intern_str_s();
            rc_str_stack_index++;
            break;
        case 290:
            rc_media_cls();
            break;
        case 291:
            rc_str_stack[rc_str_stack_index] = rc_intern_env();
            rc_str_stack_index++;
            break;
        case 292:
            rc_num_stack[rc_num_stack_index] = rc_intern_setEnv();
            rc_num_stack_index++;
            break;
        case 293:
            rc_str_stack[rc_str_stack_index] = rc_intern_prefPath();
            rc_str_stack_index++;
            break;

        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    return;
}

inline void rc_vm_delete()
{
    //cout << "<delete>\n";
    unsigned int i_val1;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 4:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            delete[] rc_nid[i_val1];
            rc_nid[i_val1] = NULL;
            rc_nid[i_val1] = new double[1];
            rc_nid_w[i_val1] = 0;
            rc_nid_h[i_val1] = 0;
            rc_nid_d[i_val1] = 0;
            break;
        case 5:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            delete[] rc_sid[i_val1];
            rc_sid[i_val1] = NULL;
            rc_sid[i_val1] = new string[1];
            rc_sid_w[i_val1] = 0;
            rc_sid_h[i_val1] = 0;
            rc_sid_d[i_val1] = 0;
            break;
        default:
            cout << "could not delete pointer" << endl;
            RC_VM_ACTIVE = 0;
            return;
    }
    //return 1;
}

inline void rc_vm_ptr()
{
    //cout << "<ptr>\n";
    unsigned int i_val1;
    unsigned int i_val2;
    char arg_type1 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;
    switch(arg_type1)
    {
        case 7:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            RC_PTR_TYPE = 0;
            break;
        case 8:
            i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            RC_CURRENT_ADDRESS += vm_data_size;
            RC_PTR_TYPE = 1;
            break;
        default:
            RC_VM_ACTIVE = 0;
            return;
            //return 0;
    }

    char arg_type2 = RC_SEGMENT[RC_CURRENT_SEGMENT][RC_CURRENT_ADDRESS];
    RC_CURRENT_ADDRESS++;

    switch(arg_type2)
    {
        case 4:
            i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            rc_nid[i_val1] = &rc_nid[i_val2][0];
            rc_nid_d[i_val1] = rc_nid_d[i_val2];
            rc_nid_h[i_val1] = rc_nid_h[i_val2];
            rc_nid_w[i_val1] = rc_nid_w[i_val2];
            RC_CURRENT_ADDRESS += vm_data_size;
            //cout << "end of <4>\n";
            return;
        case 5:
            i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
            rc_sid[i_val1] = &rc_sid[i_val2][0];
            rc_sid_d[i_val1] = rc_sid_d[i_val2];
            rc_sid_h[i_val1] = rc_sid_h[i_val2];
            rc_sid_w[i_val1] = rc_sid_w[i_val2];
            RC_CURRENT_ADDRESS += vm_data_size;
            //cout << "end of <5>\n";
            return;
        default:
            RC_VM_ACTIVE = 0;
            return;
    }
    //return 1;
}

rc_ushort tst_end;
unsigned int tst_addr = 0;

inline void rc_for_loop(double * f_val, double f_end, double f_inc)
{
    //cout << "\n*******FOR*************\n";
    //cout << "I = " << *f_val << endl;
    //cout << "STEP = " << f_inc << endl;
    //cout << "END = " << f_end << endl << endl;
    if(*f_val < f_end)
    {
        if( (*f_val + f_inc ) >= f_end)
        {
            *f_val = f_end;
        }
        else if( (*f_val + (f_inc*2) ) > f_end)
        {
            //for_overflow = 1;
            *f_val = f_end;
        }
        else
        {
            *f_val += f_inc;
        }
    }
    else if(*f_val > f_end)
    {
        if( (*f_val + f_inc ) <= f_end)
        {
            *f_val = f_end;
        }
        else if( (*f_val + (f_inc*2) ) < f_end)
        {
            //for_overflow = 1;
            *f_val = f_end;
        }
        else
            *f_val += f_inc;
    }
    //cout << "I = " << *f_val << endl;
    //cout << "******STEP COMPLETE***\n";
    return;
}

bool rc_checkEvent()
{
    for(int i = 0; i < MAX_WINDOWS; i++)
        if(rc_win[i]!=NULL)
            return true;
    return false;
}

inline int rc_vm_run()
{
    int rn_val = 1;
    rc_ushort vm_cmd;
    vm_cmd.s = rc_segment_getUShort(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
    tst_addr = RC_CURRENT_ADDRESS;
    RC_CURRENT_ADDRESS += 2;
    tst_end = vm_cmd;

    //cout << "VM cmd = " << vm_cmd.s << endl;
    //cycleVideo();


    if(vm_cmd.s == 32) //mov
    {
        rc_vm_mov();
    }
    else if(vm_cmd.s == 33) //mov$
    {
        rc_vm_mov_str();
    }
    else if(vm_cmd.s == 34) //pwr
    {
        rc_vm_pwr();
    }
    else if(vm_cmd.s == 35) //mul
    {
        rc_vm_mul();
    }
    else if(vm_cmd.s == 36) //mul
    {
        rc_vm_div();
    }
    else if(vm_cmd.s == 37) //mul
    {
        rc_vm_mod();
    }
    else if(vm_cmd.s == 38) //mul
    {
        rc_vm_add();
    }
    else if(vm_cmd.s == 39) //mul
    {
        rc_vm_add_str();
    }
    else if(vm_cmd.s == 40) //mul
    {
        rc_vm_sub();
    }
    else if(vm_cmd.s == 41) //mul
    {
        rc_vm_shl();
    }
    else if(vm_cmd.s == 42) //mul
    {
        rc_vm_shr();
    }
    else if(vm_cmd.s == 43) //mul
    {
        rc_vm_cmp();
    }
    else if(vm_cmd.s == 44) //mul
    {
        rc_vm_cmp_str();
    }
    else if(vm_cmd.s == 45) //mul
    {
        rc_vm_convertToStr();
    }
    else if(vm_cmd.s == 46) //mul
    {
        rc_vm_and();
    }
    else if(vm_cmd.s == 47) //mul
    {
        rc_vm_or();
    }
    else if(vm_cmd.s == 48) //mul
    {
        rc_vm_push();
    }
    else if(vm_cmd.s == 49) //mul
    {
        rc_vm_push_str();
    }
    else if(vm_cmd.s == 50) //mul
    {
        rc_vm_pop();
    }
    else if(vm_cmd.s == 51) //mul
    {
        rc_vm_pop_str();
    }
    else if(vm_cmd.s == 52) //mul
    {
        rc_vm_gosub();
    }
    else if(vm_cmd.s == 53) //mul
    {
        rc_vm_arr1();
    }
    else if(vm_cmd.s == 54) //mul
    {
        rc_vm_arr2();
    }
    else if(vm_cmd.s == 55) //mul
    {
        rc_vm_arr3();
    }
    else if(vm_cmd.s == 56) //mul
    {
        rc_vm_arr1_str();
    }
    else if(vm_cmd.s == 57) //mul
    {
        rc_vm_arr2_str();
    }
    else if(vm_cmd.s == 58) //mul
    {
        rc_vm_arr3_str();
    }
    else if(vm_cmd.s == 59) //mul
    {
        rc_vm_alloc();
    }
    else if(vm_cmd.s == 62) //mul
    {
        rc_vm_setArr1();
    }
    else if(vm_cmd.s == 63) //mul
    {
        rc_vm_setArr2();
    }
    else if(vm_cmd.s == 64) //mul
    {
        rc_vm_setArr3();
    }
    else if(vm_cmd.s == 65) //mul
    {
        rc_vm_setArr1_str();
    }
    else if(vm_cmd.s == 66) //mul
    {
        rc_vm_setArr2_str();
    }
    else if(vm_cmd.s == 67) //mul
    {
        rc_vm_setArr3_str();
    }
    else if(vm_cmd.s == 68) //mul
    {
        rc_vm_jmp();
    }
    else if(vm_cmd.s == 69) //mul
    {
        rc_vm_je();
    }
    else if(vm_cmd.s == 70) //mul
    {
        rc_vm_jne();
    }
    else if(vm_cmd.s == 71) //mul
    {
        rc_vm_jl();
    }
    else if(vm_cmd.s == 72) //mul
    {
        rc_vm_jg();
    }
    else if(vm_cmd.s == 73) //mul
    {
        rc_vm_jle();
    }
    else if(vm_cmd.s == 74) //mul
    {
        rc_vm_jge();
    }
    else if(vm_cmd.s == 75) //mul
    {
        rc_vm_abs();
    }
    else if(vm_cmd.s == 76) //mul
    {
        rc_vm_end();
        exit(EXIT_SUCCESS);
        return -1;
    }
    else if(vm_cmd.s == 77) //mul
    {
        rc_vm_return();
        return 77;
    }
    else if(vm_cmd.s == 78) //mul
    {
        rc_vm_xor();
    }
    else if(vm_cmd.s == 79) //mul
    {
        rc_vm_not();
    }
    else if(vm_cmd.s == 80) //mul
    {
        rc_vm_dim();
    }
    else if(vm_cmd.s == 81) //mul
    {
        rc_vm_dim_str();
    }
    else if(vm_cmd.s == 82) //mul
    {
        rc_vm_intern();
    }
    else if(vm_cmd.s == 83) //mul
    {
        rc_vm_delete();
    }
    else if(vm_cmd.s == 84) //mul
    {
        rc_vm_ptr();
    }
    else if(vm_cmd.s == 85) //mul
    {
        rc_vm_abs_sub();
    }
    else if(vm_cmd.s == 86) //for loop
    {
        RC_CURRENT_ADDRESS++;
        unsigned int i_val1 = rc_segment_getUInt(RC_CURRENT_SEGMENT,RC_CURRENT_ADDRESS); //id
        RC_CURRENT_ADDRESS += vm_data_size + 1;
        //cout << "1: " << i_val1 << endl;

        unsigned int i_val2 = rc_segment_getUInt(RC_CURRENT_SEGMENT,RC_CURRENT_ADDRESS); //end
        RC_CURRENT_ADDRESS += vm_data_size + 1;
        //cout << "2: " << i_val2 << endl;

        unsigned int i_val3 = rc_segment_getUInt(RC_CURRENT_SEGMENT,RC_CURRENT_ADDRESS); //step
        RC_CURRENT_ADDRESS += vm_data_size;
        //cout << rc_segment_getUShort(RC_CURRENT_SEGMENT,RC_CURRENT_ADDRESS) << endl << endl;
        //cout << "3: " << i_val3 << endl;

        //cout << "FOR I = " << rc_nid[i_val1][0];
        //cout << " TO " << rc_vm_f[i_val2];
        //cout << " STEP " << rc_vm_f[i_val3] << endl;
        //string s; cin >> s;
        int f_err = 0;
        int f_start = RC_CURRENT_ADDRESS;
        unsigned int f_end = 0;

        //cout << "LOOP (" << loop_scope << ") START" << endl;
        //loop_scope++;


        rc_vm_stack.push(rc_vm_f[i_val2]);
        rc_vm_stack.push(rc_vm_f[i_val3]);

        if(rc_nid[i_val1][0] <= rc_vm_f[i_val2])
        {
            for(; rc_nid[i_val1][0] <= rc_vm_f[i_val2]; rc_nid[i_val1][0] += rc_vm_f[i_val3])
            {
                RC_CURRENT_ADDRESS = f_start;
                rc_textinput_flag = false;
                cycleVideo();
                if(rc_checkEvent())
                {
                    while(rc_getEvents()){}
                    //SDL_PumpEvents();
                    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
                }
                rc_vm_f[i_val3] = rc_vm_stack.top();
                rc_vm_stack.pop();
                rc_vm_f[i_val2] = rc_vm_stack.top();
                rc_vm_stack.pop();
                rc_vm_stack.push(rc_vm_f[i_val2]);
                rc_vm_stack.push(rc_vm_f[i_val3]);
                //cout << endl << "rc_nid = " << rc_nid[i_val1][0] << endl;
                //cout << "rc_vm_f[2] = " << rc_vm_f[i_val2] << endl << endl;
                f_err = 0;
                while(f_err != 87)
                {
                    f_err = rc_vm_run();

                    if(f_err == 86)
                    {
                        rc_vm_f[i_val3] = rc_vm_stack.top();
                        rc_vm_stack.pop();
                        rc_vm_f[i_val2] = rc_vm_stack.top();
                        rc_vm_stack.pop();
                        rc_vm_stack.push(rc_vm_f[i_val2]);
                        rc_vm_stack.push(rc_vm_f[i_val3]);
                    }
                    else if(f_err == 94)
                    {
                        //cout << "we made it\n";
                        if(exit_arg > 0)
                        {
                            //cout << "exit--\n";
                            exit_arg = exit_arg - 1;
                            rc_vm_stack.pop();
                            rc_vm_stack.pop();
                            return 94;
                        }
                        else
                        {
                            rc_vm_f[i_val3] = rc_vm_stack.top();
                            rc_vm_stack.pop();
                            rc_vm_f[i_val2] = rc_vm_stack.top();
                            rc_vm_stack.pop();
                            rc_vm_stack.push(rc_vm_f[i_val2]);
                            rc_vm_stack.push(rc_vm_f[i_val3]);
                            rc_vm_run();
                            continue;
                        }
                    }
                    else if(f_err == 96)
                    {
                        //cout << "gangsta\n";
                        if(exit_arg > 0)
                        {
                            //cout << "balls sack" << endl;
                            rc_vm_stack.pop();
                            rc_vm_stack.pop();
                            exit_arg = exit_arg - 1;
                            return 96;
                        }
                        else
                        {
                            rc_vm_f[i_val3] = rc_vm_stack.top();
                            rc_vm_stack.pop();
                            rc_vm_f[i_val2] = rc_vm_stack.top();
                            rc_vm_stack.pop();
                            rc_vm_stack.push(rc_vm_f[i_val2]);
                            rc_vm_stack.push(rc_vm_f[i_val3]);
                            while(rc_vm_run()!=77){}
                            //cout << "Return Called\n";
                            //cout << "NEW SEGMENT = " << RC_CURRENT_SEGMENT << endl;
                            //cout << "NEW ADDRESS = " << RC_CURRENT_ADDRESS << endl;
                            //string ss; cin >> ss;
                            //return 1;
                        }
                        //RC_CURRENT_ADDRESS = st_addr;
                        //break;
                    }
                    else if(f_err == -1)
                        return -1;
                }
            }
            rc_nid[i_val1][0] -= rc_vm_f[i_val3];
        }
        else
        {
            for(; rc_nid[i_val1][0] >= rc_vm_f[i_val2]; rc_nid[i_val1][0] += rc_vm_f[i_val3])
            {
                RC_CURRENT_ADDRESS = f_start;
                rc_textinput_flag = false;
                cycleVideo();
                if(rc_checkEvent())
                {
                    while(rc_getEvents()){}
                    //SDL_PumpEvents();
                    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
                }
                rc_vm_f[i_val3] = rc_vm_stack.top();
                rc_vm_stack.pop();
                rc_vm_f[i_val2] = rc_vm_stack.top();
                rc_vm_stack.pop();
                rc_vm_stack.push(rc_vm_f[i_val2]);
                rc_vm_stack.push(rc_vm_f[i_val3]);
                //cout << endl << "rc_nid = " << rc_nid[i_val1][0] << endl;
                //cout << "rc_vm_f[2] = " << rc_vm_f[i_val2] << endl << endl;
                f_err = 0;
                while(f_err != 87)
                {
                    f_err = rc_vm_run();

                    if(f_err == 86)
                    {
                        rc_vm_f[i_val3] = rc_vm_stack.top();
                        rc_vm_stack.pop();
                        rc_vm_f[i_val2] = rc_vm_stack.top();
                        rc_vm_stack.pop();
                        rc_vm_stack.push(rc_vm_f[i_val2]);
                        rc_vm_stack.push(rc_vm_f[i_val3]);
                    }
                    else if(f_err == 94)
                    {
                        //cout << "we made it\n";
                        if(exit_arg > 0)
                        {
                            //cout << "exit--\n";
                            exit_arg = exit_arg - 1;
                            rc_vm_stack.pop();
                            rc_vm_stack.pop();
                            return 94;
                        }
                        else
                        {
                            rc_vm_f[i_val3] = rc_vm_stack.top();
                            rc_vm_stack.pop();
                            rc_vm_f[i_val2] = rc_vm_stack.top();
                            rc_vm_stack.pop();
                            rc_vm_stack.push(rc_vm_f[i_val2]);
                            rc_vm_stack.push(rc_vm_f[i_val3]);
                            rc_vm_run();
                            continue;
                        }
                    }
                    else if(f_err == 96)
                    {
                        //cout << "gangsta\n";
                        if(exit_arg > 0)
                        {
                            //cout << "balls sack" << endl;
                            rc_vm_stack.pop();
                            rc_vm_stack.pop();
                            exit_arg = exit_arg - 1;
                            return 96;
                        }
                        else
                        {
                            rc_vm_f[i_val3] = rc_vm_stack.top();
                            rc_vm_stack.pop();
                            rc_vm_f[i_val2] = rc_vm_stack.top();
                            rc_vm_stack.pop();
                            rc_vm_stack.push(rc_vm_f[i_val2]);
                            rc_vm_stack.push(rc_vm_f[i_val3]);
                            while(rc_vm_run()!=77){}
                            //cout << "Return Called\n";
                            //cout << "NEW SEGMENT = " << RC_CURRENT_SEGMENT << endl;
                            //cout << "NEW ADDRESS = " << RC_CURRENT_ADDRESS << endl;
                            //string ss; cin >> ss;
                            //return 1;
                        }
                        //RC_CURRENT_ADDRESS = st_addr;
                        //break;
                    }
                    else if(f_err == -1)
                        return -1;
                }
            }
            rc_nid[i_val1][0] += rc_vm_f[i_val3];
        }

        //cout << endl << "end --- rc_nid = " << rc_nid[i_val1][0] << endl;
        //cout << "end --- rc_vm_f[2] = " << rc_vm_f[i_val2] << endl << endl;
        rc_vm_stack.pop();
        rc_vm_stack.pop();
        return 86;
    }
    else if(vm_cmd.s == 87)
    {
        return 87;
    }
    else if(vm_cmd.s == 88) //while
    {
        unsigned int wend_con = RC_CURRENT_ADDRESS - 2;
        RC_CURRENT_ADDRESS++;
        unsigned int i = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
        RC_CURRENT_ADDRESS += vm_data_size;
        //cout << "SET START ADDR\n";
        unsigned int st_addr = RC_CURRENT_ADDRESS;
        //cout << "test out << " << rc_segment_getUShort(RC_CURRENT_SEGMENT,RC_CURRENT_ADDRESS) << endl;
        unsigned int w_err = 1;


        while(rc_vm_m[i]!=0)
        {
            //cout << rc_segment_getUShort(RC_CURRENT_SEGMENT,RC_CURRENT_ADDRESS)<<endl;
            //cout << "RC_VM[" << i << "] = " << rc_vm_m[i] << endl;

            RC_CURRENT_ADDRESS = st_addr;

            rc_textinput_flag = false;
            cycleVideo();

            while(rc_getEvents()){}

            while(true)
            {
                w_err = rc_vm_run();

                if(w_err == 0)
                {
                    cout << "ERROR in WHILE\n";
                    return 0;
                }
                else if(w_err == -1)
                {
                    //cout << "ERROR in WHILE\n";
                    return -1;
                }
                else if(w_err == 89)
                {
                    //RC_CURRENT_ADDRESS = st_addr;
                    break;
                }
                else if(w_err == 94)
                {
                    if(exit_arg > 0)
                    {
                        exit_arg = exit_arg - 1;
                        return 94;
                    }
                    else
                    {
                        rc_vm_run();
                        continue;
                    }
                }
                else if(w_err == 96)
                {
                    if(exit_arg > 0)
                    {
                        //cout << "balls sack" << endl;
                        exit_arg = exit_arg - 1;
                        return 96;
                    }
                    else
                    {
                        while(rc_vm_run()!=77){}
                        //cout << "Return Called\n";
                        //cout << "NEW SEGMENT = " << RC_CURRENT_SEGMENT << endl;
                        //cout << "NEW ADDRESS = " << RC_CURRENT_ADDRESS << endl;
                        //string ss; cin >> ss;
                        //return 1;
                    }
                    //RC_CURRENT_ADDRESS = st_addr;
                    //break;
                }

            }
//            if(exit_arg > 0)
//            {
//                cout << "Exit Arg = " << exit_arg << endl;
//                loop_exit = false;
//                exit_arg--;
//                return 94;
//            }
        }

        //cout << "Loop end\n"; string s; cin >> s;
        //RC_CURRENT_ADDRESS = wend_addr;
    }
    else if(vm_cmd.s == 89)
    {
        RC_CURRENT_ADDRESS+=vm_data_size + 1;
        //RC_CURRENT_ADDRESS = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
        return 89;
    }
    else if(vm_cmd.s == 90)//DO LOOP
    {
        int d_err = 1;
        unsigned int d_start_addr = RC_CURRENT_ADDRESS;
        do
        {
            RC_CURRENT_ADDRESS = d_start_addr;
            rc_textinput_flag = false;
            cycleVideo();

            while(rc_getEvents()){}

            while(true)
            {
                d_err = rc_vm_run();
                if(d_err == 0)
                {
                    cout << "ERROR in DO\n";
                    return 0;
                }
                else if(d_err == -1)
                {
                    cout << "ERROR in DO\n";
                    return -1;
                }
                else if(d_err == 94)
                {
                    if(exit_arg > 0)
                    {
                        exit_arg = exit_arg - 1;
                        return 94;
                    }
                    else
                    {
                        rc_vm_run();
                        continue;
                    }
                }
                else if(d_err == 96)
                {
                    if(exit_arg > 0)
                    {
                        //cout << "balls sack" << endl;
                        exit_arg = exit_arg - 1;
                        return 96;
                    }
                    else
                    {
                        while(rc_vm_run()!=77){}
                        //cout << "Return Called\n";
                        //cout << "NEW SEGMENT = " << RC_CURRENT_SEGMENT << endl;
                        //cout << "NEW ADDRESS = " << RC_CURRENT_ADDRESS << endl;
                        //string ss; cin >> ss;
                        //return 1;
                    }
                    //RC_CURRENT_ADDRESS = st_addr;
                    //break;
                }
                else if(d_err == 90)
                {
                    //cout << "check condition\n";
                    break;
                }
            }

        }while(!loop_exit);
        //cout << "NLP\n";
        loop_exit = false;
        return 1;
    }
    else if(vm_cmd.s == 91)
    {
        loop_exit = false;
        return 90;
    }
    else if(vm_cmd.s == 92)
    {
        RC_CURRENT_ADDRESS++;
        int i = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
        RC_CURRENT_ADDRESS+=vm_data_size;
        if(rc_vm_m[i]!=0)
            loop_exit = true;
        return 90;
    }
    else if(vm_cmd.s == 93)
    {
        RC_CURRENT_ADDRESS++;
        int i = rc_segment_getUInt(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS);
        RC_CURRENT_ADDRESS+=vm_data_size;
        if(rc_vm_m[i]==0)
        {
            //cout << "END OF LOOP\n";
            loop_exit = true;
        }
        return 90;
    }
    else if(vm_cmd.s == 94)
    {
        //loop_exit = true;
        RC_CURRENT_ADDRESS++;
        exit_arg = rc_segment_getUInt(RC_CURRENT_SEGMENT,RC_CURRENT_ADDRESS);
        RC_CURRENT_ADDRESS += vm_data_size;
        return 94;
    }
    else if(vm_cmd.s == 95)
    {
        rc_vm_print();
    }
    else if(vm_cmd.s == 96)
    {
        //loop_exit = true;
        RC_CURRENT_ADDRESS++;
        exit_arg = rc_segment_getUInt(RC_CURRENT_SEGMENT,RC_CURRENT_ADDRESS);
        RC_CURRENT_ADDRESS += vm_data_size;

        //cout << "\nNext command = " << (int)rc_segment_getUShort(RC_CURRENT_SEGMENT, RC_CURRENT_ADDRESS) << endl;
        //cout << "SEGMENT = " << RC_CURRENT_SEGMENT << endl;
        //cout << "ADDRESS = " << RC_CURRENT_ADDRESS << endl;
        //cout << "EXIT ARG = " << exit_arg << endl << endl;
        return 96;
    }
    else
    {
        cout << "VM Crashed on " << vm_cmd.s << endl;
        //string s; cin >> s;
        rn_val = 0;
        return -1;
    }
    return 1;
}

void rc_closeVM()
{
    delete[] RC_SEGMENT[RC_CODE_SEGMENT];
    delete[] RC_SEGMENT[RC_DATA_SEGMENT];
    delete[] rc_vm_m;
    delete[] rc_vm_s;
    delete[] rc_vm_f;

    //cout << "seg0 delete\n";

    for(int i = 0; i < rc_byref_nid_count; i++)
    {
        //cout << "rc_nid_byref[" << i << "] = " << rc_byref_nid[i] << endl;
        rc_nid[rc_byref_nid[i]] = NULL;
    }

    //cout << "byref nid" << endl;

    for(int i = 0; i < rc_byref_sid_count; i++)
    {
        rc_sid[rc_byref_sid[i]] = NULL;
    }

    //cout << "byref sid" << endl;

    for(int i = 0; i < rc_nid_size; i++)
    {
        //cout << "delete " << i << endl;
        delete[] rc_nid[i];
    }

    //cout << "seg1 delete\n";

    delete[] rc_nid_w;
    delete[] rc_nid_h;
    delete[] rc_nid_d;

    //cout << "seg2 delete\n";

    for(int i = 0;  i < rc_sid_size; i++)
    {
        //cout << "delete sid[ " << i << " ]\n";
        delete[] rc_sid[i];
    }

    //cout << "seg3 delete\n";

    delete[] rc_sid_w;
    delete[] rc_sid_h;
    delete[] rc_sid_d;

    //cout << "seg4 delete\n";

    delete[] rc_num_stack;
    delete[] rc_str_stack;
}

void rc_runtime_clean()
{
    rc_media_quit();
    rc_closeVM();
    rc_vm_isClean = true;
    //cout << "CLEAN" << endl;
}

int main(int argc, char * argv[])
{
	atexit(rc_runtime_clean);
	rc_cmd_count = argc - 1;
	if(rc_cmd_count > 0)
    {
        rc_cmd_args = new string[rc_cmd_count];
        for(int i = 0; i < rc_cmd_count; i++)
            rc_cmd_args[i] = argv[i+1];
    }
    //for(int i = 0; i < argc; i++)
    //    cout << "ARG# " << i << ": " << argv[i] << endl;
    //return 0;
    SDL_RWops * rc_file;
    //rc_file.open("/home/cunningham/Desktop/test_project/test_compiler/test.cbc", fstream::in | fstream::binary); //for testing purposes

    //if(argc > 1)
    //    rc_file.open(argv[1], fstream::in | fstream::binary);
    //else
    //    rc_file.open("/home/cunningham/Desktop/projects/rc_basic_compiler/rc_basic_compiler/test.cbc", fstream::in | fstream::binary); //comment out this else in release

    rc_file = SDL_RWFromFile("main.cbc","rb");

    if(rc_file == NULL)
     {
    	//string s = "this is a test";
    	//SDL_LogError(SDL_LOG_CATEGORY_ERROR, "flsl");
    	return 0;
     }

    rc_uint rc_size;

    //cout << "debug 1\n";

    for(int i = 0; i < vm_data_size; i++)
    {
        //cout << "vm data [" << i << "]\n";
        //rc_size.data[i] = rc_file.get();
    	SDL_RWread(rc_file, &rc_size.data[i], 1, 1);
    }

    rc_code_segment_size = rc_size.ui;

    //cout << "debug 2\n";

    for(int i = 0; i < vm_data_size; i++)
        SDL_RWread(rc_file, &rc_size.data[i], 1, 1);
    	//rc_size.data[i] = rc_file.get();

    rc_data_segment_size = rc_size.ui;

    //cout << "debug 3\n";

    for(int i = 0; i < vm_data_size; i++)
        SDL_RWread(rc_file, &rc_size.data[i], 1, 1);
    	//rc_size.data[i] = rc_file.get();

    rc_byref_nid_count = rc_size.ui;

    for(int i = 0; i < vm_data_size; i++)
        SDL_RWread(rc_file, &rc_size.data[i], 1, 1);
    	//rc_size.data[i] = rc_file.get();

    rc_byref_sid_count = rc_size.ui;

    //cout << "debug 4\n";

    rc_byref_nid = new Uint64[rc_byref_nid_count];
    rc_byref_sid = new Uint64[rc_byref_sid_count];

    //cout << "debug 5\n";

    for(int i = 0; i < rc_byref_nid_count; i++)
    {
        for(int n = 0; n < vm_data_size; n++)
            SDL_RWread(rc_file, &rc_size.data[n], 1, 1);
        	//rc_size.data[n] = rc_file.get();

        rc_byref_nid[i] = rc_size.ui;
        //cout << "byref = " <<rc_byref_nid[i] << endl;
        //string bs;cin >>bs;
    }

    //cout << "debug 6\n";

    for(int i = 0; i < rc_byref_sid_count; i++)
    {
        for(int n = 0; n < vm_data_size; n++)
            SDL_RWread(rc_file, &rc_size.data[n], 1, 1);
        	//rc_size.data[n] = rc_file.get();

        rc_byref_sid[i] = rc_size.ui;
    }

    //cout << "debug 7\n";

    for(int i = 0; i < vm_data_size; i++)
        SDL_RWread(rc_file, &rc_size.data[i], 1, 1);
    	//rc_size.data[i] = rc_file.get();

    rc_nid_size = rc_size.ui;

    //cout << "debug 8\n";

    for(int i = 0; i < vm_data_size; i++)
        SDL_RWread(rc_file, &rc_size.data[i], 1, 1);
    	//rc_size.data[i] = rc_file.get();

    rc_sid_size = rc_size.ui;

    for(int i = 0; i < vm_data_size; i++)
        SDL_RWread(rc_file, &rc_size.data[i], 1, 1);
    	//rc_size.data[i] = rc_file.get();

    rc_max_m = rc_size.ui;

    //cout << "debug 9\n";

    for(int i = 0; i < vm_data_size; i++)
        SDL_RWread(rc_file, &rc_size.data[i], 1, 1);
    	//rc_size.data[i] = rc_file.get();

    rc_max_s = rc_size.ui;

    for(int i = 0; i < vm_data_size; i++)
        SDL_RWread(rc_file, &rc_size.data[i], 1, 1);
    	//rc_size.data[i] = rc_file.get();

    rc_max_f = rc_size.ui;

    for(int i = 0; i < vm_data_size; i++)
        SDL_RWread(rc_file, &rc_size.data[i], 1, 1);
    	//rc_size.data[i] = rc_file.get();

    rc_num_stack_size = rc_size.ui;

    for(int i = 0; i < vm_data_size; i++)
        SDL_RWread(rc_file, &rc_size.data[i], 1, 1);
    	//rc_size.data[i] = rc_file.get();

    rc_str_stack_size = rc_size.ui;

    //char * buf = "";
    //int n = sprintf(buf, "Code Segment Size = %d", rc_code_segment_size);
    __android_log_print(ANDROID_LOG_ERROR, "RC_DEBUG", "TELL %lld", SDL_RWtell(rc_file));
    //cout << "Code Segment Size = " << rc_code_segment_size << endl;
    __android_log_print(ANDROID_LOG_ERROR, "RC_CODE_SEG_SIZE","%lld",rc_code_segment_size);
    //cout << "Data Segment Size = " << rc_data_segment_size << endl;
    __android_log_print(ANDROID_LOG_ERROR, "RC_DATA_SEG_SIZE", "%lld", rc_data_segment_size);
    //cout << "RC_VM_M Size = " << rc_max_m << endl;
    __android_log_print(ANDROID_LOG_ERROR, "RC_MAX_M", "%lld", rc_max_m);
    //cout << "RC_VM_S Size = " << rc_max_s << endl;
    __android_log_print(ANDROID_LOG_ERROR, "RC_MAX_S", "%lld", rc_max_s);
    //cout << "RC_VM_F Size = " << rc_max_f << endl;
    //cout << "RC_NUM_ID Size = " << rc_nid_size << endl;
    //cout << "RC_STR_ID Size = " << rc_sid_size << endl;
    //cout << "RC_NUM_STACK Size = " << rc_num_stack_size << endl;
    //cout << "RC_STR_STACK Size = " << rc_str_stack_size << endl << endl << endl;
    //return 0;

    RC_SEGMENT[RC_CODE_SEGMENT] = new unsigned char[rc_code_segment_size];
    RC_SEGMENT[RC_DATA_SEGMENT] = new unsigned char[rc_data_segment_size];
    rc_vm_m = new double[rc_max_m];
    rc_vm_s = new string[rc_max_s];
    rc_vm_f = new double[rc_max_f];
    rc_nid = new double*[rc_nid_size];
    rc_nid_w = new Uint64[rc_nid_size];
    rc_nid_h = new Uint64[rc_nid_size];
    rc_nid_d = new Uint64[rc_nid_size];
    rc_sid = new string*[rc_sid_size];
    rc_sid_w = new Uint64[rc_sid_size];
    rc_sid_h = new Uint64[rc_sid_size];
    rc_sid_d = new Uint64[rc_sid_size];
    rc_num_stack = new double[rc_num_stack_size];
    rc_str_stack = new string[rc_str_stack_size];

    for(int i = 0; i < rc_nid_size; i++)
    {
        rc_nid[i] = new double[1];
        rc_nid_w[i] = 0;
        rc_nid_h[i] = 0;
        rc_nid_d[i] = 0;
    }

    for(int i = 0; i < rc_sid_size; i++)
    {
        rc_sid[i] = new string[1];
        rc_sid_w[i] = 0;
        rc_sid_h[i] = 0;
        rc_sid_d[i] = 0;
    }

    int rc_end_of_file = 1;

    for(int i = 0; i < rc_code_segment_size; i++)
    {
        if(rc_end_of_file == 0)
            break;
        else
            rc_end_of_file = SDL_RWread(rc_file, &RC_SEGMENT[RC_CODE_SEGMENT][i], 1, 1);
        	//RC_SEGMENT[RC_CODE_SEGMENT][i] = rc_file.get();
    }

    for(int i = 0; i < rc_data_segment_size; i++)
    {
        if(rc_end_of_file == 0)
            break;
        else
            rc_end_of_file = SDL_RWread(rc_file, &RC_SEGMENT[RC_DATA_SEGMENT][i], 1, 1);
        	//RC_SEGMENT[RC_DATA_SEGMENT][i] = rc_file.get();
    }

    SDL_RWclose(rc_file);

    //cout << "BYREF # COUNT = " << rc_byref_nid_count << endl;
    //cout << "BYREF $ COUNT = " << rc_byref_sid_count << endl;

    for(int i = 0; i < rc_byref_nid_count; i++)
        delete [] rc_nid[rc_byref_nid[i]];

    for(int i = 0; i < rc_byref_sid_count; i++)
        delete [] rc_sid[rc_byref_sid[i]];

    int l = 0;

    //SDL_GetBasePath();
    //cout << rc_dir_path << endl; string s; cin >> s;
    //if(chdir(rc_dir_path.c_str())!=0)
    //{
    	//could not change cwd to base directory
    //}

    string init_dir = (string)argv[0];
    init_dir = init_dir.substr(0,init_dir.find_last_of("/")+1);

    int dbg_vm = 0;

    rc_media_init();

    while(RC_VM_ACTIVE)
    {
        //cout << "line " << l << endl;
        //cycleVideo();
        if(rc_checkEvent())
        {
            rc_getEvents();
            //SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
        }
        if(rc_vm_run() < 0)
        {
            break;
        }

        //cout << "RC_VM_ACT = " << RC_VM_ACTIVE << endl;
        //l += 1;
    }

    if(!rc_vm_isClean)
        rc_runtime_clean();

    //cout << "rc media quit\n";

    cout << endl << endl;

    //cout << "\n\n\n@ " << tst_addr << endl;
    //cout << "END = " << (unsigned int) tst_end.data[0] << " : " << (unsigned int) tst_end.data[1] << endl;

    string tst = "";

    //cout << "i = " << rc_nid[2][0] << endl;
    //cout << "x dim = " << rc_sid_w[0] << " " << rc_sid_h[0] << " " << rc_sid_d[0] << endl;

    /*while(true)
    {
        cin >> tst;
        cout << "tst = " << tst << endl;
        if(tst.compare("exit")==0)
            break;
    }*/
    //cout << "rc_closeVM start\n";

    //cout << "VM End\n";
    //rc_closeVM(); //cout << "rc_closeVM success\n";

    //cout << "rc vm memory free\n";

    return 0;
}
