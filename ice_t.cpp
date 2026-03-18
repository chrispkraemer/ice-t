/**
 * C++ functions as part of the extension.
 * 
 * A function defined here will run on the micro:bit, there is also a "shim"
 * in the main.ts file that will be run in the simulator.
 * Info about shims: https://makecode.com/simshim
 */

#include <memory>
#include "pxt.h"
#include <string>

/*Notes for Tom and Joe*/

/*
1) Sorry for the mess
2) I don't do much of my acutal development here. I do it in main.cpp of microbitv2samples because it is easier to debug(ELF file)
3) Any char to int or int to char functions can be ignored(chartoUnsignedInt(), numlookup(), writeNum())
    - This was for an attempt at checkpointing with the datalogger. Possible but the FRAM is easier
4) FRAM functions are:
    - fram_init(): connect to the FRAM board over SPI
    - write_enable(): enable writes to the FRAM
    - read8(): read a single byte from the FRAM
    - write8(): write a single byte to the FRAM
    - checkpoint() and writeRAM():
        - checkpoint(): User facing function(shim). Pushes the regs onto the stack. Calls writeRAM(). Pops regs from the stack
        - writeRAM(): writes all of SRAM from 0x20000000 to 0x20020000 onto the FRAM board
    - restore() and readRAM():
        - restore(): User facing function(shim). Checks if a checkpoint is present and which of the two(double buffer) to choose from.
                     Moves the SP "out of the way." by 1000 bytes. Bad things can happen when the memory you are using to overwrite memory is itself overwritten
        - readRAM(): Reads the checkpoint from FRAM and overwrites SRAM. The microbit will panic when addresses below ~0x20008000 are overwritten.
                     So, allocate a huge object in the heap, load the checkpoint data from 0x20000000 to 0x2000a000(extra for good measure) into 
                     that object, turn off interrupts, overwrite 0x20000000 to 0x2000a000, enable interrupts and continue overwriting memory.
                     Restore memory until the "out of the way" section, skip over it until the start of the stack(according to the checkpoint, not the current SP),
                     and then overwrite the stack and the SP. 

                     * Returning to the user program after the checkpoint has been restored is a little ugly. Unconditional branch to the end
                       of writeRAM. The target is the label "trampoline_target." We branch to the end of writeRAM because that is technically the
                       state/position/location the checkpoint is refering to. From writeRAM(), the function returns as normal to checkpoint() where
                       the registers are restored by being poppped off of the stack.
    - clearFRAM()
        - erases checkpoint data by writing 0x0 to the generation value which lives at address 0x0 on the FRAM. 

    - The checkpoints are double buffered to ensure one accurate checkpoint always exists(power could fail while writing a checkpoint)
        - If gen is <=0, no checkpoint is present
        - If gen is 1, read from buffer1 and write to buffer2, update gen to 2 once the checkpoint is written
        - If gen is 2, read from buffer2 and write to buffer1, update gen to 1 once the checkpoint is written

*/


enum OPCODES {
    OPCODE_READ = 3,
    OPCODE_WRITE = 2,
    OPCODE_RDID = 159,
    OPCODE_WREN = 6
};

int once = 0;
#if MICROBIT_CODAL
codal::SPI* fram_spi = new codal::SPI(MOSI, MISO, SCK); 
#endif
int generation = -1;
unsigned int lowbufferAddr = 4;
unsigned int highbufferAddr = 130000;
unsigned int RAMaddrStart = 0x20000000;

using namespace pxt;

namespace ice_t {
    #define V1_MULTIPLIER 2
    #define V2_MULTIPLIER 3

    #if MICROBIT_CODAL

    void write_enable(){

        uBit.io.P9.setDigitalValue(0);
        fram_spi->write(OPCODE_WREN);
        uBit.io.P9.setDigitalValue(1);
    }

    unsigned int read8(unsigned int addr){

         uBit.io.P9.setDigitalValue(0);
         fram_spi->write(OPCODE_READ);
         fram_spi->write(addr >> 16);
         fram_spi->write(addr >> 8);
         fram_spi->write(addr & 0xff);
         unsigned int retval = fram_spi->write(255);
         uBit.io.P9.setDigitalValue(1);
         return retval;
    }

