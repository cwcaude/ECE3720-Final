#include <plib.h>
#include <math.h>

#pragma config FPBDIV = DIV_2

//r is a Rest.  
#define r 3000

//Cs = C sharp.  Eb = E flat.  Everything else is normal.
//lowest notes
#define a 4545
#define as 4292
#define b 4050
#define C 3817
#define Cs 3610
#define D 3402
#define Ds 3216
#define E 3031
#define F 2866
#define Fs 2703
#define G 2551
#define Gs 2410
#define Ab 2410
#define A 2273
#define As 2146
#define B 2025
#define CC 1911
#define CCs 1805
#define DD 1701
#define DDs 1608
#define EE 1515
//highest notes

//Our base note duration - change this to make your song play faster or slower.
#define t4 250

//t2 = half notet4 = quarter note.  t8 = eighth note.  Etc
#define t2 t4*2
#define t1 t2*2
#define t8 t4/2
#define t16 t8/2
#define t32 t16/2

//dotted notes - dq or d4 = dotted quarter, etc
#define d2 t2+t4
#define d4 t4+t8
#define d8 t8+t16
#define d16 t16+t32

//the number of notes in your song
#define num_notes 20

/*Initialize global variables*/
int note, noteTime, mode;

short musicNotes[num_notes + 1] = {
a, as, b, C, Cs, D, Ds, E, F, Fs, G, Gs, A, As, B, CC, CCs, DD, DDs, EE, r};

short musicLengths[10] = {
t1, d2, t2, d4, t4, d8, t8, d16, t16, t32
};

short s1[num_notes] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19
};

short l1[num_notes] = {
    4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4
};


short hexNums[num_notes] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67, 0x3F + 0x80, 0x06 + 0x80, 0x5B + 0x80, 0x4F + 0x80, 0x66 + 0x80, 0x6D + 0x80, 0x7D + 0x80, 0x07 + 0x80, 0x7F + 0x80, 0x67 + 0x80 
};

short hexLets[12] = {
    0x37, 0xB7, 0x7C, 0x58, 0xD8, 0x5E, 0xDE, 0x79, 0x71, 0xF1, 0x6F, 0xEF
};

short delay[num_notes * 2] = {
t4, t32, t4 ,t32,t4, t32, t4, t32, t4, t32, t4,t32, t4, t32, t4, t32, t4, t32, t4,t32, t4, t32, t4, t32, t4, t32, t4,t32, t4, t32, t4, t32, t4,t32, t4, t32, t4, t32, t4, t32};

short SongNotes[num_notes * 2] = {
C, r, C ,r, C, r,C, r, C, r, C,r, C, r, C, r,C, r, C,r, C, r, C, r, C, r, C,r, C, r, C, r, C,r, C, r, C, r, C, r};


/*---- Interrupt Functions ----*/


//Music Interrupt Function
void __ISR(8) Music(void){
   
    noteTime++;
   
    if(SongNotes[note] == r)
        {}//Wait for rest
    else{
         LATBbits.LATB0 = LATBbits.LATB0^1; //write high or low to our port
        }
   
    PR2 = SongNotes[note]; //Tell it to count to our desired frequency
    IFS0bits.T2IF = 0;  // Clear Interrupt Flag
}

