# I2C-bus-checker-
I2C protocal imlpementation using C++(passing ifstream and ofstream as reference)

In this code an input file conataining SDA and SCL data is provided as an input.
ifstream library is used for parsing the values of SDA and SCL.
Parsed data is processed using I2C protocal and the output is stored in an outputfile.
Outputfile contains following details related to parsed data :
List
1.  Type  Address  Data

Eg  MWR     xxh     xxh

2.Total number of transactions 

3.Total number of Master writes

4.Total number of Master reads

5.Total number of ACK transactions

6.Total number of NACK transactions

