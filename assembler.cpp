#include <bits/stdc++.h>
using namespace std;

#define bit_1 1
#define bit_3 3
#define bit_5 5
#define bit_7 7
#define bit_12 12
#define bit_20 20
#define bit_32 32

vector<string> inst_vector;

enum class inst_type;

class instruction;
class inst_func3_rs1;
class inst_func7_rs2;
class inst_func12;
class inst_func20;

typedef inst_func7_rs2 r_type;
typedef inst_func12 i_type;
typedef inst_func7_rs2 s_type;
typedef inst_func7_rs2 b_type;
typedef inst_func20 u_type;
typedef inst_func20 j_type;

char* decimal_to_binary(const int dec);
int string_to_decimal(const char *str);
void convert_to_binary();
void convert_to_lowercase(char *str);
vector<char*> extract_from_string(char *str);
int get_bit(const int val, const int i);
bool is_num(const char *str);

enum class inst_type{
    r       = 0x33,
    i1      = 0x13,
    i2      = 0x03,
    i3      = 0x67,
    s       = 0x23,
    b       = 0x63,
    j       = 0x6f,
    u       = 0x37
};

bool is_num(const char *str){
    int strn=strlen(str);
    if(strn<=0) return false;
    if((str[0]=='-') || (('0'<=str[0]) && (str[0]<='9'))) return true;
    return false;
}

void convert_to_lowercase(char *str){
    int strn=strlen(str);
    for(int i=0;i<strn;i++){
        if(('A'<=str[i]) && (str[i]<='Z')){
            str[i]=str[i]-'A'+'a';
        }
    }
}

char* decimal_to_binary(const int dec){
    char *bin = new char[bit_32+1];
    for(int i=0;i<32;i++){
        bin[31-i]=(((1LL << i) & (dec)) != 0)+'0';
    }
    bin[bit_32]='\0';
    return bin;
}

int string_to_decimal(const char *str){
    int val=0;
    long long strn=strlen(str),ten_pow=1,sixteen_pow=1;
    if((strn>=2) && (str[0]=='0') && (str[1]=='x')){
        for(int i=strn-1;i>=2;i--){
            if(('0'<=str[i]) && (str[i]<='9')){
                val+=(str[i]-'0')*sixteen_pow;
            }
            else if(('a'<=str[i]) && (str[i]<='f')){
                val+=(str[i]-'a'+10)*sixteen_pow;
            }
            sixteen_pow*=16;
        }
    }
    else{
        for(int i=strn-1;i>=0;i--){
            if(str[i]=='-'){
                val*=(-1);
                break;
            }
            else{
                val+=(str[i]-'0')*ten_pow;
                ten_pow*=10;
            }
        }
        
    }
    return val;
}

int get_bit(const int val, const int i){
    return (((1LL << i) & (val)) != 0);
}

class instruction{
private:
    char code[bit_32+1];
    
protected:
    void set_bits(const int start, const int end, const int _val){
        char *bin=decimal_to_binary(_val);
        for(int i=end;i>=start;i--){
            code[i]=bin[i+31-end];
        }
        delete bin;
    }
    
public:
    instruction(){
        sprintf(code,"%032d",0);
    }
    
    void set_opcode(const int _op){
        int start=bit_32-bit_7;
        int end=start+bit_7-1;
        set_bits(start,end,_op);
    }
    
    void set_rd(const int _rd){
        int start=bit_32-bit_7-bit_5;
        int end=start+bit_5-1;
        set_bits(start,end,_rd);
    }
    
    void print_code(){
        printf("%s\n",code);
    }
    
    void print_to_file(fstream& output_file){
        output_file<<code<<"\n";
        inst_vector.push_back(code);
    }
};

class inst_func3_rs1 : public instruction{
public:
    inst_func3_rs1() : instruction(){}

    void set_func3(const int _func3){
        int start=bit_32-bit_7-bit_5-bit_3;
        int end=start+bit_3-1;
        set_bits(start,end,_func3);
    }
    
    void set_rs1(const int _rs1){
        int start=bit_32-bit_7-bit_5-bit_3-bit_5;
        int end=start+bit_5-1;
        set_bits(start,end,_rs1);
    }
};

class inst_func7_rs2 : public inst_func3_rs1{
public:
    inst_func7_rs2() : inst_func3_rs1(){}

