//
//  main.cpp
//  MipsCPU
//
//  Created by Jeanne Mystery on 2018/4/11.
//  Copyright © 2018年 Jeanne Mystery. All rights reserved.
//

#include <iostream>
#include <string.h>
#include <cstring>
#include <iostream>
#include <sstream>
using namespace std;
string opt[] = {"R-","D-","U-","A-","T-"}; // option
string REG[] = { "$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra" };

class MipsCPU{
public:
    int *Rgf;
    //string *name;
    //int *position;
    int PC;
    short *Memory;
    string *opts;
    string Now_opt;
    int Flag_New = 0;
    
    MipsCPU( int msize ){
        Memory = new short[msize];
        Rgf = new int[32];
        Rgf[0] = 0;
        PC = 0;
        opts = new string[msize];
    }
    
    MipsCPU(){};
    ~MipsCPU(){};
    
    //读入汇编
    void Read(){
        Run_Recode();
        for( int i = 0 ; Memory[i] != 0 ; i += 2 ){
            int IR;
            IR = (Memory[i]<<16)|(Memory[i+1]&0xFFFF); PC += 2;
            cout << i << ":     ";
            printf("0x%08x\n",IR);
        }
    }
    //反汇编
    void Load(){
        Decode_Run();
        for( int i = 0 ; !opts[i].empty() ; i +=2 ){
            cout << i << ":     " << opts[i] << endl;
        }
    }
    void Show_Reg(){
        cout << REG[0] << "   " << Rgf[0] << endl;
        for( int i = 1 ; i < 32 ; i++ ){
            cout << REG[i] << "     " << Rgf[i] << endl;
        }
    }
    void Assign(){
        string opt;
        stringstream ss;
        int i ;
        cout << "       >" ;
        for(i = 0; !opts[i].empty() ; i += 2);
        getline(cin,opts[i]);
        getline(cin,opts[i]);
    }
    
    void CPU_run(){
        cout << "Mini MipsCPU for test." << endl;
        for(;;){
            cout << "MipsCPU>" ;
            cin >> Now_opt;
            int i;
            for( i =0 ; i < 5 ; i ++ )
                if( !Now_opt.compare(opt[i]) )break;
            switch(i){
                case 0://看寄存器 R-
                    Show_Reg();
                    break;
                case 1://数据方式看内存 D-
                    Load();
                    break;
                case 2://指令方式看内存 U-
                    Read();
                    break;
                case 3://写汇编指令到内存 A-
                    Assign();
                    break;
                case 4://单步执行内存指令 T-
                    Run();
                    break;
                default:
                    cout << "error!" << endl;
            }
        }
    }
    
    void Run(){
        int IR,op,rs,rt,rd,dat,adr,fun;//临时存放的寄存器
        IR = (Memory[PC]<<16)|(Memory[PC+1]&0xFFFF); PC += 2; //16-bit ,取指令
        op = (IR>>26)&63;
        rs = (IR>>21)&31;
        rt = (IR>>16)&31;
        rd = (IR>>11)&31;
        adr = IR&0x3FFFFFF;
        fun = IR&63;  //功能区 低6位
        dat = (int)(short)IR&0xFFFF;  // 有符号 低16位
        switch(op){ //执行
            case 0:
                switch(fun){
                    case 32:    //add
                        Rgf[rd] = Rgf[rs] + Rgf[rt];
                        break;
                    case 34:    //sub
                        Rgf[rd] = Rgf[rs] - Rgf[rt];
                        break;
                    default:
                        break;
                }
            case 35://lw
                Rgf[rt] = (Memory[Rgf[rs]+dat]<<16)|((Memory[Rgf[rs]+dat+1])&0xFFFF);
                break;
                
            case 43://sw
                Memory[Rgf[rs]+dat] = Rgf[rt]>>16;
                Memory[Rgf[rs]+dat+1] = Rgf[rt]&0xFFFF;
                break;
                
            case 4://beq
                if(Rgf[rs]==Rgf[rt]){
                    PC += dat*2; //16-bit
                }
                break;
            case 2://j
                PC = (PC&0xF8000000)|(adr<<1);
                break;
            default:
                cout << "Error!" << endl;
        }
    }
    
    void Decode_Run(){
        int i ;
        for( i = 0 ; Memory[i] != 0 ; i += 2 ){
            int IR,op,dat,fun;//临时存放的寄存器
            IR = (Memory[i]<<16)|(Memory[PC+1]&0xFFFF); //16-bit ,取指令
            op = (IR>>26)&63;
            fun = IR&63;  //功能区 低6位
            dat = (int)(short)IR&0xFFFF;  // 有符号 低16位
            switch(op){
                case 0:
                    switch(fun){
                        case 32://add
                            Decode_ADD(i);
                            break;
                        default:
                            cout << op << i << "Error1:No such Fun!" << endl;
                    }
                    break;
                case 2://j
                    Decode_J(i);
                    break;
                case 4://beq
                    Decode_BEQ(i);
                    break;
                case 35://lw
                    Decode_LW(i);
                    break;
                case 43://sw
                    Decode_SW(i);
                    break;
                default:
                    cout << op << i <<  "Error2:No such Opt!" << endl;
            }
        }
    }
    
