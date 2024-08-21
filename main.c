#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t Register_File[31];
uint32_t Data_Memory[0x8000];
uint32_t Instruction_Memory[0x8000];
uint32_t PC = 0x1000;
uint8_t PCSrc = 0;
uint8_t Branch = 0;
uint8_t ResultSrc = 0;
uint8_t MemWrite = 0;
uint8_t ALUSrc = 0;
uint8_t ImmSrc = 0;
uint8_t RegWrite = 0;
uint8_t ALUOp = 0;
uint8_t ALUControl = 0;

uint32_t Get_Bits(uint32_t Data, uint32_t h_bit_num, uint32_t l_bit_num);


void Main_Decoder(uint32_t Data) {

    switch (Get_Bits(Data,6,0))
    {
        case 3:
            RegWrite = 0b1;
            ImmSrc = 0b00;
            ALUSrc = 0b1;
            MemWrite = 0b0;
            ResultSrc = 0b1;
            Branch = 0b0;
            ALUOp = 0b00;
          break;

        case 35:
            RegWrite = 0b0;
            ImmSrc = 0b01;
            ALUSrc = 0b1;
            MemWrite = 0b1;
            ResultSrc;
            Branch = 0b0;
            ALUOp = 0b00;

          break;

        case 51:
            RegWrite = 0b1;
            ImmSrc;
            ALUSrc = 0b0;
            MemWrite = 0b0;
            ResultSrc = 0b0;
            Branch = 0b0;
            ALUOp = 0b10;

          break;

        case 99:
            RegWrite = 0b0;
            ImmSrc = 0b10;
            ALUSrc = 0b00;
            MemWrite = 0b0;
            ResultSrc;
            Branch = 0b1;
            ALUOp = 0b01;

          break;


    }
    printf("RegWrite : %d\n", RegWrite);
    printf("ImmSrc : %d\n", ImmSrc);
    printf("ALUSrc: %d\n",ALUSrc);
    printf("MemWrite : %d\n", MemWrite);
    printf("ResultSrc: %d\n", ResultSrc);
    printf("Branch : %d\n", Branch);
    printf("ALUOp : %d\n", ALUOp);
}

uint8_t ALU_Decoder(uint32_t _Instruction)
{
    uint32_t op = Get_Bits(_Instruction, 6, 0);
    uint32_t func3 = Get_Bits(_Instruction, 14, 12);
    uint32_t func7 = Get_Bits(_Instruction, 31, 25);
    uint32_t op_func_5 = (Get_Bits(op, 5, 5) << 1) | Get_Bits(func7, 5, 5);

    printf("op_func_5 = %d \n", op_func_5);
    printf("op: %d \n ", op);
    printf("func3: %d \n ", func3);
    printf("func7: %d \n ", func7);

    switch(ALUOp)
    {
        case 0b00:
            return 0;

        case 0b01:
            return 0b001;

        case 0b10:
            switch(func3)
            {
                case 0b000:
                    switch(op_func_5) {
                        case 0b00:
                        case 0b01:
                        case 0b10:
                            return 0;
                        case 0b11:
                            return 0b001;
                    }
                    break;

                case 0b010:
                    return 0b101;

                case 0b110:
                    return 0b011;

                case 0b111:
                    return 0b010;
            }
            break;

    }
    return 0;
}

uint32_t Get_Instruction(uint32_t Address)
{
    return Instruction_Memory[Address/4];
}
DBG_Print_Register()
{
    for(int i=0;i<=31; i++)
        printf("Register File[%d]: %d (0x%X) \n ",i, Register_File[i], Register_File[i]);

}
DBG_Print_DataMemory(uint32_t Address, int32_t Number)
{
    for(int32_t i=Address;i<Address + Number; i++)
        printf("Data Memory[%d]: %d (0x%X) \n ",i, Data_Memory[i], Data_Memory[i]);

}
uint32_t Get_Bits(uint32_t Data, uint32_t h_bit_num, uint32_t l_bit_num)
{
    uint32_t x = Data << (31 - h_bit_num);
    x = x >> 31 - (h_bit_num - l_bit_num);

    return x;
}