    void set_rs2(const int _rs2){
        int start=bit_32-bit_7-bit_5-bit_3-bit_5-bit_5;
        int end=start+bit_5-1;
        set_bits(start,end,_rs2);
    }
    
    void set_func7(const int _func7){
        int start=bit_32-bit_7-bit_5-bit_3-bit_5-bit_5-bit_7;
        int end=start+bit_7-1;
        set_bits(start,end,_func7);
    }
};

class inst_func12 : public inst_func3_rs1{
public:
    inst_func12() : inst_func3_rs1(){}

    void set_func12(const int _func12){
        int start=bit_32-bit_7-bit_5-bit_3-bit_5-bit_12;
        int end=start+bit_12-1;
        set_bits(start,end,_func12);
    }
};

class inst_func20 : public instruction{
public:
    inst_func20() : instruction(){}

    void set_func20(const int _func20){
        int start=bit_32-bit_7-bit_5-bit_20;
        int end=start+bit_20-1;
        set_bits(start,end,_func20);
    }
};

vector<char*> extract_from_string(char *str){
    vector<char*> extraction;
    int strn=strlen(str);
    if(strn==0 || str[0]=='#') return extraction;
    bool curr=false;
    for(int i=0;i<strn;i++){
        if(str[i]=='#'){
            str[i]='\0';
            break;
        }
        if((str[i]==' ') || (str[i]==',') || (str[i]=='(') || (str[i]==')') || (str[i]==':')){
            curr=false;
            str[i]='\0';
        }
        else if(!curr){
            curr=true;
            extraction.push_back(str+i);
        }
    }
    return extraction;
}

