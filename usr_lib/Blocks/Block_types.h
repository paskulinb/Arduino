#ifndef Block_types_h
#define Block_types_h

enum BlockType {
    B_TYPE_AI,      //Analog Input
    B_TYPE_AO,      //Analog Output
    B_TYPE_DI,      //Digital Input
    B_TYPE_DO,      //Digital Output

    /* sensors */
    B_TYPE_STE,     //Sensor TEmperature
    
    /* primary value */
    B_TYPE_CV,      //Constant Value
    B_TYPE_CRN,     //Cron
                    //eg.: "0,10,20 5 0/3 * *"
    B_TYPE_TDV,     //Time Dependent Value
                    //eg.: "7:00/22,8:30/15,16:20/22,23:00/15" (hh:mm:ss/value)
    B_TYPE_WFM,     //WaveForM (PULSE,...)
                    //eg.: "pulse,30s|1m40s,2" (pulse,on|off|on...,number_of_repeats) (0=forever,default=1)

    /* operators */
    B_TYPE_OP,      //OPeration; logic (AND,OR) arithmetic (ADD,SUB,MUL,DIV) comparison (==,<,>,<=,>=,!)

    /* utility */
    B_TYPE_REG      //ON/OFF REGulator &INREF,&INVAL,HIST,INVOUT
};
#endif