    void Decode_ADD( int i ){
        int IR,rs,rt,rd;
        stringstream ss;
        IR = (Memory[i]<<16)|(Memory[i+1]&0xFFFF);
        rs = (IR>>21)&31;
        rt = (IR>>16)&31;
        rd = (IR>>11)&31;
        ss << "add  " << REG[rs] << "," << REG[rt] << "," << REG[rd];
        opts[i] = ss.str();
    }
    void Decode_LW( int i ){
        int IR,rs,rd,imm;
        stringstream ss;
        IR = (Memory[i]<<16)|(Memory[i+1]&0xFFFF);
        rs = (IR>>21)&31;
        rd = (IR>>16)&31;
        imm = (int)(short)IR&0xFFFF;
        ss << "lw   " << REG[rd] << "," << imm << "(" << REG[rs] << ")";
        opts[i] = ss.str();
    }
    void Decode_SW( int i ){
        int IR,imm;
        short rt,rs;
        stringstream ss;
        IR = (Memory[i]<<16)|(Memory[i+1]&0xFFFF);
        rt = (IR>>21)&31;
        rs = (IR>>16)&31;
        imm = (int)(short)IR&0xFFFF;
        ss << "sw   " << REG[rs] << "," << imm << "(" << REG[rt] << ")";
        opts[i] = ss.str();
    }
    void Decode_BEQ( int i ){
        int IR,rs,rt;
        short adr;
        stringstream ss;
        IR = (Memory[i]<<16)|(Memory[i+1]&0xFFFF);
        //cout << rs <<" " << rt << endl;
        rs = (IR>>21)&31;
        rt = (IR>>16)&31;
        //cout << rs <<" " << rt << endl;
        adr = (unsigned int)(short)IR&0xFFFF;
        ss << "beq  " << REG[rs] << "," << REG[rt] << "," << adr;
        opts[i] = ss.str();
    }
    void Decode_J( int i ){
        int IR;
        short adr;
        stringstream ss;
        IR = (Memory[i]<<16)|(Memory[i+1]&0xFFFF);
        adr = (IR&0x3FFFFFF);
        ss << "j    " << dec << adr;
        opts[i] = ss.str();
    }
    ////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////RECODE _ PART
    void Run_Recode(){
        int i = 0;
        // int n ;
        /* for( ; !opts[i].empty() ; i += 2 ){
           int idx_name;
            idx_name = (int)opts[i].find(":");
            if( idx_name != -1 ){
                for( n = 0 ; !name[n].empty() ; n ++ );
                name[n] = opts[i].substr(0,idx_name);
                position[n] = i;
                opts[i] = opts[i].substr(idx_name+1);
            }
        }*/
        for( i = 0; !opts[i].empty() ; i += 2 ){
            if( opts[i].find("add") != -1)
                ADD_Recode(i);
            else if ( opts[i].find("lw") != -1)
                LW_Recode(i);
            else if ( opts[i].find("sw") != -1)
                SW_Recode(i);
            else if ( opts[i].find("beq") != -1)
                BEQ_Recode(i);
            else if ( opts[i].find("j") != -1)
                J_Recode(i);
            else
                cout << "Error3:No such option!" << endl;
        }
    }
    
    void ADD_Recode( int i ){
        long idx1 = opts[i].find_first_of('$',0);
        long idx2 = opts[i].find_first_of(',',0);
        long length = idx2 - idx1;
        long rs,rt,rd;
        long IR;
        rd = Get_Reg(i, (int)idx1, (int)length);
        //cout << "rd: " << rd << "\tidx1: " << idx1 << "\tlength: " << length << endl;
        idx1 = opts[i].find_first_of('$',idx1+1);
        idx2 = opts[i].find_first_of(',',idx2+1);
        length = idx2 - idx1;
        rs = Get_Reg(i, (int)idx1, (int)length);
        //cout << "rs: " << rs << "\tidx1: " << idx1 << "\tlength: " << length << endl;
        idx1 = opts[i].find_first_of('$',idx1+1);
        idx2 = opts[i].find_first_of(',',idx2+1);
        length = idx2 - idx1;
        rt = Get_Reg(i, (int)idx1, (int)length);
        //cout << "rt: " << rt << "\tidx1: " << idx1 << "\tlength: " << length << endl;
        IR = 0x00000020|(rd<<21)|(rs<<16)|(rt<<11);
        Memory[i] = (IR>>16)&0xFFFF;
        Memory[i+1] = IR&0xFFFF;
    }
    