// Optical Encoder Interrupt Functions
void __ISR(15) interruptAfunction(void) {
    int AI = PORTBbits.RB7;  //Read in values from pins
    int BI = PORTCbits.RC8;
    int i;
    
    if(mode == 0)
    {
        if(AI == 1)  //Rising Edge
        {
            if(BI == 0) note++;
            else       note--;
        }
        if(AI == 0)
        {
            if(BI == 1) note--;
            else       note++;
        }
        
        if(note > num_notes - 1) note = 0;
        if(note < 0) note = 19;

   
    }

    else if(mode == 1)
    {
        if(AI == 1)  //Rising Edge
        {
            if(B == 0) s1[note]++;
            else       s1[note]--;
        }
        if(AI == 0)
        {
            if(BI == 1) s1[note]--;
            else       s1[note]++;
        }
        
        if(s1[note] > num_notes - 1) s1[note] = 0;
        if(s1[note] < 0) s1[note] = 19;
        
        i = s1[note];
        if(i > 12) i = i-12;

    
    }
    
    else if(mode == 2)
    {
        if(AI == 1)  //Rising Edge
        {
            if(BI == 0) l1[note]++;
            else       l1[note]--;
        }
        if(AI == 0)
        {
            if(BI == 1) l1[note]--;
            else       l1[note]++;
        }
        
        if(l1[note] > 9) l1[note] = 0;
        if(l1[note] < 0) l1[note] = 9;
        
        i = l1[note];

    }
    
    else if(mode == 3)
    {
    
    }
    
    INTCONbits.INT3EP = (AI ^ 1); //Flips the trigger
   
    IFS0bits.INT3IF = 0; //Set interrupt 3 flag status
}

void __ISR(7) interruptBFunction(void) {
    int AI = PORTBbits.RB7;  //Read in values from pins
    int BI = PORTCbits.RC8;
    
    int i;
   
    if(mode == 0)
    {
    
        if(BI == 1)  //Rising Edge
        {
            if(AI==1) note++;   //CW
            else     note--;   //CCW
        }
        else
        {
            if(AI==1)  note--;  //CCW
            else      note++;  //CW
        }
    
        if(note > num_notes - 1) note = 0;
        if(note < 0) note = 19;
        
        LATC = hexNums[note];
        LATAbits.LATA1 = 1;
        LATAbits.LATA0 = 0;
        LATBbits.LATB15 = 0;
    }
    
    else if(mode == 1)
    {
    
        if(BI == 1)  //Rising Edge
        {
            if(AI==1) s1[note]++;   //CW
            else     s1[note]--;   //CCW
        }
        else
        {
            if(AI==1)  s1[note]--;  //CCW
            else      s1[note]++;  //CW
        }
    
        if(s1[note] > num_notes - 1) s1[note] = 0;
        if(s1[note] < 0) s1[note] = 19;
        
        i = s1[note];
        if(i > 12) i = i-12;

        LATC = hexLets[i];
        LATAbits.LATA1 = 0;
        LATAbits.LATA0 = 1;
        LATBbits.LATB15 = 0;
    }
    
    else if(mode == 2)
    {
    
        if(BI == 1)  //Rising Edge
        {
            if(AI==1) l1[note]++;   //CW
            else     l1[note]--;   //CCW
        }
        else
        {
            if(AI==1)  l1[note]--;  //CCW
            else      l1[note]++;  //CW
        }
    
        if(l1[note] > 9) l1[note] = 0;
        if(l1[note] < 0) l1[note] = 9;

    }
    
    else if(mode == 3)
    {
    
    }
    
    INTCONbits.INT1EP = (BI ^ 1);
    IFS0bits.INT1IF = 0; //Clear the Flag
}

void delaytime(){
    int i, j;
    for(i = 0; i < 500; i++)
        for(j = 0; j < 500; j++);
}