int32_t Extender (int32_t Data, int32_t ImmSrc)
{
    int32_t Imm1;
    int32_t Imm2;
    int32_t Imm3;
    int32_t Imm4;
    int32_t tmp;

    /**
    Changed switch cases to binary
    **/
    switch (ImmSrc)
    {
        case 0b00: // I Type
            Data = Get_Bits(Data, 31, 20);

          break;

        case 0b01: // S Type
            Imm1 = Get_Bits(Data, 11, 7);  // 0000000000000000xxxxx
            Imm2 = Get_Bits(Data, 31, 25); // 000000000000000xxxxxxx
            Data = (Imm2 << 5) | Imm1;

          break;

        // INSTRUCTION 4 - PART 1
        /**
        According to the book, ImmExt structure starts with the 7th bit of the instruction,
        followed by bits 30:25 and finally bits 11:8
        Imm1 = Bit 31 (12)
        Imm2 = Bit 7 (11)
        Imm3 = Bits 30:25 (10:5)
        Imm4 = Bits 11:8 (4:1)
        We shift Imm1 to the left by 6 to make space for the 6 bits of Imm2 and place them using OR operation
        Then we shift the result by 4 to make space for bits 11:8 and place it next to them.
        Since B Type has 13-bit signed immediate, we shift the final result by 1 to the left in order to add one more right most bit.
        **/
        case 0b10: // B Type
//            Imm1 = Get_Bits(Data, 31, 31); // 1
//            Imm2 = Get_Bits(Data, 7, 7); // 1
//            Imm3 = Get_Bits(Data, 30, 25); // 111111
//            Imm4 = Get_Bits(Data, 11, 8); // 1010
//            tmp = (Imm1 << 1) | Imm2; // 11
//            tmp = (tmp << 6) | Imm3; // 1111111
//            Data = ((tmp << 4) | Imm4) << 1; // 1 1 111111 1010 0
            Imm1 = Get_Bits(Data, 7, 7); // 1 // 11
            Imm2 = Get_Bits(Data, 30, 25); // 1 // 10 9 8 7 6 5
            Imm3 = Get_Bits(Data, 31, 31); // 111111 // 12
            Imm4 = Get_Bits(Data, 11, 8); // 1010 // 4 3 2 1
            Imm3 = (Imm3 << 1) | Imm1;
            Imm3 = (Imm3 << 6) | Imm2;
            Imm3 = (Imm3 << 4)| Imm4;
            Data = Imm3 << 1; // 1 1 111111 1010 0
    }
     if (Data >= 0x800){
      Data= 0xfffff000 | Data;
     }


    return Data;
}


int32_t ALU (int32_t SRCA,int32_t SRCB,uint32_t ALUControl)
{
    int32_t TMP =0;
    switch (ALUControl)
    {
        case 0: // ADD
            TMP= SRCA+SRCB;
          break;

          // INSTRUCTION 4 - PART 1.1
          /**
          Added subtraction logic for ALU
          **/
        case 1: // Subtract
            TMP= SRCA-SRCB;
          break;

        case 2: // AND

          break;

        case 3: // OR
            TMP = SRCA | SRCB;
          break;

        case 5: // SLT

          break;
        default:
            TMP =0;

    }
    return TMP;
}

