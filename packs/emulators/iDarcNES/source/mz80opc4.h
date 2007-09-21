/** Z80: portable Z80 emulator *******************************/
/**                                                         **/
/**                         CodesED.h                       **/
/**                                                         **/
/** This file contains implementation for the ED table of   **/
/** Z80 commands. It is included from Z80.c.                **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994,1995,1996,1997       **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/

/* $Id: mz80opc4.h,v 1.2 1999/11/08 01:33:53 nyef Exp $ */

/** This is a special patch for emulating BIOS calls: ********/
/* case DB_FE:     PatchZ80(R);break; */
/*************************************************************/

case ADC_HL_BC: M_ADCW(BC);break;
case ADC_HL_DE: M_ADCW(DE);break;
case ADC_HL_HL: M_ADCW(HL);break;
case ADC_HL_SP: M_ADCW(SP);break;

case SBC_HL_BC: M_SBCW(BC);break;
case SBC_HL_DE: M_SBCW(DE);break;
case SBC_HL_HL: M_SBCW(HL);break;
case SBC_HL_SP: M_SBCW(SP);break;

case LD_xWORDe_HL:
  J.B.l=OpZ80(R);
  J.B.h=OpZ80(R);
  WrZ80(J.W++,R->HL.B.l);
  WrZ80(J.W,R->HL.B.h);
  break;
case LD_xWORDe_DE:
  J.B.l=OpZ80(R);
  J.B.h=OpZ80(R);
  WrZ80(J.W++,R->DE.B.l);
  WrZ80(J.W,R->DE.B.h);
  break;
case LD_xWORDe_BC:
  J.B.l=OpZ80(R);
  J.B.h=OpZ80(R);
  WrZ80(J.W++,R->BC.B.l);
  WrZ80(J.W,R->BC.B.h);
  break;
case LD_xWORDe_SP:
  J.B.l=OpZ80(R);
  J.B.h=OpZ80(R);
  WrZ80(J.W++,R->SP.B.l);
  WrZ80(J.W,R->SP.B.h);
  break;

case LD_HL_xWORDe:
  J.B.l=OpZ80(R);
  J.B.h=OpZ80(R);
  R->HL.B.l=RdZ80(J.W++);
  R->HL.B.h=RdZ80(J.W);
  break;
case LD_DE_xWORDe:
  J.B.l=OpZ80(R);
  J.B.h=OpZ80(R);
  R->DE.B.l=RdZ80(J.W++);
  R->DE.B.h=RdZ80(J.W);
  break;
case LD_BC_xWORDe:
  J.B.l=OpZ80(R);
  J.B.h=OpZ80(R);
  R->BC.B.l=RdZ80(J.W++);
  R->BC.B.h=RdZ80(J.W);
  break;
case LD_SP_xWORDe:
  J.B.l=OpZ80(R);
  J.B.h=OpZ80(R);
  R->SP.B.l=RdZ80(J.W++);
  R->SP.B.h=RdZ80(J.W);
  break;

case RRD:
  I=RdZ80(R->HL.W);
  J.B.l=(I>>4)|(R->AF.B.h<<4);
  WrZ80(R->HL.W,J.B.l);
  R->AF.B.h=(I&0x0F)|(R->AF.B.h&0xF0);
  R->AF.B.l=PZSTable[R->AF.B.h]|(R->AF.B.l&C_FLAG);
  break;
case RLD:
  I=RdZ80(R->HL.W);
  J.B.l=(I<<4)|(R->AF.B.h&0x0F);
  WrZ80(R->HL.W,J.B.l);
  R->AF.B.h=(I>>4)|(R->AF.B.h&0xF0);
  R->AF.B.l=PZSTable[R->AF.B.h]|(R->AF.B.l&C_FLAG);
  break;

case LD_A_I:
  R->AF.B.h=R->I;
  R->AF.B.l=(R->AF.B.l&C_FLAG)|(R->IFF&1? P_FLAG:0)|ZSTable[R->AF.B.h];
  break;

case LD_A_R:
  R->AF.B.h=(byte)(-R->ICount&0xFF);
  R->AF.B.l=(R->AF.B.l&C_FLAG)|(R->IFF&1? P_FLAG:0)|ZSTable[R->AF.B.h];
  break;