    void LW_Recode( int i ){
        long idx1 = opts[i].find_first_of('$',0);
        long idx2 = opts[i].find_first_of(',',0);
        long length = idx2 - idx1;
        long rd,rs,dat,IR;
        rd = Get_Reg(i, (int)idx1, (int)length);
        idx1 = opts[i].find_first_of('(',0);
        idx2 = opts[i].find_first_of(')',0);
        length = idx2 - idx1 - 1;
        rs = Get_Reg(i, (int)(idx1)+1, (int)length);
        idx1 = opts[i].find_first_of(',',0);
        idx2 = opts[i].find_first_of('(',0);
        length = idx2 - idx1 - 1;
        dat = Get_imm(i, (int)idx1+1, (int)length);
        IR = 0x8C000000|(rs<<21)|(rd<<16)|(((short)dat)<<16>>16);
        Memory[i] = (IR>>16)&0xFFFF;
        Memory[i+1] = IR&0xFFFF;
    }
    
    void SW_Recode( int i ){
        long idx1 = opts[i].find_first_of('$',0);
        long idx2 = opts[i].find_first_of(',',0);
        long length = idx2 - idx1;
        long rd,rs, dat,IR;
        rs = Get_Reg(i, (int)idx1, (int)length);
        //cout << rs <<endl;
        idx1 = opts[i].find_first_of('(',0);
        idx2 = opts[i].find_first_of(')',0);
        length = idx2 - idx1 - 1;
        rd = Get_Reg(i, (int)(idx1)+1, (int)length);
        idx1 = opts[i].find_first_of(',',0);
        idx2 = opts[i].find_first_of('(',0);
        length = idx2 - idx1 - 1;
        dat = Get_imm(i, (int)idx1+1, (int)length);
        IR = 0xAC000000|(rd<<21)|(rs<<16)|(((unsigned long)dat)<<16>>16);
        Memory[i] = (IR>>16)&0xFFFF;
        Memory[i+1] = IR&0xFFFF;
    }
    
    void BEQ_Recode( int i ){
        long idx1 = opts[i].find_first_of("$",0);
        long idx2 = opts[i].find_first_of(",",0);
        long length = idx2 - idx1;
        long r1,r2,IR;
        long ADDr;
        r1 = Get_Reg(i, (int)idx1, (int)length);
        //cout << REG[r1] << endl;
        idx1 = opts[i].find_first_of('$',idx1+1);
        idx2 = opts[i].find_first_of(',',idx2+1);
        length = idx2 - idx1;
        r2 = Get_Reg(i, (int)idx1, (int)length);
        //cout << REG[r2] << endl;
        idx1 = idx2;
        idx2 = opts[i].find_first_of(',',idx2 + 1);
        length = idx2 - idx1;
        ADDr = Get_pos(i, (int)idx1+1,-1);
        cout << ADDr << endl;
        IR = 0x10000000|(r1<<21)|(r2<<16);
        IR = IR|(((unsigned long)ADDr)<<16>>16);
        Memory[i] = (IR>>16)&0xFFFF;
        Memory[i+1] = IR&0xFFFF;
    }
    
    void J_Recode( int i ){
        int idx1;
        const char *test = opts[i].data();
        long IR;
        long ADDr;
        for( idx1 = 1 ; test[idx1] == ' ' ; idx1++);
        ADDr = Get_pos(i, idx1, -1);
        //cout << ADDr << endl;
        IR = 0x09000000|(((unsigned long)ADDr)<<6>>6);
        Memory[i] = (IR>>16)&0xFFFF;
        Memory[i+1] = IR&0xFFFF;
    }
    
    long Get_Reg( int i , int idx_start , int length ){
        string reg;
        int m;
        if (length > 0)
            reg = opts[i].substr(idx_start,length);
        else
            reg = opts[i].substr(idx_start);
        for( m = 0; m < 32 ; m ++ ){
            if( !reg.compare(REG[m]) )
                break;
        }
        return m;
    }
    
    long Get_pos( int i, int idx_start , int length ){
        string Nname;
        if( length < 0 ) Nname = opts[i].substr(idx_start);
        else Nname = opts[i].substr(idx_start,length);
        if( AllisNum(Nname) )
            return Get_imm(i, idx_start,length);
       /* else
            for( int m = 0 ; !name[m].empty() ; m ++ ){
                if ( name[m].compare(Nname) != -1 )
                    return (long)position[m];
            }*/
        return -1;
    }
    
    long Get_imm( int i , int idx_start , int length ){
        string imm = opts[i].substr(idx_start,length);
        stringstream ss;
        long imm_r;
        ss << imm;
        ss >> imm_r;
        return imm_r;
    }
    
    bool AllisNum(string str){
        for( int i = 0; i < str.size(); i ++ ){
            int tmp = (int)str[i];
            if(tmp >= 48 && tmp <= 57)
                continue;
            else
                return false;
        }
        return true;
    }
    
};

int main(int argc, const char * argv[]) {
        MipsCPU w = MipsCPU(8192);
        w.CPU_run();
        //std::cout << "Hello, World!\n";
    return 0;
}
