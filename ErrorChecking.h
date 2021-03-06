/*
    Author: Dinh Hong Quan 20192031
*/

#ifndef _ERROR_CHECKING_
#define _ERROR_CHECKING_

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <math.h>

#include "GenerateData.h"

using namespace std;

//check file open, nếu mở được return true, ko thì return false
bool checkOpen(char a[])  //input là arg[1]
{
    ifstream fin(a);                             
    if(!fin.is_open())  
	{
		cout<<"Error: " << a <<" could not be opened.";
        return false;
	}
    else
    {
        fin.close();
        return true;
    }
}

//hàm check xem một string có file hex không nếu là file hex thì trả về true, ko phải thì trả về false

bool isHex(char* text, int size, char a[])    //input là một string
{
    for(int i = 0; i < size ; i = i + 1)
    {
        char c = text[i];
        if (c == 0)
        {
            break;
        }
        
        if (c !=':' && c !='\n' && (c<48 || (57<c && c<65) || (70<c && c<97) || 102<c)) 
        {
            cout << "Error: " << a << " is not Hex File." << endl;
            return false;
        }
    }
    return true;
}

//check các lỗi như checksum, lỗi count_byte, lỗi option_byte
//đầu vào là string tất cả data, trả về true nếu không lỗi, trả về false + in ra loại lỗi ở dòng nào đó (lỗi ở dòng đầu tiên) 
bool checkFormat(char* data, int size)
{
    int line = 1;                                               //lưu dòng đang xét là dòng bao nhiêu
    while(1)
    {


        string data_temp = separateLine(data, size);            //xét từng dòng một của file data


        if (data_temp == "")                                    //nếu không còn dòng nào nữa để xét
        {
            break;
        }


        int result=0;                                           //lưu giá trị decimal khi cộng các các byte trong quá trình checksum
        string result_1="";                                     //lưu giá trị hex hoặc binary khi cộng các byte trong quá trình checksum
        string error_check = "";                                //biến lưu tạm 1 byte nào đó 

                                       
        error_check += data_temp[6];
        error_check += data_temp[7];

        //check option_byte của dòng đang xét xem có thỏa mãn không

        if(error_check!="00"&&error_check!="01"&&error_check!="02"&&error_check!="04")
        {
            cout<<"Error: Record type "<<error_check<<" is not valid in line "<<line;
            return false;
        }


        int number_of_bytes_error = data_temp.size()-10;
            if(number_of_bytes_error%2==1)
            {
                cout<<"Error: Record-length is not matched with the number data bytes in line "<<line;
                return false;
            }
            else
            {
                error_check = "";
                error_check += data_temp[0];
                error_check += data_temp[1];
                if(number_of_bytes_error/2!=hex_to_dec(error_check))
                {
                    cout<<"Error: Record-length is not matched with the number data bytes in line "<<line;
                    return false;
                }
            }

        for(int j=0;j<=data_temp.size()-4;j+=2)                 //cộng tất cả các byte phải checksum của một dòng
        {
            char a[3];                                      //lưu 1 byte một trong một dòng (gồm 2 chữ cái hex liên tục) ở dạng string hex
            a[0] = data_temp[j];
            a[1] = data_temp[j+1];
            result+=HexChar2Byte(a);                              //đổi string hex sang decimal rồi cộng vào result
        }                                                      
        //result =  tổng các byte cần checksum trong một dòng ở dạng decimal


        if(dec_to_bin(result).size()<8)                         //nếu kích thước của result nhỏ hơn 8 bits
        {
            for(int n=1;n<=(8-dec_to_bin(result).size());n++)   //chèm them các bit 0 đằng trước để đủ 1 8 bits
            {
                result_1+='0';                                  
            }
        }


        else if(dec_to_bin(result).size()>=8)                   //nếu result đã đủ 8 bits hoặc nhiều hơn 8 bits
        {
            result_1+='0';                                      //thêm một bit 0 đằng trước 
        }

        
        result_1 += dec_to_bin(result);                         //chuyển result sang string binary rồi lưu vào result_1

        result_1 = inverse(result_1);                           //đảo bit result_1

        result = bin_to_dec(result_1);                          //chuyenr result_1 sang decimal rồi lưu vào result
        result+=1;                                              //result cộng thêm 1
        result_1 = dec_to_hex(result);                          //chuyển result sang dạng string hex rồi lưu vào result_1
        
        char res_data[3];
        get2LastBit(res_data, data_temp);               //lấy 2 kí tự cuối của dòng đang xét (byte checksum)
        uint8_t temp_data = HexChar2Byte(res_data);
        
        char res_final[3];
        get2LastBit(res_final, result_1);
        uint8_t temp_final = HexChar2Byte(res_final);               //lấy 2 kí tự cuối của result_1  (1 byte)
        
        if(temp_data!=temp_final)                                 //so sánh 1 byte cuối của result_1 với byte checksum của dòng đang xét
        {
            cout<<"Error: Wrong checksum in line "<<line;              //in lỗi nếu có
            return false;
        }
        line+=1;
    }
    return true;
}

#endif 