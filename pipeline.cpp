#include <bits/stdc++.h>
#include <windows.h>
#include "assembler.cpp"
using namespace std;

#define REG_MEM_SIZE 32
#define DATA_MEM_SIZE 1024

#define TL (char)201
#define TR (char)187
#define BL (char)200
#define BR (char)188
#define HOR (char)205
#define VER (char)186
#define MARGIN 10
#define TIME_DELAY 100

vector<int> inst_nums;

enum class inst_type;

struct control_word;
struct pipe_IF;
struct pipe_IFID;
struct pipe_IDEX;
struct pipe_EXMO;
struct pipe_MOWB;

int reg_mem[REG_MEM_SIZE];
int reg_stall[REG_MEM_SIZE];
int data_mem[DATA_MEM_SIZE];

// enum class inst_type{
//     r       = 0x33,
//     i1      = 0x13,
//     i2      = 0x03,
//     i3      = 0x67,
//     s       = 0x23,
//     b       = 0x63,
//     j       = 0x6f,
//     u       = 0x37
// };

int get_bits_in_range(int val, int start, int end){
    if(start>end) swap(start,end);
    int ans=0;
    ans=(val>>start);
    ans&=((1u<<(end-start+1))-1);
    return ans;
}

int binary_to_decimal(const char *str){
    int decimal=0u;
    int strn=strlen(str);
    for(int i=strn-1;i>=0;i--){
        if(str[i]=='0') continue;
        decimal|=(1u<<(strn-1-i));
    }
    return decimal;
}

struct control_word{
    int RegRead;    // 2-bit number, 1th bit represents rs1, 0th bit represents rs2
    bool RegWrite;
    bool ALUSrc;
    int ALUOp;
    bool MemRead;
    bool MemWrite;
    bool MemToReg;
    bool Branch;
    bool Jump;

    control_word(){
        RegRead=0x0;
        RegWrite=false;
        ALUSrc=false;
        ALUOp=0;
        MemRead=false;
        MemWrite=false;
        MemToReg=false;
        Branch=false;
        Jump=false;
    }
};

struct pipe_IF{
    bool active;
    int pc;
    
    pipe_IF(){
        pc=0;
        active=true;
    }
};

struct pipe_IFID{
    bool active;
    int dpc;
    int npc;
    int ir;
    
    pipe_IFID(){
        active=false;
    }
};

struct pipe_IDEX{
    bool active;
    int dpc;
    int jpc;
    int imm;
    int rs1;
    int rs2;
    int func3;
    int func7;
    int rd;
    control_word cw;
    
    pipe_IDEX() : cw(){
        active=false;
    }
};

struct pipe_EXMO{
    bool active;
    int ALUOut;
    int rs2;
    int rd;
    int dpc;        // just for printing purposes, not used in any logic
    control_word cw;
    
    pipe_EXMO() : cw(){
        active=false;
    }
};

struct pipe_MOWB{
    bool active;
    int LDOut;
    int ALUOut;
    int rd;
    int dpc;        // just for printing purposes, not used in any logic
    control_word cw;
    
    pipe_MOWB() : cw(){
        active=false;
    }
};

int get_opcode(int inst){
    return get_bits_in_range(inst,0,6);
}

int get_rd(int inst){
    return get_bits_in_range(inst,7,11);
}

int get_func3(int inst){
    return get_bits_in_range(inst,12,14);
}

int get_rs1(int inst){
    return get_bits_in_range(inst,15,19);
}

int get_rs2(int inst){
    return get_bits_in_range(inst,20,24);
}

int get_func7(int inst){
    return get_bits_in_range(inst,25,31);
}

int fix_imm(int imm, int bits){
    int _bit=get_bits_in_range(imm,bits-1,bits-1);
    if(_bit==0) return imm;
    int bitmask=(0xffffffff)-((1u<<(bits-1))-1);
    return (imm|bitmask);
}

int get_imm(int inst){
    int imm=0;
    int _opcode=get_opcode(inst);
    if((_opcode==(int)inst_type::i1) || (_opcode==(int)inst_type::i2) || (_opcode==(int)inst_type::i3)){
        imm=get_bits_in_range(inst,20,31);
        imm=fix_imm(imm,12);
    }
    else if(_opcode==(int)inst_type::s){
        imm=(get_bits_in_range(inst,7,11)+((get_bits_in_range(inst,25,31))<<5));
        imm=fix_imm(imm,12);
    }
    else if(_opcode==(int)inst_type::b){
        imm=(get_bits_in_range(inst,8,11))+(get_bits_in_range(inst,25,30)<<4)+(get_bits_in_range(inst,7,7)<<10)+(get_bits_in_range(inst,31,31)<<11);
        imm=fix_imm(imm,12);
    }
    else if(_opcode==(int)inst_type::u){
        imm=get_bits_in_range(inst,12,31);
        imm=fix_imm(imm,20);
    }
    else if(_opcode==(int)inst_type::j){
        imm=(get_bits_in_range(inst,21,30))+(get_bits_in_range(inst,20,20)<<10)+(get_bits_in_range(inst,12,19)<<11)+(get_bits_in_range(inst,31,31)<<19);
        imm=fix_imm(imm,20);
    }
    return imm;
}

