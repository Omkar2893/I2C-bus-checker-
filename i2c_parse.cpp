#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <math.h>

using namespace std;


void i2c_parse(ifstream & ,ofstream &);
void state_mc(int line_no,int sclk,int data_in);
void addr_transac_details(int A_D_array[], string W_R_stat);
void data_transac_details(int A_D_array[], string W_R_stat);
void print_Addr();
void print_Data();


int ack,nack,W,R,trans_cnt;
int sda,sclk,lim,stop_set;
int Addr_ele[10];
int data_ele[10];
int data_op[5];
int l = 0;
int h = 0;
string final_op_addr;
string o_path;
ofstream outfile;
ifstream infile;

enum  mc_states {s0,s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13};

int main()
{
    string filename, out_file;
    string path = "C:\\omi\\406\\Exam 1 TH\\input files\\"; // name of path where files are stored
    
    cout << "Enter file name :";
    cin >> filename;
    path = path + filename;

    infile.open(path.c_str());

    out_file = filename +"_output.txt";
    o_path ="C:\\omi\\406\\Exam 1 TH\\output files\\"; // same path with output folder
    o_path = o_path + out_file;
    outfile.open(o_path.c_str());
    
    i2c_parse(infile,outfile);

    return 0;
}

//-------------------------------------------------------------------------
void i2c_parse(ifstream &infile, ofstream &outfile)
{
    int FinalACK,FinalNack,No_Trans,Mas_W,Mas_R;
    int data_set, scl, sda;
    string op;
	
//File verification
    if(infile.fail()) {										
        cout << "Error opening file. " << endl;
        exit(1);
        infile.close();
    } else if(infile.good()) {
        cout << "OK. " << endl;
    }
    
    outfile<<"List"<<endl;
    outfile<<"Type  "<<"Address  "<<"Data  "<<endl;

    string dummy ;
    getline(infile,dummy);
    
    while(infile >> data_set >> scl >> sda) {
            
    state_mc(data_set,scl,sda);
        
    }
    infile.close();
    
    FinalACK = ack;
    FinalNack = nack;
    Mas_W = W;
    Mas_R = R;
    No_Trans = trans_cnt;
    
    outfile<<"Total number of transactions : "<<dec<<No_Trans<<endl;
    outfile<<"Total number of Master writes : "<<dec<<Mas_W<<endl;
    outfile<<"Total number of Master reads : "<<dec<<Mas_R<<endl;
    outfile<<"Total number of ACK transactions : "<<dec<<FinalACK<<endl;
    outfile<<"Total number of NACK transactions : "<<dec<<FinalNack<<endl;
    outfile.close();
}