int main()
{
    printf("Hello world!\n");
    Register_File[5] = 6;
    Register_File[9] = 0x2004;
    Data_Memory[0x2000] = 10;
    Instruction_Memory[0x1000 /4] = 0xFFC4A303;
    Instruction_Memory[0x1004 /4] = 0x0064A423;
    Instruction_Memory[0x1008 /4] = 0x0062E233;
    Instruction_Memory[0x100C /4] = 0xFE420AE3;
    DBG_Print_Register();
    DBG_Print_DataMemory(0x2000, 1);
    printf("######################################################### \n");
    /////////////////////////////////////
    for(int i=0; i<4; i++) {
        printf("Loop: %d\n", i);
        uint32_t Instr = Get_Instruction(PC);
        Main_Decoder(Instr);
        ALUControl = ALU_Decoder(Instr);
        printf("ALUControl result in Decimal: %d\n", ALUControl);

        uint32_t Register_File_A1 = Get_Bits(Instr, 19, 15);
        printf("(0x%X) \n ", Instr);
        printf("Register_File_A1 result in Decimal: %d\n", Register_File_A1);

        uint32_t Register_File_A2 = Get_Bits(Instr, 24, 20);
        printf("(0x%X) \n ", Instr);
        printf("Register_File_A2 result in Decimal: %d\n", Register_File_A2);

        uint32_t Register_File_RD1 = Register_File[Register_File_A1];
        printf("Register_File_RD1 result in Decimal: %d (0x%X) \n ", Register_File_RD1, Register_File_RD1);

        uint32_t Register_File_RD2 = Register_File[Register_File_A2];
        printf("Register_File_RD2 result in Decimal: %d (0x%X) \n ", Register_File_RD2, Register_File_RD2);

        // uint32_t Extend_In = Get_Bits(Instr, 31, 20);
        // printf("Extend_In result in Decimal: %d (0x%X) \n ", Extend_In, Extend_In);

        int32_t ImmEx = Extender(Instr, ImmSrc);
        printf("ImmEx result in Decimal: %d (0x%X) \n ", ImmEx, ImmEx);

        int32_t ALUResult =0;
        if(ALUSrc == 0) {
            ALUResult= ALU(Register_File_RD1,Register_File_RD2, ALUControl);
        } else {
            ALUResult= ALU (Register_File_RD1,ImmEx, ALUControl);
        }
        printf("ALUResult result in Decimal: %d (0x%X) \n ", ALUResult, ALUResult);

        // INSTRUCTION 4 - PART 2 & 3
        /**
        PCSrc is set to 1 when ALU result is Zero (due to subtraction which indicates true condition in case of 0)
        and when Branch is set to 1
        **/
        if(ALUResult == 0 && Branch == 1) {
            PCSrc = 1;
        } else {
            PCSrc = 0;
        }

        uint32_t Data_Memory_RD= Data_Memory[ALUResult];
        printf("Data_Memory_RD result in Decimal: %d (0x%X) \n ", Data_Memory_RD, Data_Memory_RD);
        uint32_t  ReadData=Data_Memory_RD;
        uint32_t RegisterFile_WD3 = 0;

        if(ResultSrc == 0) {
            RegisterFile_WD3 = ALUResult;

        } else {
            RegisterFile_WD3 = ReadData;
        }

        uint32_t Register_File_A3 = Get_Bits(Instr, 11, 7);
        printf("Register_File_A3 result in Decimal: %d (0x%X) \n ",Register_File_A3, Register_File_A3 );
        uint32_t RegWrite = 1;

        //
        // REAL PROCESSOR WAITS FOR CLOCK IN THIS PART
        //
        if(MemWrite == 1) {
            printf("PASSED THROUGH MEMWRITE");
            Data_Memory[ALUResult] = Register_File_RD2;
        }

        if (RegWrite == 1)
        {
            Register_File[Register_File_A3] = RegisterFile_WD3;
        }

        // INSTRUCTION 4 - PART 4 & 5
        /**
        if PCSrc is 1, that means its our beq instruction
        which gets the value of PC from PCTarget Adder result
        **/
        if(PCSrc == 1){
            PC = ImmEx + PC;
        } else {
            PC = PC + 4;
        }
        printf("######################################################### \n");
        printf("(0x%X) \n", PC);
    }

    return 0;
}