void execute_control_unit(control_word& _control_word, int _opcode){
    _control_word=control_word();
    _control_word.ALUOp=_opcode;
    if(_opcode==(int)inst_type::r){
        _control_word.RegRead=0x3;
        _control_word.RegWrite=true;
    }
    else if(_opcode==(int)inst_type::i1){
        _control_word.RegRead=0x2;
        _control_word.RegWrite=true;
        _control_word.ALUSrc=true;
    }
    else if(_opcode==(int)inst_type::i2){
        _control_word.RegRead=0x2;
        _control_word.RegWrite=true;
        _control_word.ALUSrc=true;
        _control_word.MemRead=true;
        _control_word.MemToReg=true;
    }
    else if(_opcode==(int)inst_type::i3){
        _control_word.RegRead=0x2;
        _control_word.RegWrite=true;
        _control_word.ALUSrc=true;
        _control_word.Jump=true;
    }
    else if(_opcode==(int)inst_type::s){
        _control_word.RegRead=0x3;
        _control_word.ALUSrc=true;
        _control_word.MemWrite=true;
    }
    else if(_opcode==(int)inst_type::b){
        _control_word.RegRead=0x3;
        _control_word.Branch=true;
    }
    else if(_opcode==(int)inst_type::u){
        _control_word.RegWrite=true;
    }
    else if(_opcode==(int)inst_type::j){
        _control_word.RegWrite=true;
        _control_word.Jump=true;
    }
}

bool fetch(pipe_IF& _pipe_IF, pipe_IFID& _pipe_IFID, vector<int>& inst_mem){
    if(!_pipe_IF.active) return false;

    int inst_mem_n=inst_mem.size();

    if((_pipe_IF.pc>=inst_mem_n) || (_pipe_IF.pc<0)){
        _pipe_IF.active=false;
        return false;
    }

    _pipe_IFID.dpc=_pipe_IF.pc;
    _pipe_IFID.npc=_pipe_IF.pc+1;
    _pipe_IFID.ir=inst_mem[_pipe_IF.pc];
    _pipe_IFID.active=true;
    inst_nums[1]=_pipe_IFID.dpc+1;

    int _opcode=get_opcode(inst_mem[_pipe_IF.pc]);
    if((_opcode==(int)inst_type::b) || (_opcode==(int)inst_type::j) || (_opcode==(int)inst_type::i3)){
        _pipe_IF.active=false;
    }
    else{
        _pipe_IF.pc+=1;
        _pipe_IF.active=true;
        inst_nums[0]=_pipe_IF.pc+1;
    }
    return true;
}

bool decode(pipe_IFID& _pipe_IFID, pipe_IDEX& _pipe_IDEX){
    if(!_pipe_IFID.active) return false;

    int _opcode=get_opcode(_pipe_IFID.ir);
    execute_control_unit(_pipe_IDEX.cw,_opcode);
    int _rs1=(-1);
    if(((_pipe_IDEX.cw.RegRead)&(0x2))!=0) _rs1=get_rs1(_pipe_IFID.ir);
    int _rs2=(-1);
    if(((_pipe_IDEX.cw.RegRead)&(0x1))!=0) _rs2=get_rs2(_pipe_IFID.ir);
    int _rd=get_rd(_pipe_IFID.ir);
    if(((_rs1!=(-1)) && (reg_stall[_rs1]!=0)) || ((_rs2!=(-1)) && (reg_stall[_rs2]!=0))) return false;

    if(_pipe_IDEX.cw.RegWrite) reg_stall[_rd]++;
    _pipe_IDEX.rs1=reg_mem[_rs1];
    _pipe_IDEX.rs2=reg_mem[_rs2];
    _pipe_IDEX.rd=_rd;
    _pipe_IDEX.func3=get_func3(_pipe_IFID.ir);
    _pipe_IDEX.func7=get_func7(_pipe_IFID.ir);
    _pipe_IDEX.imm=get_imm(_pipe_IFID.ir);
    _pipe_IDEX.dpc=_pipe_IFID.dpc;
    _pipe_IDEX.jpc=((_pipe_IDEX.imm)>>1);
    _pipe_IDEX.active=true;
    _pipe_IFID.active=false;
    inst_nums[2]=_pipe_IDEX.dpc+1;
    inst_nums[1]=0;
    return true;
}

