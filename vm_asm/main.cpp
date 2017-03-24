#include <iostream>
#include <vector>
#include <inttypes.h>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>

using namespace std;

#define RC_DATA_SEGMENT_SWITCH 0
#define RC_CODE_SEGMENT_SWITCH 1

vector<string> rc_label;
vector<string> rc_id;
int rc_getID_index = 0;

fstream RC_CODE_SEGMENT;
fstream RC_DATA_SEGMENT;

uint64_t rc_code_offset = 0;
uint64_t rc_data_offset = 0;

int seg_switch = 0;

int vm_data_size = 8;

int nid_byref_count = 0;
int sid_byref_count = 0;

vector<int> nid_byref_index;
vector<int> sid_byref_index;

int nid_count = 0;
int sid_count = 0;
int max_m = 0;
int max_s = 0;
int max_f = 0;
int num_stack_size = 0;
int str_stack_size = 0;

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

std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

void rc_loadData()
{
    fstream id_file("rcbasic_data.data", fstream::in);
    string data;
    while(id_file.good())
    {
        getline(id_file, data);
        if(data.substr(0, data.find_first_of(" ")).compare("nid_byref_count")==0)
        {
            nid_byref_count = atoi(data.substr(data.find_first_of(" ")).c_str());
        }
        else if(data.substr(0, data.find_first_of(" ")).compare("sid_byref_count")==0)
        {
            sid_byref_count = atoi(data.substr(data.find_first_of(" ")).c_str());
        }
        else if(data.substr(0, data.find_first_of(" ")).compare("nid_byref")==0)
        {
            nid_byref_index.push_back(atoi(data.substr(data.find_first_of(" ")).c_str()));
        }
        else if(data.substr(0, data.find_first_of(" ")).compare("sid_byref")==0)
        {
            sid_byref_index.push_back(atoi(data.substr(data.find_first_of(" ")).c_str()));
        }
        else if(data.substr(0, data.find_first_of(" ")).compare("nid_count")==0)
        {
            nid_count = atoi(data.substr(data.find_first_of(" ")).c_str());
        }
        else if(data.substr(0, data.find_first_of(" ")).compare("sid_count")==0)
        {
            sid_count = atoi(data.substr(data.find_first_of(" ")).c_str());
        }
        else if(data.substr(0, data.find_first_of(" ")).compare("max_m")==0)
        {
            max_m = atoi(data.substr(data.find_first_of(" ")).c_str());
        }
        else if(data.substr(0, data.find_first_of(" ")).compare("max_s")==0)
        {
            max_s = atoi(data.substr(data.find_first_of(" ")).c_str());
        }
        else if(data.substr(0, data.find_first_of(" ")).compare("max_f")==0)
        {
            max_f = atoi(data.substr(data.find_first_of(" ")).c_str());
        }
        else if(data.substr(0, data.find_first_of(" ")).compare("num_stack")==0)
        {
            num_stack_size = atoi(data.substr(data.find_first_of(" ")).c_str());
        }
        else if(data.substr(0, data.find_first_of(" ")).compare("str_stack")==0)
        {
            str_stack_size = atoi(data.substr(data.find_first_of(" ")).c_str());
        }
        else if(data.compare("")!=0)
        {
            rc_id.push_back(data);
        }
    }
    id_file.close();
}

string rc_intToString(int a)
{
    stringstream ss;
    ss << a;
    string str = ss.str();
    ss.clear();
    return str;
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

string rc_getID_data(string id, string id_query)
{
    int id_index = 0;
    int line_item = 0;
    int current_count = 0;
    string current_item = "";

    string rc_id_str = "";

    for(id_index = 0; id_index < rc_id.size(); id_index++)
    {
        rc_id_str = rc_id.at(id_index);
        if(rc_id_str.substr(0,rc_id_str.find_first_of(" ")).compare(id)==0)
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

            for(int i = 0; i < rc_id_str.length(); i++)
            {
                //rc_id_str = rc_getStringArrayValue(rc_id, id_index);
                if(rc_id_str.substr(i,1).compare(" ") == 0)
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
                    current_item += rc_id_str.substr(i,1);
                    //cout<<"tr: "<<current_item<<endl;
                }

            }

        }
        else
        {
            //cout << " CHECK<ARBY'S> = " << rc_id[id_index].substr(0,rc_id[id_index].find_first_of(" ")) << " != " << id << endl;
        }
    }

    return "";
}

