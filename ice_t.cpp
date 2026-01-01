/**
 * C++ functions as part of the extension.
 * 
 * A function defined here will run on the micro:bit, there is also a "shim"
 * in the main.ts file that will be run in the simulator.
 * Info about shims: https://makecode.com/simshim
 */
#include "pxt.h"
#include <string>

int once = 0;

using namespace pxt;

namespace ice_t {
    #define V1_MULTIPLIER 2
    #define V2_MULTIPLIER 3

    #if MICROBIT_CODAL
    static void writeNum(char *buf, uint32_t n)
    {
        int i = 0;
        int sh = 28;
        while (sh >= 0)
        {
            int d = (n >> sh) & 0xf;
            buf[i++] = d > 9 ? 'A' + d - 10 : '0' + d;
            sh -= 4;
        }
        buf[i] = 0;
    }

    unsigned int numLookup(char character){
        unsigned int result = 0x0;
        switch (character)
        {
        case '0':
            /* code */
            result = 0x0;
            break;
        case '1':
            /* code */
            result = 0x1;
            break;
        case '2':
            /* code */
            result = 0x2;
            break;
        case '3':
            /* code */
            result = 0x3;
            break;
        case '4':
            /* code */
            result = 0x4;
            break;
        case '5':
            /* code */
            result = 0x5;
            break;
        case '6':
            /* code */
            result = 0x6;
            break;
        case '7':
            /* code */
            result = 0x7;
            break;
        case '8':
            /* code */
            result = 0x8;
            break;
        case '9':
            /* code */
            result = 0x9;
            break;
        case 'A':
            /* code */
            result = 0xA;
            break;
        case 'B':
            /* code */
            result = 0xB;
            break;
        case 'C':
            /* code */
            result = 0xC;
            break;
        case 'D':
            /* code */
            result = 0xD;
            break;
        case 'E':
            /* code */
            result = 0xE;
            break;
        case 'F':
            /* code */
            result = 0xF;
            break;
        
        default:
            uBit.serial.printf("bad char found\n");
            while(1) ;
            break;
        }

        return result;
    }

    unsigned int charToUnsignedInt(char *buf){
        unsigned int result = 0x0;
            char a, b;
            a = buf[0];
            b = buf[1];
            unsigned int tophalf = numLookup(a);
            unsigned int bottomhalf = numLookup(b);
            unsigned int b0, b1, b2, b3;
            b0 = (tophalf << 4) | bottomhalf;


            tophalf = numLookup(buf[2]);
            bottomhalf = numLookup(buf[3]);

            b1 = (tophalf << 4) | bottomhalf;

            tophalf = numLookup(buf[4]);
            bottomhalf = numLookup(buf[5]);

            b2 = (tophalf << 4) | bottomhalf;


            tophalf = numLookup(buf[6]);
            bottomhalf = numLookup(buf[7]);

            b3 = (tophalf << 4) | bottomhalf;

            result = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;

            return result;
    }


    void writeRAM(){


            if(!once){

                uBit.log.clear(true);

                unsigned int beef = 0xF0F0DDCC;

                std::string beef_str = std::to_string(beef);
                char rows[8];
//                itoa(beef, rows);
                writeNum(rows, beef);

                //codal::ManagedString managed_beef_str(rows, 9);

                for (int i = 0; i < 0; i++){
                    uBit.log.beginRow();
                    uBit.log.logData("Regs", rows);
                    uBit.log.endRow();
                }

                //once++;
            


            
            unsigned int regs[2];
            volatile unsigned int *sp_ptr = (unsigned int *)0x2001FEA4;
            volatile unsigned int fv0, fv1, fv2, fv3;
            unsigned int addr;
            fv0 = 0xDEADBEEF;
            regs[0] = 1;
            regs[1] = 4;

            
            
            asm volatile(
                "NOP\n\t"
                "NOP\n\t"
                "NOP\n\t"
                "MOV %0, sp\n\t"
                "MOV %1, lr\n\r"
                "NOP\n\t"
                "NOP\n\t"
            :"=r" (regs[0]), "=r" (regs[1])
            :   
            :
            );

            

            uBit.serial.printf("sp before: %X\r\n", regs[0]);

            sp_ptr = (unsigned int *)regs[0];

            writeNum(rows, regs[0]);
            uBit.log.beginRow();
            uBit.log.logData("Regs", rows);
            uBit.log.endRow();

            writeNum(rows, regs[1]);
            uBit.log.beginRow();
            uBit.log.logData("Regs", rows);
            uBit.log.endRow();

            while((unsigned int)sp_ptr < 0x20020000){
                uBit.serial.printf("Addr: 0x%X: %X\r\n", sp_ptr, *sp_ptr);
                writeNum(rows, *sp_ptr);
                uBit.log.beginRow();
                uBit.log.logData("Regs", rows);
                uBit.log.endRow();

                sp_ptr++;
            }

            uBit.serial.printf("done with loop\r\n");

            asm volatile(
                "NOP\n\t"
                "NOP\n\t"
                "NOP\n\t"
                "trampoline_target:\n\t"
                "MOV R8, R8\n\t"
                "NOP\n\t"
                "NOP\n\t"
                "MOV %0, SP\n\t"
                "MOV %1, LR\n\t"
                "NOP\n\t"
                "NOP\n\t"
                "NOP\n\t"
                "NOP\n\t"
                : "=r" (regs[0]), "=r" (regs[1])
                :
                :);

            //uBit.serial.printf("sp after: %X\r\n", regs[0]);
            //uBit.serial.printf("lr after: %X\r\n", regs[1]);

            //uBit.serial.printf("leaving writeRAM\r\n");
            }

    }

#endif