//------------------------------------------------------------------------
void state_mc(int line_no, int sclk,int data_in)
{
    
    static mc_states ps;
    static mc_states ns;
    static int temp[7];
    static int Data[7];
    static string op;
    static int a_cnt = 8;
    static int d_cnt = 8;
    
    ps=ns;
    switch(ps)
    {
        
     case s0: if((sclk == 1) && (data_in == 0))
                    ns = s1;
                else
                    ns = s0;
                    break;
    
    case s1: if((sclk == 1))
                ns = s1;
                else 
            
                    ns = s2;
                break;
   
    case s2: if((sclk == 0))
                ns = s2;
                else
                    
                    ns = s3;
                    break;
    
										//storing 8 bits in a temporary array
    case s3: if (sclk == 1)
                ns = s3;
                else
                {
                    ns = s4;
                    temp[a_cnt] = data_in;
                    a_cnt = a_cnt - 1;

                }
                break;

										//address in & read/write bit
    case s4: if (a_cnt != 0)
                ns = s2;
                else
                {   
                    ns = s5;
                    cout<<"Address :";
                    for(int k=8; k>1; k--)
                    {
                        cout<<temp[k];
                    }
                    cout<<endl;
                    
                    if (temp[1] == 0)
                       op = "W";
                    else
                       op = "R";
                    cout<< "Operation :"<<op<<endl;
                }
                break;
                
    case s5: if (sclk == 1)
                ps = s6 ;
                else 
                ps = s5;
	
										// Address ACK and NACK verification	
    case s6: if (sclk == 1)
                ns = s6;
                else 
                {
                    if(data_in == 0)
                    {
                       ns = s7; 
                        ack = ack + 1;
                        cout<<"ACK ="<<ack<<endl;
                    }
                        else
                        {
                             ns = s0;
                             a_cnt = 8;
                            nack = nack + 1;
                            cout<<"NACK ="<<nack<<endl;
        
                            cout<<"Stop"<<endl;
                            cout<<endl;
                            trans_cnt+=1;
                        }
                    
                } 
                break;
    
   case s7: if (sclk == 0)
                ns = s7;
                else
                ns = s8;
                break;

    case s8: if (sclk == 1)
                ns = s8;
                else
                {  
                    ns = s9;
                    Data[d_cnt]= data_in;
                    d_cnt = d_cnt-1;
                }
                break;
 
												//Data in 
    case s9: if (d_cnt != 0)
                ns = s7;
            else
            {
                ns = s10;
                cout<<"Data :";
                for(int p=8; p>0; p--)
                {
                    cout<<Data[p];
                }
                cout<<endl;
            }
            break;
            
												//ACK/NACK bit of received data
    case s10:  if (sclk == 0)
                {   
                    
                    ns = s10;
                }
                else 
                {

                    if(data_in == 0)
                    {
                        ns = s11;
                        ack = ack + 1;
                        cout<<"ACK ="<<ack<<endl;
                       
                        if(op == "W")
                                W++;
                            if(op == "R")
                                R++;
                       
                       if((h >= 1 && l >= 1))
                       { 
                        print_Addr();
                       }
                       else
                           addr_transac_details(temp,op);
                           
                       data_transac_details(Data,op);
                    }
                        else
                        {   ns = s0;
                            a_cnt = 8;
                            d_cnt = 8;
                            nack = nack + 1;
                            cout<<"NACK ="<<nack<<endl;
                            cout<<"Stop"<<endl;
                            cout<<endl;
                            trans_cnt+=1;
                            cout<<endl;
                        }
                } 
                break;
    
    case s11: if (sclk == 0)
                ns = s11;
                else
                ns = s12;
                break;
    
													//Stop verification
    case s12: if ((sclk == 1) && (data_in == 1))
                {
                cout<<"Stop"<<endl;
                l = 0;
                h = 0;
                cout<<endl;
                trans_cnt+=1;
                ns = s13;
                }
                else if((sclk == 0) && (data_in == 0))
                    {
                        d_cnt = 7;
                        ns = s8;
                        Data[8] = data_in;
                    }
                    
                break;
        
    case s13: if ((sclk == 1) && (data_in == 1))
                {  
                    a_cnt = 8;
                    d_cnt = 8;
                    ns = s0;
                }
                break;
    }
}

void addr_transac_details(int A_D_array[], string W_R_stat)
{ 
    int a_val= 0;
    int sq = 0;
    
     for(int p=2; p<9; p++)
    {
        int v = pow(2,sq);
        a_val = a_val + A_D_array[p]*(v);
        sq++;
    }
    if (W_R_stat == "W")
        data_op[h] = 0;
    else if (W_R_stat == "R")
        data_op[h] = 1;
    Addr_ele[h]=a_val;
    cout<<"Addr : "<<a_val<<endl;
    print_Addr(); 
    h++;
}

void data_transac_details(int A_D_array[], string W_R_stat)
{ 
    int d_val=0;
    int sq = 0;
    
     for(int p=1; p<9; p++)
    {
        int v = pow(2,sq);
        d_val = d_val + A_D_array[p]*(v);
        sq++;
    }
    data_ele[l]=d_val;
    cout<<"Data : "<<d_val<<endl;
    print_Data();
    l++;
}

void print_Addr()
{

    if (h >= 1 )
    {
        if (data_op[h-1] == 0)
        {
            final_op_addr = "MWR";
        }
            else if(data_op[h-1] == 1)
                final_op_addr = "MR";
    
        outfile<<final_op_addr<<"    "<<hex<<Addr_ele[h-1]<<"       ";
    }
    else{
        if (data_op[h] == 0)
        {
            final_op_addr = "MWR";
        }
        else if(data_op[h] == 1)
            final_op_addr = "MR";
        
        outfile<<final_op_addr<<"    "<<hex<<Addr_ele[h]<<"       ";
    }
}

void print_Data()
{
    outfile<<hex<<data_ele[l]<<endl;
}