bool rc_isID(string s_line)
{
    for(int i = 0; i < rc_id.size(); i++)
    {
        if(s_line.compare(rc_id.at(i).substr(0,s_line.length()))==0)
            return true;
    }
    return false;
}

int rc_getLabelAddress(string lbl)
{
    string l_addr = "";
    for(int i = 0; i < rc_label.size(); i++)
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
    for(int i = start_index; i < rc_label.size(); i++)
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
    if(seg_switch == RC_CODE_SEGMENT_SWITCH)
    {
        RC_CODE_SEGMENT.put(seg_byte);
        rc_code_offset++;
        return 0;
    }
    else if(seg_switch == RC_DATA_SEGMENT_SWITCH)
    {
        RC_DATA_SEGMENT.put(seg_byte);
        rc_data_offset++;
        return 0;
    }
    return 1;
}

struct lbl_cnt
{
    unsigned int tmp_index;
    unsigned int label_index;
};

vector<lbl_cnt> tmp_to_label;

unsigned int rc_getLabelIndex(int tmp_index)
{
    for(int i = 0; i < tmp_to_label.size(); i++)
    {
        if(tmp_to_label.at(i).tmp_index == tmp_index)
            return tmp_to_label.at(i).label_index;
    }
    return 0;
}

int rc_writeByteCode(string in_file)
{
    RC_CODE_SEGMENT.open("rcbasic_code.rc_bin", fstream::out | fstream::binary);
    RC_DATA_SEGMENT.open("rcbasic_data.rc_bin", fstream::app | fstream::binary);
    int byte_count = 0;
    string lbl = "";
    unsigned int seg[3];
    seg[0] = 0;
    seg[1] = 0;
    seg[2] = 0;
    unsigned int cs_code = 0;
    unsigned int cs_data = 1;
    unsigned int cs_index = 0;
    fstream rc_asm_file(in_file.c_str(), fstream::in);

    string tmp_str("");
    lbl_cnt lc;
    //get address of labels
    int ttl_index = 0;
    int rc_label_count = 0;

    //cout << "RC_CODE_SEG = " << RC_CODE_SEGMENT_SWITCH << endl;
    //cout << "RC_DATA_SEG = " << RC_DATA_SEGMENT_SWITCH << endl;

    for(int i = 0; !rc_asm_file.eof(); i++)
    {
        //tmp_to_label[i] = rc_label_count;
        //tmp_str = rc_getStringArrayValue(tmp,i);
        //cout << "cs_start: " << cs_index << endl;

        getline(rc_asm_file, tmp_str);

        //cout << "ASM " << seg[cs_index] << ": " << tmp_str << endl;

        tmp_str += " ";

        //cout << "line " << i << ": " << tmp_str << endl;

        if(tmp_str.substr(0, tmp_str.find_first_of(" ")).compare(".CODE")==0)
        {
            //cout << "change seg" << endl;
            cs_index = RC_CODE_SEGMENT_SWITCH;
        }

        if(tmp_str.substr(0, tmp_str.find_first_of(" ")).compare(".DATA")==0)
        {
            //cout << "change seg" << endl;
            cs_index = RC_DATA_SEGMENT_SWITCH;
        }

        //cout << "cs_end: " << cs_index << endl;

        if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("mov")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("mov$")==0 ||
           tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("pwr")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("mul")==0 ||
           tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("div")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("mod")==0 ||
           tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("add")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("add$")==0 ||
           tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("sub")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("shl")==0 ||
           tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("shr")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("cmp")==0 ||
           tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("cmp$")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("and")==0 ||
           tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("or")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("str$")==0 ||
           tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("ptr")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("abs_sub")==0 ||
           tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("xor")==0)
        {
            seg[cs_index] += 20;
        }
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("push")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("push$")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("pop")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("pop$")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("abs")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("end")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("gosub")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("intern")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("while")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("wend")==0)
        {
            seg[cs_index] += 11;
        }
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("arr1")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("arr1$")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("set_arr1")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("set_arr1$")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("alloc")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("for")==0)
        {
            seg[cs_index] += 29;
        }
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("arr2")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("arr2$")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("set_arr2")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("set_arr2$")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("dim")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("dim$")==0)
        {
            seg[cs_index] += 38;
        }
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("arr3")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("arr3$")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("set_arr3")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("set_arr3$")==0)
        {
            seg[cs_index] += 47;
        }
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("return")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("next")==0||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("do")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("loop")==0)
        {
            seg[cs_index] += 2;
        }
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jmp")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("je")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jne")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jg")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jge")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jl")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jle")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("loop_until")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("loop_while")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("print")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("not")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("exit")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("r_exit")==0)
        {
            seg[cs_index] += 11;
        }
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("label")==0)
        {
            lc.tmp_index = i;
            lc.label_index = rc_label_count;
            tmp_to_label.push_back(lc);
            //ttl_index++;
            lbl = tmp_str.substr(tmp_str.find_first_of("[")+1);
            lbl = lbl.substr(0, lbl.find_last_of("]"));
            //cout << "###" <<lbl << "###" << endl;
            //vector<string>::iterator it = rc_label.begin();
            if(cs_index == RC_CODE_SEGMENT_SWITCH)
            {
                rc_label.push_back(lbl + " " + rc_intToString(seg[cs_index]+rc_code_offset));
            }
            else if(cs_index == RC_DATA_SEGMENT_SWITCH)
            {
                rc_label.push_back(lbl + " " + rc_intToString(seg[cs_index]+rc_data_offset));
            }
            else
            {
                cout << "Writing to non existent segment: " << cs_index << endl;
                //cout << "data seg: " << RC_DATA_SEGMENT_SWITCH << endl;
                //cout << "code seg: " << RC_CODE_SEGMENT_SWITCH << endl;
                return -1;
            }
            //cout << "{" << cs_index << "} " << lbl << ": CREATE LB = " << seg[cs_index]+rc_code_offset << endl;
            //cout << "LABEL_COUNT = " << rc_label_count << endl;
            //cout << "label = " << rc_label.at(rc_label_count) << endl;
            //cout << "cs = " << cs_index << endl;
            //cout << "TTL tmp = " << tmp_to_label.at(rc_label_count).tmp_index << endl;
            rc_label_count++;
        }
        //cout << "LINE [" << i << "] = " << seg[cs_index] << endl;
    }

    rc_asm_file.close();

    //cout << "phase 2" << endl;

    rc_asm_file.open("rcbasic.rc_asm", fstream::in);

    fstream rc_final_asm("rcbasic_final.rc_asm", fstream::out | fstream::trunc);

    //replace jmp and gosub labels with address
    for(int i = 0; rc_asm_file.good(); i++)
    {
        getline(rc_asm_file, tmp_str);

        if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jmp")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("je")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jne")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jg")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jge")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jl")==0 ||
                tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jle")==0 || tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("wend")==0)
        {
            //cout << "L_LINE = " << tmp[i] << endl;
            lbl = tmp_str.substr(tmp_str.find_first_of("[")+1);
            lbl = lbl.substr(0,lbl.find_first_of("]"));
            if(tmp_str.find("!fwd!")!=string::npos)
            {
                //cout << "encountered FWD: " << tmp_to_label[i] << endl; string minaj; cin >> minaj;
                rc_final_asm << tmp_str.substr(0, tmp_str.find_first_of(" ")) + " " + rc_intToString(rc_getLabelAddressFrom(lbl, rc_getLabelIndex(i)))<<endl;
            }
            else
            {
                rc_final_asm << tmp_str.substr(0, tmp_str.find_first_of(" ")) + " " + rc_intToString(rc_getLabelAddress(lbl))<<endl;
            }
            //cout << "%%% " << lbl << "=" << rc_intToString(rc_getLabelAddress(lbl)) << endl;
        }
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("gosub")==0)
        {
            lbl = tmp_str.substr(tmp_str.find_first_of("[")+1);
            lbl = lbl.substr(0,lbl.find_first_of("]"));
            rc_final_asm << tmp_str.substr(0, tmp_str.find_first_of(" ")) + " " + rc_intToString(rc_getLabelAddress(lbl)) << endl;
            //cout << "N_LINE = " << tmp[i] << endl;
        }
        else
        {
            rc_final_asm << tmp_str << endl;
        }
    }

    rc_asm_file.close();
    rc_final_asm.close();

    //replace readable instructions with byte code
    rc_final_asm.open("rcbasic_final.rc_asm", fstream::in);
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
    for(int i = 0; rc_final_asm.good(); i++)
    {
        //tmp_str = rc_getStringArrayValue(tmp,i);
        getline(rc_final_asm, tmp_str);
        //cout << "GB:"<<tmp_str<<endl;

        if(tmp_str.compare(".CODE")==0)
            cs_index = RC_CODE_SEGMENT_SWITCH;

        if(tmp_str.compare(".DATA")==0)
            cs_index = RC_DATA_SEGMENT_SWITCH;

        if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("mov")==0)
        {
            cmd.i = 32;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("mov$")==0)
        {
            cmd.i = 33;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("pwr")==0)
        {
            cmd.i = 34;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("mul")==0)
        {
            cmd.i = 35;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("div")==0)
        {
            cmd.i = 36;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("mod")==0)
        {
            cmd.i = 37;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("add")==0)
        {
            cmd.i = 38;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("add$")==0)
        {
            cmd.i = 39;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("sub")==0)
        {
            cmd.i = 40;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("shl")==0)
        {
            cmd.i = 41;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("shr")==0)
        {
            cmd.i = 42;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("cmp")==0)
        {
            cmd.i = 43;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("cmp$")==0)
        {
            cmd.i = 44;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("str$")==0)
        {
            cmd.i = 45;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("and")==0)
        {
            cmd.i = 46;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("or")==0)
        {
            cmd.i = 47;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("push")==0)
        {
            cmd.i = 48;
            v1 = rc_getParameter(tmp_str, 1);

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("push$")==0)
        {
            cmd.i = 49;
            v1 = rc_getParameter(tmp_str, 1);

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("pop")==0)
        {
            cmd.i = 50;
            v1 = rc_getParameter(tmp_str, 1);

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("pop$")==0)
        {
            cmd.i = 51;
            v1 = rc_getParameter(tmp_str, 1);

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("gosub")==0)
        {
            cmd.i = 52;
            v1 = rc_getParameter(tmp_str, 1);

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("arr1")==0)
        {
            cmd.i = 53;
            v1 = rc_getParameter(tmp_str, 1); //ID
            v2 = rc_getParameter(tmp_str, 2); //Index
            v3 = rc_getParameter(tmp_str, 3); //Destination

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("arr2")==0)
        {
            cmd.i = 54;
            v1 = rc_getParameter(tmp_str, 1); //ID
            v2 = rc_getParameter(tmp_str, 2); //Index 1
            v3 = rc_getParameter(tmp_str, 3); //Index 2
            v4 = rc_getParameter(tmp_str, 4); //Destination

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("arr3")==0)
        {
            cmd.i = 55;
            v1 = rc_getParameter(tmp_str, 1); //ID
            v2 = rc_getParameter(tmp_str, 2); //Index 1
            v3 = rc_getParameter(tmp_str, 3); //Index 2
            v4 = rc_getParameter(tmp_str, 4); //Index 3
            v5 = rc_getParameter(tmp_str, 5); //Destination

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("arr1$")==0)
        {
            cmd.i = 56;
            v1 = rc_getParameter(tmp_str, 1); //ID
            v2 = rc_getParameter(tmp_str, 2); //Index
            v3 = rc_getParameter(tmp_str, 3); //Destination

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("arr2$")==0)
        {
            cmd.i = 57;
            v1 = rc_getParameter(tmp_str, 1); //ID
            v2 = rc_getParameter(tmp_str, 2); //Index 1
            v3 = rc_getParameter(tmp_str, 3); //Index 2
            v4 = rc_getParameter(tmp_str, 4); //Destination

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("arr3$")==0)
        {
            cmd.i = 58;
            v1 = rc_getParameter(tmp_str, 1); //ID
            v2 = rc_getParameter(tmp_str, 2); //Index 1
            v3 = rc_getParameter(tmp_str, 3); //Index 2
            v4 = rc_getParameter(tmp_str, 4); //Index 3
            v5 = rc_getParameter(tmp_str, 5); //Destination

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("alloc")==0)
        {
            cmd.i = 59;
            v1 = rc_getParameter(tmp_str, 1); //Size
            v2 = rc_getParameter(tmp_str, 2); //Type
            v3 = rc_getParameter(tmp_str, 3); //Index

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("set_arr1")==0)
        {
            cmd.i = 62;
            v1 = rc_getParameter(tmp_str, 1); //ID
            v2 = rc_getParameter(tmp_str, 2); //Index
            v3 = rc_getParameter(tmp_str, 3); //Value

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("set_arr2")==0)
        {
            cmd.i = 63;
            v1 = rc_getParameter(tmp_str, 1); //ID
            v2 = rc_getParameter(tmp_str, 2); //Index 1
            v3 = rc_getParameter(tmp_str, 3); //Index 2
            v4 = rc_getParameter(tmp_str, 4); //Value

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("set_arr3")==0)
        {
            cmd.i = 64;
            v1 = rc_getParameter(tmp_str, 1); //ID
            v2 = rc_getParameter(tmp_str, 2); //Index 1
            v3 = rc_getParameter(tmp_str, 3); //Index 2
            v4 = rc_getParameter(tmp_str, 4); //Index 3
            v5 = rc_getParameter(tmp_str, 5); //Value

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("set_arr1$")==0)
        {
            cmd.i = 65;
            v1 = rc_getParameter(tmp_str, 1); //ID
            v2 = rc_getParameter(tmp_str, 2); //Index
            v3 = rc_getParameter(tmp_str, 3); //Value

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("set_arr2$")==0)
        {
            cmd.i = 66;
            v1 = rc_getParameter(tmp_str, 1); //ID
            v2 = rc_getParameter(tmp_str, 2); //Index 1
            v3 = rc_getParameter(tmp_str, 3); //Index 2
            v4 = rc_getParameter(tmp_str, 4); //Value

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("set_arr3$")==0)
        {
            cmd.i = 67;
            v1 = rc_getParameter(tmp_str, 1); //ID
            v2 = rc_getParameter(tmp_str, 2); //Index 1
            v3 = rc_getParameter(tmp_str, 3); //Index 2
            v4 = rc_getParameter(tmp_str, 4); //Index 3
            v5 = rc_getParameter(tmp_str, 5); //Value

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jmp")==0)
        {
            cmd.i = 68;
            v1 = rc_getParameter(tmp_str, 1); //address

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("je")==0)
        {
            cmd.i = 69;
            v1 = rc_getParameter(tmp_str, 1); //address

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jne")==0)
        {
            cmd.i = 70;
            v1 = rc_getParameter(tmp_str, 1); //address

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jl")==0)
        {
            cmd.i = 71;
            v1 = rc_getParameter(tmp_str, 1); //address

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jg")==0)
        {
            cmd.i = 72;
            v1 = rc_getParameter(tmp_str, 1); //address

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jle")==0)
        {
            cmd.i = 73;
            v1 = rc_getParameter(tmp_str, 1); //address

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("jge")==0)
        {
            cmd.i = 74;
            v1 = rc_getParameter(tmp_str, 1); //address

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("abs")==0)
        {
            cmd.i = 75;
            v1 = rc_getParameter(tmp_str, 1); //address

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("end")==0)
        {
            cmd.i = 76;
            v1 = rc_getParameter(tmp_str, 1); //address

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("return")==0)
        {
            cmd.i = 77;

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " " << endl;
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);
        }
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("xor")==0)
        {
            cmd.i = 78;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("not")==0)
        {
            cmd.i = 79;
            v1 = rc_getParameter(tmp_str, 1); //address

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("dim")==0)
        {
            cmd.i = 80;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
            v3 = rc_getParameter(tmp_str, 3);
            v4 = rc_getParameter(tmp_str, 4);

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("dim$")==0)
        {
            cmd.i = 81;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
            v3 = rc_getParameter(tmp_str, 3);
            v4 = rc_getParameter(tmp_str, 4);

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("intern")==0)
        {
            cmd.i = 82;
            v1 = rc_getParameter(tmp_str, 1);

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("delete")==0)
        {
            cmd.i = 83;
            v1 = rc_getParameter(tmp_str, 1);


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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("ptr")==0)
        {
            cmd.i = 84;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("abs_sub")==0)
        {
            cmd.i = 85;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("for")==0)
        {
            cmd.i = 86;
            v1 = rc_getParameter(tmp_str, 1);
            v2 = rc_getParameter(tmp_str, 2);
            v3 = rc_getParameter(tmp_str, 3);

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("next")==0)
        {
            cmd.i = 87;

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << endl;
        }
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("while")==0)
        {
            cmd.i = 88;
            v1 = rc_getParameter(tmp_str, 1);

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("wend")==0)
        {
            cmd.i = 89;
            v1 = rc_getParameter(tmp_str, 1);

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("do")==0)
        {
            cmd.i = 90;

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << endl;
        }
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("loop")==0)
        {
            cmd.i = 91;

            //cout << "OUTPUT: " << (int)cmd.data[0] << " " << (int)cmd.data[1] << " ";
            rc_pokeSegment(cs_index, cmd.data[0]);
            rc_pokeSegment(cs_index, cmd.data[1]);

            //cout << endl;
        }
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("loop_until")==0)
        {
            cmd.i = 92;
            v1 = rc_getParameter(tmp_str, 1);

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("loop_while")==0)
        {
            cmd.i = 93;
            v1 = rc_getParameter(tmp_str, 1);

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("exit")==0)
        {
            //cout << "exit cmd\n";
            cmd.i = 94;
            v1 = rc_getParameter(tmp_str, 1);

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("print")==0)
        {
            cmd.i = 95;
            v1 = rc_getParameter(tmp_str, 1);

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
        else if(tmp_str.substr(0,tmp_str.find_first_of(" ")).compare("r_exit")==0)
        {
            //cout << "r_exit cmd\n";
            cmd.i = 96;
            v1 = rc_getParameter(tmp_str, 1);

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

    rc_final_asm.close();

    RC_DATA_SEGMENT.close();
    RC_CODE_SEGMENT.close();

    return 0;
}

int rc_outputCBC(string out_file)
{
    //    string out_file = in_file;
    //out_file = out_file.substr(0, out_file.find_first_of(".")) + ".cbc";
    cout << "Outputting " << out_file << endl;
    //return 0;

    int rc_dbg_var = 0;

    //cout << "Final Steps" << endl;

    fstream rc_file(out_file.c_str(), fstream::out | fstream::binary | fstream::trunc);
    u_index seg_size;

    //cout << "debug " << rc_dbg_var << endl;
    //rc_dbg_var++;

    seg_size.i = rc_code_offset;

    //cout << seg_size.i << endl;

    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    //cout << "debug " << rc_dbg_var << endl;
    //rc_dbg_var++;

    seg_size.i = rc_data_offset;

    //cout << seg_size.i << endl;

    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    //cout << "debug " << rc_dbg_var << endl;
    //rc_dbg_var++;

    seg_size.i = nid_byref_count;

    //cout << seg_size.i << endl;

    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    //cout << "debug " << rc_dbg_var << endl;
    //rc_dbg_var++;

    seg_size.i = sid_byref_count;

    //cout << seg_size.i << endl;

    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    //cout << "debug " << rc_dbg_var << endl;
    //rc_dbg_var++;

    for(int i = 0; i < nid_byref_count; i++)
    {
        //cout << "BYREF -> " << nid_byref_index[i] << endl;
        seg_size.i = nid_byref_index[i];

        //cout << seg_size.i << endl;

        for(int n = 0; n < sizeof(seg_size.i); n++)
            rc_file.put(seg_size.data[n]);
    }
    //string bs;cin>>bs;

    //cout << "debug " << rc_dbg_var << endl;
    //rc_dbg_var++;

    for(int i = 0; i < sid_byref_count; i++)
    {
        seg_size.i = sid_byref_index[i];

        //cout << seg_size.i << endl;

        for(int n = 0; n < sizeof(seg_size.i); n++)
            rc_file.put(seg_size.data[n]);
    }

    //cout << "debug " << rc_dbg_var << endl;
    //rc_dbg_var++;

    seg_size.i = nid_count;
    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    //cout << "debug " << rc_dbg_var << endl;
    //rc_dbg_var++;

    seg_size.i = sid_count;
    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    //cout << "debug " << rc_dbg_var << endl;
    //rc_dbg_var++;

    seg_size.i = max_m;
    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    //cout << "debug " << rc_dbg_var << endl;
    //rc_dbg_var++;

    seg_size.i = max_s;
    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    //cout << "debug " << rc_dbg_var << endl;
    //rc_dbg_var++;

    seg_size.i = max_f;
    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    //cout << "debug " << rc_dbg_var << endl;
    //rc_dbg_var++;

    seg_size.i = num_stack_size+32;

    //cout << seg_size.i << endl;

    //cout << "debug " << rc_dbg_var << endl;
    //rc_dbg_var++;

    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    //cout << "debug " << rc_dbg_var << endl;
    //rc_dbg_var++;

    seg_size.i = str_stack_size+32;
    for(int i = 0; i < sizeof(seg_size.i); i++)
        rc_file.put(seg_size.data[i]);

    //cout << "debug " << rc_dbg_var << endl;
    //rc_dbg_var++;

    //cout << "Code Bin" << endl;

    RC_CODE_SEGMENT.open("rcbasic_code.rc_bin", fstream::in | fstream::binary);
    unsigned char byte_in = 0;
    for(int i = 0; i < rc_code_offset; i++)
    {
        if(RC_CODE_SEGMENT.eof())
        {
            cout << "code seg error" << endl;
            rc_file.close();
            RC_CODE_SEGMENT.close();
            //remove("rcbasic_code.rc_bin");
            return 0;
        }
        byte_in = RC_CODE_SEGMENT.get();
        rc_file.put(byte_in);
        //rc_file.put(RC_CODE_SEGMENT.data[i]);
    }
    RC_CODE_SEGMENT.close();

    cout << "code offset: " << rc_code_offset << endl;
    //rc_dbg_var++;

    RC_DATA_SEGMENT.open("rcbasic_data.rc_bin", fstream::in | fstream::binary);
    for(int i = 0; i < rc_data_offset; i++)
    {
        if(RC_DATA_SEGMENT.eof())
        {
            cout << "data seg error: " << i << " ; " << rc_data_offset << endl;
            cout << "last byte: " << (int)byte_in << endl;
            rc_file.close();
            RC_DATA_SEGMENT.close();
            //remove("rcbasic_code.rc_bin");
            return 0;
        }
        byte_in = RC_DATA_SEGMENT.get();
        rc_file.put(byte_in);
        //rc_file.put(RC_DATA_SEGMENT.data[i]);
    }

    cout << "data offset: " << rc_data_offset << endl;

    RC_DATA_SEGMENT.close();

    rc_file.close();

    cout << "Compiled " << out_file << " Successfully" << endl;

}

void rc_compileCBC(string out_file)
{
    rc_data_offset = filesize("rcbasic_data.rc_bin");
    rc_loadData();
//    cout << "nid_byref_count = " << nid_byref_count << endl;
//    cout << "sid_byref_count = " << sid_byref_count << endl;
//    for(int i = 0; i < nid_byref_index.size(); i++)
//        cout << "nid_b " << nid_byref_index.at(i) << endl;
//    for(int i = 0; i < sid_byref_index.size(); i++)
//        cout << "sid_b " << sid_byref_index.at(i) << endl;
//    cout << "nid_count = " << nid_count << endl;
//    cout << "sid_count = " << sid_count << endl;
//    cout << "max_m = " << max_m << endl;
//    cout << "max_s = " << max_s << endl;
//    cout << "max_f = " << max_f << endl;
//    cout << "num_stack = " << num_stack_size << endl;
//    cout << "str_stack = " << str_stack_size << endl;
    rc_writeByteCode("rcbasic.rc_asm");
    rc_outputCBC(out_file);
}

int main(int argc, char * argv[])
{
    string out_file = "";
    if(argc > 1)
        out_file = argv[1];
    else
    {
        cout << "Expected Output File As Argument" << endl;
        return 0;
    }

    cout << endl << "Generating Bytecode...." << endl;
    rc_compileCBC(out_file);
    exit(EXIT_SUCCESS);
    return 0;
}