    //0x20018000 top address

    /**
     * function to checkpoint the user's code
     */
    //%
    void checkpoint() {


        #if MICROBIT_CODAL
            // On micro:bit V2 the multiplier is higher than V1
        asm volatile(
            "NOP\n\t"
            "NOP\n\t"
            "NOP\n\t"
            "NOP\n\t"
            "PUSH {r0, r1, r2, r3, r4, r5, r6, r7}\n\t"
            "NOP\n\t"
            "NOP\n\t"
        :
        :
        :
        );

        writeRAM();

        uBit.serial.printf("leaving CHeckpoint\r\n");

        asm volatile(
            "NOP\n\t"
            "NOP\n\t"
            "NOP\n\t"
            "NOP\n\t"
            "POP {r0, r1, r2, r3, r4, r5, r6, r7}\n\t"
            "NOP\n\t"
            "NOP\n\t"
        :
        :
        :
        );


#else
            //UHHHHHHH
        #endif
    }

     /**
     * This function uses the C preprocessor to compile different code
     * depending on the version of the micro:bit.
     */
    //%
    int banana_multiplier(int bananas) {
        #if MICROBIT_CODAL
            // On micro:bit V2 the multiplier is higher than V1
            return bananas * V2_MULTIPLIER;
        #else
            // On micro:bit V1 the multiplier is lower than V2
            return bananas * V1_MULTIPLIER;
        #endif
    }

    /**
     * function to restore a checkpoint
     */
    //%
    void restore() {
        #if MICROBIT_CODAL
            // On micro:bit V2 the multiplier is higher than V1
            
            unsigned int csvLen = uBit.log.getDataLength(DataFormat::CSV);
            void* csvData = malloc(csvLen);
            int rows = uBit.log.getNumberOfRows();
            codal::ManagedString cheese("", 8);
            uBit.serial.printf("number of rows found: %d\r\n", rows);
            cheese = uBit.log.getRows(0, rows);
            //const char* blah = cheese.toCharArray();

            char *blah;
            unsigned int *sp_ptr;
            int data[4];
            if(uBit.log.readData((void*)csvData, 0,csvLen,codal::DataFormat::CSV, csvLen) == DEVICE_OK && rows){
                blah = (char *)csvData;
                char subsection[9];
                int offset = 5;
                for (int i = 0; i < 8; i++)
                {
                    subsection[i] = blah[i + offset];
                }
                subsection[8] = 0;
                unsigned int sp = charToUnsignedInt(subsection);
                offset = 14;
                for (int i = 0; i < 8; i++)
                {
                    subsection[i] = blah[i + offset];
                }
                subsection[8] = 0;
                unsigned int lr = charToUnsignedInt(subsection);

                uBit.serial.printf("%s\r\n", blah);
                uBit.serial.printf("subsection: %s\r\n", subsection);
                uBit.serial.printf("lr: %X\r\n", lr);
                uBit.serial.printf("sp: %X\r\n", sp);
                sp_ptr = (unsigned int *)sp;
                unsigned int stack_val;
                while ((unsigned int)sp_ptr < 0x20020000)
                {
                    offset = offset + 9; //8bytes + null term
                    for (int i = 0; i < 8; i++)
                    {
                        subsection[i] = blah[i + offset];
                    }
                    subsection[8] = 0;
                    stack_val = charToUnsignedInt(subsection);
                    //*sp_ptr = stack_val;
                    uBit.serial.printf("At %X just wrote: %X\r\n", sp_ptr, stack_val);
                    sp_ptr++;
                }



                free(csvData);

               /* 

                asm volatile(
                    "NOP\n\t"
                    "NOP\n\t"
                    "NOP\n\t"
                    "MOV SP, %0\n\t"
                    "MOV LR, %1\n\t"
                    "MOV R8, R8\n\t"
                    "NOP\n\t"
                    "NOP\n\t"
                    "B trampoline_target\n\t"
                    :
                    :"r" (sp), "r" (lr)
                    :);
                */


                //blash
                
            } else {
                uBit.serial.printf("did not find data in flash\r\n");
                free(csvData);
            }


            long int beef = atol(cheese.toCharArray());
            // uBit.serial.printf(cheese.toCharArray());
            uBit.serial.printf("\n");

#else
                // On micro:bit V1 the multiplier is lower than V2
                //UGHHHHHHHHHHHH
        #endif
    }

}