bool execute(pipe_IDEX& _pipe_IDEX, pipe_EXMO& _pipe_EXMO, pipe_IF& _pipe_IF, unordered_map<string,int>& func3_map, unordered_map<string,int>& func7_map){
    if(!_pipe_IDEX.active) return false;

    int input1,input2,ALUResult;
    input1=_pipe_IDEX.rs1;
    if(_pipe_IDEX.cw.ALUSrc) input2=_pipe_IDEX.imm;
    else input2=_pipe_IDEX.rs2;
    int _func3=_pipe_IDEX.func3;
    int _func7=_pipe_IDEX.func7;
    bool ALUSignal=false;

    _pipe_IDEX.active=false;
    inst_nums[2]=0;
    if(_pipe_IDEX.cw.ALUOp==(int)inst_type::r){
        if((_func3==func3_map["add"]) && (_func7==func7_map["add"])) ALUResult=input1+input2;
        else if((_func3==func3_map["sub"]) && (_func7==func7_map["sub"])) ALUResult=input1-input2;
        else if((_func3==func3_map["xor"]) && (_func7==func7_map["xor"])) ALUResult=input1^input2;
        else if((_func3==func3_map["or"]) && (_func7==func7_map["or"])) ALUResult=input1|input2;
        else if((_func3==func3_map["and"]) && (_func7==func7_map["and"])) ALUResult=input1&input2;
        else if((_func3==func3_map["sll"]) && (_func7==func7_map["sll"])) ALUResult=input1<<input2;
        else if((_func3==func3_map["srl"]) && (_func7==func7_map["srl"])) ALUResult=input1>>input2;
        _pipe_EXMO.active=true;
    }
    else if(_pipe_IDEX.cw.ALUOp==(int)inst_type::i1){
        if(_func3==func3_map["addi"]) ALUResult=input1+input2;
        else if(_func3==func3_map["xori"]) ALUResult=input1^input2;
        else if(_func3==func3_map["ori"]) ALUResult=input1|input2;
        else if(_func3==func3_map["andi"]) ALUResult=input1&input2;
        else if(_func3==func3_map["slli"]) ALUResult=input1<<input2;
        else if(_func3==func3_map["srli"]) ALUResult=input1>>input2;
        _pipe_EXMO.active=true;
    }
    else if((_pipe_IDEX.cw.ALUOp==(int)inst_type::i2) || (_pipe_IDEX.cw.ALUOp==(int)inst_type::s)){
        ALUResult=((input1+input2)>>2);
        _pipe_EXMO.active=true;
    }
    else if(_pipe_IDEX.cw.ALUOp==(int)inst_type::b){
        if(_func3==func3_map["beq"]) ALUSignal=(input1==input2);
        else if(_func3==func3_map["bne"]) ALUSignal=(input1!=input2);
        else if(_func3==func3_map["blt"]) ALUSignal=(input1<input2);
        else if(_func3==func3_map["bge"]) ALUSignal=(input1>=input2);
    }
    else if((_pipe_IDEX.cw.ALUOp==(int)inst_type::j) || (_pipe_IDEX.cw.ALUOp==(int)inst_type::i3)){
        ALUResult=((_pipe_IDEX.dpc)<<2)+4;
        _pipe_EXMO.active=true;
    }

    if(_pipe_IDEX.cw.Jump){
        _pipe_IF.active=true;
        if(_pipe_IDEX.cw.ALUOp==(int)inst_type::i3) _pipe_IF.pc=((_pipe_IDEX.rs1+_pipe_IDEX.imm)>>2);
        else _pipe_IF.pc=_pipe_IDEX.dpc+(((_pipe_IDEX.imm)<<1)>>2);
        // else _pipe_IF.pc=(((_pipe_IDEX.imm)<<1)>>2)-1;
        inst_nums[0]=_pipe_IF.pc+1;
    }
    else if(_pipe_IDEX.cw.Branch && ALUSignal){
        _pipe_IF.active=true;
        _pipe_IF.pc=_pipe_IDEX.dpc+(((_pipe_IDEX.imm)<<1)>>2);
        // _pipe_IF.pc=(((_pipe_IDEX.imm)<<1)>>2)-1;
        inst_nums[0]=_pipe_IF.pc+1;
        return true;
    }
    else if(_pipe_IDEX.cw.Branch){
        _pipe_IF.active=true;
        _pipe_IF.pc=_pipe_IDEX.dpc+1;
        inst_nums[0]=_pipe_IF.pc+1;
        return true;
    }

    _pipe_EXMO.cw=_pipe_IDEX.cw;
    _pipe_EXMO.ALUOut=ALUResult;
    _pipe_EXMO.rs2=_pipe_IDEX.rs2;
    _pipe_EXMO.rd=_pipe_IDEX.rd;
    _pipe_EXMO.dpc=_pipe_IDEX.dpc;
    inst_nums[3]=_pipe_EXMO.dpc+1;
    return true;
}