void convert_to_binary(){
    unordered_map<string,int> inst_map,func3_map,func7_map,reg_map,labels;

    inst_map["add"]=(int)inst_type::r;
    inst_map["sub"]=(int)inst_type::r;
    inst_map["xor"]=(int)inst_type::r;
    inst_map["or"]=(int)inst_type::r;
    inst_map["and"]=(int)inst_type::r;
    inst_map["sll"]=(int)inst_type::r;
    inst_map["srl"]=(int)inst_type::r;
    inst_map["sra"]=(int)inst_type::r;
    inst_map["slt"]=(int)inst_type::r;
    inst_map["sltu"]=(int)inst_type::r;
    
    inst_map["addi"]=(int)inst_type::i1;
    inst_map["xori"]=(int)inst_type::i1;
    inst_map["ori"]=(int)inst_type::i1;
    inst_map["andi"]=(int)inst_type::i1;
    inst_map["slli"]=(int)inst_type::i1;
    inst_map["srli"]=(int)inst_type::i1;
    inst_map["srai"]=(int)inst_type::i1;
    inst_map["slti"]=(int)inst_type::i1;
    inst_map["sltiu"]=(int)inst_type::i1;

    inst_map["lb"]=(int)inst_type::i2;
    inst_map["lh"]=(int)inst_type::i2;
    inst_map["lw"]=(int)inst_type::i2;
    inst_map["lbu"]=(int)inst_type::i2;
    inst_map["lhu"]=(int)inst_type::i2;

    inst_map["jalr"]=(int)inst_type::i3;
    
    inst_map["sb"]=(int)inst_type::s;
    inst_map["sh"]=(int)inst_type::s;
    inst_map["sw"]=(int)inst_type::s;
    
    inst_map["beq"]=(int)inst_type::b;
    inst_map["bne"]=(int)inst_type::b;
    inst_map["blt"]=(int)inst_type::b;
    inst_map["bge"]=(int)inst_type::b;
    inst_map["bltu"]=(int)inst_type::b;
    inst_map["bgeu"]=(int)inst_type::b;
    
    inst_map["lui"]=(int)inst_type::u;
    inst_map["auipc"]=(int)inst_type::u;
    
    inst_map["jal"]=(int)inst_type::j;
    
    func3_map["add"]=0x0;
    func3_map["sub"]=0x0;
    func3_map["xor"]=0x4;
    func3_map["or"]=0x6;
    func3_map["and"]=0x7;
    func3_map["sll"]=0x1;
    func3_map["srl"]=0x5;
    func3_map["sra"]=0x5;
    func3_map["slt"]=0x2;
    func3_map["sltu"]=0x3;
    
    func3_map["addi"]=0x0;
    func3_map["xori"]=0x4;
    func3_map["ori"]=0x6;
    func3_map["andi"]=0x7;
    func3_map["slli"]=0x1;
    func3_map["srli"]=0x5;
    func3_map["srai"]=0x5;
    func3_map["slti"]=0x2;
    func3_map["sltiu"]=0x3;
    
    func3_map["lb"]=0x0;
    func3_map["lh"]=0x1;
    func3_map["lw"]=0x2;
    func3_map["lbu"]=0x4;
    func3_map["lhu"]=0x5;
    
    func3_map["sb"]=0x0;
    func3_map["sh"]=0x1;
    func3_map["sw"]=0x2;
    
    func3_map["beq"]=0x0;
    func3_map["bne"]=0x1;
    func3_map["blt"]=0x4;
    func3_map["bge"]=0x5;
    func3_map["bltu"]=0x6;
    func3_map["bgeu"]=0x7;

    func3_map["jalr"]=0x0;
    
    func7_map["add"]=0x00;
    func7_map["sub"]=0x20;
    func7_map["xor"]=0x00;
    func7_map["or"]=0x00;
    func7_map["and"]=0x00;
    func7_map["sll"]=0x00;
    func7_map["srl"]=0x00;
    func7_map["sra"]=0x20;
    func7_map["slt"]=0x00;
    func7_map["sltu"]=0x00;
    
    reg_map["zero"]=reg_map["x0"]=0;
    reg_map["ra"]=reg_map["x1"]=1;
    reg_map["sp"]=reg_map["x2"]=2;
    reg_map["gp"]=reg_map["x3"]=3;
    reg_map["tp"]=reg_map["x4"]=4;
    reg_map["t0"]=reg_map["x5"]=5;
    reg_map["t1"]=reg_map["x6"]=6;
    reg_map["t2"]=reg_map["x7"]=7;
    reg_map["s0"]=reg_map["fp"]=reg_map["x8"]=8;
    reg_map["s1"]=reg_map["x9"]=9;
    reg_map["a0"]=reg_map["x10"]=10;
    reg_map["a1"]=reg_map["x11"]=11;
    reg_map["a2"]=reg_map["x12"]=12;
    reg_map["a3"]=reg_map["x13"]=13;
    reg_map["a4"]=reg_map["x14"]=14;
    reg_map["a5"]=reg_map["x15"]=15;
    reg_map["a6"]=reg_map["x16"]=16;
    reg_map["a7"]=reg_map["x17"]=17;
    reg_map["s2"]=reg_map["x18"]=18;
    reg_map["s3"]=reg_map["x19"]=19;
    reg_map["s4"]=reg_map["x20"]=20;
    reg_map["s5"]=reg_map["x21"]=21;
    reg_map["s6"]=reg_map["x22"]=22;
    reg_map["s7"]=reg_map["x23"]=23;
    reg_map["s8"]=reg_map["x24"]=24;
    reg_map["s9"]=reg_map["x25"]=25;
    reg_map["s10"]=reg_map["x26"]=26;
    reg_map["s11"]=reg_map["x27"]=27;
    reg_map["t3"]=reg_map["x28"]=28;
    reg_map["t4"]=reg_map["x29"]=29;
    reg_map["t5"]=reg_map["x30"]=30;
    reg_map["t6"]=reg_map["x31"]=31;
    
    int line_number=0;
    const char *input_file_name="assembly_file.s";
    const char *output_file_name="binary_file.o";
    fstream input_file,output_file;

    input_file.open(input_file_name,ios::in);
    input_file.seekg(0,ios::beg);
    line_number=0;
    while(!(input_file.eof())){
        char _line[100];
        input_file.getline(_line,sizeof(_line));
        
        vector<char*> components=extract_from_string(_line);
        int compn=components.size();
        if(compn==0) continue;
        if(compn==1){
            labels[components[0]]=line_number+1;
            continue;
        }
        line_number++;
    }
    input_file.close();

    input_file.open(input_file_name,ios::in);
    input_file.seekg(0,ios::beg);
    output_file.open(output_file_name,ios::out);
    output_file.seekp(0,ios::beg);
    line_number=0;
    while(!(input_file.eof())){
        char _line[100],original_line[100];
        input_file.getline(_line,sizeof(_line));
        strcpy(original_line,_line);
        vector<char*> original_components=extract_from_string(original_line);
        convert_to_lowercase(_line);
        
        vector<char*> components=extract_from_string(_line);
        int compn=components.size();
        if(compn<=1) continue;
        int _type=inst_map[components[0]];
        line_number++;
        
        if(_type==(int)inst_type::r){
            r_type inst;
            inst.set_opcode((int)inst_type::r);
            inst.set_rd(reg_map[components[1]]);
            inst.set_rs1(reg_map[components[2]]);
            inst.set_rs2(reg_map[components[3]]);
            inst.set_func3(func3_map[components[0]]);
            inst.set_func7(func7_map[components[0]]);
            inst.print_to_file(output_file);
        }
        else if(_type==(int)inst_type::i1){
            int imm=string_to_decimal(components[3]);
            if((strcmp(components[0],"slli")==0) || (strcmp(components[0],"srli")==0) || (strcmp(components[0],"srai")==0)){
                imm&=(0x1f);
                if(strcmp(components[0],"srai")==0) imm|=(0x400);
            }
            i_type inst;
            inst.set_opcode((int)inst_type::i1);
            inst.set_rd(reg_map[components[1]]);
            inst.set_rs1(reg_map[components[2]]);
            inst.set_func3(func3_map[components[0]]);
            inst.set_func12(imm);
            inst.print_to_file(output_file);
        }
        else if(_type==(int)inst_type::i2){
            int imm=string_to_decimal(components[2]);
            i_type inst;
            inst.set_opcode((int)inst_type::i2);
            inst.set_rd(reg_map[components[1]]);
            inst.set_rs1(reg_map[components[3]]);
            inst.set_func3(func3_map[components[0]]);
            inst.set_func12(imm);
            inst.print_to_file(output_file);
        }
        else if(_type==(int)inst_type::i3){
            int imm=string_to_decimal(components[2]);
            i_type inst;
            inst.set_opcode((int)inst_type::i3);
            inst.set_rd(reg_map[components[1]]);
            inst.set_rs1(reg_map[components[3]]);
            inst.set_func3(func3_map[components[0]]);
            inst.set_func12(imm);
            inst.print_to_file(output_file);
        }
        else if(_type==(int)inst_type::s){
            int imm=string_to_decimal(components[2]);
            s_type inst;
            inst.set_opcode((int)inst_type::s);
            inst.set_rd(imm);
            inst.set_rs1(reg_map[components[3]]);
            inst.set_rs2(reg_map[components[1]]);
            inst.set_func3(func3_map[components[0]]);
            inst.set_func7(imm>>bit_5);
            inst.print_to_file(output_file);
        }
        else if(_type==(int)inst_type::b){
            int imm;
            if(is_num(components[3])) imm=string_to_decimal(components[3]);
            else imm=(labels[original_components[3]]-line_number)<<2;
            // else imm=(labels[original_components[3]])<<2;
            b_type inst;
            inst.set_opcode((int)inst_type::b);
            inst.set_rs1(reg_map[components[1]]);
            inst.set_rs2(reg_map[components[2]]);
            inst.set_rd(((imm>>bit_1)<<bit_1) + get_bit(imm,11));
            inst.set_func7(((imm>>bit_5)&(0x8000003f)) + (get_bit(imm,12)<<6));
            inst.set_func3(func3_map[components[0]]);
            inst.print_to_file(output_file);
        }
        else if(_type==(int)inst_type::u){
            int imm=string_to_decimal(components[2]);
            u_type inst;
            inst.set_opcode((int)inst_type::u);
            inst.set_rd(reg_map[components[1]]);
            inst.set_func20(imm);
            inst.print_to_file(output_file);
        }
        else if(_type==(int)inst_type::j){
            int imm;
            if(is_num(components[2])) imm=string_to_decimal(components[2]);
            else imm=(labels[original_components[2]]-line_number)<<2;
            // else imm=(labels[original_components[2]])<<2;
            j_type inst;
            inst.set_opcode((int)inst_type::j);
            inst.set_rd(reg_map[components[1]]);
            inst.set_func20(((imm>>bit_12)&(0x800000ff)) + (get_bit(imm,11)<<8) + (((imm>>bit_1)&(0x800003ff))<<9) + (get_bit(imm,20)<<19));
            inst.print_to_file(output_file);
        }
    }
    input_file.close();
    output_file.close();
}

// int main(){
//     convert_to_binary();
//     return 0;
// }