#include <inttypes.h>

using namespace std;

#define RC_STRING_SIZE 512

#define RC_ID_ID 0
#define RC_TMP_ID 1
#define RC_LBL_ID 2

uint64_t rc_id_address = 0;
uint64_t rc_id_size = 10000;
uint64_t rc_id_inc = 10000;

uint64_t rc_tmp_address = 0;
uint64_t rc_tmp_size = 256;
uint64_t rc_tmp_inc = 256;

uint64_t rc_str3_address = 0;
uint64_t rc_str3_size = 5000;
uint64_t rc_str3_inc = 5000;

struct rc_string_array
{
    char * str = NULL;
    uint64_t array_size = 0;
    uint64_t inc_size = 0;
    uint16_t str_size = 0;
    uint64_t str_address = 0;
    int id;
};

struct rc_string_array2
{
    string str[512];
};

struct rc_string_array3
{
    char * str = NULL;
    uint64_t array_size = 0;
    uint64_t inc_size = 0;
    uint16_t str_size = 0;
    uint64_t str_address = 0;
    int id;
};

struct rc_uchar_array
{
    unsigned char * data = NULL;
    uint64_t array_size = 0;
    uint64_t inc_size = 0;
};

rc_string_array rc_createStringArray(uint64_t arr_size = 2048, int id = 0)
{
    rc_string_array new_array;
    new_array.str = (char*)malloc(arr_size * RC_STRING_SIZE);
    new_array.array_size = arr_size;
    new_array.str_size = RC_STRING_SIZE;
    new_array.id = id;

    rc_id_address = (uint64_t)new_array.str;
    rc_id_size = arr_size;

    for(int i = 0; i < arr_size * RC_STRING_SIZE; i++)
    {
        new_array.str[i] = '\0';
    }
    new_array.inc_size = arr_size;
    new_array.str_address = (uint64_t)new_array.str;
    return new_array;
}

rc_uchar_array rc_createUCharArray(uint64_t arr_size)
{
    rc_uchar_array new_array;
    new_array.data = (unsigned char*)malloc(sizeof(unsigned char)*arr_size);
    new_array.array_size = arr_size;
    new_array.inc_size = 1000;
    return new_array;
}

rc_string_array rc_resizeStringArray(rc_string_array* old_array, uint64_t new_size)
{
    rc_string_array new_array;
    int str_size = RC_STRING_SIZE;
    int arr_size = 0;

    arr_size = rc_id_size;
    rc_id_size = new_size;

    new_array.str = (char*)malloc(new_size * RC_STRING_SIZE);
    new_array.array_size = new_size;
    new_array.str_size = RC_STRING_SIZE;
    new_array.id = old_array->id;


    rc_id_address = (uint64_t)new_array.str;

    for(uint64_t i = 0; i < new_size; i++)
    {
        if(i < arr_size)
        {
            int str_size = RC_STRING_SIZE;
            for(int n = 0; n < RC_STRING_SIZE; n++)
            {
                new_array.str[(i*str_size)+n] = old_array->str[(i*str_size)+n];
            }
        }
    }
    free(old_array->str);
    old_array->str = NULL;
    new_array.str_address = (uint64_t)new_array.str;
    return new_array;
}

void rc_deleteStringArray(rc_string_array* rc_array)
{
    rc_array->str = (char*)rc_id_address;

    free(rc_array->str);
    rc_array->str = NULL;
    rc_array->array_size = 0;
}

void rc_setStringArray(rc_string_array* rc_array, int index, string data)
{
    int str_size = RC_STRING_SIZE;
    int inc_size = 0;
    inc_size = rc_id_inc;
    rc_array->array_size = rc_id_size;
    rc_array->str_address = rc_id_address;
    rc_array->str_size = RC_STRING_SIZE;
    rc_array->str = (char*)rc_id_address;

    if(index >= rc_array->array_size)
    {
        *rc_array = rc_resizeStringArray(rc_array, index+inc_size);
    }
    int i = 0;
    for(; i < data.length() && i < str_size; i++)
    {
        rc_array->str[(index*str_size)+i] = data.substr(i,1)[0];
    }
    for(; i < str_size; i++)
    {
        rc_array->str[(index*str_size)+i] = '\0';
    }
}

void rc_setStringArray(rc_string_array2* rc_array, int index, string data)
{
    string s = data;
    rc_array->str[index].swap(s);
}

string rc_getStringArrayValue(rc_string_array2 rc_array, int index)
{
    if(index < 512)
        return rc_array.str[index];
    return "";
}

void rc_clearStringArray(rc_string_array2 * rc_array)
{
    for(int i = 0; i < 512; i++)
    {
        rc_array->str[i] = "";
    }
}