bool memory_operation(pipe_EXMO& _pipe_EXMO, pipe_MOWB& _pipe_MOWB){
    if(!_pipe_EXMO.active) return false;

    _pipe_EXMO.active=false;
    inst_nums[3]=0;
    if(_pipe_EXMO.cw.MemWrite){
        data_mem[_pipe_EXMO.ALUOut]=_pipe_EXMO.rs2;
        return true;
    }
    if(_pipe_EXMO.cw.MemRead){
        _pipe_MOWB.LDOut=data_mem[_pipe_EXMO.ALUOut];
    }
    _pipe_MOWB.cw=_pipe_EXMO.cw;
    _pipe_MOWB.ALUOut=_pipe_EXMO.ALUOut;
    _pipe_MOWB.rd=_pipe_EXMO.rd;
    _pipe_MOWB.dpc=_pipe_EXMO.dpc;
    _pipe_MOWB.active=true;
    inst_nums[4]=_pipe_MOWB.dpc+1;
    return true;
}

bool writeback(pipe_MOWB& _pipe_MOWB){
    if(!_pipe_MOWB.active) return false;

    _pipe_MOWB.active=false;
    inst_nums[4]=0;
    if(!_pipe_MOWB.cw.RegWrite) return false;
    int write_result;
    if(_pipe_MOWB.cw.MemToReg) write_result=_pipe_MOWB.LDOut;
    else write_result=_pipe_MOWB.ALUOut;
    if(_pipe_MOWB.rd!=0) reg_mem[_pipe_MOWB.rd]=write_result;
    reg_stall[_pipe_MOWB.rd]--;
    return true;
}

void initialise_mem(){
    for(int i=0;i<REG_MEM_SIZE;i++){
        reg_mem[i]=0u;
        reg_stall[i]=0u;
    }
    for(int i=0;i<DATA_MEM_SIZE;i++){
        data_mem[i]=0u;
    }
}

vector<int> process_binary_file(){
    vector<int> inst_mem;
    // const char* instructions[]={
    //      "00000000000000000000000000000000"
    // };

    // int n=sizeof(instructions)/sizeof(char*);
    int n=inst_vector.size();
    for(int i=0;i<n;i++){
        char temp_str[33];
        for(int j=0;j<33;j++) temp_str[j]=inst_vector[i][j];
        inst_mem.push_back(binary_to_decimal(temp_str));
        // inst_mem.push_back(binary_to_decimal(instructions[i]));
    }
    return inst_mem;
}

void initialise_maps(unordered_map<string,int>& func3_map, unordered_map<string,int>& func7_map){
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
}

void print_data(){
    cout<<"Register Memory -\n\n";
    for(int i=0;i<REG_MEM_SIZE;i++){
        if(i<10) cout<<" x"<<i<<" : "<<reg_mem[i]<<"\n";
        else cout<<"x"<<i<<" : "<<reg_mem[i]<<"\n";
    }
    cout<<"\n";
    cout<<"Data Memory -\n\n";
    for(int i=0;i<50;i++){
        int di=i*4;
        cout<<"0x";
        cout.setf(ios::hex,ios::basefield);
        cout.width(8);
        cout.fill('0');
        cout<<di;
        cout.unsetf(ios::hex);
        cout<<" : "<<data_mem[i]<<"\n";
    }
    cout<<"\n";
}

void print_num(int n){
    char str[25];
    if(n!=0) sprintf(str,"%d",n);
    else str[0]='\0';
    int strn=strlen(str);
    int space=(5-strn)/2;
    for(int i=0;i<space;i++) cout<<" ";
    if(n!=0) cout<<n;
    for(int i=0;i<5-strn-space;i++) cout<<" ";
}

void print_margin(int n){
    for(int i=0;i<n;i++) cout<<" ";
}