    void write8(unsigned int addr, unsigned int val)
    {

        write_enable();

        uBit.io.P9.setDigitalValue(0);
        fram_spi->write(OPCODE_WRITE);
        fram_spi->write(addr >> 16);
        fram_spi->write(addr >> 8);
        fram_spi->write(addr & 0xff);
        fram_spi->write(val & 0xff);
        uBit.io.P9.setDigitalValue(1);
    }






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
            b0 = (tophalf << 4) | (bottomhalf & 0xff);


            tophalf = numLookup(buf[2]);
            bottomhalf = numLookup(buf[3]);

            b1 = (tophalf << 4) | (bottomhalf & 0xff) ;

            tophalf = numLookup(buf[4]);
            bottomhalf = numLookup(buf[5]);

            b2 = (tophalf << 4) | (bottomhalf & 0xff);


            tophalf = numLookup(buf[6]);
            bottomhalf = numLookup(buf[7]);

            b3 = (tophalf << 4) | (bottomhalf & 0xff);

            result = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;

            return result;
    }


    void readRAM(){

        volatile int regs[2];
        volatile unsigned int *sp_ptr = (unsigned int *)0x20020000;
        volatile unsigned int *RAM_overwrite_addr = (unsigned int*) 0x0;
        volatile unsigned int fv0, fv1, fv2, fv3, ramVal;
        unsigned int addr;
        fv0 = 0xDEADBEEF;
        regs[0] = 1;
        regs[1] = 4;

        if(generation <= 0){
            return;
        } else if(generation == 1){
            addr = lowbufferAddr;
        } else if(generation == 2){
            addr = highbufferAddr;
        } else {
            uBit.serial.printf("READRAM generation error\r\n");
            while(1);
        }



        uBit.io.P9.setDigitalValue(0);
        fram_spi->write(OPCODE_READ);
        fram_spi->write(addr >> 16);
        fram_spi->write(addr >> 8);
        fram_spi->write(addr & 0xff); 
        fv0 =fram_spi->write(255);
        fv1 =fram_spi->write(255);
        fv2 =fram_spi->write(255);
        fv3 =fram_spi->write(255); //get SP

        regs[0] = (fv0 << 24) | (fv1 << 16) | (fv2 << 8) | (fv3);


        fv0 =fram_spi->write(255);
        fv1 =fram_spi->write(255);
        fv2 =fram_spi->write(255);
        fv3 =fram_spi->write(255); //get LR

        regs[1] = (fv0 << 24) | (fv1 << 16) | (fv2 << 8) | (fv3);

        sp_ptr = (unsigned int*)regs[0];



        //again for now, read entire sram space
        RAM_overwrite_addr = (unsigned int*)RAMaddrStart;

        unsigned int heapStopAddr = regs[0] - 1000; //overwrite until where this func is using data, then overwrite the stack

        unsigned int* ckptd_runtime_ptr = (unsigned int*)malloc(0xa000); //oh lord

        uBit.serial.printf("ckptd_ptr at addr %x\r\n", ckptd_runtime_ptr);

        unsigned int i = 0;

        unsigned int distance = 0xa000/4;

        while(i < distance){ //0x6000/4=6144. Gross for now
            fv0 =fram_spi->write(255);
            fv1 =fram_spi->write(255);
            fv2 =fram_spi->write(255);
            fv3 =fram_spi->write(255);

            ramVal = (fv0 << 24) | (fv1 << 16) | (fv2 << 8) | (fv3);

            *(ckptd_runtime_ptr+i) = ramVal;
            i++;
            //pointer offset by i = val

        }

        //so now the runtime has been loaded into the huge heap object

        //now disable interrupts, overwrite 0x20000000-0x2000a000, enable interrupts,
        //and continue as normal starting at 0x2000a000

        target_disable_irq();
        i = 0;
        while((unsigned int)RAM_overwrite_addr < 0x2000a000){
            *RAM_overwrite_addr = *(ckptd_runtime_ptr+i);
            i++;
            RAM_overwrite_addr++;
            //very gross but what is happening here:
            //      disable ints, overwrite important ram by reading from uninportant heap
            //      enable ints
        }

/*

TODO

Things are working *seemingly* correct up until this point. Interrupts are turned back on and shit hits the fan. Maybe there is a way to clear the interrupts queue(is that a thing?)

It is also possible that this overwrite isn't quite correct. I will want to double check that.

(DONE)1) Check that the checkpoint for this region passes the smell test

1.5) CHECKPOINT JUST WORKED! HEAP OBJECT IS GIGANTIC! TEST IT SOME MORE TOMORROW!

2) Don't forget to flash binary.hex as well! things work(in a bad way) when I forget this step

3) Maybe moving the enable to later would be helpful. I need to enable interrupts for spi to work

4) Consider moving fram_spi object initialization

*/



        //free the huge object because we don't need it anymore

        free(ckptd_runtime_ptr);

        target_enable_irq();


        while ((unsigned int)RAM_overwrite_addr < heapStopAddr)
        {

            fv0 =fram_spi->write(255);
            fv1 =fram_spi->write(255);
            fv2 =fram_spi->write(255);
            fv3 =fram_spi->write(255);

            ramVal = (fv0 << 24) | (fv1 << 16) | (fv2 << 8) | (fv3);

            *RAM_overwrite_addr = ramVal;

            //uBit.serial.printf("FRAM VALUE: Addr: 0x%X: %X\r\n", sp_ptr, ramVal);
            //uBit.serial.printf("Addr: 0x%X: %X\r\n", sp_ptr, *sp_ptr);


            RAM_overwrite_addr++;
        }


        //skip unused part of the checkpoint
        //ugly for now
        while((unsigned int)RAM_overwrite_addr < (unsigned int)sp_ptr){

            fv0 =fram_spi->write(255);
            fv1 =fram_spi->write(255);
            fv2 =fram_spi->write(255);
            fv3 =fram_spi->write(255);

            ramVal = (fv0 << 24) | (fv1 << 16) | (fv2 << 8) | (fv3);

            //*RAM_overwrite_addr = ramVal;

            RAM_overwrite_addr++;
        }
        
        RAM_overwrite_addr = sp_ptr;
        while((unsigned int)RAM_overwrite_addr < 0x20020000){
            fv0 =fram_spi->write(255);
            fv1 =fram_spi->write(255);
            fv2 =fram_spi->write(255);
            fv3 =fram_spi->write(255);

            ramVal = (fv0 << 24) | (fv1 << 16) | (fv2 << 8) | (fv3);

            *RAM_overwrite_addr = ramVal;

            //uBit.serial.printf("FRAM VALUE: Addr: 0x%X: %X\r\n", sp_ptr, ramVal);
            //uBit.serial.printf("Addr: 0x%X: %X\r\n", sp_ptr, *sp_ptr);


            RAM_overwrite_addr++;

        }




        uBit.io.P9.setDigitalValue(1);

        asm volatile("mov sp, %0\n\t"
                //"mov r2, %2\n\t"
                //"mov r3, %3\n\t"
                //"mov r7, %4\n\t"
                :
                : "r"((unsigned int)sp_ptr)  //,"r" (regs[2]),"r" (regs[3]),"r" (regs[4])
                :);



        asm volatile(
                "NOP\n\t"
                "NOP\n\t"
                "NOP\n\t"
                "NOP\n\t"
                "NOP\n\t"
                "B trampoline_target\n\t"
                "NOP\n\t"
                "NOP\n\t"
                "NOP\n\t"
                "NOP\n\t"
                :
                :
                :
                );


    }


    void writeRAM(){

        volatile unsigned int regs[2];
        volatile unsigned int *sp_ptr = (unsigned int *)0x20020000;
        volatile unsigned int fv0, fv1, fv2, fv3;
        volatile unsigned int addr;
        fv0 = 0xDEADBEEF;
        regs[0] = 1;
        regs[1] = 4;

        uBit.serial.printf("in writeRAM, gen=%d\r\n", generation);

        if (generation == -1)
        {
            return;
        }
        else if (generation == 1)
        {
            addr = highbufferAddr;
            generation = 2;
        }
        else
        {
            addr = lowbufferAddr;
            generation = 1;
        }

        asm volatile("NOP\n\t"
                     "NOP\n\t"
                     "NOP\n\t"
                     "MOV %0, sp\n\t"
                     "MOV %1, lr\n\r"
                     "NOP\n\t"
                     "NOP\n\t"
                     : "=r"(regs[0]), "=r"(regs[1])
                     :
                     :);

        uBit.serial.printf("sp before: %X\r\n", regs[0]);

        sp_ptr = (unsigned int *)regs[0];

        write_enable();
        uBit.io.P9.setDigitalValue(0);
        fram_spi->write(OPCODE_WRITE);
        fram_spi->write(addr >> 16);
        fram_spi->write(addr >> 8);
        fram_spi->write(addr & 0xff); // just putting at addr 0x0 for now
        fram_spi->write((regs[0] >> 24) & 0xff);
        fram_spi->write((regs[0] >> 16) & 0xff);
        fram_spi->write((regs[0] >> 8) & 0xff);
        fram_spi->write((regs[0]) & 0xff); // write SP to FRAM
        fram_spi->write((regs[1] >> 24) & 0xff);
        fram_spi->write((regs[1] >> 16) & 0xff);
        fram_spi->write((regs[1] >> 8) & 0xff);
        fram_spi->write((regs[1]) & 0xff); // write LR to FRAM

        // for now, write the entire SRAM space
        sp_ptr = (unsigned int *)RAMaddrStart;

        while ((unsigned int)sp_ptr < 0x20020000)
        {
            // uBit.serial.printf("Addr: 0x%X: %X\r\n", sp_ptr, *sp_ptr);

            fram_spi->write((*sp_ptr >> 24) & 0xff);
            fram_spi->write((*sp_ptr >> 16) & 0xff);
            fram_spi->write((*sp_ptr >> 8) & 0xff);
            fram_spi->write((*sp_ptr) & 0xff); // write LR to FRAM

            sp_ptr++;
        }
        uBit.io.P9.setDigitalValue(1);

        uBit.serial.printf("done with loop\r\n");

        write_enable();
        uBit.io.P9.setDigitalValue(0);
        fram_spi->write(OPCODE_WRITE);
        fram_spi->write(0);
        fram_spi->write(0);
        fram_spi->write(0); // just putting at addr 0x0 for now
        fram_spi->write(generation);
        uBit.io.P9.setDigitalValue(1);

        asm volatile("NOP\n\t"
                     "NOP\n\t"
                     "NOP\n\t"
                     "trampoline_target:\n\t"
                     "MOV R8, R8\n\t"
                     "NOP\n\t"
                     "NOP\n\t"
                     "NOP\n\t"
                     "NOP\n\t"
                     "NOP\n\t"
                     "NOP\n\t"
                     :
                     :
                     :);
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
            "PUSH {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9}\n\t"
            "SUB SP, 120\n\t"
            "NOP\n\t"
            "NOP\n\t"
        :
        :
        :
        );

        writeRAM();


        asm volatile(
            "NOP\n\t"
            "NOP\n\t"
            "NOP\n\t"
            "NOP\n\t"
            "ADD SP, 120\n\t"
            "POP {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9}\n\t"
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
     * function to INITIALIZE the FRAM(non-volatile storage) 
     */
    //%
    void fram_init(){

        #if MICROBIT_CODAL
        int tx, whoami, wh0, wh1, wh2, wh3;

        uBit.io.P9.setDigitalValue(0);
        tx = 159;
        whoami = fram_spi->write(OPCODE_RDID);

        // uBit.serial.printf("passed whoami: %d\r\n", whoami);
        tx = 255;
        wh0 = fram_spi->write(255);
        // uBit.serial.printf("passed wh0: %d\r\n", wh0);
        wh1 = fram_spi->write(255);
        // uBit.serial.printf("passed wh1: %d\r\n", wh1);
        wh2 = fram_spi->write(255);
        // uBit.serial.printf("passed wh2: %d\r\n", wh2);
        wh3 = fram_spi->write(255);
        // uBit.serial.printf("passed wh3: %d\r\n", wh3);
        uBit.io.P9.setDigitalValue(1);

        // uBit.serial.printf("received %d\r\n", wh1);

        if (wh1 == 127)
        {
            uBit.serial.printf("FRAM Connected\r\n");
        }
        else
        {
            uBit.serial.printf("FRAM NOT connected\r\n");
        }

        generation = read8(0x0);
        
        #endif
    }


    /**
     * function to clear the FRAM(non-volatile storage) 
     */
    //%
    void clear_FRAM(){
        #if MICROBIT_CODAL
        write_enable();
        uBit.io.P9.setDigitalValue(0);
        fram_spi->write(OPCODE_WRITE);
        fram_spi->write(0);
        fram_spi->write(0);
        fram_spi->write(0);
        fram_spi->write(0);
        fram_spi->write(0);
        fram_spi->write(0);
        fram_spi->write(0);
        /*
        for (int i = 0; i < 256000; i++)
        {
           fram_spi->write(0);
        }
        */
        uBit.io.P9.setDigitalValue(1);

        uBit.serial.printf("FRAM Cleared\r\n");

        while (1)
            ;
        
        #endif
    }



    /**
     * function to add a breakpoint for debugging purposes 
     */
    //%
    void breakpoint(){
        #if MICROBIT_CODAL
    
        asm volatile(
            "NOP \n\t"
            "NOP \n\t"
            "BKPT \n\t"
            "NOP \n\t"
            "NOP \n\t"
            :
            :
            :
        );

        #endif
    }


    /**
     * function to restore a checkpoint
     */
    //%
    void restore() {
        #if MICROBIT_CODAL




        volatile int regs[2];
        volatile unsigned int *sp_ptr = (unsigned int *)0x20020000;
        volatile unsigned int fv0, fv1, fv2, fv3, ramVal, displacedSP;
        unsigned int addr;
        fv0 = 0xDEADBEEF;
        regs[0] = 1;
        regs[1] = 4;

        uBit.serial.printf("in restore\r\n");

        if (generation <= 0)
        {
            return;
        }
        else if (generation == 1)
        {
            addr = lowbufferAddr;
        }
        else if (generation == 2)
        {
            addr = highbufferAddr;
        }
        else
        {
            uBit.serial.printf("READRAM generation error\r\n");
            while (1)
                ;
        }

        
        uBit.serial.printf("found a checkpoint\r\n");

        uBit.io.P9.setDigitalValue(0);
        fram_spi->write(OPCODE_READ);
        fram_spi->write(addr >> 16);
        fram_spi->write(addr >> 8);
        fram_spi->write(addr & 0xff); // read from addr 0x0
        fv0 = fram_spi->write(255);
        fv1 = fram_spi->write(255);
        fv2 = fram_spi->write(255);
        fv3 = fram_spi->write(255); // get SP

        regs[0] = (fv0 << 24) | (fv1 << 16) | (fv2 << 8) | (fv3);

        fv0 = fram_spi->write(255);
        fv1 = fram_spi->write(255);
        fv2 = fram_spi->write(255);
        fv3 = fram_spi->write(255); // get LR

        uBit.io.P9.setDigitalValue(1);
        regs[1] = (fv0 << 24) | (fv1 << 16) | (fv2 << 8) | (fv3);

        sp_ptr = (unsigned int *)regs[0];
        displacedSP = regs[0] - 1000;


        /*TODO
        
        Some of this this code is redundant with ReadRAM(). I needed space to move the SP and this was the fastest way I could think of. 
        
        In order to overwrite all of RAM, it is a good idea to not overwrite the area of memory you are currently working with, so:

            * Move the SP ~1000 bytes down. 
            * Restore the checkpoint up until this point, then skip over this current working area, then restore the stack.
        
        */

        asm volatile(
            "NOP\n\t"
            "NOP\n\t"
            "NOP\n\t"
            "NOP\n\t"
            "MOV SP, %0\n\t"
            "NOP\n\t"
            "NOP\n\t"
            "NOP\n\t"
            "NOP\n\t"
            "NOP\n\t"
            :
            : "r"(displacedSP)
            :
       );


       readRAM();

#else
                // On micro:bit V1 the multiplier is lower than V2
                //UGHHHHHHHHHHHH
        #endif
    }

}