string rc_getStringArrayValue(rc_string_array rc_array, int index)
{
    string rc_array_return("");
    int str_size = RC_STRING_SIZE;
    int arr_size = 0;

    rc_array.str = (char*)rc_id_address;
    arr_size = rc_id_size;

    if(index >= arr_size)
    {
        cout << "attempted to read: _" << rc_array.id << "_ [" << arr_size << "]" << endl;
        cout << "actual size: " << rc_id_size << endl;
        cout << "test: " << rc_array.id << endl;
        cout << "could not read " << index << endl;
        return "";
    }
    for(int i = 0; i < str_size; i++)
    {
        if(rc_array.str[(index*str_size)+i] != '\0')
            rc_array_return.push_back(rc_array.str[(index*str_size)+i]);
        else
            break;
    }
    return rc_array_return;
}


void rc_clearStringArray(rc_string_array * rc_array)
{
    int arr_size = 0;

    arr_size = rc_tmp_size;
    rc_array->str = (char*)rc_tmp_address;

    for(int i = 0; i < arr_size*RC_STRING_SIZE; i++)
    {
        rc_array->str[i] = '\0';
    }
}

rc_string_array3 rc_createStringArray3(uint64_t arr_size = 2048, int id = 0)
{
    rc_string_array3 new_array;
    new_array.str = (char*)malloc(arr_size * RC_STRING_SIZE);
    new_array.array_size = arr_size;
    new_array.str_size = RC_STRING_SIZE;
    new_array.id = id;

    rc_str3_address = (uint64_t)new_array.str;
    rc_str3_size = arr_size;

    for(int i = 0; i < arr_size * RC_STRING_SIZE; i++)
    {
        new_array.str[i] = '\0';
    }
    new_array.inc_size = arr_size;
    new_array.str_address = (uint64_t)new_array.str;
    return new_array;
}

rc_string_array3 rc_resizeStringArray(rc_string_array3* old_array, uint64_t new_size)
{
    rc_string_array3 new_array;
    old_array->str = (char*)rc_str3_address;
    int str_size = RC_STRING_SIZE;
    int arr_size = 0;

    arr_size = rc_str3_size;
    rc_str3_size = new_size;

    new_array.str = (char*)malloc(new_size * RC_STRING_SIZE);
    new_array.array_size = new_size;
    new_array.str_size = RC_STRING_SIZE;
    new_array.id = old_array->id;


    rc_str3_address = (uint64_t)new_array.str;


    for(uint64_t i = 0; i < new_size; i++)
    {
        if(i < arr_size)
        {
            int str_size = RC_STRING_SIZE;
            for(int n = 0; n < RC_STRING_SIZE; n++)
            {
                new_array.str[(i*str_size)+n] = old_array->str[(i*str_size)+n];
            }
        }
    }
    free(old_array->str);
    old_array->str = NULL;
    new_array.str_address = (uint64_t)new_array.str;
    return new_array;
}

void rc_setStringArray(rc_string_array3* rc_array, int index, string data)
{
    int str_size = RC_STRING_SIZE;
    int inc_size = 0;

    inc_size = rc_str3_inc;
    rc_array->array_size = rc_str3_size;
    rc_array->str_address = rc_str3_address;
    rc_array->str_size = RC_STRING_SIZE;
    rc_array->str = (char*)rc_str3_address;

    if(index >= rc_array->array_size)
    {
        *rc_array = rc_resizeStringArray(rc_array, index+inc_size);
    }
    int i = 0;
    for(; i < data.length() && i < str_size; i++)
    {
        rc_array->str[(index*str_size)+i] = data.substr(i,1)[0];
    }
    for(; i < str_size; i++)
    {
        rc_array->str[(index*str_size)+i] = '\0';
    }
}

string rc_getStringArrayValue(rc_string_array3 rc_array, int index)
{
    string rc_array_return("");
    int str_size = RC_STRING_SIZE;
    int arr_size = 0;

    rc_array.str = (char*)rc_str3_address;
    arr_size = rc_str3_size;

    if(index >= arr_size)
    {
        cout << "attempted to read: _" << rc_array.id << "_ [" << arr_size << "]" << endl;
        cout << "actual size: " << rc_id_size << endl;
        cout << "test: " << rc_array.id << endl;
        cout << "could not read " << index << endl;
        return "";
    }
    for(int i = 0; i < str_size; i++)
    {
        if(rc_array.str[(index*str_size)+i] != '\0')
            rc_array_return.push_back(rc_array.str[(index*str_size)+i]);
        else
            break;
    }
    return rc_array_return;
}

void rc_clearStringArray(rc_string_array3 * rc_array)
{
    int arr_size = 0;

    arr_size = rc_str3_size;
    rc_array->str = (char*)rc_str3_address;

    for(int i = 0; i < arr_size*RC_STRING_SIZE; i++)
    {
        rc_array->str[i] = '\0';
    }
}

void rc_deleteStringArray(rc_string_array3* rc_array)
{
    rc_array->str = (char*)rc_str3_address;

    free(rc_array->str);
    rc_array->str = NULL;
    rc_array->array_size = 0;
}