void print_pipes(vector<int> num){
    system("cls");
    print_margin(20);
    for(int i=0;i<5;i++){
        cout<<TL<<HOR<<HOR<<HOR<<HOR<<HOR<<TR;
        if(i!=4) print_margin(MARGIN);
        if(i==4) cout<<"\n";
    }
    print_margin(20);
    for(int i=0;i<5;i++){
        cout<<VER<<"     "<<VER;
        if(i!=4) print_margin(MARGIN);
        if(i==4) cout<<"\n";
    }
    print_margin(20);
    for(int i=0;i<5;i++){
        cout<<VER<<"  #  "<<VER;
        if(i!=4) print_margin(MARGIN);
        if(i==4) cout<<"\n";
    }
    print_margin(20);
    for(int i=0;i<5;i++){
        if(i!=0){cout<<VER; print_num(num[i]); cout<<VER;}
        else cout<<VER<<" PC  "<<VER;
        if(i==0)      cout<<"  Fetch   ";
        else if(i==1) cout<<"  Decode  ";
        else if(i==2) cout<<" Execute  ";
        else if(i==3) cout<<"  MemOp   ";
        else if(i==4) cout<<" Writeback";
        if(i==4) cout<<"\n";
    }
    print_margin(20);
    for(int i=0;i<5;i++){
        cout<<VER<<"     "<<VER;
        if(i!=4) print_margin(MARGIN);
        if(i==4) cout<<"\n";
    }
    print_margin(20);
    for(int i=0;i<5;i++){
        cout<<BL<<HOR<<HOR<<HOR<<HOR<<HOR<<BR;
        if(i!=4) print_margin(MARGIN);
        if(i==4) cout<<"\n";
    }
    print_margin(20);
    for(int i=0;i<5;i++){
        if(i==0)      cout<<"  PC   ";
        else if(i==1) cout<<" IFID  ";
        else if(i==2) cout<<" IDEX  ";
        else if(i==3) cout<<" EXMO  ";
        else if(i==4) cout<<" MOWB  ";
        if(i!=4) print_margin(MARGIN);
        if(i==4) cout<<"\n";
    }
    cout<<"\n";
}

void run_pipeline(){
    initialise_mem();
    vector<int> inst_mem;
    inst_mem=process_binary_file();

    unordered_map<string,int> func3_map,func7_map;
    initialise_maps(func3_map,func7_map);
    
    pipe_IF _pipe_IF;
    pipe_IFID _pipe_IFID;
    pipe_IDEX _pipe_IDEX;
    pipe_EXMO _pipe_EXMO;
    pipe_MOWB _pipe_MOWB;

    int cycle=0;
    int inst_count=0;

    vector<bool> check(5,false);
    inst_nums=vector<int>(5,0);
    print_pipes(inst_nums);
    cout<<"Clock Cycles : "<<cycle<<"\n";
    cout<<"Instructions : "<<inst_count<<"\n\n";
    // Sleep(TIME_DELAY);

    while(true){
        // system("pause");

        check=vector<bool>(5,false);
        check[4]=writeback(_pipe_MOWB);
        if(!_pipe_MOWB.active) check[3]=memory_operation(_pipe_EXMO,_pipe_MOWB);
        if((!_pipe_IF.active) || (!_pipe_EXMO.active)) check[2]=execute(_pipe_IDEX,_pipe_EXMO,_pipe_IF,func3_map,func7_map);
        if(!_pipe_IDEX.active) check[1]=decode(_pipe_IFID,_pipe_IDEX);
        if(!_pipe_IFID.active) check[0]=fetch(_pipe_IF,_pipe_IFID,inst_mem);

        cycle++;
        if(check[0]) inst_count++;

        if((!_pipe_IF.active) && (!_pipe_IFID.active) && (!_pipe_IDEX.active) && (!_pipe_EXMO.active) && (!_pipe_MOWB.active)) break;

        print_pipes(inst_nums);
        cout<<"Clock Cycles : "<<cycle<<"\n";
        cout<<"Instructions : "<<inst_count<<"\n\n";
        // Sleep(TIME_DELAY);
    }

    print_pipes(inst_nums);
    cout<<"Clock Cycles : "<<cycle<<"\n";
    cout<<"Instructions : "<<inst_count<<"\n";
    cout<<"IPC          : "<<((float)inst_count)/((float)cycle)<<"\n\n";
    // Sleep(TIME_DELAY);

    print_data();
}

int main(){
    convert_to_binary();
    run_pipeline();
    return 0;
}

// while writing jalr remember that syntax is different in different platform
// remove stall for x0
// add u type instruction
// change rd to rdl
// rd = reg_mem[rd]  , rdl = rd register number
// consider byte addressable also (lb, sb)
// since I recently changed jump/branch offset, please run some codes
// for slli, srli, srai check imm also , this comment is on line 330
// mention in comment the syntax for jalr