case LD_I_A:   R->I=R->AF.B.h;break;
case LD_R_A:   break;

case IM_0:     R->IFF&=0xF9;break;
case IM_1:     R->IFF=(R->IFF&0xF9)|2;break;
case IM_2:     R->IFF=(R->IFF&0xF9)|4;break;

case RETI:     M_RET;break;
case RETN:     if(R->IFF&0x40) R->IFF|=0x01; else R->IFF&=0xFE;
               M_RET;break;

case NEG:      I=R->AF.B.h;R->AF.B.h=0;M_SUB(I);break;

case IN_B_xC:  M_IN(R->BC.B.h);break;
case IN_C_xC:  M_IN(R->BC.B.l);break;
case IN_D_xC:  M_IN(R->DE.B.h);break;
case IN_E_xC:  M_IN(R->DE.B.l);break;
case IN_H_xC:  M_IN(R->HL.B.h);break;
case IN_L_xC:  M_IN(R->HL.B.l);break;
case IN_A_xC:  M_IN(R->AF.B.h);break;
case IN_F_xC:  M_IN(J.B.l);break;

case OUT_xC_B: OutZ80(R->BC.B.l,R->BC.B.h);break;
case OUT_xC_C: OutZ80(R->BC.B.l,R->BC.B.l);break;
case OUT_xC_D: OutZ80(R->BC.B.l,R->DE.B.h);break;
case OUT_xC_E: OutZ80(R->BC.B.l,R->DE.B.l);break;
case OUT_xC_H: OutZ80(R->BC.B.l,R->HL.B.h);break;
case OUT_xC_L: OutZ80(R->BC.B.l,R->HL.B.l);break;
case OUT_xC_A: OutZ80(R->BC.B.l,R->AF.B.h);break;

case INI:
  WrZ80(R->HL.W++,InZ80(R->BC.B.l));
  R->BC.B.h--;
  R->AF.B.l=N_FLAG|(R->BC.B.h? 0:Z_FLAG);
  break;

case INIR:
  do
  {
    WrZ80(R->HL.W++,InZ80(R->BC.B.l));
    R->BC.B.h--;R->ICount-=21;
  }
  while(R->BC.B.h&&(R->ICount>0));
  if(R->BC.B.h) { R->AF.B.l=N_FLAG;R->PC.W-=2;mz80_cache_ip(R); }
  else { R->AF.B.l=Z_FLAG|N_FLAG;R->ICount+=5; }
  break;

case IND:
  WrZ80(R->HL.W--,InZ80(R->BC.B.l));
  R->BC.B.h--;
  R->AF.B.l=N_FLAG|(R->BC.B.h? 0:Z_FLAG);
  break;

case INDR:
  do
  {
    WrZ80(R->HL.W--,InZ80(R->BC.B.l));
    R->BC.B.h--;R->ICount-=21;
  }
  while(R->BC.B.h&&(R->ICount>0));
  if(R->BC.B.h) { R->AF.B.l=N_FLAG;R->PC.W-=2;mz80_cache_ip(R); }
  else { R->AF.B.l=Z_FLAG|N_FLAG;R->ICount+=5; }
  break;

case OUTI:
  OutZ80(R->BC.B.l,RdZ80(R->HL.W++));
  R->BC.B.h--;
  R->AF.B.l=N_FLAG|(R->BC.B.h? 0:Z_FLAG);
  break;

case OTIR:
  do
  {
    OutZ80(R->BC.B.l,RdZ80(R->HL.W++));
    R->BC.B.h--;R->ICount-=21;
  }
  while(R->BC.B.h&&(R->ICount>0));
  if(R->BC.B.h) { R->AF.B.l=N_FLAG;R->PC.W-=2;mz80_cache_ip(R); }
  else { R->AF.B.l=Z_FLAG|N_FLAG;R->ICount+=5; }
  break;

case OUTD:
  OutZ80(R->BC.B.l,RdZ80(R->HL.W--));
  R->BC.B.h--;
  R->AF.B.l=N_FLAG|(R->BC.B.h? 0:Z_FLAG);
  break;