main()
{
    note = 0; //Begin on the first note
    //mode = 0; //Begin in the first mode
    
    /*---- Speaker Pin Setup and Interrupt Setup ----*/
    
    TRISBbits.TRISB0 = 0; //Assign Speaker Output to pin B0
    ANSELB = 0; //Set all pins as Digital
   
    //Enable interrupts
    INTEnableSystemMultiVectoredInt();
    
    IPC2bits.T2IP = 1;  //Timer 2 Interrupt Priority
   
    T2CONbits.TCKPS = 0; //Set 1:1 prescale value
    T2CONbits.TGATE = 0; //Disable Gated time accumulation.
    T2CONbits.TCS = 0; //Set it to use the internal peripheral clock
    T2CONbits.T32 = 0;//Just use the 16-bit version;

    PR2 = musicNotes[0];
    
   
    note = 0; //Start at the beginning of the song
    
    /*---- 7-Segment Display Pin Setup ----*/
    
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC3 = 0;
    TRISCbits.TRISC4 = 0;
    TRISCbits.TRISC5 = 0;
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 0;
    
    /*---- Mode LEDs and Button Setup ----*/
    
    TRISBbits.TRISB15 = 0; //Top LED
    TRISAbits.TRISA0 = 0; //Middle LED
    TRISAbits.TRISA1 = 0; //Bottom LED
    
    TRISAbits.TRISA7 = 1; //Button Input
    
    /*---- Optical Encoder Setup ----*/
    
    PPSInput(2,INT3, RPC8); //Setup C8 as INT3 PPS
    INTCONbits.INT0EP = 1; //Enable INT0
    INTCONbits.INT3EP = 1; //Enable INT3
    IEC0bits.INT0IE = 1; //Enable Interrupt 0
    IEC0bits.INT3IE = 1; //Enable Interrupt 3
    IFS0bits.INT0IF = 0; //Set interrupt 0 flag status
    IFS0bits.INT3IF = 0; //Set interrupt 3 flag status
    IPC0bits.INT0IP = 2; //Set the interrupt 0 priority
    IPC3bits.INT3IP = 3; //Set the interrupt 3 priority

    INTCONbits.INT0EP = !(PORTBbits.RB7);   //Look for Rising or Falling Trigger
    INTCONbits.INT3EP = !(PORTCbits.RC8);  
    
    LATC = hexNums[0];
    
    
    PPSInput(2,INT3, RPC8); //Setup C8 as INT3 PPS
    
    T2CONbits.ON = 0;//Enable Timer
    IEC0bits.T2IE = 1;  //Timer 2 Interrupt Enable
    
    short i;
    int j = 0;
    int bval = !PORTAbits.RA7;
    int modeLocal = 0;
    
    while(1)
    {
        if(PORTAbits.RA7 == bval ) {
           delaytime();
           modeLocal++;
           if(mode > 3) modeLocal = 0;
       }
        
        mode = modeLocal;
       
        i = 0;
        
        for(j=0; j < 20; j++)
        {
           delay[i] = musicLengths[l1[j]];
           SongNotes[i] = musicNotes[s1[j]];
           i = i + 2;
        }
        
        if(modeLocal == 0)
        {
            T2CONbits.ON = 0;//Enable Timer
            LATC = hexNums[note];
            LATAbits.LATA1 = 1;
            LATAbits.LATA0 = 0;
            LATBbits.LATB15 = 0;
        }
        
        else if(modeLocal == 1)
       {
           T2CONbits.ON = 0;//Enable Timer
           LATC = hexLets[s1[note]];
           LATAbits.LATA1 = 0;
           LATAbits.LATA0 = 1;
           LATBbits.LATB15 = 0;
       }
        
       else if(modeLocal == 2)
       {
           T2CONbits.ON = 0;//Enable Timer
           LATC = hexNums[l1[note]];
           LATAbits.LATA1 = 0;
           LATAbits.LATA0 = 0;
           LATBbits.LATB15 = 1;
       }
        else if(modeLocal == 3)
       {
           T2CONbits.ON = 1;//Enable Timer
           LATAbits.LATA1 = 1;
           LATAbits.LATA0 = 1;
           LATBbits.LATB15 = 1;
           i = 0;
           
            for(note=0; note < num_notes * 2; note++) //Loop to play Notes
            {
                i++;
                if(i >= num_notes) i = note - num_notes; 
                noteTime = 0;
                LATC = hexNums[i];
                while(noteTime < delay[note]) 
                {
                        //Wait for note Time to be done
                        }
            }
           modeLocal = 1;
           T2CONbits.ON = 0;//Enable Timer
        }
    }
}