case OTDR:
  do
  {
    OutZ80(R->BC.B.l,RdZ80(R->HL.W--));
    R->BC.B.h--;R->ICount-=21;
  }
  while(R->BC.B.h&&(R->ICount>0));
  if(R->BC.B.h) { R->AF.B.l=N_FLAG;R->PC.W-=2;mz80_cache_ip(R); }
  else { R->AF.B.l=Z_FLAG|N_FLAG;R->ICount+=5; }
  break;

case LDI:
  WrZ80(R->DE.W++,RdZ80(R->HL.W++));
  R->BC.W--;
  R->AF.B.l=(R->AF.B.l&~(N_FLAG|H_FLAG|P_FLAG))|(R->BC.W? P_FLAG:0);
  break;

case LDIR:
  do
  {
    WrZ80(R->DE.W++,RdZ80(R->HL.W++));
    R->BC.W--;R->ICount-=21;
  }
  while(R->BC.W&&(R->ICount>0));
  R->AF.B.l&=~(N_FLAG|H_FLAG|P_FLAG);
  if(R->BC.W) { R->AF.B.l|=N_FLAG;R->PC.W-=2;mz80_cache_ip(R); }
  else R->ICount+=5;
  break;

case LDD:
  WrZ80(R->DE.W--,RdZ80(R->HL.W--));
  R->BC.W--;
  R->AF.B.l=(R->AF.B.l&~(N_FLAG|H_FLAG|P_FLAG))|(R->BC.W? P_FLAG:0);
  break;

case LDDR:
  do
  {
    WrZ80(R->DE.W--,RdZ80(R->HL.W--));
    R->BC.W--;R->ICount-=21;
  }
  while(R->BC.W&&(R->ICount>0));
  R->AF.B.l&=~(N_FLAG|H_FLAG|P_FLAG);
  if(R->BC.W) { R->AF.B.l|=N_FLAG;R->PC.W-=2;mz80_cache_ip(R); }
  else R->ICount+=5;
  break;

case CPI:
  I=RdZ80(R->HL.W++);
  J.B.l=R->AF.B.h-I;
  R->BC.W--;
  R->AF.B.l =
    N_FLAG|(R->AF.B.l&C_FLAG)|ZSTable[J.B.l]|
    ((R->AF.B.h^I^J.B.l)&H_FLAG)|(R->BC.W? P_FLAG:0);
  break;

case CPIR:
  do
  {
    I=RdZ80(R->HL.W++);
    J.B.l=R->AF.B.h-I;
    R->BC.W--;R->ICount-=21;
  }  
  while(R->BC.W&&J.B.l&&(R->ICount>0));
  R->AF.B.l =
    N_FLAG|(R->AF.B.l&C_FLAG)|ZSTable[J.B.l]|
    ((R->AF.B.h^I^J.B.l)&H_FLAG)|(R->BC.W? P_FLAG:0);
  if(R->BC.W&&J.B.l) { R->PC.W-=2;mz80_cache_ip(R); } else R->ICount+=5;
  break;  

case CPD:
  I=RdZ80(R->HL.W--);
  J.B.l=R->AF.B.h-I;
  R->BC.W--;
  R->AF.B.l =
    N_FLAG|(R->AF.B.l&C_FLAG)|ZSTable[J.B.l]|
    ((R->AF.B.h^I^J.B.l)&H_FLAG)|(R->BC.W? P_FLAG:0);
  break;

case CPDR:
  do
  {
    I=RdZ80(R->HL.W--);
    J.B.l=R->AF.B.h-I;
    R->BC.W--;R->ICount-=21;
  }
  while(R->BC.W&&J.B.l);
  R->AF.B.l =
    N_FLAG|(R->AF.B.l&C_FLAG)|ZSTable[J.B.l]|
    ((R->AF.B.h^I^J.B.l)&H_FLAG)|(R->BC.W? P_FLAG:0);
  if(R->BC.W&&J.B.l) { R->PC.W-=2;mz80_cache_ip(R); } else R->ICount+=5;
  break;

/*
 * $Log: mz80opc4.h,v $
 * Revision 1.2  1999/11/08 01:33:53  nyef
 * added preliminary implementation of ranged mmu
 *
 * Revision 1.1  1999/01/03 02:27:45  nyef
 * Initial revision
 *
 */