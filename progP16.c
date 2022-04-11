/**
 * \file progP16.c - algorithms to program the PIC16 (14 bit word) family of microcontrollers
 * Copyright (C) 2009-2021 Alberto Maccioni
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 * or see <http://www.gnu.org/licenses/>
 */

#include "common.h"

struct ID16{
	int id;
	char *device;
	int revmask;
} PIC16LIST[]={
	{0x00A>>1,"16F72 rev%d\r\n",0x1F},			//00 0000 101x xxxx
	{0x046>>1,"12F683 rev%d\r\n",0x1F},			//00 0100 011x xxxx
	{0x04A>>1,"16F685 rev%d\r\n",0x1F},			//00 0100 101x xxxx
	{0x04C>>1,"16F818 rev%d\r\n",0x1F},			//00 0100 110x xxxx
	{0x04E>>1,"16F819 rev%d\r\n",0x1F},			//00 0100 111x xxxx
	{0x056>>1,"16F84A rev%d\r\n",0x1F}, 		//00 0101 011x xxxx
	{0x060>>1,"16F73 rev%d\r\n",0x1F},  		//00 0110 000x xxxx
	{0x062>>1,"16F74 rev%d\r\n",0x1F}, 	 		//00 0110 001x xxxx
	{0x064>>1,"16F76 rev%d\r\n",0x1F},			//00 0110 010x xxxx
	{0x066>>1,"16F77 rev%d\r\n",0x1F},			//00 0110 011x xxxx
	{0x072>>1,"16F87 rev%d\r\n",0x1F},			//00 0111 001x xxxx
	{0x076>>1,"16F88 rev%d\r\n",0x1F},			//00 0111 011x xxxx
	{0x07A>>1,"16F627 rev%d\r\n",0x1F},			//00 0111 101x xxxx
	{0x07C>>1,"16F628 rev%d\r\n",0x1F},			//00 0111 110x xxxx
	{0x08E>>1,"16F872 rev%d\r\n",0x1F},			//00 1000 111x xxxx
	{0x092>>1,"16F874 rev%d\r\n",0x1F},			//00 1001 001x xxxx
	{0x096>>1,"16F873 rev%d\r\n",0x1F},			//00 1001 011x xxxx
	{0x09A>>1,"16F877 rev%d\r\n",0x1F},			//00 1001 101x xxxx
	{0x09E>>1,"16F876 rev%d\r\n",0x1F},			//00 1001 111x xxxx
	{0x0BA>>1,"16F737 rev%d\r\n",0x1F},			//00 1011 101x xxxx
	{0x0BE>>1,"16F747 rev%d\r\n",0x1F},			//00 1011 111x xxxx
	{0x0D0>>1,"16F870 rev%d\r\n",0x1F},			//00 1101 000x xxxx
	{0x0D2>>1,"16F871 rev%d\r\n",0x1F},			//00 1101 001x xxxx
	{0x0DE>>1,"16F777 rev%d\r\n",0x1F},			//00 1101 111x xxxx
	{0x0E0>>1,"16F876A rev%d\r\n",0xF},			//00 1110 0000 xxxx
	{0x0E2>>1,"16F877A rev%d\r\n",0xF},			//00 1110 0010 xxxx
	{0x0E4>>1,"16F873A rev%d\r\n",0xF},			//00 1110 0100 xxxx
	{0x0E6>>1,"16F874A rev%d\r\n",0xF},			//00 1110 0110 xxxx
	{0x0EA>>1,"16F767 rev%d\r\n",0x1F},			//00 1110 101x xxxx
	{0x0F8>>1,"12F629 rev%d\r\n",0x1F},			//00 1111 100x xxxx
	{0x0FA>>1,"12F635 rev%d\r\n",0x1F},			//00 1111 101x xxxx
	{0x0FC>>1,"12F675 rev%d\r\n",0x1F},			//00 1111 110x xxxx
	{0x104>>1,"16F627A rev%d\r\n",0x1F},		//01 0000 010x xxxx
	{0x106>>1,"16F628A rev%d\r\n",0x1F},		//01 0000 011x xxxx
	{0x108>>1,"16F684 rev%d\r\n",0x1F},			//01 0000 100x xxxx
	{0x110>>1,"16F648A rev%d\r\n",0x1F},		//01 0001 000x xxxx
	{0x10A>>1,"16F636-639 rev%d\r\n",0x1F},		//01 0000 101x xxxx
	{0x10C>>1,"16F630 rev%d\r\n",0x1F},			//01 0000 110x xxxx
	{0x10E>>1,"16F676 rev%d\r\n",0x1F},			//01 0000 111x xxxx
	{0x114>>1,"16F716 rev%d\r\n",0x1F},			//01 0001 010x xxxx
	{0x118>>1,"16F688 rev%d\r\n",0x1F},			//01 0001 100x xxxx
	{0x120>>1,"16F785 rev%d\r\n",0x1F},			//01 0010 000x xxxx
	{0x122>>1,"16HV785 rev%d\r\n",0x1F},		//01 0010 001x xxxx
	{0x124>>1,"16F616 rev%d\r\n",0x1F},			//01 0010 010x xxxx
	{0x126>>1,"16HV616 rev%d\r\n",0x1F},		//01 0010 011x xxxx
	{0x132>>1,"16F687 rev%d\r\n",0x1F},			//01 0011 001x xxxx
	{0x134>>1,"16F689 rev%d\r\n",0x1F},			//01 0011 010x xxxx
	{0x138>>1,"16F917 rev%d\r\n",0xF},			//01 0011 1000 xxxx
	{0x13A>>1,"16F916 rev%d\r\n",0xF},			//01 0011 1010 xxxx
	{0x13C>>1,"16F914 rev%d\r\n",0xF},			//01 0011 1100 xxxx
	{0x13E>>1,"16F913 rev%d\r\n",0xF},			//01 0011 1110 xxxx
	{0x140>>1,"16F690 rev%d\r\n",0x1F},			//01 0100 000x xxxx
	{0x142>>1,"16F631 rev%d\r\n",0x1F},			//01 0100 001x xxxx
	{0x144>>1,"16F677 rev%d\r\n",0x1F},			//01 0100 010x xxxx
	{0x146>>1,"16F946 rev%d\r\n",0xF},			//01 0100 0110 xxxx
	{0x148>>1,"16F1847 rev%d\r\n",0x1F},		//01 0100 100x xxxx
	{0x14A>>1,"16LF1847 rev%d\r\n",0x1F},		//01 0100 101x xxxx
	{0x150>>1,"12F752 rev%d\r\n",0x1F},			//01 0101 000x xxxx
	{0x152>>1,"12HV752 rev%d\r\n",0x1F},		//01 0101 001x xxxx
	{0x158>>1,"16F1526 rev%d\r\n",0x1F},		//01 0101 100x xxxx
	{0x15A>>1,"16F1527 rev%d\r\n",0x1F},		//01 0101 101x xxxx
	{0x15C>>1,"16LF1526 rev%d\r\n",0x1F},		//01 0101 110x xxxx
	{0x15E>>1,"16LF1527 rev%d\r\n",0x1F},		//01 0101 111x xxxx
	{0x164>>1,"16F1513 rev%d\r\n",0x1F},		//01 0110 010x xxxx
	{0x168>>1,"16F1516 rev%d\r\n",0x1F},		//01 0110 100x xxxx
	{0x16A>>1,"16F1517 rev%d\r\n",0x1F},		//01 0110 101x xxxx
	{0x16C>>1,"16F1518 rev%d\r\n",0x1F},		//01 0110 110x xxxx
	{0x16E>>1,"16F1519 rev%d\r\n",0x1F},		//01 0110 111x xxxx
	{0x170>>1,"16F1512 rev%d\r\n",0x1F},		//01 0111 000x xxxx
	{0x172>>1,"16LF1512 rev%d\r\n",0x1F},		//01 0111 001x xxxx
	{0x174>>1,"16LF1513 rev%d\r\n",0x1F},		//01 0111 010x xxxx
	{0x178>>1,"16LF1516 rev%d\r\n",0x1F},		//01 0111 100x xxxx
	{0x17A>>1,"16LF1517 rev%d\r\n",0x1F},		//01 0111 101x xxxx
	{0x17C>>1,"16LF1518 rev%d\r\n",0x1F},		//01 0111 110x xxxx
	{0x17E>>1,"16LF1519 rev%d\r\n",0x1F},		//01 0111 111x xxxx
	{0x180>>1,"16F727 rev%d\r\n",0x1F},			//01 1000 000x xxxx
	{0x182>>1,"16F726 rev%d\r\n",0x1F},			//01 1000 001x xxxx
	{0x184>>1,"16F724 rev%d\r\n",0x1F},			//01 1000 010x xxxx
	{0x186>>1,"16F723 rev%d\r\n",0x1F},			//01 1000 011x xxxx
	{0x188>>1,"16F722 rev%d\r\n",0x1F},			//01 1000 100x xxxx
	{0x190>>1,"16LF727 rev%d\r\n",0x1F},		//01 1001 000x xxxx
	{0x192>>1,"16LF726 rev%d\r\n",0x1F},		//01 1001 001x xxxx
	{0x194>>1,"16LF724 rev%d\r\n",0x1F},		//01 1001 010x xxxx
	{0x196>>1,"16LF723 rev%d\r\n",0x1F},		//01 1001 011x xxxx
	{0x198>>1,"16LF722 rev%d\r\n",0x1F},		//01 1001 100x xxxx
	{0x1AC>>1,"16F707 rev%d\r\n",0x1F},			//01 1010 110x xxxx
	{0x1AE>>1,"16LF707 rev%d\r\n",0x1F},		//01 1010 111x xxxx
	{0x1B0>>1,"16F723A rev%d\r\n",0x1F},		//01 1011 000x xxxx
	{0x1B2>>1,"16F722A rev%d\r\n",0x1F},		//01 1011 001x xxxx
	{0x1B4>>1,"16LF723A rev%d\r\n",0x1F},		//01 1011 010x xxxx
	{0x1B6>>1,"16LF722A rev%d\r\n",0x1F},		//01 1011 011x xxxx
	{0x1B8>>1,"12F1840 rev%d\r\n",0x1F},		//01 1011 100x xxxx
	{0x1BA>>1,"12LF1840 rev%d\r\n",0x1F},		//01 1011 101x xxxx
	{0x1C0>>1,"16F720 rev%d\r\n",0x1F},			//01 1100 000x xxxx
	{0x1C2>>1,"16F721 rev%d\r\n",0x1F},			//01 1100 001x xxxx
	{0x1C4>>1,"16LF720 rev%d\r\n",0x1F},		//01 1100 010x xxxx
	{0x1C6>>1,"16LF721 rev%d\r\n",0x1F},		//01 1100 011x xxxx
	{0x200>>1,"16F882 rev%d\r\n",0x1F},			//10 0000 000x xxxx
	{0x202>>1,"16F883 rev%d\r\n",0x1F},			//10 0000 001x xxxx
	{0x204>>1,"16F884 rev%d\r\n",0x1F},			//10 0000 010x xxxx
	{0x206>>1,"16F886 rev%d\r\n",0x1F},			//10 0000 011x xxxx
	{0x208>>1,"16F887 rev%d\r\n",0x1F},			//10 0000 100x xxxx
	{0x218>>1,"12F615 rev%d\r\n",0x1F},			//10 0001 100x xxxx
	{0x21A>>1,"12HV615 rev%d\r\n",0x1F},		//10 0001 101x xxxx
	{0x224>>1,"12F609 rev%d\r\n",0x1F},			//10 0010 010x xxxx
	{0x226>>1,"16F610 rev%d\r\n",0x1F},			//10 0010 011x xxxx
	{0x228>>1,"12HV609 rev%d\r\n",0x1F},		//10 0010 100x xxxx
	{0x22A>>1,"16HV610 rev%d\r\n",0x1F},		//10 0010 101x xxxx
	{0x232>>1,"16F1933 rev%d\r\n",0x1F},		//10 0011 001x xxxx
	{0x234>>1,"16F1934 rev%d\r\n",0x1F},		//10 0011 010x xxxx
	{0x236>>1,"16F1936 rev%d\r\n",0x1F},		//10 0011 011x xxxx
	{0x238>>1,"16F1937 rev%d\r\n",0x1F},		//10 0011 100x xxxx
	{0x23A>>1,"16F1938 rev%d\r\n",0x1F},		//10 0011 101x xxxx
	{0x23C>>1,"16F1939 rev%d\r\n",0x1F},		//10 0011 110x xxxx
	{0x242>>1,"16LF1933 rev%d\r\n",0x1F},		//10 0100 001x xxxx
	{0x244>>1,"16LF1934 rev%d\r\n",0x1F},		//10 0100 010x xxxx
	{0x246>>1,"16LF1936 rev%d\r\n",0x1F},		//10 0100 011x xxxx
	{0x248>>1,"16LF1937 rev%d\r\n",0x1F},		//10 0100 100x xxxx
	{0x24A>>1,"16LF1938 rev%d\r\n",0x1F},		//10 0100 101x xxxx
	{0x24C>>1,"16LF1939 rev%d\r\n",0x1F},		//10 0100 110x xxxx
	{0x250>>1,"16F1946 rev%d\r\n",0x1F},		//10 0101 000x xxxx
	{0x252>>1,"16F1947 rev%d\r\n",0x1F},		//10 0101 001x xxxx
	{0x258>>1,"16LF1946 rev%d\r\n",0x1F},		//10 0101 100x xxxx
	{0x25A>>1,"16LF1947 rev%d\r\n",0x1F},		//10 0101 101x xxxx
	{0x270>>1,"12F1822 rev%d\r\n",0x1F},		//10 0111 000x xxxx
	{0x272>>1,"16F1823 rev%d\r\n",0x1F},		//10 0111 001x xxxx
	{0x274>>1,"16F1824 rev%d\r\n",0x1F},		//10 0111 010x xxxx
	{0x276>>1,"16F1825 rev%d\r\n",0x1F},		//10 0111 011x xxxx
	{0x278>>1,"16F1826 rev%d\r\n",0x1F},		//10 0111 100x xxxx
	{0x27A>>1,"16F1827 rev%d\r\n",0x1F},		//10 0111 101x xxxx
	{0x280>>1,"12LF1822 rev%d\r\n",0x1F},		//10 1000 000x xxxx
	{0x282>>1,"16LF1823 rev%d\r\n",0x1F},		//10 1000 001x xxxx
	{0x284>>1,"16LF1824 rev%d\r\n",0x1F},		//10 1000 010x xxxx
	{0x286>>1,"16LF1825 rev%d\r\n",0x1F},		//10 1000 011x xxxx
	{0x288>>1,"16LF1826 rev%d\r\n",0x1F},		//10 1000 100x xxxx
	{0x28A>>1,"16LF1827 rev%d\r\n",0x1F},		//10 1000 101x xxxx
	{0x298>>1,"10F322 rev%d\r\n",0x1F},			//10 1001 100x xxxx
	{0x29A>>1,"10F320 rev%d\r\n",0x1F},			//10 1001 101x xxxx
	{0x29C>>1,"10LF322 rev%d\r\n",0x1F},		//10 1001 110x xxxx
	{0x29E>>1,"10LF320 rev%d\r\n",0x1F},		//10 1001 111x xxxx
	{0x2A0>>1,"16F1782 rev%d\r\n",0x1F},		//10 1010 000x xxxx
	{0x2A2>>1,"16F1783 rev%d\r\n",0x1F},		//10 1010 001x xxxx
	{0x2A4>>1,"16F1784 rev%d\r\n",0x1F},		//10 1010 010x xxxx
	{0x2A6>>1,"16F1786 rev%d\r\n",0x1F},		//10 1010 011x xxxx
	{0x2A8>>1,"16F1787 rev%d\r\n",0x1F},		//10 1010 100x xxxx
	{0x2AA>>1,"16LF1782 rev%d\r\n",0x1F},		//10 1010 101x xxxx
	{0x2AC>>1,"16LF1783 rev%d\r\n",0x1F},		//10 1010 110x xxxx
	{0x2AE>>1,"16LF1784 rev%d\r\n",0x1F},		//10 1010 111x xxxx
	{0x2B0>>1,"16LF1786 rev%d\r\n",0x1F},		//10 1011 000x xxxx
	{0x2B2>>1,"16LF1787 rev%d\r\n",0x1F},		//10 1011 001x xxxx
	{0x2C0>>1,"16LF1903 rev%d\r\n",0x1F},		//10 1100 000x xxxx
	{0x2C2>>1,"16LF1902 rev%d\r\n",0x1F},		//10 1100 001x xxxx
	{0x2C4>>1,"16LF1907 rev%d\r\n",0x1F},		//10 1100 010x xxxx
	{0x2C6>>1,"16LF1906 rev%d\r\n",0x1F},		//10 1100 011x xxxx
	{0x2C8>>1,"16LF1904 rev%d\r\n",0x1F},		//10 1100 100x xxxx
	{0x2CC>>1,"12F1501 rev%d\r\n",0x1F},		//10 1100 110x xxxx
	{0x2CE>>1,"16F1503 rev%d\r\n",0x1F},		//10 1100 111x xxxx
	{0x2D0>>1,"16F1507 rev%d\r\n",0x1F},		//10 1101 000x xxxx
	{0x2D2>>1,"16F1508 rev%d\r\n",0x1F},		//10 1101 001x xxxx
	{0x2D4>>1,"16F1509 rev%d\r\n",0x1F},		//10 1101 010x xxxx
	{0x2D8>>1,"12LF1501 rev%d\r\n",0x1F},		//10 1101 100x xxxx
	{0x2DA>>1,"16LF1503 rev%d\r\n",0x1F},		//10 1101 101x xxxx
	{0x2DC>>1,"16LF1507 rev%d\r\n",0x1F},		//10 1101 110x xxxx
	{0x2DE>>1,"16LF1508 rev%d\r\n",0x1F},		//10 1101 111x xxxx
	{0x2E0>>1,"16LF1509 rev%d\r\n",0x1F},		//10 1110 000x xxxx
	{0x2F0>>1,"16LF1554 rev%d\r\n",0x1F},		//10 1111 000x xxxx
	{0x2F2>>1,"16LF1559 rev%d\r\n",0x1F},		//10 1111 001x xxxx
// New devices with DevID & DevRev in different locations:
	{0x3000,"16F1574\r\n",0},
	{0x3001,"16F1575\r\n",0},
	{0x3002,"16F1578\r\n",0},
	{0x3003,"16F1579\r\n",0},
	{0x3004,"16LF1574\r\n",0},
	{0x3005,"16LF1575\r\n",0},
	{0x3006,"16LF1578\r\n",0},
	{0x3007,"16LF1579\r\n",0},
	{0x3020,"16F1454\r\n",0},
	{0x3021,"16F1455\r\n",0},
	{0x3023,"16F1459\r\n",0},
	{0x3024,"16LF1454\r\n",0},
	{0x3025,"16LF1455\r\n",0},
	{0x3027,"16LF1459\r\n",0},
	{0x302A,"16F1789\r\n",0},
	{0x302B,"16F1788\r\n",0},
	{0x302C,"16LF1789\r\n",0},
	{0x302D,"16LF1788\r\n",0},
	{0x3030,"16F753\r\n",0},
	{0x3031,"16HV753\r\n",0},
	{0x303A,"16F18324\r\n",0},
	{0x303B,"16F18344\r\n",0},
	{0x303C,"16LF18324\r\n",0},
	{0x303D,"16LF18344\r\n",0},
	{0x303E,"16F18325\r\n",0},
	{0x303F,"16F18345\r\n",0},
	{0x3040,"16LF18325\r\n",0},
	{0x3041,"16LF18345\r\n",0},
	{0x3042,"16F1708\r\n",0},
	{0x3043,"16F1704\r\n",0},
	{0x3044,"16LF1708\r\n",0},
	{0x3045,"16LF1704\r\n",0},
	{0x3048,"16F1716\r\n",0},
	{0x3049,"16F1713\r\n",0},
	{0x304A,"16LF1716\r\n",0},
	{0x304B,"16LF1713\r\n",0},
	{0x304C,"16F1613\r\n",0},
	{0x304D,"16LF1613\r\n",0},
	{0x3050,"12F1572\r\n",0},
	{0x3051,"12F1571\r\n",0},
	{0x3052,"12LF1572\r\n",0},
	{0x3053,"12LF1571\r\n",0},
	{0x3054,"16F1709\r\n",0},
	{0x3055,"16F1705\r\n",0},
	{0x3056,"16LF1709\r\n",0},
	{0x3057,"16LF1705\r\n",0},
	{0x3058,"12F1612\r\n",0},
	{0x3059,"12LF1612\r\n",0},
	{0x305A,"16F1719\r\n",0},
	{0x305B,"16F1718\r\n",0},
	{0x305C,"16F1717\r\n",0},
	{0x305D,"16LF1719\r\n",0},
	{0x305E,"16LF1718\r\n",0},
	{0x305F,"16LF1717\r\n",0},
	{0x3060,"16F1707\r\n",0},
	{0x3061,"16F1703\r\n",0},
	{0x3062,"16LF1707\r\n",0},
	{0x3063,"16LF1703\r\n",0},
	{0x3066,"16F18313\r\n",0},
	{0x3067,"16F18323\r\n",0},
	{0x3068,"16LF18313\r\n",0},
	{0x3069,"16LF18323\r\n",0},
	{0x306A,"16F18854\r\n",0},
	{0x306B,"16LF18854\r\n",0},
	{0x306C,"16F18855\r\n",0},
	{0x306D,"16F18875\r\n",0},
	{0x306E,"16LF18855\r\n",0},
	{0x306F,"16LF18875\r\n",0},
	{0x3070,"16F18856\r\n",0},
	{0x3071,"16F18876\r\n",0},
	{0x3072,"16LF18856\r\n",0},
	{0x3073,"16LF18876\r\n",0},
	{0x3074,"16F18857\r\n",0},
	{0x3075,"16F18877\r\n",0},
	{0x3076,"16LF18857\r\n",0},
	{0x3077,"16LF18877\r\n",0},
	{0x3078,"16F1614\r\n",0},
	{0x3079,"16F1618\r\n",0},
	{0x307A,"16LF1614\r\n",0},
	{0x307B,"16LF1618\r\n",0},
	{0x307C,"16F1615\r\n",0},
	{0x307D,"16F1619\r\n",0},
	{0x307E,"16LF1615\r\n",0},
	{0x307F,"16LF1619\r\n",0},
	{0x3080,"16F1764\r\n",0},
	{0x3081,"16F1765\r\n",0},
	{0x3082,"16LF1764\r\n",0},
	{0x3083,"16LF1765\r\n",0},
	{0x3084,"16F1768\r\n",0},
	{0x3085,"16F1769\r\n",0},
	{0x3086,"16LF1768\r\n",0},
	{0x3087,"16LF1769\r\n",0},
	{0x308A,"16F1773\r\n",0},
	{0x308B,"16F1776\r\n",0},
	{0x308C,"16LF1773\r\n",0},
	{0x308D,"16LF1776\r\n",0},
	{0x308E,"16F1777\r\n",0},
	{0x308F,"16F1778\r\n",0},
	{0x3090,"16F1779\r\n",0},
	{0x3091,"16LF1777\r\n",0},
	{0x3092,"16LF1778\r\n",0},
	{0x3093,"16LF1779\r\n",0},
	{0x3096,"16F19155\r\n",0},
	{0x3097,"16LF19155\r\n",0},
	{0x3098,"16F19156\r\n",0},
	{0x3099,"16LF19156\r\n",0},
	{0x309A,"16F19175\r\n",0},
	{0x309B,"16LF19175\r\n",0},
	{0x309C,"16F19176\r\n",0},
	{0x309D,"16LF19176\r\n",0},
	{0x309E,"16F19195\r\n",0},
	{0x309F,"16LF19195\r\n",0},
	{0x30A0,"16F19196\r\n",0},
	{0x30A1,"16LF19196\r\n",0},
	{0x30A2,"16F19197\r\n",0},
	{0x30A3,"16LF19197\r\n",0},
	{0x30A4,"16F18326\r\n",0},
	{0x30A5,"16F18346\r\n",0},
	{0x30A6,"16LF18326\r\n",0},
	{0x30A7,"16LF18346\r\n",0},
	{0x30AC,"16F15354\r\n",0},
	{0x30AD,"16LF15354\r\n",0},
	{0x30AE,"16F15355\r\n",0},
	{0x30AF,"16LF15355\r\n",0},
	{0x30B0,"16F15356\r\n",0},
	{0x30B1,"16LF15356\r\n",0},
	{0x30B2,"16F15375\r\n",0},
	{0x30B3,"16LF15375\r\n",0},
	{0x30B4,"16F15376\r\n",0},
	{0x30B5,"16LF15376\r\n",0},
	{0x30B6,"16F15385\r\n",0},
	{0x30B7,"16LF15385\r\n",0},
	{0x30B8,"16F15386\r\n",0},
	{0x30B9,"16LF15386\r\n",0},
	{0x30BE,"16F15313\r\n",0},
	{0x30BF,"16LF15313\r\n",0},
	{0x30BA,"16F19185\r\n",0},
	{0x30BB,"16LF19185\r\n",0},
	{0x30BC,"16F19186\r\n",0},
	{0x30BD,"16LF19186\r\n",0},
	{0x30C0,"16F15323\r\n",0},
	{0x30C1,"16LF15323\r\n",0},
	{0x30C2,"16F15324\r\n",0},
	{0x30C3,"16LF15324\r\n",0},
	{0x30C4,"16F15344\r\n",0},
	{0x30C5,"16LF15344\r\n",0},
	{0x30C6,"16F15325\r\n",0},
	{0x30C7,"16LF15325\r\n",0},
	{0x30C8,"16F15345\r\n",0},
	{0x30C9,"16LF15345\r\n",0},
	{0x30CA,"16F18424\r\n",0},
	{0x30CB,"16LF18424\r\n",0},
	{0x30CC,"16F18425\r\n",0},
	{0x30CD,"16LF18425\r\n",0},
	{0x30CE,"16F18444\r\n",0},
	{0x30CF,"16LF18444\r\n",0},
	{0x30D0,"16F18445\r\n",0},
	{0x30D1,"16LF18445\r\n",0},
	{0x30D2,"16F18426\r\n",0},
	{0x30D3,"16LF18426\r\n",0},
	{0x30D4,"16F18446\r\n",0},
	{0x30D5,"16LF18446\r\n",0},
	{0x30D7,"16F18455\r\n",0},
	{0x30D8,"16LF18455\r\n",0},
	{0x30D9,"16F18456\r\n",0},
	{0x30DA,"16LF18456\r\n",0}
};

void PIC16_ID(int id)
{
	char s[64];
	int i;
	for(i=0;i<sizeof(PIC16LIST)/sizeof(PIC16LIST[0]);i++){
		if(PIC16LIST[i].revmask&&(id>>5)==PIC16LIST[i].id){		//id + rev in same location
			sprintf(s,PIC16LIST[i].device,id&PIC16LIST[i].revmask);
			PrintMessage(s);
			return;
		}
		else if(!PIC16LIST[i].revmask&&id==PIC16LIST[i].id){		//id separate from rev
			sprintf(s,PIC16LIST[i].device);
			PrintMessage(s);
			return;
		}
	}
	sprintf(s,"%s",strings[S_nodev]); //"Unknown device\r\n");
	PrintMessage(s);
}

void DisplayCODE16F(int size){
// display 14 bit PIC CODE memory
	char s[256]="",t[256]="";
	char* aux=(char*)malloc((size/COL+1)*(16+COL*5));
	aux[0]=0;
	int valid=0,empty=1,i,j,lines=0;
	for(i=0;i<size&&i<sizeW;i+=COL){
		valid=0;
		for(j=i;j<i+COL&&j<sizeW&&i<size;j++){
			sprintf(t,"%04X ",memCODE_W[j]);
			strcat(s,t);
			if((memCODE_W[j]&0x3fff)<0x3fff) valid=1;
		}
		if(valid){
			sprintf(t,"%04X: %s\r\n",i,s);
			empty=0;
			strcat(aux,t);
			lines++;
			if(lines>500){	//limit number of lines printed
				strcat(aux,"(...)\r\n");
				i=(sizeW<size?sizeW:size)-COL*2;
				lines=490;
			}
		}
		s[0]=0;
	}
	if(empty) PrintMessage(strings[S_Empty]);	//empty
	else PrintMessage(aux);
	free(aux);
}

void DisplayEE16F(int size){
// display 14 bit PIC EEPROM memory
	int valid=0,empty=1,i,j;
	char s[256]="",t[256]="",v[256]="";
	char* aux=(char*)malloc((size/COL+1)*(16+COL*5));
	aux[0]=0;
#ifdef __GTK_H__
	char *g;
#endif
	empty=1;
	v[0]=0;
	for(i=0x2100;i<0x2100+size&&i<sizeW;i+=COL){
		valid=0;
		for(j=i;j<i+COL&&j<0x2100+size&&j<sizeW;j++){
			sprintf(t,"%02X ",memCODE_W[j]&0xff);
			strcat(s,t);
			sprintf(t,"%c",isprint(memCODE_W[j]&0xFF)&&(memCODE_W[j]&0xFF)<0xFF?memCODE_W[j]&0xFF:'.');
#ifdef __GTK_H__
			g=g_locale_to_utf8(t,-1,NULL,NULL,NULL);
			if(g) strcat(v,g);
			g_free(g);
#else
			strcat(v,t);
#endif
			if((memCODE_W[j]&0xFF)<0xff) valid=1;/**/
		}
		if(valid){
			sprintf(t,"%04X: %s %s\r\n",i,s,v);
			empty=0;
			strcat(aux,t);
		}
		s[0]=0;
		v[0]=0;
	}
	if(empty) PrintMessage(strings[S_Empty]);	//empty
	else PrintMessage(aux);
	free(aux);
}

void Read16Fxxx(int dim,int dim2,int dim3,int vdd)
// read 14 bit PIC
// dim=program size 	dim2=eeprom size   dim3=config size
// dim2<0 -> eeprom @ 0x2200
// vdd=0 -> vpp before vdd
// vdd=1 -> vdd (+50ms) before vpp
// vdd=2 -> vdd before vpp
// DevREV@0x2005
// DevID@0x2006
// Config@0x2007
// Calib1/Config2@0x2008
// Calib2/Calib1@0x2009
// eeprom@0x2100
{
	int k=0,k2=0,z=0,i,j,ee2200=0;
	char s[512],t[256],*aux;
	if(dim2<0){
		dim2=-dim2;
		ee2200=1;
	}
	if(dim>0x2000||dim<0){
		PrintMessage(strings[S_CodeLim]);	//"Code size exceeds limits\r\n"
		return;
	}
	if(dim2>0x400||dim2<0){		//Max 1K
		PrintMessage(strings[S_EELim]);	//"EEPROM size exceeds limits\r\n"
		return;
	}
	if(dim3>0x100||dim3<0){
		PrintMessage(strings[S_ConfigLim]);	//"Config area size exceeds limits\r\n"
		return;
	}
	if(dim3<8)dim3=8;
	if(saveLog){
		OpenLogFile();	//"log.txt"
		fprintf(logfile,"Read16Fxxx(%d,%d,%d,%d)\n",dim,dim2,dim3,vdd);
	}
	sizeW=0x2100+dim2;
	if(memCODE_W) free(memCODE_W);
	memCODE_W=(WORD*)malloc(sizeof(WORD)*sizeW);
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	if(vdd==0){						//VPP before VDD
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=4;				//VPP
		bufferU[j++]=NOP;
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=0x5;			//VDD+VPP
	}
	else if(vdd==1){				//VDD before VPP with delay 50ms
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=1;				//VDD
		bufferU[j++]=SET_PARAMETER;
		bufferU[j++]=SET_T3;
		bufferU[j++]=25000>>8;
		bufferU[j++]=25000&0xff;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=SET_PARAMETER;
		bufferU[j++]=SET_T3;
		bufferU[j++]=2000>>8;
		bufferU[j++]=2000&0xff;
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=0x5;			//VDD+VPP
	}
	else if(vdd==2){				//VDD before VPP without delay
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=1;				//VDD
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=0x5;			//VDD+VPP
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(vdd?55:5);
//****************** read code ********************
	PrintMessage(strings[S_CodeReading1]);		//read code ...
	if(saveLog) fprintf(logfile,"%s\n",strings[S_CodeReading1]);		//read code ...
	PrintStatusSetup();
	for(i=0,j=0;i<dim;i++){
		bufferU[j++]=READ_DATA_PROG;
		bufferU[j++]=INC_ADDR;
		if(j>DIMBUF*2/4-3||i==dim-1){		//2B cmd -> 4B data
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(5);
			for(z=0;z<DIMBUF-2;z++){
				if(bufferI[z]==READ_DATA_PROG){
					memCODE_W[k++]=(bufferI[z+1]<<8)+bufferI[z+2];
					z+=2;
				}
			}
			PrintStatus(strings[S_CodeReading],i*100/(dim+dim2+dim3),i);	//"Read: %d%%, addr. %03X"
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
			}
		}
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadCodeErr],dim,k);	//"Error reading code area, requested %d words, read %d\r\n"
	}
	else PrintMessage(strings[S_Compl]);
	for(i=k;i<0x2000;i++) memCODE_W[i]=0x3fff;
//****************** read config area ********************
	PrintMessage(strings[S_Read_CONFIG_A]);		//read config ...
	if(saveLog) fprintf(logfile,"%s\n",strings[S_Read_CONFIG_A]);		//read config ...
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	for(i=0x2000;i<0x2000+dim3;i++){		//Config
		bufferU[j++]=READ_DATA_PROG;
		bufferU[j++]=INC_ADDR;
		if(j>DIMBUF*2/4-3||i==0x2000+dim3-1){		//2B cmd -> 4B data
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(5);
			for(z=0;z<DIMBUF-2;z++){
				if(bufferI[z]==READ_DATA_PROG){
					memCODE_W[0x2000+k2++]=(bufferI[z+1]<<8)+bufferI[z+2];
					z+=2;
				}
			}
			PrintStatus(strings[S_CodeReading],(i-0x2000+dim)*100/(dim+dim2+dim3),i);	//"Read: %d%%, addr. %03X"
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,k2,k2);	//"i=%d(0x%X), k=%d(0x%X)\n"
			}
		}
	}
	if(k2!=dim3){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ConfigErr],dim3,k2);	//"Error reading config area, requested %d words, read %d\r\n"
	}
	else PrintMessage(strings[S_Compl]);
	for(i=0x2000+k2;i<0x2000+dim3;i++) memCODE_W[i]=0x3fff;
//****************** read eeprom ********************
	if(dim2){
		PrintMessage(strings[S_ReadEE]);		//Read EEPROM ...
		if(saveLog) fprintf(logfile,"%s\n",strings[S_ReadEE]);		//Read EEPROM ...
		PrintStatusSetup();
		if(ee2200){		//eeprom a 0x2200
			bufferU[j++]=INC_ADDR_N;
			bufferU[j++]=0xFF;
			bufferU[j++]=INC_ADDR;
		}
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=0x100-dim3;
		for(k2=0,i=0x2100;i<0x2100+dim2;i++){
			bufferU[j++]=READ_DATA_DATA;
			bufferU[j++]=INC_ADDR;
			if(j>DIMBUF*2/3-3||i==0x2100+dim2-1){		//2B cmd -> 3B data
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(8);
				for(z=0;z<DIMBUF-1;z++){
					if(bufferI[z]==READ_DATA_DATA){
						memCODE_W[0x2100+k2++]=bufferI[z+1];
						z++;
					}
				}
				PrintStatus(strings[S_CodeReading],(i-0x2100+dim)*100/(dim+dim2+dim3),i);	//"Read: %d%%, addr. %03X"
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,k2,k2);	//"i=%d(0x%X), k=%d(0x%X)\n"
				}
			}
		}
		PrintStatusEnd();
		if(k2!=dim2){
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ReadEEErr],dim2,k2);	//"Error reading EE area, ..."
			for(i=0x2100+k2;i<0x2100+dim2;i++) memCODE_W[i]=0x3fff;
		}
		else PrintMessage(strings[S_Compl]);
	}
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=1;					//VDD
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();			//clear status report
//****************** visualize ********************
	for(i=0;i<4;i+=2){
		PrintMessage4("ID%d: 0x%04X\tID%d: 0x%04X\r\n",i,memCODE_W[0x2000+i],i+1,memCODE_W[0x2000+i+1]);
	}
	PrintMessage1(strings[S_DevID],memCODE_W[0x2006]);	//"DevID: 0x%04X\r\n"
	if(memCODE_W[0x2005]<0x3FFF) PrintMessage1(strings[S_DevREV],memCODE_W[0x2005]);	//"DevREV: 0x%04X\r\n"
	PIC16_ID(memCODE_W[0x2006]);
	PrintMessage1(strings[S_ConfigWord],memCODE_W[0x2007]);	//"Configuration word: 0x%04X\r\n"
	if(dim3>8){
		PrintMessage1(strings[S_Config2Cal1],memCODE_W[0x2008]);	//"Config2 or Cal1: 0x%04X\r\n"
	}
	if(dim3>9){
		PrintMessage1(strings[S_Calib1_2],memCODE_W[0x2009]);	//"Calibration word 1 or 2: 0x%04X\r\n"
	}
	PrintMessage(strings[S_CodeMem2]);	//"\r\nCode memory\r\n"
	DisplayCODE16F(dim);
	s[0]=0;
	int valid=0,empty=1;
	if(dim3>8){
		aux=(char*)malloc((dim3/COL+1)*(16+COL*5));
		aux[0]=0;
		empty=1;
		PrintMessage(strings[S_ConfigResMem]);	//"\r\nConfig and reserved memory:\r\n"
		for(i=0x2000;i<0x2000+dim3;i+=COL){
			valid=0;
			for(j=i;j<i+COL&&j<0x2000+dim3;j++){
				sprintf(t,"%04X ",memCODE_W[j]);
				strcat(s,t);
				if(memCODE_W[j]<0x3fff) valid=1;
			}
			if(valid){
				snprintf(t,sizeof(t),"%04X: %s\r\n",i,s);
				empty=0;
				strcat(aux,t);
			}
			s[0]=0;
		}
		if(empty) PrintMessage(strings[S_Empty]);	//empty
		else PrintMessage(aux);
		free(aux);
	}
	if(dim2){
		PrintMessage(strings[S_EEMem]);	//"\r\nEEPROM memory:\r\n"
		DisplayEE16F(dim2);
	}
	sprintf(str,strings[S_End],(stop-start)/1000.0);	//"\r\nEnd (%.2f s)\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

void Read16F1xxx(int dim,int dim2,int dim3,int options)
// read 14 bit enhanced PIC
// dim=program size (up to 0x8000)
// dim2=eeprom size (up to 0x400)
// dim3=config area size @0x8000
// options:
//		bit0=0 -> vpp before vdd
//		bit0=1 -> vdd before vpp
//		bit1=1 -> LVP programming
//		bit2=1 -> Config3@0x8009
//		bit3=1 -> Config4@0x800A
//		bit4=1 -> PIC16F18xxx (calib words @0xE000 + new commands)
// DevREV@0x8005     DevID@0x8006
// Config1@0x8007    Config2@0x8008
// Config3@0x8009    Config4@0x800A
// Calib1@0x8009/A   Calib2@0x800A/B    Calib3@0x800B/C
// eeprom@0x0 or 0xF000
{
	int k=0,k2=0,z=0,i,j;
	int F18x=options&16;
	if(F18x&&FWVersion<0xA00){		//only for 16F18xxx
		PrintMessage1(strings[S_FWver2old],"0.10.0");	//"This firmware is too old. Version %s is required\r\n"
		return;
	}
	if(!CheckV33Regulator()){
		PrintMessage(strings[S_noV33reg]);	//Can't find 3.3V expansion board
		return;
	}
	char s[256],t[256],*aux;
	if(dim>0x8000||dim<0){
		PrintMessage(strings[S_CodeLim]);	//"Code size exceeds limits\r\n"
		return;
	}
	if(dim2>0x400||dim2<0){		//Max 1K
		PrintMessage(strings[S_EELim]);	//"EEPROM size exceeds limits\r\n"
		return;
	}
	if(dim3>0x200||dim3<0){
		PrintMessage(strings[S_ConfigLim]);	//"Config area size exceeds limits\r\n"
		return;
	}
	if(dim3<11)dim3=11;		//at least config1-2 + calib1-2
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"Read16F1xxx(%d,%d,%d,%d)\n",dim,dim2,dim3,options);
	}
	if((options&2)==0){				//HV entry
		if(!StartHVReg(8.5)){
			PrintMessage(strings[S_HVregErr]); //"HV regulator error\r\n"
			return;
		}
		else hvreg=8.5;
	}
	else StartHVReg(-1);			//LVP mode, turn off HV
	sizeW=0x8200;
	if(F18x) sizeW=0xE004;
	sizeEE=dim2;
	if(memCODE_W) free(memCODE_W);
	memCODE_W=(WORD*)malloc(sizeof(WORD)*sizeW);
	for(i=0;i<sizeW;i++) memCODE_W[i]=0x3fff;
	if(memEE) free(memEE);
	memEE=(unsigned char*)malloc(sizeEE);			//EEPROM
	for(i=0;i<sizeEE;i++) memEE[i]=0xFF;
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	if((options&2)==0){				//HV entry
		if((options&1)==0){				//VPP before VDD
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=4;				//VPP
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=0x5;			//VDD+VPP
	}
		else{							//VDD before VPP without delay
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=1;				//VDD
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=0x5;			//VDD+VPP
	}
	}
	else{			//Low voltage programming
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=4;				//VPP
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=TX16;			//0000 1010 0001 0010 1100 0010 1011 0010 = 0A12C2B2
		bufferU[j++]=2;
		bufferU[j++]=0x0A;
		bufferU[j++]=0x12;
		bufferU[j++]=0xC2;
		bufferU[j++]=0xB2;
		bufferU[j++]=SET_CK_D;		//Clock pulse
		bufferU[j++]=0x4;
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x0;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
//****************** read code ********************
	PrintMessage(strings[S_CodeReading1]);		//read code ...
	if(saveLog) fprintf(logfile,"%s\n",strings[S_CodeReading1]);		//read code ...
	PrintStatusSetup();
	for(i=0,j=0;i<dim;i++){
		bufferU[j++]=READ_DATA_PROG;
		bufferU[j++]=INC_ADDR;
		if(j>DIMBUF*2/4-3||i==dim-1){		//2B cmd -> 4B data
			bufferU[j++]=FLUSH;				//remember: FLUSH generates a response even empty!
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(5);
			for(z=0;z<DIMBUF-2;z++){
				if(bufferI[z]==READ_DATA_PROG){
					memCODE_W[k++]=(bufferI[z+1]<<8)+bufferI[z+2];
					z+=2;
				}
			}
			PrintStatus(strings[S_CodeReading],i*100/(dim+dim2+dim3),i);	//"Read: %d%%, addr. %03X"
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
			}
		}
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadCodeErr],dim,k);	//"Error reading code area, requested %d words, read %d\r\n"
	}
	else PrintMessage(strings[S_Compl]);
//****************** read config area ********************
	PrintMessage(strings[S_Read_CONFIG_A]);		//read config ...
	if(saveLog) fprintf(logfile,"%s\n",strings[S_Read_CONFIG_A]);		//read config ...
	bufferU[j++]=LOAD_CONF;			//counter at 0x8000
	bufferU[j++]=0xFF;
	bufferU[j++]=0xFF;
	for(i=0x8000;i<0x8000+dim3;i++){		//Config
		bufferU[j++]=READ_DATA_PROG;
		bufferU[j++]=INC_ADDR;
		if(j>DIMBUF*2/4-3||i==0x8000+dim3-1){		//2B cmd -> 4B data
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(5);
			for(z=0;z<DIMBUF-2;z++){
				if(bufferI[z]==READ_DATA_PROG){
					memCODE_W[0x8000+k2++]=(bufferI[z+1]<<8)+bufferI[z+2];
					z+=2;
				}
			}
			PrintStatus(strings[S_CodeReading],(i-0x8000+dim)*100/(dim+dim2+dim3),i);	//"Read: %d%%, addr %03X"
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,k2,k2);	//"i=%d(0x%X), k=%d(0x%X)\n"
			}
		}
	}
	if(k2!=dim3){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ConfigErr],dim3,k2);	//"Error reading config area, requested %d words, read %d\r\n"
	}
	else PrintMessage(strings[S_Compl]);
//	for(i=0x8000+k2;i<0x8000+dim3;i++) memCODE_W[i]=0x3fff;
//****************** read calibration area ********************
	if(F18x){
		k2=0;
		PrintMessage(strings[S_Read_CONFIG_A]);		//read config ...
		bufferU[j++]=LOAD_PC;			//counter at 0xE000
		bufferU[j++]=0xE0;
		bufferU[j++]=0x00;
		for(i=0;i<4;i++) bufferU[j++]=READ_DATA_INC;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(5);
		for(z=0;z<DIMBUF-2;z++){
			if(bufferI[z]==READ_DATA_INC){
				memCODE_W[0xE000+k2++]=(bufferI[z+1]<<8)+bufferI[z+2];
				z+=2;
			}
		}
		j=0;
		if(saveLog){
			fprintf(logfile,"Calibration area @0xE000\n");
		}
		if(k2!=4){
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ConfigErr],dim3,k2);	//"Error reading config area, requested %d words, read %d\r\n"
		}
		else PrintMessage(strings[S_Compl]);
	}
//****************** read eeprom ********************
	if(dim2&&!F18x){					//16F1xxx
		PrintMessage(strings[S_ReadEE]);		//Read EEPROM ...
		if(saveLog) fprintf(logfile,"%s\n",strings[S_ReadEE]);		//Read EEPROM ...
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x16;		//Reset address
		for(i=k=0;i<dim2;i++){
			bufferU[j++]=READ_DATA_DATA;
			bufferU[j++]=INC_ADDR;
			if(j>DIMBUF*2/3-3||i==dim2-1){		//2B cmd -> 3B data
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(5);
				for(z=0;z<DIMBUF-1;z++){
					if(bufferI[z]==READ_DATA_DATA){
						memEE[k++]=bufferI[z+1];
						z++;
					}
				}
				PrintStatus(strings[S_CodeReading],i*100/(dim+dim2+dim3),i);	//"Read: %d%%, addr %03X"
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
				}
			}
		}
		PrintStatusEnd();
		if(i!=dim2){
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ReadEEErr],dim2,i);	//"Error reading EE area, ..."
			for(;i<dim2;i++) memEE[i]=0xff;
		}
		else PrintMessage(strings[S_Compl]);
	}
	else if(dim2&&F18x){					//16F18xxx
		PrintMessage(strings[S_ReadEE]);		//Read EEPROM ...
		if(saveLog) fprintf(logfile,"%s\n",strings[S_ReadEE]);		//Read EEPROM ...
		bufferU[j++]=LOAD_PC;				//counter at 0xF000
		bufferU[j++]=0xF0;
		bufferU[j++]=0x00;
		for(i=k=0;i<dim2;i++){
			bufferU[j++]=READ_DATA_INC;
			if(j>DIMBUF/3-3||i==dim2-1){		//1B cmd -> 3B answer
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(5);
				for(z=0;z<DIMBUF-1;z++){
					if(bufferI[z]==READ_DATA_INC){
						memEE[k++]=bufferI[z+2];	//LSB
						z+=2;
					}
				}
				PrintStatus(strings[S_CodeReading],i*100/(dim+dim2+dim3),i);	//"Read: %d%%, addr %03X"
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
				}
			}
		}
		PrintStatusEnd();
		if(i!=dim2){
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ReadEEErr],dim2,i);	//"Error reading EE area, ..."
			for(;i<dim2;i++) memEE[i]=0xff;
		}
		else PrintMessage(strings[S_Compl]);
	}
/***********************************/
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=1;					//VDD
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();			//clear status report
//****************** visualize ********************
	for(i=0;i<4;i+=2){
		PrintMessage4("ID%d: 0x%04X\tID%d: 0x%04X\r\n",i,memCODE_W[0x8000+i],i+1,memCODE_W[0x8000+i+1]);
	}
	PrintMessage1(strings[S_DevID],memCODE_W[0x8006]);	//"DevID: 0x%04X\r\n"
	if(memCODE_W[0x8005]<0x3FFF) PrintMessage1(strings[S_DevREV],memCODE_W[0x8005]);	//"DevREV: 0x%04X\r\n"
	PIC16_ID(memCODE_W[0x8006]);
	PrintMessage2(strings[S_ConfigWordX],1,memCODE_W[0x8007]);	//"Configuration word %d: 0x%04X\r\n"
	PrintMessage2(strings[S_ConfigWordX],2,memCODE_W[0x8008]);	//"Configuration word %d: 0x%04X\r\n"
	if((options&0xC)==0&&F18x==0){	//2 config + 2/3 calib words
		PrintMessage2(strings[S_CalibWordX],1,memCODE_W[0x8009]);	//"Calibration word %d: 0x%04X\r\n"
		PrintMessage2(strings[S_CalibWordX],2,memCODE_W[0x800A]);	//"Calibration word %d: 0x%04X\r\n"
		if(memCODE_W[0x800B]<0x3FFF) PrintMessage2(strings[S_CalibWordX],3,memCODE_W[0x800B]);	//"Calibration word %d: 0x%04X\r\n"
	}
	else if((options&0xC)==4&&F18x==0){	//3 config + 3 calib words
		PrintMessage2(strings[S_ConfigWordX],3,memCODE_W[0x8009]);	//"Configuration word %d: 0x%04X\r\n"
		PrintMessage2(strings[S_CalibWordX],1,memCODE_W[0x800A]);	//"Calibration word %d: 0x%04X\r\n"
		PrintMessage2(strings[S_CalibWordX],2,memCODE_W[0x800B]);	//"Calibration word %d: 0x%04X\r\n"
		PrintMessage2(strings[S_CalibWordX],3,memCODE_W[0x800C]);	//"Calibration word %d: 0x%04X\r\n"
	}
	else if((options&0xC)==8){		//4 config
		PrintMessage2(strings[S_ConfigWordX],3,memCODE_W[0x8009]);	//"Configuration word %d: 0x%04X\r\n"
		PrintMessage2(strings[S_ConfigWordX],4,memCODE_W[0x800A]);	//"Configuration word %d: 0x%04X\r\n"
	}
	if(F18x){			//4 calib words @0xE000
		PrintMessage2(strings[S_CalibWordX],1,memCODE_W[0xE000]);	//"Calibration word %d: 0x%04X\r\n"
		PrintMessage2(strings[S_CalibWordX],2,memCODE_W[0xE001]);	//"Calibration word %d: 0x%04X\r\n"
		PrintMessage2(strings[S_CalibWordX],3,memCODE_W[0xE002]);	//"Calibration word %d: 0x%04X\r\n"
		PrintMessage2(strings[S_CalibWordX],4,memCODE_W[0xE003]);	//"Calibration word %d: 0x%04X\r\n"
	}
	PrintMessage(strings[S_CodeMem2]);	//"\r\nCode memory:\r\n"
	DisplayCODE16F(dim);
	if(dim3>15){
		int valid=0,empty=1;
		s[0]=0;
		aux=(char*)malloc(dim3/COL*(16+COL*5));
		aux[0]=0;
		empty=1;
		PrintMessage(strings[S_ConfigResMem]);	//"\r\nConfig and reserved memory:\r\n"
		for(i=0x8000;i<0x8000+dim3;i+=COL){
			valid=0;
			for(j=i;j<i+COL&&j<0x8000+dim3;j++){
				sprintf(t,"%04X ",memCODE_W[j]);
				strcat(s,t);
				if(memCODE_W[j]<0x3fff) valid=1;
			}
			if(valid){
				sprintf(t,"%04X: %s\r\n",i,s);
				empty=0;
				strcat(aux,t);
			}
			s[0]=0;
		}
		if(empty) PrintMessage(strings[S_Empty]);	//empty
		else PrintMessage(aux);
		free(aux);
	}
	if(dim2) DisplayEE();	//visualize
	sprintf(str,strings[S_End],(stop-start)/1000.0);	//"\r\nEnd (%.2f s)\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

#define LOAD_PC_ADDR 0x80
#define BULK_ERASE_PROGRAM_MEM 0x18
#define ROW_ERASE_PROGRAM_MEM 0xF0
#define LOAD_NVM 0x00
#define LOAD_NVM_INC 0x02
#define READ_NVM 0xFC
#define READ_NVM_INC 0xFE
#define INC_ADDR8 0xF8
#define BEGIN_INT_PROG 0xE0
#define BEGIN_EXT_PROG 0xC0
#define END_EXT_PROG 0x82

void Read16F18xxx(int dim,int dim2,int dim3,int options)
// read 14 bit enhanced PIC with new 8b commands
// dim=program size (up to 0x8000)
// dim2=if>0 use eeprom, size is automatic when reading DCI
// dim3=not used
// options:
//		bit0=0 -> vpp before vdd
//		bit0=1 -> vdd before vpp
//		bit1=1 -> LVP programming
//		bit4=1 -> do not use DIA&DCI
// DevREV@0x8005     DevID@0x8006
// Config1@0x8007    Config2@0x8008 ...
// Device info area @0x8100
// Device configuration info area @0x8200
{
	int k=0,k2=0,z=0,i,j;
	int useDCI=(options&0x10)==0?1:0;
	if(FWVersion<0xB00){		//only for 16F18xxx
		PrintMessage1(strings[S_FWver2old],"0.11.0");	//"This firmware is too old. Version %s is required\r\n"
		return;
	}
	if(!CheckV33Regulator()){
		PrintMessage(strings[S_noV33reg]);	//Can't find 3.3V expansion board
		return;
	}
	char s[256],t[256],*aux;
	if(dim>0x8000||dim<0){
		PrintMessage(strings[S_CodeLim]);	//"Code size exceeds limits\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"Read16F18xxx(%d,%d,%d,%d)\n",dim,dim2,dim3,options);
	}
	if((options&2)==0){				//HV entry
		if(!StartHVReg(8.5)){
			PrintMessage(strings[S_HVregErr]); //"HV regulator error\r\n"
			return;
		}
		else hvreg=8.5;
	}
	else StartHVReg(-1);			//LVP mode, turn off HV
	sizeW=0x8220;
	if(memCODE_W) free(memCODE_W);
	memCODE_W=(WORD*)malloc(sizeof(WORD)*sizeW);
	for(i=0;i<sizeW;i++) memCODE_W[i]=0x3fff;
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	if((options&2)==0){				//HV entry
		if((options&1)==0){				//VPP before VDD
			bufferU[j++]=EN_VPP_VCC;
			bufferU[j++]=4;				//VPP
			bufferU[j++]=EN_VPP_VCC;
			bufferU[j++]=0x5;			//VDD+VPP
		}
		else{							//VDD before VPP without delay
			bufferU[j++]=EN_VPP_VCC;
			bufferU[j++]=1;				//VDD
			bufferU[j++]=EN_VPP_VCC;
			bufferU[j++]=0x5;			//VDD+VPP
		}
	}
	else{			//Low voltage programming
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=4;				//VPP
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=TX16;			//0000 1010 0001 0010 1100 0010 1011 0010 = 0A12C2B2
		bufferU[j++]=2;
		bufferU[j++]=0x0A;
		bufferU[j++]=0x12;
		bufferU[j++]=0xC2;
		bufferU[j++]=0xB2;
		bufferU[j++]=SET_CK_D;		//Clock pulse
		bufferU[j++]=0x4;
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x0;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
//****************** read code ********************
	PrintMessage(strings[S_CodeReading1]);		//read code ...
	if(saveLog) fprintf(logfile,"%s\n",strings[S_CodeReading1]);		//read code ...
	PrintStatusSetup();
	for(i=0,j=0;i<dim;i++){
		bufferU[j++]=ICSP8_READ;
		bufferU[j++]=READ_NVM_INC;
		if((j+1)/2*3+3>DIMBUF||i==dim-1){		//2B cmd -> 3B data
			bufferU[j++]=FLUSH;				//remember: FLUSH generates a response (even empty)!
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(5);
			for(z=0;z<DIMBUF-2;z++){
				if(bufferI[z]==ICSP8_READ){
					memCODE_W[k++]=(bufferI[z+1]<<8)+bufferI[z+2];
					z+=2;
				}
			}
			PrintStatus(strings[S_CodeReading],i*100/(dim+dim2+0x60),i);	//"Read: %d%%, addr. %03X"
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
			}
		}
	}
	PrintStatusEnd();
	if(k!=dim){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ReadCodeErr],dim,k);	//"Error reading code area, requested %d words, read %d\r\n"
	}
	else PrintMessage(strings[S_Compl]);
//****************** read config area ********************
	PrintMessage(strings[S_Read_CONFIG_A]);		//read config ...
	if(saveLog) fprintf(logfile,"%s\n",strings[S_Read_CONFIG_A]);		//read config ...
	bufferU[j++]=ICSP8_LOAD;			//counter at 0x8000
	bufferU[j++]=LOAD_PC_ADDR;
	bufferU[j++]=0x80;
	bufferU[j++]=0x00;
	k2=0;
	for(i=0x8000;i<0x8010;i++){		//Config
		bufferU[j++]=ICSP8_READ;
		bufferU[j++]=READ_NVM_INC;
		if((j+1)/2*3+3>DIMBUF||i==0x800F){		//2B cmd -> 3B data
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(5);
			for(z=0;z<DIMBUF-2;z++){
				if(bufferI[z]==ICSP8_READ){
					memCODE_W[0x8000+k2++]=(bufferI[z+1]<<8)+bufferI[z+2];
					z+=2;
				}
			}
			PrintStatus(strings[S_CodeReading],(i-0x8000+dim)*100/(dim+dim2+0x60),i);	//"Read: %d%%, addr %03X"
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,k2,k2);	//"i=%d(0x%X), k=%d(0x%X)\n"
			}
		}
	}
	if(k2!=0x10){
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ConfigErr],0x10,k2);	//"Error reading config area, requested %d words, read %d\r\n"
	}
	else PrintMessage(strings[S_Compl]);
//****************** read device info area ********************
	if(useDCI){	//if not disabled
		if(saveLog) fprintf(logfile,"Read device info area @0x8100\n");
		bufferU[j++]=ICSP8_LOAD;			//counter at 0x8000
		bufferU[j++]=LOAD_PC_ADDR;
		bufferU[j++]=0x81;
		bufferU[j++]=0x00;
		k2=0;
		for(i=0x8100;i<0x8120;i++){		//DIA
			bufferU[j++]=ICSP8_READ;
			bufferU[j++]=READ_NVM_INC;
			if((j+1)/2*3+3>DIMBUF||i==0x811F){		//2B cmd -> 3B data
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(5);
				for(z=0;z<DIMBUF-2;z++){
					if(bufferI[z]==ICSP8_READ){
						memCODE_W[0x8100+k2++]=(bufferI[z+1]<<8)+bufferI[z+2];
						z+=2;
					}
				}
				PrintStatus(strings[S_CodeReading],(i-0x8100+dim+0x20)*100/(dim+dim2+0x60),i);	//"Read: %d%%, addr %03X"
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,k2,k2);	//"i=%d(0x%X), k=%d(0x%X)\n"
				}
			}
		}
		if(k2!=0x20){
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ConfigErr],0x20,k2);	//"Error reading config area, requested %d words, read %d\r\n"
		}
	//****************** read calibration area (Device Configuration Information) ********************
		if(saveLog) fprintf(logfile,"Read Device Configuration Information @0x8200\n");
		bufferU[j++]=ICSP8_LOAD;			//counter at 0x8200
		bufferU[j++]=LOAD_PC_ADDR;
		bufferU[j++]=0x82;
		bufferU[j++]=0x00;
		k2=0;
		for(i=0x8200;i<0x8220;i++){		//DCI
			bufferU[j++]=ICSP8_READ;
			bufferU[j++]=READ_NVM_INC;
			if((j+1)/2*3+3>DIMBUF||i==0x821F){		//2B cmd -> 3B data
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(5);
				for(z=0;z<DIMBUF-2;z++){
					if(bufferI[z]==ICSP8_READ){
						memCODE_W[0x8200+k2++]=(bufferI[z+1]<<8)+bufferI[z+2];
						z+=2;
					}
				}
				PrintStatus(strings[S_CodeReading],(i-0x8200+dim+0x40)*100/(dim+dim2+0x60),i);	//"Read: %d%%, addr %03X"
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,k2,k2);	//"i=%d(0x%X), k=%d(0x%X)\n"
				}
			}
		}
		if(k2!=0x20){
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ConfigErr],0x20,k2);	//"Error reading config area, requested %d words, read %d\r\n"
		}
	}
//****************** read eeprom ********************
	if(dim2){					//EEPROM
		if(useDCI) dim2=memCODE_W[0x8203]; 	//EEPROM size from DCI
		if(dim2>0x1000||dim2<0){		//Max 4K
			PrintMessage(strings[S_EELim]);	//"EEPROM size exceeds limits\r\n"
			return;
		}
		sizeEE=dim2;
		if(memEE) free(memEE);
		memEE=(unsigned char*)malloc(sizeEE);			//EEPROM
		for(i=0;i<sizeEE;i++) memEE[i]=0xFF;
		PrintMessage(strings[S_ReadEE]);		//Read EEPROM ...
		if(saveLog) fprintf(logfile,"%s\n",strings[S_ReadEE]);		//Read EEPROM ...
		bufferU[j++]=ICSP8_LOAD;			//counter at 0xF000
		bufferU[j++]=LOAD_PC_ADDR;
		bufferU[j++]=0xF0;
		bufferU[j++]=0x00;
		for(i=k=0;i<dim2;i++){
			bufferU[j++]=ICSP8_READ;
			bufferU[j++]=READ_NVM_INC;
			if((j+1)/2*3+3>DIMBUF||i==dim2-1){		//2B cmd -> 3B answer
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(5);
				for(z=0;z<DIMBUF-2;z++){
					if(bufferI[z]==ICSP8_READ){
						memEE[k++]=bufferI[z+2];	//LSB
						z+=2;
					}
				}
				PrintStatus(strings[S_CodeReading],i*100+dim+0x60/(dim+dim2+0x60),i);	//"Read: %d%%, addr %03X"
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,k,k);	//"i=%d(0x%X), k=%d(0x%X)\n"
				}
			}
		}
		PrintStatusEnd();
		if(i!=dim2){
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ReadEEErr],dim2,i);	//"Error reading EE area, ..."
			for(;i<dim2;i++) memEE[i]=0xff;
		}
		else PrintMessage(strings[S_Compl]);
	}
/***********************************/
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=1;					//VDD
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	PrintStatusClear();			//clear status report
//****************** visualize ********************
	for(i=0;i<4;i+=2){
		PrintMessage4("ID%d: 0x%04X\tID%d: 0x%04X\r\n",i,memCODE_W[0x8000+i],i+1,memCODE_W[0x8000+i+1]);
	}
	PrintMessage1(strings[S_DevID],memCODE_W[0x8006]);	//"DevID: 0x%04X\r\n"
	if(memCODE_W[0x8005]<0x3FFF) PrintMessage1(strings[S_DevREV],memCODE_W[0x8005]);	//"DevREV: 0x%04X\r\n"
	PIC16_ID(memCODE_W[0x8006]);
	for(i=0;i<5;i++){
		PrintMessage2(strings[S_ConfigWordX],i+1,memCODE_W[0x8007+i]);	//"Configuration word %d: 0x%04X\r\n"
	}
	if(useDCI){	//if not disabled
		PrintMessage("Device Information Area @0x8100\r\n");
		s[0]=0;
		for(i=0;i<0x20;i+=COL){
			sprintf(t,"%04X: ",0x8100+i);
			strcat(s,t);
			for(j=i;j<i+COL&&j<0x20;j++){
				sprintf(t,"%04X ",memCODE_W[0x8100+j]);
				strcat(s,t);
			}
			strcat(s,"\r\n");
		}
		PrintMessage(s);
		PrintMessage("Device Configuration Information @0x8200\r\n");
		s[0]=0;
		for(i=0;i<0x20;i+=COL){
			sprintf(t,"%04X: ",0x8200+i);
			strcat(s,t);
			for(j=i;j<i+COL&&j<0x20;j++){
				sprintf(t,"%04X ",memCODE_W[0x8200+j]);
				strcat(s,t);
			}
			strcat(s,"\r\n");
		}
		PrintMessage(s);
		PrintMessage1("Erase row size: %d words\r\n",memCODE_W[0x8200]);
		PrintMessage1("Write latches: %d\r\n",memCODE_W[0x8201]);
		PrintMessage1("User rows: %d\r\n",memCODE_W[0x8202]);
		PrintMessage1("->%d Flash words\r\n",memCODE_W[0x8200]*memCODE_W[0x8202]);
		PrintMessage1("EE data memory size: %d\r\n",memCODE_W[0x8203]);
		PrintMessage1("Pin count: %d\r\n",memCODE_W[0x8204]);
		if(dim!=memCODE_W[0x8200]*memCODE_W[0x8202]) PrintMessage(strings[S_WarnFlashSize]);	//"Warning, flash size is different from the expected value"
	}
	PrintMessage(strings[S_CodeMem2]);	//"\r\nCode memory:\r\n"
	DisplayCODE16F(dim);
	if(dim2) DisplayEE();	//visualize
	sprintf(str,strings[S_End],(stop-start)/1000.0);	//"\r\nEnd (%.2f s)\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

void Write12F6xx(int dim,int dim2)
// write 14 bit PIC
// dim=program size 	dim2=eeprom size
// vpp before vdd
// DevID@0x2006
// Config@0x2007
// Calib1@0x2008 (save)
// Calib2@0x2009 (save)
// eeprom@0x2100
// erase: BULK_ERASE_PROG (1001) +10ms
// write:LOAD_DATA_PROG (0010) + BEGIN_PROG (1000) + 4ms
// eeprom:	BULK_ERASE_DATA (1011) + 16ms
//			LOAD_DATA_DATA (0011) + BEGIN_PROG (1000) + 8ms
// verify during write
{
	int err=0;
	WORD devID=0x3fff,calib1=0x3fff,calib2=0x3fff;
	int k=0,z=0,i,j,w;
	if(sizeW<0x2007){
		PrintMessage(strings[S_NoConfigW3]);	//"Can't find CONFIG (0x2007)\r\nEnd\r\n"
		return;
	}
	if(load_calibword){
		if(sizeW>0x2009) load_calibword=2;
		else if(sizeW>0x2008) load_calibword=1;
		else{
			PrintMessage(strings[S_NoCalibW]);	//"Can't find calibration data\r\n"
			load_calibword=0;
		}
	}
	if(dim2){
		if(sizeW<0x2100){
			dim2=0;
			PrintMessage(strings[S_NoEEMem]);	//"Can't find EEPROM data\r\n"
		}
		else if(dim2>sizeW-0x2100) dim2=sizeW-0x2100;
	}
	if(saveLog){
		OpenLogFile();	//"log.txt"
		fprintf(logfile,"Write12F6xx(%d,%d)\n",dim,dim2);
	}
	for(i=0;i<0x2009&&i<sizeW;i++) memCODE_W[i]&=0x3FFF;
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VPP
	bufferU[j++]=0x4;
	bufferU[j++]=NOP;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=NOP;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=0x06;
	bufferU[j++]=READ_DATA_PROG;	//DevID
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=READ_DATA_PROG;	//Calib1
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=READ_DATA_PROG;	//Calib2
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=10000>>8;
	bufferU[j++]=10000&0xff;
	bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(15);
	for(z=0;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	devID=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_DevID],devID);	//"DevID: 0x%04X\r\n"
	PIC16_ID(devID);
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	calib1=(bufferI[z+1]<<8)+bufferI[z+2];
	if(calib1<0x3fff){
		PrintMessage1(strings[S_CalibWord1],calib1);	//"Calib1: 0x%04X\r\n"
	}
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	calib2=(bufferI[z+1]<<8)+bufferI[z+2];
	if(calib2<0x3fff){
		PrintMessage1(strings[S_CalibWord2],calib2);	//"Calib2: 0x%04X\r\n"
	}
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	j=0;
	bufferU[j++]=EN_VPP_VCC;		//VPP
	bufferU[j++]=0x4;
	bufferU[j++]=NOP;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=WAIT_T3;		//necessary when erasing fully written 16F62xA
								//not mentioned in the prog spec!
	if(programID||load_calibword||ICDenable){
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=0xFF;				//fake config
		if(load_calibword){
			bufferU[j++]=INC_ADDR_N;
			if(load_calibword==2) bufferU[j++]=0x09;
			else bufferU[j++]=0x08;
		}
	}
	bufferU[j++]=LOAD_DATA_PROG;
	bufferU[j++]=0xFF;
	bufferU[j++]=0xFF;
	bufferU[j++]=BULK_ERASE_PROG;
	bufferU[j++]=WAIT_T3;			// delay T3=10ms
	if(dim2){
		bufferU[j++]=BULK_ERASE_DATA;
		bufferU[j++]=WAIT_T3;			// delay T3=10ms
	}
	bufferU[j++]=EN_VPP_VCC;		//exit program mode
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T3;			// delay T3=10ms
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x4;
	bufferU[j++]=NOP;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x5;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=4000>>8;
	bufferU[j++]=4000&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(50);
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	PrintStatusSetup();
	for(w=i=k=0,j=0;i<dim;i++){
		if(memCODE_W[i]<0x3fff){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG;			//internally timed, T=3ms min
			bufferU[j++]=WAIT_T3;				//Tprogram 4ms
			bufferU[j++]=READ_DATA_PROG;
			w++;
		}
		bufferU[j++]=INC_ADDR;
		if(j>DIMBUF-12||i==dim-1){
			PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Writing: %d%%, add. %03X"
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(w*5+2);
			w=0;
			for(z=0;z<DIMBUF-5;z++){
				if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0x3fff) k++;
				else if(bufferI[z]==LOAD_DATA_PROG&&bufferI[z+3]==READ_DATA_PROG){
					if (memCODE_W[k]!=(bufferI[z+4]<<8)+bufferI[z+5]){
						PrintMessage("\r\n");
						PrintMessage3(strings[S_CodeWError2],k,memCODE_W[k],(bufferI[z+4]<<8)+bufferI[z+5]);	//"Error writing address %3X: written %04X, read %04X\r\n"
						err++;
						if(max_err&&err>max_err){
							PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
							PrintMessage(strings[S_IntW]);	//"Write interrupted"
							i=dim;
							z=DIMBUF;
						}
					}
					k++;
					z+=6;
				}
			}
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d,\n"
			}
		}
	}
	err+=i-k;
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
//****************** write eeprom ********************
	if(dim2){
		int err_e=0;
		PrintMessage(strings[S_EEAreaW]);	//"Writing EEPROM ... "
		PrintStatusSetup();
		j=0;
		bufferU[j++]=SET_PARAMETER;
		bufferU[j++]=SET_T3;
		bufferU[j++]=8000>>8;
		bufferU[j++]=8000&0xff;
		bufferU[j++]=BULK_ERASE_DATA;
		bufferU[j++]=WAIT_T3;			// delay T3=8ms
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=INC_ADDR_N;		//use only INC_ADDR_N so verification does not look at it
		bufferU[j++]=1;
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=0x2100-0x2001;		//EEPROM area: counter at 0x2100
		for(w=2,i=k=0x2100;i<0x2100+dim2;i++){
			if(memCODE_W[i]<0xff){
				bufferU[j++]=LOAD_DATA_DATA;
				bufferU[j++]=memCODE_W[i]&0xff;
				bufferU[j++]=BEGIN_PROG;			//internally timed, T=6ms min
				bufferU[j++]=WAIT_T3;				//Tprogram 8ms
				bufferU[j++]=READ_DATA_DATA;
				w++;
			}
			bufferU[j++]=INC_ADDR;
			if(j>DIMBUF-12||i==0x2100+dim2-1){
				PrintStatus(strings[S_CodeWriting],(i-0x2100+dim)*100/(dim+dim2),i);	//"Writing: %d%%, add. %03X"
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(w*9+2);
				w=0;
				for(z=0;z<DIMBUF-4;z++){
					if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0xff) k++;
					else if(bufferI[z]==LOAD_DATA_DATA&&bufferI[z+3]==READ_DATA_DATA){
						if (memCODE_W[k]!=bufferI[z+4]){
							PrintMessage("\r\n");
							PrintMessage3(strings[S_CodeWError3],k,memCODE_W[k],bufferI[z+4]);	//"Error writing address %4X: written %02X, read %02X\r\n"
							err_e++;
							if(max_err&&err+err_e>max_err){
								PrintMessage1(strings[S_MaxErr],err+err_e);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
								PrintMessage(strings[S_IntW]);	//"Write interrupted"
								i=0x2200;
								z=DIMBUF;
							}
						}
						k++;
						z+=5;
					}
				}
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d \n"
				}
			}
		}
		err_e+=i-k;
		err+=err_e;
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],err_e);	//"completed, %d errors\r\n"
	}
//****************** write ID, CONFIG, CALIB ********************
	PrintMessage(strings[S_ConfigAreaW]);	//"Writing CONFIG area ... "
	int err_c=0;
	int ICDgoto=0x2800+(ICDaddr&0x7FF);		//GOTO ICD routine (0x28xx)
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=4000>>8;
	bufferU[j++]=4000&0xff;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	if(programID){
		for(i=0x2000;i<0x2004;i++){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG;			//internally timed, T=3ms min
			bufferU[j++]=WAIT_T3;				//Tprogram 4ms
			bufferU[j++]=READ_DATA_PROG;
			bufferU[j++]=INC_ADDR;
		}
	}
	else{
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=4;
	}
	if(ICDenable){		//write a GOTO ICD routine (0x28xx)
		bufferU[j++]=LOAD_DATA_PROG;
		bufferU[j++]=ICDgoto>>8;		//MSB
		bufferU[j++]=ICDgoto&0xFF;			//LSB
		bufferU[j++]=BEGIN_PROG;			//internally timed, T=3ms min
		bufferU[j++]=WAIT_T3;				//Tprogram
		bufferU[j++]=READ_DATA_PROG;
	}
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=3;
	bufferU[j++]=LOAD_DATA_PROG;			//Config word 0x2007
	bufferU[j++]=memCODE_W[0x2007]>>8;		//MSB
	bufferU[j++]=memCODE_W[0x2007]&0xff;		//LSB
	bufferU[j++]=BEGIN_PROG;			//internally timed, T=3ms min
	bufferU[j++]=WAIT_T3;				//Tprogram 4ms
	bufferU[j++]=READ_DATA_PROG;
	bufferU[j++]=INC_ADDR;
	if(load_calibword){
		bufferU[j++]=LOAD_DATA_PROG;			//Calib word 1
		bufferU[j++]=memCODE_W[0x2008]>>8;		//MSB
		bufferU[j++]=memCODE_W[0x2008]&0xff;		//LSB
		bufferU[j++]=BEGIN_PROG;			//internally timed, T=3ms min
		bufferU[j++]=WAIT_T3;				//Tprogram 4ms
		bufferU[j++]=READ_DATA_PROG;
		bufferU[j++]=INC_ADDR;
		bufferU[j++]=LOAD_DATA_PROG;			//Calib word 2
		bufferU[j++]=memCODE_W[0x2009]>>8;		//MSB
		bufferU[j++]=memCODE_W[0x2009]&0xff;		//LSB
		bufferU[j++]=BEGIN_PROG;			//internally timed, T=3ms min
		bufferU[j++]=WAIT_T3;				//Tprogram 4ms
		bufferU[j++]=READ_DATA_PROG;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(35);
	for(i=0,z=0;programID&&i<4;i++){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (memCODE_W[0x2000+i]!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage("\r\n");
			PrintMessage3(strings[S_IDErr],i,memCODE_W[0x2000+i],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ID%d: written %04X, read %04X\r\n"
			err_c++;
		}
		z+=6;
	}
	if(ICDenable){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (ICDgoto!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage4(strings[S_ICDErr],0x2004,i,ICDgoto,(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ICD (0x%X): written %04X, read %04X\r\n"
			err_c++;
		}
		z+=6;
	}
	for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	if(~memCODE_W[0x2007]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2007],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
		err_c++;
	}
	if(load_calibword){
		for(z+=6;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (memCODE_W[0x2008]!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage("\r\n");
			PrintMessage2(strings[S_Calib1Err],memCODE_W[0x2008],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing Calib1: written %04X, read %04X\r\n"
			err_c++;
		}
		if(load_calibword==2){
			for(z+=6;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
			if (memCODE_W[0x2009]!=(bufferI[z+1]<<8)+bufferI[z+2]){
				PrintMessage("\r\n");
				PrintMessage2(strings[S_Calib2Err],memCODE_W[0x2009],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing Calib2: written %04X, read %04X\r\n"
				err_c++;
			}
		}
	}
	err+=err_c;
	PrintMessage1(strings[S_ComplErr],err_c);	//"completed, %d errors\r\n"
	if(saveLog){
		fprintf(logfile,strings[S_Log9],err);	//"Area config. 	errors=%d \n"
	}
//****************** exit ********************
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

void Write16F8x (int dim,int dim2)
// write 14 bit PIC
// dim=program size 	dim2=eeprom size
// vdd + 50ms + vdd&vpp
// DevID@0x2006
// Config@0x2007
// eeprom@0x2100
// erase if protected:
// LOAD_CONF (0)(0x3FFF) + CUST_CMD (0001) + CUST_CMD (0111)
// + BEGIN_PROG (1000) + 10ms
// + CUST_CMD (0001) + CUST_CMD (0111)
// erase if not protected and DevID=16F84A:
// LOAD_DATA_PROG (0010)(0x3FFF) + BULK_ERASE_PROG (1001) +10ms
// LOAD_DATA_DATA (0011)(0xFF) + BULK_ERASE_DATA (1011) + BEGIN_PROG (1000) + 10ms
// erase if not protected and DevID!=16F84A:
// LOAD_DATA_PROG (0010)(0x3FFF) + CUST_CMD (0001) + CUST_CMD (0111)
// + BEGIN_PROG (1000) + 10ms + CUST_CMD (0001) + CUST_CMD (0111)
// LOAD_DATA_DATA (0011)(0xFF) + CUST_CMD (0001) + CUST_CMD (0111)
// + BEGIN_PROG (1000) + 10ms + CUST_CMD (0001) + CUST_CMD (0111)
// write: LOAD_DATA_PROG (0010) + BEGIN_PROG (1000) + 20ms or 8ms(16F84A)
// write eeprom: LOAD_DATA_DATA (0011) + BEGIN_PROG (1000) + 20ms or 8ms(16F84A)
// verify during write
{
	int err=0;
	WORD devID,config;
	int k=0,z=0,i,j,w,r;
	if(sizeW<0x2007){
		PrintMessage(strings[S_NoConfigW3]);	//"Can't find CONFIG (0x2007)\r\nEnd\r\n"
		return;
	}
	if(dim2){
		if(sizeW<0x2100){
			dim2=0;
			PrintMessage(strings[S_NoEEMem]);	//"Can't find EEPROM data\r\n"
		}
		else if(dim2>sizeW-0x2100) dim2=sizeW-0x2100;
	}
	if(saveLog){
		OpenLogFile();	//"log.txt"
		fprintf(logfile,"Write16F8x(%d,%d)\n",dim,dim2);
	}
	for(i=0;i<0x2009&&i<sizeW;i++) memCODE_W[i]&=0x3FFF;
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=25000>>8;
	bufferU[j++]=25000&0xff;
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=NOP;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=0x06;
	bufferU[j++]=READ_DATA_PROG;	//DevID
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=READ_DATA_PROG;	//Config
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=WAIT_T3;			//50ms
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=NOP;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(140);
	for(z=0;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	devID=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_DevID],devID);	//"DevID: 0x%04X\r\n"
	PIC16_ID(devID);
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	config=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_ConfigWord],config);	//"Config word: 0x%04X\r\n"
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=10000>>8;
	bufferU[j++]=10000&0xff;
	if(config<0x3FF0){
		PrintMessage(strings[S_ProtErase]);	//"Override write protection\r\n"
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0x3F;				//fake config
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=0x07;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x01;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x07;
		bufferU[j++]=BEGIN_PROG;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x01;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x07;
	}
	else if(devID>>5==0x2B){			//16F84A
		bufferU[j++]=LOAD_DATA_PROG;
		bufferU[j++]=0x3f;				//MSB
		bufferU[j++]=0xff;				//LSB
		bufferU[j++]=BULK_ERASE_PROG;
		bufferU[j++]=BEGIN_PROG;
		bufferU[j++]=WAIT_T3;
		if(dim2){
			bufferU[j++]=LOAD_DATA_DATA;
			bufferU[j++]=0xff;				//LSB
			bufferU[j++]=BULK_ERASE_DATA;
			bufferU[j++]=BEGIN_PROG;
			bufferU[j++]=WAIT_T3;
		}
	}
	else{								//altri
		bufferU[j++]=LOAD_DATA_PROG;
		bufferU[j++]=0x3f;				//MSB
		bufferU[j++]=0xff;				//LSB
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x01;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x07;
		bufferU[j++]=BEGIN_PROG;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x01;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x07;
		if(dim2){
			bufferU[j++]=LOAD_DATA_DATA;	//EEPROM:  spec error?
			bufferU[j++]=0xff;				//LSB
			bufferU[j++]=CUST_CMD;
			bufferU[j++]=0x01;
			bufferU[j++]=CUST_CMD;
			bufferU[j++]=0x07;
			bufferU[j++]=BEGIN_PROG;
			bufferU[j++]=WAIT_T3;
			bufferU[j++]=CUST_CMD;
			bufferU[j++]=0x01;
			bufferU[j++]=CUST_CMD;
			bufferU[j++]=0x07;
		}
	}
	if(!programID){					//back to program memory
		bufferU[j++]=NOP;				//exit program mode
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=0x1;
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=0x0;
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x0;
		bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
		bufferU[j++]=EN_VPP_VCC;		//VDD
		bufferU[j++]=0x1;
		bufferU[j++]=WAIT_T3;			//50ms
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
		bufferU[j++]=0x5;
	}
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	r=(devID>>5==0x2B)?8000:20000;
	bufferU[j++]=r>>8;
	bufferU[j++]=r&0xff;
	bufferU[j++]=FLUSH;
	r/=1000;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(!programID?140:60);
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	PrintStatusSetup();
	for(w=i=k=0,j=0;i<dim;i++){
		if(memCODE_W[i]<0x3fff){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG;			//internally timed
			bufferU[j++]=WAIT_T3;				//Tprogram
			bufferU[j++]=READ_DATA_PROG;
			w++;
		}
		bufferU[j++]=INC_ADDR;
		if(j>DIMBUF-10||i==dim-1){
			PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Writing: %d%%, add. %03X"
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(w*r+4);
			w=0;
			for(z=0;z<DIMBUF-5;z++){
				if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0x3fff) k++;
				else if(bufferI[z]==LOAD_DATA_PROG&&bufferI[z+3]==READ_DATA_PROG){
					if (memCODE_W[k]!=(bufferI[z+4]<<8)+bufferI[z+5]){
						PrintMessage("\r\n");
						PrintMessage3(strings[S_CodeWError2],k,memCODE_W[k],(bufferI[z+4]<<8)+bufferI[z+5]);	//"Error writing address %3X: written %04X, read %04X\r\n"
						err++;
						if(max_err&&err>max_err){
							PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
							PrintMessage(strings[S_IntW]);	//"Write interrupted"
							i=dim;
							z=DIMBUF;
						}
					}
					k++;
					z+=6;
				}
			}
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d \n"
			}
		}
	}
	err+=i-k;
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
//****************** write eeprom ********************
	if(dim2){
		PrintMessage(strings[S_EEAreaW]);	//"Writing EEPROM ... "
		PrintStatusSetup();
		j=0;
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=INC_ADDR_N;		//use only INC_ADDR_N so verification does not look at it
		bufferU[j++]=0xFF;
		bufferU[j++]=INC_ADDR_N;	//EEPROM: counter at 0x2100
		bufferU[j++]=1;
		for(w=0,i=k=0x2100;i<0x2100+dim2;i++){
			if(memCODE_W[i]<0xff){
				bufferU[j++]=LOAD_DATA_DATA;
				bufferU[j++]=memCODE_W[i]&0xff;
				bufferU[j++]=BEGIN_PROG;			//internally timed
				bufferU[j++]=WAIT_T3;				//Tprogram
				bufferU[j++]=READ_DATA_DATA;
				w++;
			}
			bufferU[j++]=INC_ADDR;
			if(j>DIMBUF-10||i==0x2100+dim2-1){
				PrintStatus(strings[S_CodeWriting],(i-0x2100+dim)*100/(dim+dim2),i);	//"Writing: %d%%, add. %03X"
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(w*r+5);
				w=0;
				for(z=0;z<DIMBUF-4;z++){
					if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0xff) k++;
					else if(bufferI[z]==LOAD_DATA_DATA&&bufferI[z+3]==READ_DATA_DATA){
						if (memCODE_W[k]!=bufferI[z+4]){
							PrintMessage("\r\n");
							PrintMessage3(strings[S_CodeWError3],k,memCODE_W[k],bufferI[z+4]);	//"Error writing address %4X: written %02X, read %02X\r\n"
							err++;
							if(max_err&&err>max_err){
								PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
								PrintMessage(strings[S_IntW]);	//"Write interrupted"
								i=0x2200;
								z=DIMBUF;
							}
						}
						k++;
						z+=5;
					}
				}
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d \n"
				}
			}
		}
		err+=i-k;
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],i-k);	//"completed, %d errors\r\n"
	}
//****************** write ID, CONFIG ********************
	PrintMessage(strings[S_ConfigAreaW]);	//"Writing CONFIG area ... "
	int err_c=0;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	if(programID){
		for(i=0x2000;i<0x2004;i++){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG;			//internally timed
			bufferU[j++]=WAIT_T3;				//Tprogram
			bufferU[j++]=READ_DATA_PROG;
			bufferU[j++]=INC_ADDR;
		}
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=3;
	}
	else{
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=7;
	}
	bufferU[j++]=LOAD_DATA_PROG;			//Config word 0x2007
	bufferU[j++]=memCODE_W[0x2007]>>8;		//MSB
	bufferU[j++]=memCODE_W[0x2007]&0xff;		//LSB
	bufferU[j++]=BEGIN_PROG;			//internally timed
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=READ_DATA_PROG;
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(programID?125:35);
	for(i=0,z=0;programID&&i<4;i++){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (memCODE_W[0x2000+i]!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage("\r\n");
			PrintMessage3(strings[S_IDErr],i,memCODE_W[0x2000+i],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ID%d: written %04X, read %04X\r\n"
			err_c++;
		}
		z+=6;
	}
	for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	if(~memCODE_W[0x2007]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2007],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
		err_c++;
	}
	err+=err_c;
	PrintMessage1(strings[S_ComplErr],err_c);	//"completed, %d errors\r\n"
	if(saveLog){
		fprintf(logfile,strings[S_Log9],err);	//"Area config. 	errors=%d \n"
	}
//****************** exit ********************
	j=0;
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

void Write16F62x (int dim,int dim2)
// write 14 bit PIC
// dim=program size 	dim2=eeprom size
// vpp before vdd
// DevID@0x2006
// Config@0x2007
// eeprom@0x2200
// erase if protected:
// LOAD_CONF (0000)(0) + CUST_CMD (0001) + CUST_CMD (0111)
// + BEGIN_PROG (1000) + 15ms + CUST_CMD (0001) + CUST_CMD (0111)
// erase if not protected:
// LOAD_DATA_PROG (0010)(0x3FFF) + BULK_ERASE_PROG (1001) +5ms
// LOAD_DATA_DATA (0011)(0xFF) + BULK_ERASE_DATA (1011) + BEGIN_PROG (1000) + 5ms
// write: LOAD_DATA_PROG (0010) + BEGIN_PROG2 (11000) + 8ms
// write ID: LOAD_DATA_PROG (0010) + BEGIN_PROG (1000) + 16ms
// write CONFIG: LOAD_DATA_PROG (0010) + BEGIN_PROG2 (11000) + 8ms
// eeprom: LOAD_DATA_DATA (0011) + BEGIN_PROG2 (11000) + 8ms
// verify during write
{
	int err=0;
	WORD devID,config;
	int k=0,z=0,i,j,w;
	if(sizeW<0x2007){
		PrintMessage(strings[S_NoConfigW3]);	//"Can't find CONFIG (0x2007)\r\nEnd\r\n"
		return;
	}
	if(dim2){
		if(sizeW<0x2100){
			dim2=0;
			PrintMessage(strings[S_NoEEMem]);	//"Can't find EEPROM data\r\n"
		}
		else if(dim2>sizeW-0x2100) dim2=sizeW-0x2100;
	}
	if(saveLog){
		OpenLogFile();	//"log.txt"
		fprintf(logfile,"Write16F62x(%d,%d)\n",dim,dim2);
	}
	for(i=0;i<0x2009&&i<sizeW;i++) memCODE_W[i]&=0x3FFF;
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=10000>>8;
	bufferU[j++]=10000&0xff;
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VPP
	bufferU[j++]=0x4;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=NOP;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=0x06;
	bufferU[j++]=READ_DATA_PROG;	//DevID
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=READ_DATA_PROG;	//Config
	bufferU[j++]=EN_VPP_VCC;		//VPP
	bufferU[j++]=0x4;
	bufferU[j++]=EN_VPP_VCC;		//0
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T3;			//delay exit-enter prog mode
	bufferU[j++]=EN_VPP_VCC;		//VPP
	bufferU[j++]=0x4;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(12);
	for(z=0;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	devID=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_DevID],devID);	//"DevID: 0x%04X\r\n"
	PIC16_ID(devID);
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	config=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_ConfigWord],config);	//"Config word: 0x%04X\r\n"
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=5000>>8;
	bufferU[j++]=5000&0xff;
	if(config<0x3C00){
		PrintMessage(strings[S_ProtErase]);	//"Override write protection\r\n"
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0x3F;				//fake config	spec ERROR!!! is written data=0!!
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=0x07;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x01;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x07;
		bufferU[j++]=BEGIN_PROG;		//Tera+Tprog=5+8 ms
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x01;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x07;
	}
	else{
		bufferU[j++]=LOAD_DATA_PROG;
		bufferU[j++]=0x3f;				//MSB
		bufferU[j++]=0xff;				//LSB
		bufferU[j++]=BULK_ERASE_PROG;
		bufferU[j++]=BEGIN_PROG;		//Tera=5ms
		bufferU[j++]=WAIT_T3;
		if(dim2){
			bufferU[j++]=LOAD_DATA_DATA;
			bufferU[j++]=0xff;				//LSB
			bufferU[j++]=BULK_ERASE_DATA;
			bufferU[j++]=BEGIN_PROG;		//Tera=5ms
			bufferU[j++]=WAIT_T3;
		}
	}
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x4;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=8000>>8;
	bufferU[j++]=8000&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(60);
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	PrintStatusSetup();
	for(w=i=k=0,j=0;i<dim;i++){
		if(memCODE_W[i]<0x3fff){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG2;			//program only, internally timed
			bufferU[j++]=WAIT_T3;				//Tprogram=8ms
			bufferU[j++]=READ_DATA_PROG;
			w++;
		}
		bufferU[j++]=INC_ADDR;
		if(j>DIMBUF-10||i==dim-1){
			PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Writing: %d%%, add. %03X"
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(w*9+2);
			w=0;
			for(z=0;z<DIMBUF-5;z++){
				if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0x3fff) k++;
				else if(bufferI[z]==LOAD_DATA_PROG&&bufferI[z+3]==READ_DATA_PROG){
					if (memCODE_W[k]!=(bufferI[z+4]<<8)+bufferI[z+5]){
						PrintMessage("\r\n");
						PrintMessage3(strings[S_CodeWError2],k,memCODE_W[k],(bufferI[z+4]<<8)+bufferI[z+5]);	//"Error writing address %3X: written %04X, read %04X\r\n"
						err++;
						if(max_err&&err>max_err){
							PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
							PrintMessage(strings[S_IntW]);	//"Write interrupted"
							i=dim;
							z=DIMBUF;
						}
					}
					k++;
					z+=6;
				}
			}
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d \n"
			}
		}
	}
	err+=i-k;
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
//****************** write eeprom ********************
	if(dim2){
		PrintMessage(strings[S_EEAreaW]);	//"Writing EEPROM ... "
		PrintStatusSetup();
		j=0;
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=INC_ADDR_N;		//use only INC_ADDR_N so verification does not look at it
		bufferU[j++]=0xFF;		//20FF
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=0xFF;		//21FE
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=0x2;		//EEPROM: counter at 0x2200
		for(w=0,i=k=0x2100;i<0x2100+dim2;i++){
			if(memCODE_W[i]<0xff){
				bufferU[j++]=LOAD_DATA_DATA;
				bufferU[j++]=memCODE_W[i]&0xff;
				bufferU[j++]=BEGIN_PROG2;			//internally timed
				bufferU[j++]=WAIT_T3;				//Tprogram=8ms
				bufferU[j++]=READ_DATA_DATA;
				w++;
			}
			bufferU[j++]=INC_ADDR;
			if(j>DIMBUF-10||i==0x2100+dim2-1){
				PrintStatus(strings[S_CodeWriting],(i-0x2100+dim)*100/(dim+dim2),i);	//"Writing: %d%%, add. %03X"
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(w*14+1);
				w=0;
				for(z=0;z<DIMBUF-4;z++){
					if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0xff) k++;
					else if(bufferI[z]==LOAD_DATA_DATA&&bufferI[z+3]==READ_DATA_DATA){
						if (memCODE_W[k]!=bufferI[z+4]){
							PrintMessage("\r\n");
							PrintMessage3(strings[S_CodeWError3],k,memCODE_W[k],bufferI[z+4]);	//"Error writing address %4X: written %02X, read %02X\r\n"
							err++;
							if(max_err&&err>max_err){
								PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
								PrintMessage(strings[S_IntW]);	//"Write interrupted"
								i=0x2200;
								z=DIMBUF;
							}
						}
						k++;
						z+=5;
					}
				}
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d \n"
				}
			}
		}
		err+=i-k;
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],i-k);	//"completed, %d errors\r\n"
	}
//****************** write ID, CONFIG ********************
	PrintMessage(strings[S_ConfigAreaW]);	//"Writing CONFIG area ... "
	int err_c=0;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	if(programID){
		for(i=0x2000;i<0x2004;i++){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG;			//erase + prog internally timed, T=8+5 ms
			bufferU[j++]=WAIT_T3;				//Tprogram
			bufferU[j++]=WAIT_T3;				//Tprogram
			bufferU[j++]=READ_DATA_PROG;
			bufferU[j++]=INC_ADDR;
		}
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=3;
	}
	else{
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=7;
	}
	bufferU[j++]=LOAD_DATA_PROG;			//Config word 0x2007
	bufferU[j++]=memCODE_W[0x2007]>>8;		//MSB
	bufferU[j++]=memCODE_W[0x2007]&0xff;		//LSB
	bufferU[j++]=BEGIN_PROG2;				//prog only, internally timed, T=8 ms
	bufferU[j++]=WAIT_T3;					//Tprogram 8ms
	bufferU[j++]=READ_DATA_PROG;
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(programID?125:35);
	for(i=0,z=0;programID&&i<4;i++){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (memCODE_W[0x2000+i]!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage3(strings[S_IDErr],i,memCODE_W[0x2000+i],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ID%d: written %04X, read %04X\r\n"
			err_c++;
		}
		z+=6;
	}
	for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	if(~memCODE_W[0x2007]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
		PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2007],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
		err_c++;
	}
	err+=err_c;
	PrintMessage1(strings[S_ComplErr],err_c);	//"completed, %d errors\n"
	if(saveLog){
		fprintf(logfile,strings[S_Log9],err);	//"Area config. 	errors=%d\n"
	}
//****************** exit ********************
	j=0;
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

void Write12F62x(int dim,int dim2)
// write 14 bit PIC
// dim=program size 	dim2=eeprom size
// vpp before vdd
// save OSCCAL @ dim-1
// CONFIG@0x2007 includes 2  calibration bits
// DevID@0x2006
// eeprom@0x2100
// erase: BULK_ERASE_PROG (1001) +10ms
// write: LOAD_DATA_PROG (0010) + BEGIN_PROG (1000) + 3ms
// eeprom: BULK_ERASE_DATA (1011) + 9ms
// LOAD_DATA_DATA (0011) + BEGIN_PROG (1000) + 6ms
// verify during write
{
	int err=0;
	WORD devID,config,osccal;
	int k=0,z=0,i,j,w;
	if(sizeW<0x2007){
		PrintMessage(strings[S_NoConfigW3]);	//"Can't find CONFIG (0x2007)\r\nEnd\r\n"
		return;
	}
	if(dim2){
		if(sizeW<0x2100){
			dim2=0;
			PrintMessage(strings[S_NoEEMem]);	//"Can't find EEPROM data\r\n"
		}
		else if(dim2>sizeW-0x2100) dim2=sizeW-0x2100;
	}
	if(saveLog){
		OpenLogFile();	//"log.txt"
		fprintf(logfile,"Write12F62x(%d,%d)\n",dim,dim2);
	}
	for(i=0;i<sizeW;i++) memCODE_W[i]&=0x3FFF;
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VPP
	bufferU[j++]=0x4;
	bufferU[j++]=NOP;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=NOP;
	for(i=0;i<dim-0xff;i+=0xff){
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=0xff;
	}
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=dim-1-i;
	bufferU[j++]=READ_DATA_PROG;	// OSCCAL
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=0x06;
	bufferU[j++]=READ_DATA_PROG;	//DevID
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=READ_DATA_PROG;	//Config
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=10000>>8;
	bufferU[j++]=10000&0xff;
	bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(15);
	for(z=0;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	osccal=(bufferI[z+1]<<8)+bufferI[z+2];
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	devID=(bufferI[z+1]<<8)+bufferI[z+2];
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	config=(bufferI[z+1]<<8)+bufferI[z+1];
	PrintMessage1(strings[S_DevID],devID);	//"DevID: 0x%04X\r\n"
	PIC16_ID(devID);
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	j=0;
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VPP
	bufferU[j++]=0x4;
	bufferU[j++]=NOP;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=NOP;
	if(programID){
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=0xFF;				//fake config
	}
	bufferU[j++]=BULK_ERASE_PROG;
	bufferU[j++]=WAIT_T3;			// delay T3=10ms
	bufferU[j++]=EN_VPP_VCC;		//exit program mode
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T3;			// delay T3=10ms after exiting program mode
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x4;
	bufferU[j++]=NOP;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x5;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=3000>>8;
	bufferU[j++]=3000&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(40);
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	PrintStatusSetup();
	if(!load_osccal) memCODE_W[dim-1]=osccal;	//backup osccal
	for(w=i=k=0,j=0;i<dim;i++){
		if(memCODE_W[i]<0x3fff){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG;			//internally timed, T=3ms
			bufferU[j++]=WAIT_T3;				//Tprogram 3ms
			bufferU[j++]=READ_DATA_PROG;
			w++;
		}
		bufferU[j++]=INC_ADDR;
		if(j>DIMBUF-10||i==dim-1){
			PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Writing: %d%%, add. %03X"
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(w*6.5+j*0.1);
			w=0;
			for(z=0;z<DIMBUF-5;z++){
				if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0x3fff) k++;
				else if(bufferI[z]==LOAD_DATA_PROG&&bufferI[z+3]==READ_DATA_PROG){
					if (memCODE_W[k]!=(bufferI[z+4]<<8)+bufferI[z+5]){
						PrintMessage("\r\n");
						PrintMessage3(strings[S_CodeWError2],k,memCODE_W[k],(bufferI[z+4]<<8)+bufferI[z+5]);	//"Error writing address %3X: written %04X, read %04X\r\n"
						err++;
						if(max_err&&err>max_err){
							PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
							PrintMessage(strings[S_IntW]);	//"Write interrupted"
							i=dim;
							z=DIMBUF;
						}
					}
					k++;
					z+=6;
				}
			}
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d \n"
			}
		}
	}
	PrintStatusEnd();
	err+=i-k;
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
//****************** write eeprom ********************
	if(dim2){
		PrintMessage(strings[S_EEAreaW]);	//"Writing EEPROM ... "
		PrintStatusSetup();
		j=0;
		bufferU[j++]=SET_PARAMETER;
		bufferU[j++]=SET_T3;
		bufferU[j++]=6000>>8;
		bufferU[j++]=6000&0xff;
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=INC_ADDR_N;		//use only INC_ADDR_N so verification does not look at it
		bufferU[j++]=0xFF;
		bufferU[j++]=INC_ADDR_N;	//EEPROM: counter at 0x2100
		bufferU[j++]=1;
		bufferU[j++]=BULK_ERASE_DATA;
		bufferU[j++]=WAIT_T3;			// delay=12ms
		bufferU[j++]=WAIT_T3;
		for(w=3,i=k=0x2100;i<0x2100+dim2;i++){
			if(memCODE_W[i]<0xff){
				bufferU[j++]=LOAD_DATA_DATA;
				bufferU[j++]=memCODE_W[i]&0xff;
				bufferU[j++]=BEGIN_PROG;			//internally timed, T=6ms
				bufferU[j++]=WAIT_T3;				//Tprogram 6ms
				bufferU[j++]=READ_DATA_DATA;
				w++;
			}
			bufferU[j++]=INC_ADDR;
			if(j>DIMBUF-10||i==0x2100+dim2-1){
				PrintStatus(strings[S_CodeWriting],(i-0x2100+dim)*100/(dim+dim2),i);	//"Writing: %d%%, add. %03X"
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(w*7+2);
				w=0;
				for(z=0;z<DIMBUF-4;z++){
					if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0xff) k++;
					else if(bufferI[z]==LOAD_DATA_DATA&&bufferI[z+3]==READ_DATA_DATA){
						if (memCODE_W[k]!=bufferI[z+4]){
							PrintMessage("\r\n");
							PrintMessage3(strings[S_CodeWError3],k,memCODE_W[k],bufferI[z+4]);	//"Error writing address %4X: written %02X, read %02X\r\n"
							err++;
							if(max_err&&err>max_err){
								PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
								PrintMessage(strings[S_IntW]);	//"Write interrupted"
								i=0x2200;
								z=DIMBUF;
							}
						}
						k++;
						z+=5;
					}
				}
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d \n"
				}
			}
		}
		err+=i-k;
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],i-k);	//"completed, %d errors\r\n"
	}
//****************** write ID, CONFIG, CALIB ********************
	PrintMessage(strings[S_ConfigAreaW]);	//"Writing CONFIG area ... "
	int err_c=0;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	if(programID){
		for(i=0x2000;i<0x2004;i++){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG;			//internally timed, T=3ms
			bufferU[j++]=WAIT_T3;				//Tprogram 3ms
			bufferU[j++]=READ_DATA_PROG;
			bufferU[j++]=INC_ADDR;
		}
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=3;
	}
	else{
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=7;
	}
	if(!load_calibword)	memCODE_W[0x2007]=(memCODE_W[0x2007]&0xfff)+(config&0x3000);
	bufferU[j++]=LOAD_DATA_PROG;			//Config word 0x2007
	bufferU[j++]=memCODE_W[0x2007]>>8;		//MSB
	bufferU[j++]=memCODE_W[0x2007]&0xff;		//LSB
	bufferU[j++]=BEGIN_PROG;			//internally timed, T=3ms
	bufferU[j++]=WAIT_T3;				//Tprogram 3ms
	bufferU[j++]=READ_DATA_PROG;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(35);
	for(i=0,z=0;programID&&i<4;i++){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (memCODE_W[0x2000+i]!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage3(strings[S_IDErr],i,memCODE_W[0x2000+i],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ID%d: written %04X, read %04X\r\n"
			err_c++;
		}
		z+=6;
	}
	for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	if(~memCODE_W[0x2007]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
		PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2007],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
		err_c++;
	}
	err+=err_c;
	PrintMessage1(strings[S_ComplErr],err_c);	//"completed, %d errors\r\n"
	if(saveLog){
		fprintf(logfile,strings[S_Log9],err);	//"Area config. 	errors=%d \n"
	}
//****************** exit ********************
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=EN_VPP_VCC;		//exit program mode
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

void Write16F87x (int dim,int dim2)
// write 14 bit PIC
// dim=program size 	dim2=eeprom size
// dim2<0 -> eeprom @ 0x2200
// vdd + (50ms?) + vdd&vpp
// DevID@0x2006
// Config@0x2007
// eeprom@0x2100
// erase if protected:
// LOAD_CONF (0000)(0x3FFF) + CUST_CMD (0001) + CUST_CMD (0111)
// + BEGIN_PROG (1000) + 8ms
// + CUST_CMD (0001) + CUST_CMD (0111)
// erase if not protected:
// LOAD_DATA_PROG (0010)(0x3FFF) + CUST_CMD (0001) + CUST_CMD (0111)
// + BEGIN_PROG (1000) + 8ms + CUST_CMD (0001) + CUST_CMD (0111)
// LOAD_DATA_DATA (0011)(0xFF) + CUST_CMD (0001) + CUST_CMD (0111)
// + BEGIN_PROG (1000) + 8ms + CUST_CMD (0001) + CUST_CMD (0111)
// write: LOAD_DATA_PROG (0010) + BEGIN_PROG2 (11000) + 4ms
// write eeprom: LOAD_DATA_DATA (0011) + BEGIN_PROG (1000) + 8ms
// verify during write
{
	int err=0;
	WORD devID,config;
	int k=0,z=0,i,j,w,ee2200=0;
	if(dim2<0){
		dim2=-dim2;
		ee2200=1;
	}
	if(sizeW<0x2007){
		PrintMessage(strings[S_NoConfigW3]);	//"Can't find CONFIG (0x2007)\r\nEnd\r\n"
		return;
	}
	if(dim2){
		if(sizeW<0x2100){
			dim2=0;
			PrintMessage(strings[S_NoEEMem]);	//"Can't find EEPROM data\r\n"
		}
		else if(dim2>sizeW-0x2100) dim2=sizeW-0x2100;
	}
	if(saveLog){
		OpenLogFile();	//"log.txt"
		fprintf(logfile,"Write16F87x(%d,%d)\n",dim,dim2);
	}
	for(i=0;i<sizeW;i++) memCODE_W[i]&=0x3FFF;
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=NOP;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=0x06;
	bufferU[j++]=READ_DATA_PROG;	//DevID
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=READ_DATA_PROG;	//Config
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(60);
	for(z=0;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	devID=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_DevID],devID);	//"DevID: 0x%04X\r\n"
	PIC16_ID(devID);
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	config=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_ConfigWord],config);	//"Config word: 0x%04X\r\n"
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=8000>>8;
	bufferU[j++]=8000&0xff;
	if((config&0x3130)!=0x3130){
		PrintMessage(strings[S_ProtErase]);	//"override write protection\r\n"
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0x3F;				//fake config
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=0x07;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x01;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x07;
		bufferU[j++]=BEGIN_PROG;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x01;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x07;
		bufferU[j++]=NOP;				//exit program mode
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=0x1;
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=0x0;
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x0;
		bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
		bufferU[j++]=EN_VPP_VCC;		//VDD
		bufferU[j++]=0x1;
		bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
		bufferU[j++]=0x5;
	}
	else{
		if(ICDenable||programID){			//erase 0x2000-2004 also
			bufferU[j++]=LOAD_CONF;			//counter at 0x2000
			bufferU[j++]=0x3F;				//fake config
			bufferU[j++]=0xFF;				//fake config
		}
		bufferU[j++]=LOAD_DATA_PROG;
		bufferU[j++]=0x3f;				//MSB
		bufferU[j++]=0xff;				//LSB
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x01;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x07;
		bufferU[j++]=BEGIN_PROG;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x01;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x07;
		if(dim2){
			bufferU[j++]=LOAD_DATA_DATA;	//EEPROM:  spec error?
			bufferU[j++]=0xff;				//LSB
			bufferU[j++]=CUST_CMD;
			bufferU[j++]=0x01;
			bufferU[j++]=CUST_CMD;
			bufferU[j++]=0x07;
			bufferU[j++]=BEGIN_PROG;
			bufferU[j++]=WAIT_T3;
			bufferU[j++]=CUST_CMD;
			bufferU[j++]=0x01;
			bufferU[j++]=CUST_CMD;
			bufferU[j++]=0x07;
		}
		if(ICDenable||programID){			//back to addr 0
			bufferU[j++]=EN_VPP_VCC;
			bufferU[j++]=0x1;
			bufferU[j++]=EN_VPP_VCC;
			bufferU[j++]=0x0;
			bufferU[j++]=SET_CK_D;
			bufferU[j++]=0x0;
			bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
			bufferU[j++]=EN_VPP_VCC;		//VDD
			bufferU[j++]=0x1;
			bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
			bufferU[j++]=0x5;
		}
	}
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=4000>>8;
	bufferU[j++]=4000&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(60);
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	PrintStatusSetup();
	for(w=i=k=0,j=0;i<dim;i++){
		if(memCODE_W[i]<0x3fff){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG2;			//internally timed
			bufferU[j++]=WAIT_T3;				//Tprogram
			bufferU[j++]=READ_DATA_PROG;
			w++;
		}
		bufferU[j++]=INC_ADDR;
		if(j>DIMBUF-10||i==dim-1){
			PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Writing: %d%%, add. %03X"
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(w*5+2);
			w=0;
			for(z=0;z<DIMBUF-5;z++){
				if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0x3fff) k++;
				else if(bufferI[z]==LOAD_DATA_PROG&&bufferI[z+3]==READ_DATA_PROG){
					if (memCODE_W[k]!=(bufferI[z+4]<<8)+bufferI[z+5]){
						PrintMessage("\r\n");
						PrintMessage3(strings[S_CodeWError2],k,memCODE_W[k],(bufferI[z+4]<<8)+bufferI[z+5]);	//"Error writing address %3X: written %04X, read %04X\r\n"
						err++;
						if(max_err&&err>max_err){
							PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
							PrintMessage(strings[S_IntW]);	//"Write interrupted"
							i=dim;
							z=DIMBUF;
						}
					}
					k++;
					z+=6;
				}
			}
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d \n"
			}
		}
	}
	err+=i-k;
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
//****************** write eeprom ********************
	if(dim2){
		int err_e=0;
		PrintMessage(strings[S_EEAreaW]);	//"Writing EEPROM ... "
		PrintStatusSetup();
		j=0;
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=INC_ADDR_N;		//use only INC_ADDR_N so verification does not look at it
		bufferU[j++]=0xFF;
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=1;					//EEPROM: counter at 0x2100
		if(ee2200){		//eeprom at 0x2200
			bufferU[j++]=INC_ADDR_N;
			bufferU[j++]=0xFF;
			bufferU[j++]=INC_ADDR_N;
			bufferU[j++]=1;
		}
		for(w=0,i=k=0x2100;i<0x2100+dim2;i++){
			if(memCODE_W[i]<0xff){
				bufferU[j++]=LOAD_DATA_DATA;
				bufferU[j++]=memCODE_W[i]&0xff;
				bufferU[j++]=BEGIN_PROG2;			//internally timed ?????
				bufferU[j++]=WAIT_T3;				//Tprogram         ?????
				bufferU[j++]=READ_DATA_DATA;
				w++;
			}
			bufferU[j++]=INC_ADDR;
			if(j>DIMBUF-10||i==0x2100+dim2-1){
				PrintStatus(strings[S_CodeWriting],(i-0x2100+dim)*100/(dim+dim2),i);	//"Writing: %d%%, add. %03X"
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(w*8+5);
				w=0;
				for(z=0;z<DIMBUF;z++){
					if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0xff) k++;
					else if(bufferI[z]==LOAD_DATA_DATA&&bufferI[z+3]==READ_DATA_DATA){
						if (memCODE_W[k]!=bufferI[z+4]){
							PrintMessage("\r\n");
							PrintMessage3(strings[S_CodeWError3],k,memCODE_W[k],bufferI[z+4]);	//"Error writing address %4X: written %02X, read %02X\r\n"
							err_e++;
							if(max_err&&err+err_e>max_err){
								PrintMessage1(strings[S_MaxErr],err+err_e);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
								PrintMessage(strings[S_IntW]);	//"Write interrupted"
								i=0x2200;
								z=DIMBUF;
							}
						}
						k++;
						z+=5;
					}
				}
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d \n"
				}
			}
		}
		err_e+=i-k;
		err+=err_e;
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],err_e);	//"completed, %d errors\r\n"
	}
//****************** write ID, CONFIG, CALIB ********************
	PrintMessage(strings[S_ConfigAreaW]);	//"Writing CONFIG area ... "
	int err_c=0;
	int ICDgoto=0x2800+(ICDaddr&0x7FF);		//GOTO ICD routine (0x28xx)
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=8000>>8;
	bufferU[j++]=8000&0xff;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	if(programID){
		for(i=0x2000;i<0x2004;i++){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG2;			//internally timed
			bufferU[j++]=WAIT_T3;				//Tprogram
			bufferU[j++]=READ_DATA_PROG;
			bufferU[j++]=INC_ADDR;
		}
	}
	else{
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=4;
	}
	if(ICDenable){		//write a GOTO ICD routine (0x28xx)
		bufferU[j++]=LOAD_DATA_PROG;
		bufferU[j++]=ICDgoto>>8;		//MSB
		bufferU[j++]=ICDgoto&0xFF;			//LSB
		bufferU[j++]=BEGIN_PROG2;			//internally timed
		bufferU[j++]=WAIT_T3;				//Tprogram
		bufferU[j++]=READ_DATA_PROG;
	}
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=3;
	bufferU[j++]=LOAD_DATA_PROG;			//Config word 0x2007
	bufferU[j++]=memCODE_W[0x2007]>>8;		//MSB
	bufferU[j++]=memCODE_W[0x2007]&0xff;		//LSB
	bufferU[j++]=BEGIN_PROG2;			//internally timed
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=READ_DATA_PROG;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(programID?125:35);
	for(i=0,z=0;programID&&i<4;i++){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (memCODE_W[0x2000+i]!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage3(strings[S_IDErr],i,memCODE_W[0x2000+i],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ID%d: written %04X, read %04X\r\n"
			err_c++;
		}
		z+=6;
	}
	if(ICDenable){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (ICDgoto!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage4(strings[S_ICDErr],0x2004,i,ICDgoto,(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ICD (0x%X): written %04X, read %04X\r\n"
			err_c++;
		}
		z+=6;
	}
	for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	if(~memCODE_W[0x2007]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
		PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2007],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
		err_c++;
	}
	err+=err_c;
	PrintMessage1(strings[S_ComplErr],err_c);	//"completed, %d errors\r\n"
	if(saveLog){
		fprintf(logfile,strings[S_Log9],err);	//"Area config. 	errors=%d \n"
	}
//****************** exit ********************
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

void Write16F87xA (int dim,int dim2,int seq)
// write 14 bit PIC
// dim=program size 	dim2=eeprom size
// seq=0: vdd + (50ms) + vdd&vpp
// seq=1: vdd + (50us) + vdd&vpp
// DevID@0x2006
// Config@0x2007
// write CONFIG2@0x2008 if different from 3FFF
// eeprom@0x2100
// erase:
// CHIP ERASE (11111) + 15ms
// write: LOAD_DATA_PROG (0010) + BEGIN_PROG2 (11000) + 1.2ms + END_PROGX (10111)
// write eeprom: LOAD_DATA_DATA (0011) + BEGIN_PROG (1000) + 12ms
// verify during write
{
	int err=0;
	WORD devID,config;
	int k=0,z=0,i,j,w;
	if(sizeW<0x2007){
		PrintMessage(strings[S_NoConfigW3]);	//"Can't find CONFIG (0x2007)\r\nEnd\r\n"
		return;
	}
	if(dim2){
		if(sizeW<0x2100){
			dim2=0;
			PrintMessage(strings[S_NoEEMem]);	//"Can't find EEPROM data\r\n"
		}
		else if(dim2>sizeW-0x2100) dim2=sizeW-0x2100;
	}
	if(saveLog){
		OpenLogFile();	//"log.txt"
		fprintf(logfile,"Write16F87xA(%d,%d,%d)\n",dim,dim2,seq);
	}
	for(i=0;i<sizeW;i++) memCODE_W[i]&=0x3FFF;
	unsigned int start=GetTickCount();
	bufferU[0]=0;
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=25000>>8;
	bufferU[j++]=25000&0xff;
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	if(seq==0){
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=WAIT_T3;
	}
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=NOP;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=0x06;
	bufferU[j++]=READ_DATA_PROG;	//DevID
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=READ_DATA_PROG;	//Config
	bufferU[j++]=EN_VPP_VCC;		//exit program mode
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(seq==0?90:40);
	for(z=0;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	devID=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_DevID],devID);	//"DevID: 0x%04X\r\n"
	PIC16_ID(devID);
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	config=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_ConfigWord],config);	//"Config word: 0x%04X\r\n"
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=15000>>8;
	bufferU[j++]=15000&0xff;
	if(ICDenable||programID){			//erase 0x2000-2004 also
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0x3F;				//fake config
		bufferU[j++]=0xFF;				//fake config
	}
	bufferU[j++]=CUST_CMD;
	bufferU[j++]=0x1F;					// CHIP_ERASE (11111)
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=EN_VPP_VCC;		//exit program mode
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=1200>>8;
	bufferU[j++]=1200&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(60);
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	PrintStatusSetup();
	for(w=i=k=0,j=0;i<dim;i++){
		if(memCODE_W[i]<0x3fff){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG2;			//externally timed
			bufferU[j++]=WAIT_T3;				//Tprogram
			bufferU[j++]=CUST_CMD;
			bufferU[j++]=0x17;					//END_PROGX (10111)
			bufferU[j++]=READ_DATA_PROG;
			w++;
		}
		bufferU[j++]=INC_ADDR;
		if(j>DIMBUF-11||i==dim-1){
			PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Writing: %d%%, add. %03X"
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(w*1.5+(6-w)*0.2+2);
			w=0;
			for(z=0;z<DIMBUF-6;z++){
				if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0x3fff) k++;
				else if(bufferI[z]==LOAD_DATA_PROG&&bufferI[z+4]==READ_DATA_PROG){
					if (memCODE_W[k]!=(bufferI[z+5]<<8)+bufferI[z+6]){
						PrintMessage("\r\n");
						PrintMessage3(strings[S_CodeWError2],k,memCODE_W[k],(bufferI[z+5]<<8)+bufferI[z+6]);	//"Error writing address %3X: written %04X, read %04X\r\n"
						err++;
						if(max_err&&err>max_err){
							PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
							PrintMessage(strings[S_IntW]);	//"Write interrupted"
							i=dim;
							z=DIMBUF;
						}
					}
					k++;
					z+=7;
				}
			}
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d \n"
			}
		}
	}
	err+=i-k;
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
//****************** write eeprom ********************
	if(dim2){
		int err_e=0;
		PrintMessage(strings[S_EEAreaW]);	//"Writing EEPROM ... "
		PrintStatusSetup();
		j=0;
		bufferU[j++]=SET_PARAMETER;
		bufferU[j++]=SET_T3;
		bufferU[j++]=12000>>8;
		bufferU[j++]=12000&0xff;
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=INC_ADDR_N;		//use only INC_ADDR_N so verification does not look at it
		bufferU[j++]=0xFF;
		bufferU[j++]=INC_ADDR_N;		//EEPROM: counter at 0x2100
		bufferU[j++]=1;
		for(w=0,i=k=0x2100;i<0x2100+dim2;i++){
			if(memCODE_W[i]<0xff){
				bufferU[j++]=LOAD_DATA_DATA;
				bufferU[j++]=memCODE_W[i]&0xff;
				bufferU[j++]=BEGIN_PROG;			//internally timed
				bufferU[j++]=WAIT_T3;				//Tprogram 8ms
				bufferU[j++]=READ_DATA_DATA;
				w++;
			}
			bufferU[j++]=INC_ADDR;
			if(j>DIMBUF-10||i==0x2100+dim2-1){
				PrintStatus(strings[S_CodeWriting],(i-0x2100+dim)*100/(dim+dim2),i);	//"Writing: %d%%, add. %03X"
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(w*12.5+5);
				w=0;
				for(z=0;z<DIMBUF-4;z++){
					if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0xff) k++;
					else if(bufferI[z]==LOAD_DATA_DATA&&bufferI[z+3]==READ_DATA_DATA){
						if (memCODE_W[k]!=bufferI[z+4]){
							PrintMessage("\r\n");
							PrintMessage3(strings[S_CodeWError3],k,memCODE_W[k],bufferI[z+4]);	//"Error writing address %4X: written %02X, read %02X\r\n"
							err_e++;
							if(max_err&&err>max_err){
								PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
								PrintMessage(strings[S_IntW]);	//"Write interrupted"
								i=0x2200;
								z=DIMBUF;
							}
						}
						k++;
						z+=5;
					}
				}
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d \n"
				}
			}
		}
		err_e+=i-k;
		err+=err_e;
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],err_e);	//"completed, %d errors\r\n"
	}
//****************** write ID, CONFIG, CALIB ********************
	PrintMessage(strings[S_ConfigAreaW]);	//"Writing CONFIG area ... "
	int err_c=0;
	int ICDgoto=0x2800+(ICDaddr&0x7FF);		//GOTO ICD routine (0x28xx)
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=12000>>8;
	bufferU[j++]=12000&0xff;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	if(programID){
		for(i=0x2000;i<0x2004;i++){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG;			//internally timed
			bufferU[j++]=WAIT_T3;				//Tprogram
			bufferU[j++]=READ_DATA_PROG;
			bufferU[j++]=INC_ADDR;
		}
	}
	else{
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=4;
	}
	if(ICDenable){		//write a GOTO ICD routine (0x28xx)
		bufferU[j++]=LOAD_DATA_PROG;
		bufferU[j++]=ICDgoto>>8;		//MSB
		bufferU[j++]=ICDgoto&0xFF;			//LSB
		bufferU[j++]=BEGIN_PROG;			//internally timed
		bufferU[j++]=WAIT_T3;				//Tprogram
		bufferU[j++]=READ_DATA_PROG;
	}
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=3;
	bufferU[j++]=LOAD_DATA_PROG;			//Config word 0x2007
	bufferU[j++]=memCODE_W[0x2007]>>8;		//MSB
	bufferU[j++]=memCODE_W[0x2007]&0xff;		//LSB
	bufferU[j++]=BEGIN_PROG;				//internally timed
	bufferU[j++]=WAIT_T3;					//Tprogram
	bufferU[j++]=READ_DATA_PROG;
	bufferU[j++]=INC_ADDR;
	if(sizeW>0x2008&&memCODE_W[0x2008]!=0x3fff){
		bufferU[j++]=LOAD_DATA_PROG;			//Config word2 0x2008
		bufferU[j++]=memCODE_W[0x2008]>>8;		//MSB
		bufferU[j++]=memCODE_W[0x2008]&0xff;		//LSB
		bufferU[j++]=BEGIN_PROG;				//internally timed
		bufferU[j++]=WAIT_T3;					//Tprogram
		bufferU[j++]=READ_DATA_PROG;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	i=28;
	if(programID) i+=50;
	if(ICDenable) i+=13;
	PacketIO(i);
	for(i=0,z=0;programID&&i<4;i++){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (memCODE_W[0x2000+i]!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage3(strings[S_IDErr],i,memCODE_W[0x2000+i],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ID%d: written %04X, read %04X\r\n"
			err_c++;
		}
		z+=6;
	}
	if(ICDenable){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (ICDgoto!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage4(strings[S_ICDErr],0x2004,i,ICDgoto,(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ICD (0x%X): written %04X, read %04X\r\n"
			err_c++;
		}
		z+=6;
	}
	for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	if(~memCODE_W[0x2007]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
		PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2007],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
		err_c++;
	}
	if(sizeW>0x2008&&memCODE_W[0x2008]!=0x3fff){
		for(z+=6;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if(~memCODE_W[0x2008]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
			PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2008],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
			err_c++;
		}
	}
	err+=err_c;
	PrintMessage1(strings[S_ComplErr],err_c);	//"completed, %d errors\r\n"
	if(saveLog){
		fprintf(logfile,strings[S_Log9],err);	//"Area config. 	errors=%d \n"
	}
//****************** exit ********************
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

void Write16F81x (int dim,int dim2)
// write 14 bit PIC
// dim=program size 	dim2=eeprom size
// seq=0: vdd + (50ms) + vdd&vpp
// seq=1: vdd + (50us) + vdd&vpp
// DevID@0x2006
// Config@0x2007
// write CONFIG2@0x2008 if different from 3FFF
// erase if protected: CHIP ERASE (11111) + 8ms
// erase if not protected:
// BULK_ERASE_PROG (1001) + BEGIN_PROG (1001) + 2ms + END_PROGX (10111)
// BULK_ERASE_DATA (1011) + BEGIN_PROG (1001) + 2ms + END_PROGX (10111)
// write: LOAD_DATA_PROG (0010) + BEGIN_PROG2 (11000) + 1.5ms + END_PROGX (10111)
// write eeprom: LOAD_DATA_DATA (0011) + BEGIN_PROG2 (11000) + 1.5ms + END_PROGX (10111)
// verify during write
{
	int err=0;
	WORD devID,config;
	int k=0,z=0,i,j,w;
	if(sizeW<0x2007){
		PrintMessage(strings[S_NoConfigW3]);	//"Can't find CONFIG (0x2007)\r\nEnd\r\n"
		return;
	}
	if(dim2){
		if(sizeW<0x2100){
			dim2=0;
			PrintMessage(strings[S_NoEEMem]);	//"Can't find EEPROM data\r\n"
		}
		else if(dim2>sizeW-0x2100) dim2=sizeW-0x2100;
	}
	if(saveLog){
		OpenLogFile();	//"log.txt"
		fprintf(logfile,"Write16F81x(%d,%d)\n",dim,dim2);
	}
	for(i=0;i<sizeW;i++) memCODE_W[i]&=0x3FFF;
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=25000>>8;
	bufferU[j++]=25000&0xff;
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=NOP;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=0x06;
	bufferU[j++]=READ_DATA_PROG;	//DevID
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=READ_DATA_PROG;	//Config
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(40);
	for(z=0;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	devID=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_DevID],devID);	//"DevID: 0x%04X\r\n"
	PIC16_ID(devID);
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	config=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_ConfigWord],config);	//"Config word: 0x%04X\r\n"
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	if(programID||ICDenable){
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0x3F;				//fake config
		bufferU[j++]=0xFF;				//fake config
	}
	if((config&0x2100)!=0x2100){
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x1F;					// CHIP_ERASE (11111)
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=WAIT_T3;
	}
	else{
		bufferU[j++]=BULK_ERASE_PROG;
		bufferU[j++]=BEGIN_PROG;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x17;					//END_PROGX (10111)
	}
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(15);
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	PrintStatusSetup();
	for(w=i=k=0,j=0;i<dim;i++){
		if(memCODE_W[i]<0x3fff){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG2;			//externally timed
			bufferU[j++]=WAIT_T3;				//Tprogram
			bufferU[j++]=CUST_CMD;
			bufferU[j++]=0x17;					//END_PROGX (10111)
			bufferU[j++]=READ_DATA_PROG;
			w++;
		}
		bufferU[j++]=INC_ADDR;
		if(j>DIMBUF-11||i==dim-1){
			PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Writing: %d%%, add. %03X"
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(w*2.5+3);
			w=0;
			for(z=0;z<DIMBUF-6;z++){
				if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0x3fff) k++;
				else if(bufferI[z]==LOAD_DATA_PROG&&bufferI[z+4]==READ_DATA_PROG){
					if (memCODE_W[k]!=(bufferI[z+5]<<8)+bufferI[z+6]){
						PrintMessage("\r\n");
						PrintMessage3(strings[S_CodeWError2],k,memCODE_W[k],(bufferI[z+5]<<8)+bufferI[z+6]);	//"Error writing address %3X: written %04X, read %04X\r\n"
						err++;
						if(max_err&&err>max_err){
							PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
							PrintMessage(strings[S_IntW]);	//"Write interrupted"
							i=dim;
							z=DIMBUF;
						}
					}
					k++;
					z+=7;
				}
			}
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d \n"
			}
		}
	}
	err+=i-k;
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
//****************** write eeprom ********************
	if(dim2){
		int err_e=0;
		PrintMessage(strings[S_EEAreaW]);	//"Writing EEPROM ... "
		PrintStatusSetup();
		j=0;
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=0x1;
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=0x0;
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x0;
		bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
		bufferU[j++]=EN_VPP_VCC;		//VDD
		bufferU[j++]=0x1;
		bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
		bufferU[j++]=0x5;
		bufferU[j++]=LOAD_DATA_DATA;
		bufferU[j++]=0x01;
		bufferU[j++]=BULK_ERASE_DATA;
		bufferU[j++]=BEGIN_PROG;
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x17;					//END_PROGX (10111)
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(5);
		j=0;
		for(w=0,i=k=0x2100;i<0x2100+dim2;i++){
			if(memCODE_W[i]<0xff){
				bufferU[j++]=LOAD_DATA_DATA;
				bufferU[j++]=memCODE_W[i]&0xff;
				bufferU[j++]=BEGIN_PROG2;			//externally timed
				bufferU[j++]=WAIT_T3;				//Tprogram
				bufferU[j++]=CUST_CMD;
				bufferU[j++]=0x17;					//END_PROGX (10111)
				bufferU[j++]=READ_DATA_DATA;
				w++;
			}
			bufferU[j++]=INC_ADDR;
			if(j>DIMBUF-10||i==0x2100+dim2-1){
				PrintStatus(strings[S_CodeWriting],(i-0x2100+dim)*100/(dim+dim2),i);	//"Writing: %d%%, add. %03X"
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(w*2.5+5);
				w=0;
				for(z=0;z<DIMBUF-5;z++){
					if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0xff) k++;
					else if(bufferI[z]==LOAD_DATA_DATA&&bufferI[z+4]==READ_DATA_DATA){
						if (memCODE_W[k]!=bufferI[z+5]){
							PrintMessage("\r\n");
							PrintMessage3(strings[S_CodeWError3],k,memCODE_W[k],bufferI[z+5]);	//"Error writing address %4X: written %02X, read %02X\r\n"
							err_e++;
							if(max_err&&err>max_err){
								PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
								PrintMessage(strings[S_IntW]);	//"Write interrupted"
								i=0x2200;
								z=DIMBUF;
							}
						}
						k++;
						z+=6;
					}
				}
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d \n"
				}
			}
		}
		err_e+=i-k;
		err+=err_e;
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],err_e);	//"completed, %d errors\r\n"
	}
//****************** write ID, CONFIG, CALIB ********************
	PrintMessage(strings[S_ConfigAreaW]);	//"Writing CONFIG area ... "
	int err_c=0;
	int ICDgoto=0x2800+(ICDaddr&0x7FF);		//GOTO ICD routine (0x28xx)
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	if(programID){
		for(i=0x2000;i<0x2004;i++){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG2;			//externally timed
			bufferU[j++]=WAIT_T3;				//Tprogram
			bufferU[j++]=CUST_CMD;
			bufferU[j++]=0x17;					//END_PROGX (10111)
			bufferU[j++]=READ_DATA_PROG;
			bufferU[j++]=INC_ADDR;
		}
	}
	else{
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=4;
	}
	if(ICDenable){		//write a GOTO ICD routine (0x28xx)
		bufferU[j++]=LOAD_DATA_PROG;
		bufferU[j++]=ICDgoto>>8;		//MSB
		bufferU[j++]=ICDgoto&0xFF;			//LSB
		bufferU[j++]=BEGIN_PROG2;				//externally timed
		bufferU[j++]=WAIT_T3;					//Tprogram
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x17;						//END_PROGX (10111)
		bufferU[j++]=READ_DATA_PROG;
	}
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=3;
	bufferU[j++]=LOAD_DATA_PROG;			//Config word 0x2007
	bufferU[j++]=memCODE_W[0x2007]>>8;		//MSB
	bufferU[j++]=memCODE_W[0x2007]&0xff;		//LSB
	bufferU[j++]=BEGIN_PROG2;				//externally timed
	bufferU[j++]=WAIT_T3;					//Tprogram
	bufferU[j++]=CUST_CMD;
	bufferU[j++]=0x17;						//END_PROGX (10111)
	bufferU[j++]=READ_DATA_PROG;
	bufferU[j++]=INC_ADDR;
	if(sizeW>0x2008&&memCODE_W[0x2008]!=0x3fff){
		bufferU[j++]=LOAD_DATA_PROG;			//Config word2 0x2008
		bufferU[j++]=memCODE_W[0x2008]>>8;		//MSB
		bufferU[j++]=memCODE_W[0x2008]&0xff;		//LSB
		bufferU[j++]=BEGIN_PROG2;				//externally timed
		bufferU[j++]=WAIT_T3;					//Tprogram
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x17;						//END_PROGX (10111)
		bufferU[j++]=READ_DATA_PROG;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(15);
	for(i=0,z=0;programID&&i<4;i++){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (memCODE_W[0x2000+i]!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage3(strings[S_IDErr],i,memCODE_W[0x2000+i],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ID%d: written %04X, read %04X\r\n"
			err_c++;
		}
		z+=7;
	}
	if(ICDenable){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (ICDgoto!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage4(strings[S_ICDErr],0x2004,i,ICDgoto,(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ICD (0x%X): written %04X, read %04X\r\n"
			err_c++;
		}
		z+=7;
	}
	for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	if(~memCODE_W[0x2007]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
		PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2007],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
		err_c++;
	}
	if(sizeW>0x2008&&memCODE_W[0x2008]!=0x3fff){
		for(z+=7;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if(~memCODE_W[0x2008]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
			PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2008],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
			err_c++;
		}
	}
	err+=err_c;
	PrintMessage1(strings[S_ComplErr],err_c);	//"completed, %d errors\r\n"
	if(saveLog){
		fprintf(logfile,strings[S_Log9],err);	//"Area config. 	errors=%d \n"
	}
//****************** exit ********************
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

void Write12F61x(int dim, int d, int d2)
// write 14 bit PIC
// dim=program size
// d not used
// vpp before vdd
// DevREV@0x2005
// DevID@0x2006
// Config@0x2007
// Calib1@0x2008 (save)
// Calib2@0x2009
// erase: BULK_ERASE_PROG (1001) +10ms
// write: LOAD_DATA_PROG (0010) + BEGIN_PROG2 (11000) + 4ms + END_PROG (1010)
// verify during write
{
	int err=0;
	WORD devID=0x3fff,devREV=0x3fff,calib1=0x3fff,calib2=0x3fff;
	int k=0,z=0,i,j,w;
	if(sizeW<0x2007){
		PrintMessage(strings[S_NoConfigW3]);	//"Can't find CONFIG (0x2007)\r\nEnd\r\n"
		return;
	}
	if(load_calibword){
		if(sizeW>0x2008) load_calibword=1;
		else{
			PrintMessage(strings[S_NoCalibW]);	//"Can't find calibration data\r\n"
			load_calibword=0;
		}
	}
	if(saveLog){
		OpenLogFile();	//"log.txt"
		fprintf(logfile,"Write12F61x(%d)\n",dim);
	}
	for(i=0;i<0x2009&&i<sizeW;i++) memCODE_W[i]&=0x3FFF;
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VPP
	bufferU[j++]=0x4;
	bufferU[j++]=NOP;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=NOP;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=0x05;
	bufferU[j++]=READ_DATA_PROG;	//DevREV
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=READ_DATA_PROG;	//DevID
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=READ_DATA_PROG;	//Calib1
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=READ_DATA_PROG;	//Calib2
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=10000>>8;
	bufferU[j++]=10000&0xff;
	bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(15);
	for(z=0;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	devREV=(bufferI[z+1]<<8)+bufferI[z+2];
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	devID=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_DevID],devID);	//"DevID: 0x%04X\r\n"
	if(devREV<0x3FFF) PrintMessage1(strings[S_DevREV],devREV);	//"DevREV: 0x%04X\r\n"
	PIC16_ID(devID);
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	calib1=(bufferI[z+1]<<8)+bufferI[z+2];
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	calib2=(bufferI[z+1]<<8)+bufferI[z+2];
	if(calib1<0x3fff) PrintMessage1(strings[S_CalibWord1],calib1);	//"Calib1: 0x%04X\r\n"
	if(calib2<0x3fff) PrintMessage1(strings[S_CalibWord2],calib2);	//"Calib2: 0x%04X\r\n"
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	j=0;
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VPP
	bufferU[j++]=0x4;
	bufferU[j++]=NOP;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=NOP;
	if(programID||load_calibword){
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=0xFF;				//fake config
		if(load_calibword){
			bufferU[j++]=INC_ADDR_N;
			bufferU[j++]=0x08;
		}
	}
	bufferU[j++]=BULK_ERASE_PROG;
	bufferU[j++]=WAIT_T3;			// delay T3=10ms
	bufferU[j++]=EN_VPP_VCC;		//exit program mode
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T3;			// delay T3=10ms after exiting program mode
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x4;
	bufferU[j++]=NOP;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x5;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=4000>>8;
	bufferU[j++]=4000&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(40);
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	PrintStatusSetup();
	for(w=i=k=0,j=0;i<dim;i++){
		if(memCODE_W[i]<0x3fff){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG2;			//externally timed, T=3ms min
			bufferU[j++]=WAIT_T3;				//Tprogram 4ms
			bufferU[j++]=END_PROG;
			bufferU[j++]=WAIT_T2;				//Tdischarge 100us
			bufferU[j++]=READ_DATA_PROG;
			w++;
		}
		bufferU[j++]=INC_ADDR;
		if(j>DIMBUF-12||i==dim-1){
			PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Writing: %d%%, add. %03X"
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(w*5+2.5);
			w=0;
			for(z=0;z<DIMBUF-7;z++){
				if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0x3fff) k++;
				else if(bufferI[z]==LOAD_DATA_PROG&&bufferI[z+5]==READ_DATA_PROG){
					if (memCODE_W[k]!=(bufferI[z+6]<<8)+bufferI[z+7]){
						PrintMessage("\r\n");
						PrintMessage3(strings[S_CodeWError2],k,memCODE_W[k],(bufferI[z+6]<<8)+bufferI[z+7]);	//"Error writing address %3X: written %04X, read %04X\r\n"
						err++;
						if(max_err&&err>max_err){
							PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
							PrintMessage(strings[S_IntW]);	//"Write interrupted"
							i=dim;
							z=DIMBUF;
						}
					}
					k++;
					z+=8;
				}
			}
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d,\n"
			}
		}
	}
	err+=i-k;
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
//****************** write ID, CONFIG, CALIB ********************
	PrintMessage(strings[S_ConfigAreaW]);	//"Writing CONFIG area ... "
	int err_c=0;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	if(programID){
		for(i=0x2000;i<0x2004;i++){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG2;			//externally timed, T=3ms min
			bufferU[j++]=WAIT_T3;				//Tprogram 4ms
			bufferU[j++]=END_PROG;
			bufferU[j++]=WAIT_T2;				//Tdischarge 100us
			bufferU[j++]=READ_DATA_PROG;
			bufferU[j++]=INC_ADDR;
		}
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=3;
	}
	else{
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=7;
	}
	bufferU[j++]=LOAD_DATA_PROG;			//Config word 0x2007
	bufferU[j++]=memCODE_W[0x2007]>>8;		//MSB
	bufferU[j++]=memCODE_W[0x2007]&0xff;		//LSB
	bufferU[j++]=BEGIN_PROG2;			//externally timed, T=3ms min
	bufferU[j++]=WAIT_T3;				//Tprogram 4ms
	bufferU[j++]=END_PROG;
	bufferU[j++]=WAIT_T2;				//Tdischarge 100us
	bufferU[j++]=READ_DATA_PROG;
	bufferU[j++]=INC_ADDR;
	if(load_calibword){
		bufferU[j++]=LOAD_DATA_PROG;			//Calib word 1
		bufferU[j++]=memCODE_W[0x2008]>>8;		//MSB
		bufferU[j++]=memCODE_W[0x2008]&0xff;		//LSB
		bufferU[j++]=BEGIN_PROG2;			//externally timed, T=3ms min
		bufferU[j++]=WAIT_T3;				//Tprogram 4ms
		bufferU[j++]=END_PROG;
		bufferU[j++]=WAIT_T2;				//Tdischarge 100us
		bufferU[j++]=READ_DATA_PROG;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(35);
	for(i=0,z=0;programID&&i<4;i++){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (memCODE_W[0x2000+i]!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage("\r\n");
			PrintMessage3(strings[S_IDErr],i,memCODE_W[0x2000+i],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ID%d: written %04X, read %04X\r\n"
			err_c++;
		}
		z+=8;
	}
	for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	if(~memCODE_W[0x2007]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2007],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
		err_c++;
	}
	if(load_calibword){
		for(z+=8;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (memCODE_W[0x2008]!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage("\r\n");
			PrintMessage2(strings[S_Calib1Err],memCODE_W[0x2008],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing Calib1: written %04X, read %04X\r\n"
			err_c++;
		}
	}
	err+=err_c;
	PrintMessage1(strings[S_ComplErr],err_c);	//"completed, %d errors\r\n"
	if(saveLog){
		fprintf(logfile,strings[S_Log9],err);	//"Area config. 	errors=%d \n"
	}
//****************** exit ********************
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

void Write16F88x(int dim,int dim2)
// write 14 bit PIC
// dim=program size 	dim2=eeprom size
// vpp before vdd
// DevID@0x2006
// Config@0x2007
// Config2@0x2008
// Calib1@0x2009 (salva)
// eeprom@0x2100
// erase: BULK_ERASE_PROG (1001) +6ms
// write:LOAD_DATA_PROG (0010) + BEGIN_PROG (1000) + 3ms
// eeprom:	BULK_ERASE_DATA (1011) + 6ms
//			LOAD_DATA_DATA (0011) + BEGIN_PROG (1000) + 6ms
// verify during write
{
	int err=0;
	WORD devID=0x3fff,calib1=0x3fff;
	int k=0,z=0,i,j,w;
	if(sizeW<0x2009){
		PrintMessage(strings[S_NoConfigW4]);	//"Can't find CONFIG (0x2008)\r\nEnd\r\n"
		return;
	}
	if(load_calibword){
		if(sizeW>0x200A) load_calibword=1;
		else{
			PrintMessage(strings[S_NoCalibW]);	//"Can't find calibration data\r\n"
			load_calibword=0;
		}
	}
	if(dim2){
		if(sizeW<0x2100){
			dim2=0;
			PrintMessage(strings[S_NoEEMem]);	//"Can't find EEPROM data\r\n"
		}
		else if(dim2>sizeW-0x2100) dim2=sizeW-0x2100;
	}
	if(saveLog){
		OpenLogFile();	//"log.txt"
		fprintf(logfile,"Write16F88x(%d,%d)\n",dim,dim2);
	}
	for(i=0;i<0x200A&&i<sizeW;i++) memCODE_W[i]&=0x3FFF;
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VPP
	bufferU[j++]=0x4;
	bufferU[j++]=NOP;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=NOP;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=0x06;
	bufferU[j++]=READ_DATA_PROG;	//DevID
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=READ_DATA_PROG;	//Calib1
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=6000>>8;
	bufferU[j++]=6000&0xff;
	bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(15);
	for(z=0;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	devID=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_DevID],devID);	//"DevID: 0x%04X\r\n"
	PIC16_ID(devID);
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	calib1=(bufferI[z+1]<<8)+bufferI[z+2];
	if(calib1<0x3fff){
		PrintMessage1(strings[S_CalibWord1],calib1);	//"Calib1: 0x%04X\r\n"
	}
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	j=0;
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VPP
	bufferU[j++]=0x4;
	bufferU[j++]=NOP;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=NOP;
	if(programID||load_calibword||ICDenable){
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=0xFF;				//fake config
		if(load_calibword){
			bufferU[j++]=INC_ADDR_N;
			bufferU[j++]=9;
		}
	}
	bufferU[j++]=BULK_ERASE_PROG;
	bufferU[j++]=WAIT_T3;			// delay T3=6ms
	bufferU[j++]=EN_VPP_VCC;		//exit program mode
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T3;			// delay T3=6ms after exiting program mode
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x4;
	bufferU[j++]=NOP;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x5;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=3000>>8;
	bufferU[j++]=3000&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(40);
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	PrintStatusSetup();
	for(w=i=k=0,j=0;i<dim;i++){
		if(memCODE_W[i]<0x3fff){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG;			//internally timed, T=3ms min
			bufferU[j++]=WAIT_T3;				//Tprogram 3ms
			bufferU[j++]=READ_DATA_PROG;
			w++;
		}
		bufferU[j++]=INC_ADDR;
		if(j>DIMBUF-12||i==dim-1){
			PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Writing: %d%%, add. %03X"
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(w*3+2);
			w=0;
			for(z=0;z<DIMBUF-5;z++){
				if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0x3fff) k++;
				else if(bufferI[z]==LOAD_DATA_PROG&&bufferI[z+3]==READ_DATA_PROG){
					if (memCODE_W[k]!=(bufferI[z+4]<<8)+bufferI[z+5]){
						PrintMessage("\r\n");
						PrintMessage3(strings[S_CodeWError2],k,memCODE_W[k],(bufferI[z+4]<<8)+bufferI[z+5]);	//"Error writing address %3X: written %04X, read %04X\r\n"
						err++;
						if(max_err&&err>max_err){
							PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
							PrintMessage(strings[S_IntW]);	//"Write interrupted"
							i=dim;
							z=DIMBUF;
						}
					}
					k++;
					z+=6;
				}
			}
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d,\n"
			}
		}
	}
	err+=i-k;
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
//****************** write eeprom ********************
	if(dim2){
		PrintMessage(strings[S_EEAreaW]);	//"Writing EEPROM ... "
		PrintStatusSetup();
		j=0;
		bufferU[j++]=SET_PARAMETER;
		bufferU[j++]=SET_T3;
		bufferU[j++]=6000>>8;
		bufferU[j++]=6000&0xff;
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=BULK_ERASE_DATA;
		bufferU[j++]=WAIT_T3;			// delay T3=6ms
		bufferU[j++]=INC_ADDR_N;		//use only INC_ADDR_N so verification does not look at it
		bufferU[j++]=0xFF;
		bufferU[j++]=INC_ADDR_N;		//EEPROM: counter at 0x2100
		bufferU[j++]=1;
		for(w=2,i=k=0x2100;i<0x2100+dim2;i++){
			if(memCODE_W[i]<0xff){
				bufferU[j++]=LOAD_DATA_DATA;
				bufferU[j++]=memCODE_W[i]&0xff;
				bufferU[j++]=BEGIN_PROG;			//internally timed, T=6ms min
				bufferU[j++]=WAIT_T3;				//Tprogram 6ms
				bufferU[j++]=READ_DATA_DATA;
				w++;
			}
			bufferU[j++]=INC_ADDR;
			if(j>DIMBUF-12||i==0x2100+dim2-1){
				PrintStatus(strings[S_CodeWriting],(i-0x2100+dim)*100/(dim+dim2),i);	//"Writing: %d%%, add. %03X"
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(w*6.5+2);
				w=0;
				for(z=0;z<DIMBUF-4;z++){
					if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0xff) k++;
					else if(bufferI[z]==LOAD_DATA_DATA&&bufferI[z+3]==READ_DATA_DATA){
						if (memCODE_W[k]!=bufferI[z+4]){
							PrintMessage("\r\n");
							PrintMessage3(strings[S_CodeWError3],k,memCODE_W[k],bufferI[z+4]);	//"Error writing address %4X: written %02X, read %02X\r\n"
							err++;
							if(max_err&&err>max_err){
								PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
								PrintMessage(strings[S_IntW]);	//"Write interrupted"
								i=0x2200;
								z=DIMBUF;
							}
						}
						k++;
						z+=5;
					}
				}
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d \n"
				}
			}
		}
		err+=i-k;
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],i-k);	//"completed, %d errors\r\n"
	}
//****************** write ID, CONFIG, CALIB ********************
	PrintMessage(strings[S_ConfigAreaW]);	//"Writing CONFIG area ... "
	int err_c=0;
	int ICDgoto=0x2800+(ICDaddr&0x7FF);		//GOTO ICD routine (0x28xx)
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=3000>>8;
	bufferU[j++]=3000&0xff;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	if(programID){
		for(i=0x2000;i<0x2004;i++){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG;			//internally timed, T=3ms min
			bufferU[j++]=WAIT_T3;				//Tprogram 3ms
			bufferU[j++]=READ_DATA_PROG;
			bufferU[j++]=INC_ADDR;
		}
	}
	else{
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=4;
	}
	if(ICDenable){		//write a GOTO ICD routine (0x28xx)
		bufferU[j++]=LOAD_DATA_PROG;
		bufferU[j++]=ICDgoto>>8;			//MSB
		bufferU[j++]=ICDgoto&0xFF;			//LSB
		bufferU[j++]=BEGIN_PROG;			//internally timed, T=3ms min
		bufferU[j++]=WAIT_T3;				//Tprogram 3ms
		bufferU[j++]=READ_DATA_PROG;
	}
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=3;
	bufferU[j++]=LOAD_DATA_PROG;			//Config word 0x2007
	bufferU[j++]=memCODE_W[0x2007]>>8;		//MSB
	bufferU[j++]=memCODE_W[0x2007]&0xff;		//LSB
	bufferU[j++]=BEGIN_PROG;				//internally timed, T=3ms min
	bufferU[j++]=WAIT_T3;					//Tprogram 3ms
	bufferU[j++]=READ_DATA_PROG;
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=LOAD_DATA_PROG;			//Config word2 0x2008
	bufferU[j++]=memCODE_W[0x2008]>>8;		//MSB
	bufferU[j++]=memCODE_W[0x2008]&0xff;		//LSB
	bufferU[j++]=BEGIN_PROG;				//internally timed, T=3ms min
	bufferU[j++]=WAIT_T3;					//Tprogram 3ms
	bufferU[j++]=READ_DATA_PROG;
	bufferU[j++]=INC_ADDR;
	if(load_calibword){
		bufferU[j++]=LOAD_DATA_PROG;		//Calib word 1
		bufferU[j++]=memCODE_W[0x2009]>>8;	//MSB
		bufferU[j++]=memCODE_W[0x2009]&0xff;	//LSB
		bufferU[j++]=BEGIN_PROG;			//internally timed, T=3ms min
		bufferU[j++]=WAIT_T3;				//Tprogram 3ms
		bufferU[j++]=READ_DATA_PROG;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(35);
	for(i=0,z=0;programID&&i<4;i++){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (memCODE_W[0x2000+i]!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage("\r\n");
			PrintMessage3(strings[S_IDErr],i,memCODE_W[0x2000+i],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ID%d: written %04X, read %04X\r\n"
			err_c++;
		}
		z+=6;
	}
	if(ICDenable){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (ICDgoto!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage4(strings[S_ICDErr],0x2004,i,ICDgoto,(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ICD (0x%X): written %04X, read %04X\r\n"
			err_c++;
		}
		z+=6;
	}
	for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	if(~memCODE_W[0x2007]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2007],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
		err_c++;
	}
	for(z+=6;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	if(~memCODE_W[0x2008]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2008],(bufferI[z+1]<<8)+bufferI[z+2]);	//"\r\n"
		err_c++;
	}
	if(load_calibword){
		for(z+=6;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (memCODE_W[0x2009]!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage("\r\n");
			PrintMessage2(strings[S_Calib1Err],memCODE_W[0x2009],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing Calib1: written %04X, read %04X\r\n"
			err_c++;
		}
	}
	err+=err_c;
	PrintMessage1(strings[S_ComplErr],err_c);	//"completed, %d errors\r\n"
	if(saveLog){
		fprintf(logfile,strings[S_Log9],err);	//"Area config. 	errors=%d \n"
	}
//****************** exit ********************
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

void Write16F7x(int dim,int vdd)
// dim=program size
// write 14 bit PIC
// vdd=0  vdd +50ms before vpp
// vdd=1  vdd before vpp
// DevID@0x2006
// Config@0x2007
// Config2@0x2008
// erase: BULK_ERASE_PROG (1001) +30ms
// write:LOAD_DATA_PROG (0010) + BEGIN_PROG (1000) + 1ms + END_PROG2(1110)
// verify during write
{
	int err=0;
	WORD devID=0x3fff;
	int k=0,z=0,i,j,w;
	if(sizeW<0x2007){
		PrintMessage(strings[S_NoConfigW3]);	//"Can't find CONFIG (0x2007)\r\nEnd\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"log.txt"
		fprintf(logfile,"Write16F7x(%d,%d)\n",dim,vdd);
	}
	for(i=0;i<0x2009&&i<sizeW;i++) memCODE_W[i]&=0x3FFF;
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=10000>>8;
	bufferU[j++]=10000&0xff;
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	if(vdd==0){
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
	}
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=NOP;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=0x06;
	bufferU[j++]=READ_DATA_PROG;	//DevID
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(vdd?20:70);
	for(z=0;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	devID=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_DevID],devID);	//"DevID: 0x%04X\r\n"
	PIC16_ID(devID);
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	j=0;
	//enter program mode
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	if(vdd==0){
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
	}
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=NOP;
	bufferU[j++]=BULK_ERASE_PROG;
	bufferU[j++]=WAIT_T3;			// delay T3=10ms
	bufferU[j++]=WAIT_T3;			// delay T3=10ms
	bufferU[j++]=WAIT_T3;			// delay T3=10ms
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(vdd?50:100);
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
	j=0;
	//enter program mode
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	if(vdd==0){
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
	}
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=1000>>8;
	bufferU[j++]=1000&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(vdd?2:52);
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	PrintStatusSetup();
	for(w=i=k=0,j=0;i<dim;i++){
		if(memCODE_W[i]<0x3fff){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG;			//externally timed, T=1ms min
			bufferU[j++]=WAIT_T3;				//Tprogram 1ms min
			bufferU[j++]=END_PROG2;
			bufferU[j++]=READ_DATA_PROG;
			w++;
		}
		bufferU[j++]=INC_ADDR;
		if(j>DIMBUF-10||i==dim-1){
			PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Writing: %d%%, add. %03X"
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(w*1.5+2);
			w=0;
			for(z=0;z<DIMBUF-6;z++){
				if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0x3fff) k++;
				else if(bufferI[z]==LOAD_DATA_PROG&&bufferI[z+4]==READ_DATA_PROG){
					if (memCODE_W[k]!=(bufferI[z+5]<<8)+bufferI[z+6]){
						PrintMessage("\r\n");
						PrintMessage3(strings[S_CodeWError2],k,memCODE_W[k],(bufferI[z+5]<<8)+bufferI[z+6]);	//"Error writing address %3X: written %04X, read %04X\r\n"
						err++;
						if(max_err&&err>max_err){
							PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
							PrintMessage(strings[S_IntW]);	//"Write interrupted"
							i=dim;
							z=DIMBUF;
						}
					}
					k++;
					z+=7;
				}
			}
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d,\n"
			}
		}
	}
	err+=i-k;
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
//****************** write ID, CONFIG, CALIB ********************
	PrintMessage(strings[S_ConfigAreaW]);	//"Writing CONFIG area ... "
	int err_c=0;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	if(programID){
		for(i=0x2000;i<0x2004;i++){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG;			//externally timed, T=1ms min
			bufferU[j++]=WAIT_T3;				//Tprogram 1ms
			bufferU[j++]=END_PROG2;
			bufferU[j++]=READ_DATA_PROG;
			bufferU[j++]=INC_ADDR;
		}
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=3;
	}
	else{
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=7;
	}
	bufferU[j++]=LOAD_DATA_PROG;			//Config word 0x2007
	bufferU[j++]=memCODE_W[0x2007]>>8;		//MSB
	bufferU[j++]=memCODE_W[0x2007]&0xff;		//LSB
	bufferU[j++]=BEGIN_PROG;			//externally timed, T=1ms min
	bufferU[j++]=WAIT_T3;				//Tprogram 1ms
	bufferU[j++]=END_PROG2;
	bufferU[j++]=WAIT_T3;				//Tprogram 1ms
	bufferU[j++]=READ_DATA_PROG;
	bufferU[j++]=INC_ADDR;
	if(sizeW>0x2008&&memCODE_W[0x2008]<0x3fff){
		bufferU[j++]=LOAD_DATA_PROG;			//Config word 2 0x2008
		bufferU[j++]=memCODE_W[0x2008]>>8;		//MSB
		bufferU[j++]=memCODE_W[0x2008]&0xff;		//LSB
		bufferU[j++]=BEGIN_PROG;			//externally timed, T=1ms min
		bufferU[j++]=WAIT_T3;				//Tprogram 1ms
		bufferU[j++]=END_PROG2;
		bufferU[j++]=WAIT_T3;				//Tprogram 1ms
		bufferU[j++]=READ_DATA_PROG;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(15);
	for(i=0,z=0;programID&&i<4;i++){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (memCODE_W[0x2000+i]!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage("\r\n");
			PrintMessage3(strings[S_IDErr],i,memCODE_W[0x2000+i],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ID%d: written %04X, read %04X\r\n"
			err_c++;
		}
		z+=6;
	}
	for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	if(~memCODE_W[0x2007]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2007],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
		err_c++;
	}
	if(sizeW>0x2008&&memCODE_W[0x2008]<0x3fff){
		for(z+=6;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if(~memCODE_W[0x2008]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2008],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
			err_c++;
		}
	}
	err+=err_c;
	PrintMessage1(strings[S_ComplErr],err_c);	//"completed, %d errors\r\n"
	if(saveLog){
		fprintf(logfile,strings[S_Log9],err);	//"Area config. 	errors=%d \n"
	}
//****************** exit ********************
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

void Write16F71x(int dim,int vdd)
// write 14 bit PIC
// dim=program size
// vdd=0  vdd +50ms before vpp
// vdd=1  vdd before vpp
// DevID@0x2006
// Config@0x2007
// erase: BULK_ERASE_PROG (1001) +6ms
// write:LOAD_DATA_PROG (0010) + BEGIN_PROG2 (11000) + 2ms + END_PROG2(1110)
// verify during write
{
	int err=0;
	WORD devID=0x3fff;
	int k=0,z=0,i,j,w;
	if(sizeW<0x2007){
		PrintMessage(strings[S_NoConfigW3]);	//"Can't find CONFIG (0x2007)\r\nEnd\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"log.txt"
		fprintf(logfile,"Write16F71x(%d,%d)\n",dim,vdd);
	}
	for(i=0;i<0x2009&&i<sizeW;i++) memCODE_W[i]&=0x3FFF;
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=10000>>8;
	bufferU[j++]=10000&0xff;
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	if(vdd==0){
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
	}
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=NOP;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=0x06;
	bufferU[j++]=READ_DATA_PROG;	//DevID
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(vdd?20:70);
	for(z=0;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	devID=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_DevID],devID);	//"DevID: 0x%04X\r\n"
	PIC16_ID(devID);
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	j=0;
	//enter program mode
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	if(vdd==0){
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
	}
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=NOP;
	if(programID){
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=0xFF;				//fake config
	}
	bufferU[j++]=BULK_ERASE_PROG;
	bufferU[j++]=WAIT_T3;			// delay T3=10ms
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=WAIT_T3;			//delay after exiting prog mode
	//enter program mode
	bufferU[j++]=EN_VPP_VCC;		//VDD
	bufferU[j++]=0x1;
	if(vdd==0){
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
		bufferU[j++]=WAIT_T3;			//delay between vdd and vpp
	}
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(vdd?30:130);
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	PrintStatusSetup();
	for(w=i=k=0,j=0;i<dim;i++){
		if(memCODE_W[i]<0x3fff){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG2;			//externally timed, T=1ms min
			bufferU[j++]=WAIT_T3;				//Tprogram 1ms min
			bufferU[j++]=END_PROG2;
			bufferU[j++]=WAIT_T2;				//Tdischarge 100us
			bufferU[j++]=READ_DATA_PROG;
			w++;
		}
		bufferU[j++]=INC_ADDR;
		if(j>DIMBUF-11||i==dim-1){
			PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Writing: %d%%, add. %03X"
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(w*2.5+2);
			w=0;
			for(z=0;z<DIMBUF-7;z++){
				if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0x3fff) k++;
				else if(bufferI[z]==LOAD_DATA_PROG&&bufferI[z+5]==READ_DATA_PROG){
					if (memCODE_W[k]!=(bufferI[z+6]<<8)+bufferI[z+7]){
						PrintMessage("\r\n");
						PrintMessage3(strings[S_CodeWError2],k,memCODE_W[k],(bufferI[z+6]<<8)+bufferI[z+7]);	//"Error writing address %3X: written %04X, read %04X\r\n"
						err++;
						if(max_err&&err>max_err){
							PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
							PrintMessage(strings[S_IntW]);	//"Write interrupted"
							i=dim;
							z=DIMBUF;
						}
					}
					k++;
					z+=8;
				}
			}
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d,\n"
			}
		}
	}
	err+=i-k;
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
//****************** write ID, CONFIG, CALIB ********************
	PrintMessage(strings[S_ConfigAreaW]);	//"Writing CONFIG area ... "
	int err_c=0;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	if(programID){
		for(i=0x2000;i<0x2004;i++){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG2;			//externally timed, T=1ms min
			bufferU[j++]=WAIT_T3;				//Tprogram 1ms
			bufferU[j++]=END_PROG2;
			bufferU[j++]=WAIT_T2;				//Tdischarge 100us
			bufferU[j++]=READ_DATA_PROG;
			bufferU[j++]=INC_ADDR;
		}
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=3;
	}
	else{
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=7;
	}
	bufferU[j++]=LOAD_DATA_PROG;			//Config word 0x2007
	bufferU[j++]=memCODE_W[0x2007]>>8;		//MSB
	bufferU[j++]=memCODE_W[0x2007]&0xff;		//LSB
	bufferU[j++]=BEGIN_PROG2;			//externally timed, T=1ms min
	bufferU[j++]=WAIT_T3;				//Tprogram 1ms
	bufferU[j++]=END_PROG2;
	bufferU[j++]=WAIT_T2;				//Tdischarge 100us
	bufferU[j++]=READ_DATA_PROG;
	bufferU[j++]=INC_ADDR;
	if(sizeW>0x2008&&memCODE_W[0x2008]<0x3fff){
		bufferU[j++]=LOAD_DATA_PROG;			//Config word 2 0x2008
		bufferU[j++]=memCODE_W[0x2008]>>8;		//MSB
		bufferU[j++]=memCODE_W[0x2008]&0xff;		//LSB
		bufferU[j++]=BEGIN_PROG2;			//externally timed, T=1ms min
		bufferU[j++]=WAIT_T3;				//Tprogram 1ms
		bufferU[j++]=END_PROG2;
		bufferU[j++]=WAIT_T2;				//Tdischarge 100us
		bufferU[j++]=READ_DATA_PROG;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(15);
	for(i=0,z=0;programID&&i<4;i++){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (memCODE_W[0x2000+i]!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage("\r\n");
			PrintMessage3(strings[S_IDErr],i,memCODE_W[0x2000+i],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ID%d: written %04X, read %04X\r\n"
			err_c++;
		}
		z+=6;
	}
	for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	if(~memCODE_W[0x2007]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2007],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
		err_c++;
	}
	if(sizeW>0x2008&&memCODE_W[0x2008]<0x3fff){
		for(z+=6;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if(~memCODE_W[0x2008]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2008],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
			err_c++;
		}
	}
	err+=err_c;
	PrintMessage1(strings[S_ComplErr],err_c);	//"completed, %d errors\r\n"
	if(saveLog){
		fprintf(logfile,strings[S_Log9],err);	//"Area config. 	errors=%d \n"
	}
//****************** exit ********************
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

void Write16F72x(int dim, int d, int d2)
// write 14 bit PIC
// dim=program size
// d not used
// vpp before vdd
// DevID@0x2006
// Config@0x2007
// Config2@0x2008 (not used on LF devices)
// erase: BULK_ERASE_PROG (1001) +6ms
// write:LOAD_DATA_PROG (0010) + BEGIN_PROG (1000) + 2.5ms
// verify during write
{
	int err=0;
	WORD devID=0x3fff;
	int k=0,z=0,i,j,w;
	if(!CheckV33Regulator()){
		PrintMessage(strings[S_noV33reg]);	//Can't find 3.3V expansion board
		return;
	}
	if(!StartHVReg(8.5)){
		PrintMessage(strings[S_HVregErr]); //"HV regulator error\r\n"
		return;
	}
	if(sizeW<0x2008){
		PrintMessage(strings[S_NoConfigW3]);	//"Can't find CONFIG (0x2007)\r\nEnd\r\n"
		return;
	}
	if(saveLog){
		OpenLogFile();	//"log.txt"
		fprintf(logfile,"Write16F72x(%d)\n",dim);
	}
	for(i=0;i<0x2009&&i<sizeW;i++) memCODE_W[i]&=0x3FFF;
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=EN_VPP_VCC;		//VPP
	bufferU[j++]=0x4;
	bufferU[j++]=EN_VPP_VCC;		//VDD+VPP
	bufferU[j++]=0x5;
	bufferU[j++]=WAIT_T2;
	bufferU[j++]=WAIT_T2;
	bufferU[j++]=WAIT_T2;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=0x06;
	bufferU[j++]=READ_DATA_PROG;	//DevID
	bufferU[j++]=CUST_CMD;
	bufferU[j++]=0x16;		//Reset address
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=6000>>8;
	bufferU[j++]=6000&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(3);
	for(z=0;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	devID=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_DevID],devID);	//"DevID: 0x%04X\r\n"
	PIC16_ID(devID);
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erase ... "
	j=0;
	if(programID||ICDenable){
		bufferU[j++]=LOAD_CONF;			//counter at 0x2000
		bufferU[j++]=0xFF;				//fake config
		bufferU[j++]=0xFF;				//fake config
	}
	bufferU[j++]=READ_DATA_PROG;	//DevID
	bufferU[j++]=BULK_ERASE_PROG;
	bufferU[j++]=WAIT_T3;			// delay T3=6ms
	bufferU[j++]=READ_DATA_PROG;	//DevID
	bufferU[j++]=CUST_CMD;
	bufferU[j++]=0x16;		//Reset address
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2500>>8;
	bufferU[j++]=2500&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(18);
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	PrintStatusSetup();
	for(w=i=k=0,j=0;i<dim;i++){
		if(memCODE_W[i]<0x3fff){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG;			//internally timed, T=3ms min
			bufferU[j++]=WAIT_T3;				//Tprogram 3ms
			bufferU[j++]=READ_DATA_PROG;
			w++;
		}
		bufferU[j++]=INC_ADDR;
		if(j>DIMBUF-12||i==dim-1){
			PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Writing: %d%%, add. %03X"
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(w*2.5+2);
			w=0;
			for(z=0;z<DIMBUF-5;z++){
				if(bufferI[z]==INC_ADDR&&memCODE_W[k]>=0x3fff) k++;
				else if(bufferI[z]==LOAD_DATA_PROG&&bufferI[z+3]==READ_DATA_PROG){
					if (memCODE_W[k]!=(bufferI[z+4]<<8)+bufferI[z+5]){
						PrintMessage("\r\n");
						PrintMessage3(strings[S_CodeWError2],k,memCODE_W[k],(bufferI[z+4]<<8)+bufferI[z+5]);	//"Error writing address %3X: written %04X, read %04X\r\n"
						err++;
						if(max_err&&err>max_err){
							PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
							PrintMessage(strings[S_IntW]);	//"Write interrupted"
							i=dim;
							z=DIMBUF;
						}
					}
					k++;
					z+=6;
				}
			}
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d,\n"
			}
		}
	}
	err+=i-k;
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
//****************** write ID, CONFIG, CALIB ********************
	PrintMessage(strings[S_ConfigAreaW]);	//"Writing CONFIG area ... "
	int err_c=0;
	int ICDgoto=0x2800+(ICDaddr&0x7FF);		//GOTO ICD routine (0x28xx)
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=6000>>8;
	bufferU[j++]=6000&0xff;
	bufferU[j++]=LOAD_CONF;			//counter at 0x2000
	bufferU[j++]=0xFF;				//fake config
	bufferU[j++]=0xFF;				//fake config
	if(programID){
		for(i=0x2000;i<0x2004;i++){
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG;			//internally timed, T=3ms min
			bufferU[j++]=WAIT_T3;				//Tprogram 3ms
			bufferU[j++]=READ_DATA_PROG;
			bufferU[j++]=INC_ADDR;
		}
	}
	else{
		bufferU[j++]=INC_ADDR_N;
		bufferU[j++]=4;
	}
	if(ICDenable){		//write a GOTO ICD routine (0x28xx)
		bufferU[j++]=LOAD_DATA_PROG;
		bufferU[j++]=ICDgoto>>8;		//MSB
		bufferU[j++]=ICDgoto&0xFF;			//LSB
		bufferU[j++]=BEGIN_PROG;			//internally timed, T=3ms min
		bufferU[j++]=WAIT_T3;				//Tprogram 3ms
		bufferU[j++]=READ_DATA_PROG;
	}
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=3;
	bufferU[j++]=LOAD_DATA_PROG;			//Config word 0x2007
	bufferU[j++]=memCODE_W[0x2007]>>8;		//MSB
	bufferU[j++]=memCODE_W[0x2007]&0xff;		//LSB
	bufferU[j++]=BEGIN_PROG;				//internally timed, T=5ms min
	bufferU[j++]=WAIT_T3;					//Tprogram 3ms
	bufferU[j++]=READ_DATA_PROG;
	bufferU[j++]=INC_ADDR;
	if(sizeW>0x2008){						//only if Config2 is present
		bufferU[j++]=LOAD_DATA_PROG;			//Config word 2 0x2008
		bufferU[j++]=memCODE_W[0x2008]>>8;		//MSB
		bufferU[j++]=memCODE_W[0x2008]&0xff;		//LSB
		bufferU[j++]=BEGIN_PROG;				//internally timed, T=5ms min
		bufferU[j++]=WAIT_T3;					//Tprogram 3ms
		bufferU[j++]=READ_DATA_PROG;
		bufferU[j++]=FLUSH;
	}
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(45);
	for(i=0,z=0;programID&&i<4;i++){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (memCODE_W[0x2000+i]!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage("\r\n");
			PrintMessage3(strings[S_IDErr],i,memCODE_W[0x2000+i],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ID%d: written %04X, read %04X\r\n"
			err_c++;
		}
		z+=6;
	}
	if(ICDenable){
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if (ICDgoto!=(bufferI[z+1]<<8)+bufferI[z+2]){
			PrintMessage4(strings[S_ICDErr],0x2004,i,ICDgoto,(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ICD (0x%X): written %04X, read %04X\r\n"
			err_c++;
		}
		z+=6;
	}
	for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	if(~memCODE_W[0x2007]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
		PrintMessage("\r\n");
		PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2007],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
		err_c++;
	}
	if(sizeW>0x2008){						//only if Config2 is present
		for(z+=6;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if(~memCODE_W[0x2008]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x2008],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
			err_c++;
		}
	}
	err+=err_c;
	PrintMessage1(strings[S_ComplErr],err_c);	//"completed, %d errors\r\n"
	if(saveLog){
		fprintf(logfile,strings[S_Log9],err);	//"Area config. 	errors=%d \n"
	}
//****************** exit ********************
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

void Write16F1xxx(int dim,int dim2,int options)
// write 14 bit enhanced PIC
// dim=program size
// dim2=eeprom size
// options:
//		bit0=0 -> vpp before vdd
//		bit0=1 -> vdd before vpp
//		bit1=1 -> LVP programming
//		bit2=1 -> Config3@0x8009
//		bit3=1 -> Config4@0x800A
//		bit4=1 -> PIC16F18xxx (calib words @0xE000 + new commands)
// DevREV@0x8005     DevID@0x8006
// Config1@0x8007    Config2@0x8008
// Config3@0x8009    Config4@0x800A
// Calib1@0x8009/A   Calib2@0x800A/B    Calib3@0x800B/C
// eeprom@0x0 or 0xF000
// erase: BULK_ERASE_PROG (1001) +5ms
// write:LOAD_DATA_PROG (0010) + BEGIN_PROG (1000) + 2.5ms (8 word algorithm)
// config write time 5ms
// eeprom:	BULK_ERASE_DATA (1011) + 5ms
//			LOAD_DATA_DATA (0011) + BEGIN_PROG (1000) + 2.5ms
// verify after write
// write (16F18xxx): LOAD_DATA_INC (100010) + BEGIN_PROG (1000) + 2.5ms (32 word algorithm)
{
	int err=0;
	WORD devID=0x3fff,devREV=0x3fff,calib1=0x3fff,calib2=0x3fff,calib3=0x3fff;
	int k=0,k2=0,z=0,i,j,w;
	int F18x=options&16;
	if(F18x&&FWVersion<0xA00){		//only for 16F18xxx
		PrintMessage1(strings[S_FWver2old],"0.10.0");	//"This firmware is too old. Version %s is required\r\n"
		return;
	}
	if(!CheckV33Regulator()){
		PrintMessage(strings[S_noV33reg]);	//Can't find 3.3V expansion board
		return;
	}
	if(sizeW<0x8009){
		PrintMessage(strings[S_NoConfigW5]);	//"Can't find CONFIG (0x8007-0x8008)\r\n"
		PrintMessage(strings[S_End]);
		return;
	}
	if((options&4)&&sizeW<0x800A){		//Config3 defaults to 0x3FFF
		sizeW=0x800A;
		memCODE_W=(WORD*)realloc(memCODE_W,sizeof(WORD)*sizeW);
		memCODE_W[0x8009]=0x3FFF;
	}
	if((options&8)&&sizeW<0x800B){		//Config4 defaults to 0x3FFF
		sizeW=0x800B;
		memCODE_W=(WORD*)realloc(memCODE_W,sizeof(WORD)*sizeW);
		memCODE_W[0x800A]=0x3FFF;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"Write16F1xxx(%d,%d,%d)\n",dim,dim2,options);
	}
	if(dim2>sizeEE) dim2=sizeEE;
	if((options&2)==0){				//HV entry
			if(!StartHVReg(8.5)){
			PrintMessage(strings[S_HVregErr]); //"HV regulator error\r\n"
			return;
		}
	}
	else StartHVReg(-1);			//LVP mode, turn off HV
	for(i=0;i<0x800C&&i<sizeW;i++) memCODE_W[i]&=0x3FFF;
	unsigned int start=GetTickCount();
//	bufferU[0]=0;
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	if((options&2)==0){				//HV entry
		if((options&1)==0){				//VPP before VDD
			bufferU[j++]=EN_VPP_VCC;
			bufferU[j++]=4;				//VPP
			bufferU[j++]=EN_VPP_VCC;
			bufferU[j++]=0x5;			//VDD+VPP
		}
		else{							//VDD before VPP without delay
			bufferU[j++]=EN_VPP_VCC;
			bufferU[j++]=1;				//VDD
			bufferU[j++]=EN_VPP_VCC;
			bufferU[j++]=0x5;			//VDD+VPP
		}
	}
	else{			//Low voltage programming
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=4;				//VPP
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=TX16;			//0000 1010 0001 0010 1100 0010 1011 0010 = 0A12C2B2
		bufferU[j++]=2;
		bufferU[j++]=0x0A;
		bufferU[j++]=0x12;
		bufferU[j++]=0xC2;
		bufferU[j++]=0xB2;
		bufferU[j++]=SET_CK_D;		//Clock pulse
		bufferU[j++]=0x4;
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x0;
	}
	bufferU[j++]=WAIT_T2;
	bufferU[j++]=WAIT_T2;
	bufferU[j++]=WAIT_T2;
	bufferU[j++]=LOAD_CONF;			//counter at 0x8000
	bufferU[j++]=0xFF;
	bufferU[j++]=0xFF;
	bufferU[j++]=INC_ADDR_N;
	bufferU[j++]=0x05;
	bufferU[j++]=READ_DATA_PROG;	//DevREV
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=READ_DATA_PROG;	//DevID
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=INC_ADDR;
	if(options&4) bufferU[j++]=INC_ADDR;
	bufferU[j++]=READ_DATA_PROG;	//Calib1
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=READ_DATA_PROG;	//Calib2
	bufferU[j++]=INC_ADDR;
	bufferU[j++]=READ_DATA_PROG;	//Calib3
//	bufferU[j++]=CUST_CMD;
//	bufferU[j++]=0x16;		//Reset address
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2500>>8;
	bufferU[j++]=2500&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	j=0;
	for(z=0;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	devREV=(bufferI[z+1]<<8)+bufferI[z+2];
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
	devID=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_DevID],devID);	//"DevID: 0x%04X\r\n"
	if(devREV<0x3FFF) PrintMessage1(strings[S_DevREV],devREV);	//"DevREV: 0x%04X\r\n"
	PIC16_ID(devID);
	if(memCODE_W[0x8006]<0x3FFF&&devID!=memCODE_W[0x8006]) PrintMessage(strings[S_DevMismatch]);	//"Warning: the device is different from what specified in source data"
	if(!F18x){		//16F1xxx
		for(z+=3;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		calib1=(bufferI[z+1]<<8)+bufferI[z+2];
		if(calib1<0x3fff) PrintMessage2(strings[S_CalibWordX],1,calib1);	//"Calibration word %d: 0x%04X\r\n"
		for(z+=3;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		calib2=(bufferI[z+1]<<8)+bufferI[z+2];
		if(calib2<0x3fff) PrintMessage2(strings[S_CalibWordX],2,calib2);	//"Calibration word %d: 0x%04X\r\n"
		for(z+=3;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		calib3=(bufferI[z+1]<<8)+bufferI[z+2];
		if(calib3<0x3fff) PrintMessage2(strings[S_CalibWordX],3,calib3);	//"Calibration word %d: 0x%04X\r\n"
	}
	else{		//16F18xxx
		WORD calib[4]={0x3FFF,0x3FFF,0x3FFF,0x3FFF};
		bufferU[j++]=LOAD_PC;			//counter at 0xE000
		bufferU[j++]=0xE0;
		bufferU[j++]=0x00;
		for(i=0;i<4;i++) bufferU[j++]=READ_DATA_INC;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(5);
		j=0;
		if(saveLog){
			fprintf(logfile,"Calibration area @0xE000\n");
		}
		for(z=0,i=0;z<DIMBUF-2&&i<4;z++){
			if(bufferI[z]==READ_DATA_INC){
				calib[i++]=(bufferI[z+1]<<8)+bufferI[z+2];
				z+=2;
			}
		}
		if(i!=4){
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ConfigErr],4,i);	//"Error reading config area, requested %d words, read %d\r\n"
		}
		else{
			for(i=0;i<4;i++) PrintMessage2(strings[S_CalibWordX],i+1,calib[i]);	//"Calibration word %d: 0x%04X\r\n"
		}
	}
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erasing ... "
	if(saveLog)	fprintf(logfile,"%s\n",strings[S_StartErase]);
	int dly=8;
	j=0;
	if(programID){
		bufferU[j++]=LOAD_CONF;			//PC @ 0x8000
		bufferU[j++]=0xFF;
		bufferU[j++]=0xFF;
	}
	else if(!F18x){
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x16;		//Reset address
	}
	else{	//18F18xxx
		bufferU[j++]=LOAD_PC;	//counter at 0
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
	}
	bufferU[j++]=BULK_ERASE_PROG;
	bufferU[j++]=WAIT_T3;			// wait 5ms
	bufferU[j++]=WAIT_T3;
	if(F18x){		//18F18xxx: EEPROM@0xF000
		bufferU[j++]=LOAD_PC;			//counter at 0xF000
		bufferU[j++]=0xF0;
		bufferU[j++]=0x00;
		bufferU[j++]=BULK_ERASE_PROG;
		bufferU[j++]=WAIT_T3;			// wait 5ms
		bufferU[j++]=WAIT_T3;
		dly+=6;
	}
	if(!F18x){
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x16;		//Reset address
	}
	else{	//18F18xxx
		bufferU[j++]=LOAD_PC;	//counter at 0
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(dly);
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	PrintStatusSetup();
	if(saveLog)	fprintf(logfile,"%s\n",strings[S_StartCodeProg]);
	fflush(logfile);
	int valid,inc;
	for(;dim>0&&memCODE_W[dim]>=0x3fff;dim--); //skip empty space at end
	if(!F18x){					//16F1xxx
	if(dim%8) dim+=8-dim%8;		//grow to 8 word multiple
	for(i=k=0,j=0;i<dim;i+=8){
		valid=inc=0;
		for(;i<dim&&!valid;){	//skip empty locations (8 words)
			valid=0;
			for(k=0;k<8;k++) if(memCODE_W[i+k]<0x3fff) valid=1;
			if(!valid){
				inc+=8;
				i+=8;
			}
			if(inc&&(valid||inc==248)){	//increase address to skip empty words
				bufferU[j++]=INC_ADDR_N;
				bufferU[j++]=k=inc;
				inc=0;
			}
			if(j>DIMBUF-4||(valid&&j>1)){
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(3+k/2);
				j=0;
			}
		}
		if(valid){
			k=0;
			bufferU[j++]=LOAD_DATA_PROG;
			bufferU[j++]=memCODE_W[i+k]>>8;  		//MSB
			bufferU[j++]=memCODE_W[i+k]&0xff;		//LSB
			for(k=1;k<8;k++){
				bufferU[j++]=INC_ADDR;
				bufferU[j++]=LOAD_DATA_PROG;
				bufferU[j++]=memCODE_W[i+k]>>8;  		//MSB
				bufferU[j++]=memCODE_W[i+k]&0xff;		//LSB
			}
			bufferU[j++]=BEGIN_PROG;			//internally timed, T=2.5ms
			bufferU[j++]=WAIT_T3;
			bufferU[j++]=INC_ADDR;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(3);
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log7],i,i,0,0);	//"i=%d, k=%d 0=%d\n"
				}
				PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Writing: %d%%, addr. %03X"
			}
		}
	}
	else{					//16F18xxx 32 word algorithm
		if(dim%32) dim+=32-dim%32;		//grow to 32 word multiple
		for(i=k=0,j=0;i<dim;i+=32){
			for(valid=0;i<dim&&!valid;i+=valid?0:32){	//skip empty locations (32 words)
				valid=0;
				for(k=0;k<32;k++) if(memCODE_W[i+k]<0x3fff) valid=1;
			}
			bufferU[j++]=LOAD_PC;	//update counter
			bufferU[j++]=i>>8;
			bufferU[j++]=i&0xFF;
			for(k=0;k<32&&i<dim;k++){
				bufferU[j++]=k<31?LOAD_DATA_INC:LOAD_DATA_PROG;
				bufferU[j++]=memCODE_W[i+k]>>8;  		//MSB
				bufferU[j++]=memCODE_W[i+k]&0xff;		//LSB
				if(j>DIMBUF-4){
					bufferU[j++]=FLUSH;
					for(;j<DIMBUF;j++) bufferU[j]=0x0;
					PacketIO(3);
					j=0;
				}
			}
			if(i<dim){
				bufferU[j++]=BEGIN_PROG;			//internally timed, T=2.5ms
				bufferU[j++]=WAIT_T3;
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(3);
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,0,0);	//"i=%d, k=%d 0=%d\n"
			}
			PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Writing: %d%%, addr. %03X"
		}
	}
	}
	PrintStatusEnd();
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** verify code ********************
	PrintMessage(strings[S_CodeV]);	//"Verifying code ... "
	PrintStatusSetup();
	if(saveLog)	fprintf(logfile,"%s\n",strings[S_CodeV]);
	j=0;
	if(!F18x){
	bufferU[j++]=CUST_CMD;
	bufferU[j++]=0x16;		//Reset address
	}
	else{	//18F18xxx
		bufferU[j++]=LOAD_PC;	//counter at 0
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
	}
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	j=0;
	for(i=k=0;i<dim;i++){
		if(j==0){				//skip empty locations (only after a write)
			for(valid=0;i<dim&&!valid;){
				for(k2=0;k2<255&&!valid;k2++) if(memCODE_W[i+k2]<0x3fff) valid=1;
				if(k2>16){			//increase address to skip empty words, if enough are found
					bufferU[j++]=INC_ADDR_N;
					bufferU[j++]=k2;
					i+=k2;
					k+=k2;
				}
				if(j>DIMBUF-4||(valid&&j>1)){		//if buffer is full or last skip
					bufferU[j++]=FLUSH;
					for(;j<DIMBUF;j++) bufferU[j]=0x0;
					PacketIO(3+j/2*0.1);
					j=0;
				}
			}
		}
		if(memCODE_W[i]<0x3FFF) bufferU[j++]=READ_DATA_PROG;
		bufferU[j++]=INC_ADDR;
		if(j>DIMBUF*2/4-3||i==dim-1){		//2B cmd -> 4B data
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(5);
			for(z=0;z<DIMBUF-2;z++){
				if(bufferI[z]==INC_ADDR) k++;
				else if(bufferI[z]==READ_DATA_PROG){
					if(memCODE_W[k]<0x3FFF&&(memCODE_W[k]!=(bufferI[z+1]<<8)+bufferI[z+2])){
						PrintMessage3(strings[S_CodeWError2],k,memCODE_W[k],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing address %3X: written %04X, read %04X\r\n"
						err++;
					}
					z+=2;
				}
			}
			PrintStatus(strings[S_CodeV2],i*100/(dim+dim2),i);	//"Verify: %d%%, addr. %04X"
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d\n"
			}
			if(err>=max_err) i=dim;
		}
	}
	PrintStatusEnd();
	if(k<dim){
		PrintMessage2(strings[S_CodeVError3],dim,k);	//"Error verifying code area, requested %d words, read %d\r\n"
	}
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
	if(err>=max_err){
		PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
	}
//****************** write eeprom ********************
	if(dim2&&err<max_err&&!F18x){
		int errEE=0;
		PrintMessage(strings[S_EEAreaW]);	//"Writing EEPROM ... "
		PrintStatusSetup();
		if(saveLog)	fprintf(logfile,"%s\n",strings[S_EEAreaW]);
		j=0;
		bufferU[j++]=SET_PARAMETER;
		bufferU[j++]=SET_T3;
		bufferU[j++]=5000>>8;
		bufferU[j++]=5000&0xff;
		bufferU[j++]=BULK_ERASE_DATA;
		bufferU[j++]=WAIT_T3;			// wait 5ms
		bufferU[j++]=CUST_CMD;
		bufferU[j++]=0x16;		//Reset address
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(7);
		j=0;
		for(w=i=k=0;i<dim2;i++){
			if(memEE[i]<0xff){
				bufferU[j++]=LOAD_DATA_DATA;
				bufferU[j++]=memEE[i];
				bufferU[j++]=BEGIN_PROG;			//internally timed, T=5ms max
				bufferU[j++]=WAIT_T3;				//Tprogram
				bufferU[j++]=READ_DATA_DATA;
				w++;
			}
			bufferU[j++]=INC_ADDR;
			if(j>DIMBUF-12||i==dim2-1){
				PrintStatus(strings[S_CodeWriting],(i+dim)*100/(dim+dim2),i);	//"Writing: %d%%, add. %03X"
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(w*5+2);
				w=0;
				for(z=0;z<DIMBUF-4;z++){
					if(bufferI[z]==INC_ADDR&&memEE[k]>=0xff) k++;
					else if(bufferI[z]==LOAD_DATA_DATA&&bufferI[z+3]==READ_DATA_DATA){
						if (memEE[k]!=bufferI[z+4]){
							PrintMessage("\r\n");
							PrintMessage3(strings[S_CodeWError3],k,memEE[k],bufferI[z+4]);	//"Error writing address %4X: written %02X, read %02X\r\n"
							errEE++;
							if(max_err&&err+errEE>max_err){
								PrintMessage1(strings[S_MaxErr],err+errEE);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
								PrintMessage(strings[S_IntW]);	//"write interrupted"
								i=dim2;
								z=DIMBUF;
							}
						}
						k++;
						z+=5;
					}
				}
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,errEE);	//"i=%d, k=%d, errors=%d\n"
				}
			}
		}
		errEE+=i-k;
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],errEE);	//"completed, %d errors\r\n"
		err+=errEE;
	}
	else if(dim2&&err<max_err&&F18x){	//16F18xxx
		int errEE=0;
		PrintMessage(strings[S_EEAreaW]);	//"Writing EEPROM ... "
		PrintStatusSetup();
		if(saveLog)	fprintf(logfile,"%s\n",strings[S_EEAreaW]);
		j=0;
		bufferU[j++]=SET_PARAMETER;
		bufferU[j++]=SET_T3;
		bufferU[j++]=5000>>8;
		bufferU[j++]=5000&0xff;
		bufferU[j++]=LOAD_PC;	//update counter
		bufferU[j++]=0xF0;
		bufferU[j++]=0x00;
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(7);
		j=0;
		for(w=i=k=0;i<dim2;i++){
			if(memEE[i]<0xff){
				bufferU[j++]=LOAD_DATA_PROG;
				bufferU[j++]=0;
				bufferU[j++]=memEE[i];
				bufferU[j++]=BEGIN_PROG;			//internally timed, T=2.5ms max
				bufferU[j++]=WAIT_T3;				//Tprogram
				bufferU[j++]=READ_DATA_PROG;
				w++;
			}
			bufferU[j++]=INC_ADDR;
			if(j>DIMBUF-12||i==dim2-1){
				PrintStatus(strings[S_CodeWriting],(i+dim)*100/(dim+dim2),i);	//"Writing: %d%%, add. %03X"
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(w*5+2);
				w=0;
				for(z=0;z<DIMBUF-4;z++){
					if(bufferI[z]==INC_ADDR&&memEE[k]>=0xff) k++;
					else if(bufferI[z]==LOAD_DATA_PROG&&bufferI[z+3]==READ_DATA_PROG){
						if (memEE[k]!=bufferI[z+5]){
							PrintMessage("\r\n");
							PrintMessage3(strings[S_CodeWError3],k,memEE[k],bufferI[z+5]);	//"Error writing address %4X: written %02X, read %02X\r\n"
							errEE++;
							if(max_err&&err+errEE>max_err){
								PrintMessage1(strings[S_MaxErr],err+errEE);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
								PrintMessage(strings[S_IntW]);	//"write interrupted"
								i=dim2;
								z=DIMBUF;
							}
						}
						k++;
						z+=6;
					}
				}
				j=0;
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,errEE);	//"i=%d, k=%d, errors=%d\n"
				}
			}
		}
		errEE+=i-k;
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],errEE);	//"completed, %d errors\r\n"
		err+=errEE;

	}
	if(err>=max_err){
		PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
	}
//****************** write ID, CONFIG, CALIB ********************
	if(max_err&&err<max_err){
		PrintMessage(strings[S_ConfigAreaW]);	//"Writing CONFIG area ... "
		if(saveLog)	fprintf(logfile,"%s\n",strings[S_ConfigAreaW]);
		int err_c=0;
		bufferU[j++]=SET_PARAMETER;
		bufferU[j++]=SET_T3;
		bufferU[j++]=5000>>8;
		bufferU[j++]=5000&0xff;
		bufferU[j++]=LOAD_CONF;			//PC @ 0x8000
		bufferU[j++]=0xFF;
		bufferU[j++]=0xFF;
		if(programID){
			for(i=0x8000;i<0x8004;i++){
				bufferU[j++]=LOAD_DATA_PROG;
				bufferU[j++]=memCODE_W[i]>>8;		//MSB
				bufferU[j++]=memCODE_W[i]&0xff;		//LSB
				bufferU[j++]=BEGIN_PROG;			//internally timed
				bufferU[j++]=WAIT_T3;				//Tprogram 5ms
				bufferU[j++]=READ_DATA_PROG;
				bufferU[j++]=INC_ADDR;
			}
			bufferU[j++]=INC_ADDR_N;
			bufferU[j++]=3;
		}
		else{
			bufferU[j++]=INC_ADDR_N;
			bufferU[j++]=7;
		}
		bufferU[j++]=LOAD_DATA_PROG;			//Config word 0x8007
		bufferU[j++]=memCODE_W[0x8007]>>8;		//MSB
		bufferU[j++]=memCODE_W[0x8007]&0xff;		//LSB
		bufferU[j++]=BEGIN_PROG;			//internally timed
		bufferU[j++]=WAIT_T3;				//Tprogram 5ms
		bufferU[j++]=READ_DATA_PROG;
		bufferU[j++]=INC_ADDR;
		bufferU[j++]=LOAD_DATA_PROG;			//Config word 2
		bufferU[j++]=memCODE_W[0x8008]>>8;		//MSB
		bufferU[j++]=memCODE_W[0x8008]&0xff;		//LSB
		bufferU[j++]=BEGIN_PROG;			//internally timed
		bufferU[j++]=WAIT_T3;				//Tprogram 5ms
		bufferU[j++]=READ_DATA_PROG;
		bufferU[j++]=INC_ADDR;
		if(options&0xC){
			bufferU[j++]=LOAD_DATA_PROG;			//Config word 3
			bufferU[j++]=memCODE_W[0x8009]>>8;		//MSB
			bufferU[j++]=memCODE_W[0x8009]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG;			//internally timed
			bufferU[j++]=WAIT_T3;				//Tprogram 5ms
			bufferU[j++]=READ_DATA_PROG;
			bufferU[j++]=INC_ADDR;
		}
		if(options&8){
			bufferU[j++]=LOAD_DATA_PROG;			//Config word 4
			bufferU[j++]=memCODE_W[0x800A]>>8;		//MSB
			bufferU[j++]=memCODE_W[0x800A]&0xff;		//LSB
			bufferU[j++]=BEGIN_PROG;			//internally timed
			bufferU[j++]=WAIT_T3;				//Tprogram 5ms
			bufferU[j++]=READ_DATA_PROG;
			bufferU[j++]=INC_ADDR;
		}
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		j=(programID?22:0)+(options&0xC?10:0);
		PacketIO(18+j);
		for(i=0,z=0;programID&&i<4;i++){
			for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
				if (memCODE_W[0x8000+i]!=(bufferI[z+1]<<8)+bufferI[z+2]){
				PrintMessage("\r\n");
				PrintMessage3(strings[S_IDErr],i,memCODE_W[0x8000+i],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ID%d: written %04X, read %04X\r\n"
				err_c++;
			}
			z+=6;
		}
		for(;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if(~memCODE_W[0x8007]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x8007],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
			err_c++;
		}
		for(z+=6;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
		if(~memCODE_W[0x8008]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
			PrintMessage("\r\n");
			PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x8008],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
			err_c++;
		}
		if(options&0xC){
			for(z+=6;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
			if(~memCODE_W[0x8009]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
				PrintMessage("\r\n");
				PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x8009],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
				err_c++;
			}
		}
		if(options&8){
			for(z+=6;z<DIMBUF-2&&bufferI[z]!=READ_DATA_PROG;z++);
			if(~memCODE_W[0x800A]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
				PrintMessage("\r\n");
				PrintMessage2(strings[S_ConfigWErr3],memCODE_W[0x800A],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing config area: written %04X, read %04X\r\n"
				err_c++;
			}
		}
		err+=err_c;
		PrintMessage1(strings[S_ComplErr],err_c);	//"completed, %d errors\r\n"
		if(saveLog){
			fprintf(logfile,strings[S_Log9],err);	//"Config area 	errors=%d \n"
		}
	}
//****************** exit ********************
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}



void Write16F18xxx(int dim,int dim2,int options)
// write 14 bit enhanced PIC
// dim=program size (words)
// dim2=eeprom size (bytes)
// options:
//		bit0=0 -> vpp before vdd
//		bit0=1 -> vdd before vpp
//		bit1=1 -> LVP programming
//		bit4=1 -> do not use DIA&DCI
// DevREV@0x8005     DevID@0x8006
// Config@0x8007-800B
// Device info area @0x8100
// Device configuration info area @0x8200
// erase: BULK_ERASE_PROGRAM_MEM (0x18) +14ms
// EEPROM erase: idem
// write flash: LOAD_NVM (0x0/2) + BEGIN_INT_PROG (0xE0) + 2.8ms (32 word algorithm)
// EEPROM write: one word, 6ms
// config write: one word, 10ms
// verify after write
{
	int err=0;
	WORD devID=0x3fff,devREV=0x3fff;
	int k=0,k2=0,z=0,i,j,w;
	int useDCI=(options&0x10)==0?1:0;
	int rowN=32;		//32 word algorithm
	if(FWVersion<0xB00){
		PrintMessage1(strings[S_FWver2old],"0.11.0");	//"This firmware is too old. Version %s is required\r\n"
		return;
	}
	if(!CheckV33Regulator()){
		PrintMessage(strings[S_noV33reg]);	//Can't find 3.3V expansion board
		return;
	}
	if(sizeW<0x800C){
		PrintMessage(strings[S_NoConfigW6]);	//"Can't find CONFIG (0x8007-0x800B)\r\n"
		PrintMessage1(strings[S_End],0);
		return;
	}
	if(saveLog){
		OpenLogFile();	//"Log.txt"
		fprintf(logfile,"Write16F18xxx(%d,%d,%d)\n",dim,dim2,options);
	}
	if(dim2>0) dim2=sizeEE;
	if(dim2>0x1000) dim2=0x100;
	if((options&2)==0){				//HV entry
			if(!StartHVReg(8.5)){
			PrintMessage(strings[S_HVregErr]); //"HV regulator error\r\n"
			return;
		}
	}
	else StartHVReg(-1);			//LVP mode, turn off HV
	for(i=0;i<0x800C&&i<sizeW;i++) memCODE_W[i]&=0x3FFF;
	unsigned int start=GetTickCount();
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T1T2;
	bufferU[j++]=1;						//T1=1u
	bufferU[j++]=100;					//T2=100u
	bufferU[j++]=EN_VPP_VCC;		//enter program mode
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	if((options&2)==0){				//HV entry
		if((options&1)==0){				//VPP before VDD
			bufferU[j++]=EN_VPP_VCC;
			bufferU[j++]=4;				//VPP
			bufferU[j++]=EN_VPP_VCC;
			bufferU[j++]=0x5;			//VDD+VPP
		}
		else{							//VDD before VPP without delay
			bufferU[j++]=EN_VPP_VCC;
			bufferU[j++]=1;				//VDD
			bufferU[j++]=EN_VPP_VCC;
			bufferU[j++]=0x5;			//VDD+VPP
		}
	}
	else{			//Low voltage programming
		bufferU[j++]=EN_VPP_VCC;
		bufferU[j++]=4;				//VPP
		bufferU[j++]=WAIT_T3;
		bufferU[j++]=TX16;			//0000 1010 0001 0010 1100 0010 1011 0010 = 0A12C2B2
		bufferU[j++]=2;
		bufferU[j++]=0x0A;
		bufferU[j++]=0x12;
		bufferU[j++]=0xC2;
		bufferU[j++]=0xB2;
		bufferU[j++]=SET_CK_D;		//Clock pulse
		bufferU[j++]=0x4;
		bufferU[j++]=SET_CK_D;
		bufferU[j++]=0x0;
	}
	bufferU[j++]=WAIT_T2;
	bufferU[j++]=WAIT_T2;
	bufferU[j++]=WAIT_T2;
	bufferU[j++]=ICSP8_LOAD;
	bufferU[j++]=LOAD_PC_ADDR;
	bufferU[j++]=0x80;
	bufferU[j++]=0x05;
	bufferU[j++]=ICSP8_READ;
	bufferU[j++]=READ_NVM_INC;	//DevREV
	bufferU[j++]=ICSP8_READ;
	bufferU[j++]=READ_NVM_INC;	//DevID
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2800>>8;
	bufferU[j++]=2800&0xff;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(5);
	j=0;
	for(z=0;z<DIMBUF-2&&bufferI[z]!=ICSP8_READ;z++);
	devREV=(bufferI[z+1]<<8)+bufferI[z+2];
	for(z+=3;z<DIMBUF-2&&bufferI[z]!=ICSP8_READ;z++);
	devID=(bufferI[z+1]<<8)+bufferI[z+2];
	PrintMessage1(strings[S_DevID],devID);	//"DevID: 0x%04X\r\n"
	if(devREV<0x3FFF) PrintMessage1(strings[S_DevREV],devREV);	//"DevREV: 0x%04X\r\n"
	PIC16_ID(devID);
	if(memCODE_W[0x8006]<0x3FFF&&devID!=memCODE_W[0x8006]) PrintMessage(strings[S_DevMismatch]);	//"Warning: the device is different from what specified in source data"
//****************** DIA-DCI ********************
	if(useDCI){	//if not disabled
		if(saveLog) fprintf(logfile,"Device Information Area @0x8100\n");
		WORD DIA[0x20];
		bufferU[j++]=ICSP8_LOAD;			//counter at 0x8100
		bufferU[j++]=LOAD_PC_ADDR;
		bufferU[j++]=0x81;
		bufferU[j++]=0x00;
		k=0;
		for(i=0;i<0x20;i++){		//DIA
			bufferU[j++]=ICSP8_READ;
			bufferU[j++]=READ_NVM_INC;
			if((j+1)/2*3+3>DIMBUF||i==0x1F){		//2B cmd -> 3B data
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(5);
				for(z=0;z<DIMBUF-2;z++){
					if(bufferI[z]==ICSP8_READ){
						DIA[k++]=(bufferI[z+1]<<8)+bufferI[z+2];
						z+=2;
					}
				}
				j=0;
			}
		}
		PrintMessage("Device Information Area @0x8100\r\n");
		char s[256],t[256];
		s[0]=0;
		for(i=0;i<0x20;i+=COL){
			sprintf(t,"%04X: ",0x8100+i);
			strcat(s,t);
			for(j=i;j<i+COL&&j<0x20;j++){
				sprintf(t,"%04X ",DIA[j]);
				strcat(s,t);
			}
			strcat(s,"\r\n");
		}
		PrintMessage(s);
		if(saveLog) fprintf(logfile,"Device Configuration Information @0x8200\n");
		WORD DCI[0x20];
		j=0;
		k=0;
		bufferU[j++]=ICSP8_LOAD;
		bufferU[j++]=LOAD_PC_ADDR;
		bufferU[j++]=0x82;
		bufferU[j++]=0x00;
		for(i=0;i<0x20;i++){		//DCI
			bufferU[j++]=ICSP8_READ;
			bufferU[j++]=READ_NVM_INC;
			if((j+1)/2*3+3>DIMBUF||i==0x1F){		//2B cmd -> 3B data
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(5);
				for(z=0;z<DIMBUF-2;z++){
					if(bufferI[z]==ICSP8_READ){
						DCI[k++]=(bufferI[z+1]<<8)+bufferI[z+2];
						z+=2;
					}
				}
				j=0;
			}
		}
		PrintMessage("Device Configuration Information @0x8200\r\n");
		s[0]=0;
		for(i=0;i<0x20;i+=COL){
			sprintf(t,"%04X: ",0x8200+i);
			strcat(s,t);
			for(j=i;j<i+COL&&j<0x20;j++){
				sprintf(t,"%04X ",DCI[j]);
				strcat(s,t);
			}
			strcat(s,"\r\n");
		}
		PrintMessage(s);
		PrintMessage1("Erase row size: %d words\r\n",DCI[0]);
		PrintMessage1("Write latches: %d\r\n",DCI[1]);
		PrintMessage1("User rows: %d\r\n",DCI[2]);
		PrintMessage1("->%d Flash words\r\n",DCI[0]*DCI[2]);
		PrintMessage1("EE data memory size: %d\r\n",DCI[3]);
		PrintMessage1("Pin count: %d\r\n",DCI[4]);
		if(DCI[0]*DCI[2]!=dim) PrintMessage(strings[S_WarnFlashSize]);	//"Warning, flash size is different from the expected value"
		if(dim2>DCI[3]) dim2=DCI[3];	//limit EE to the real one
	}
//****************** erase memory ********************
	PrintMessage(strings[S_StartErase]);	//"Erasing ... "
	if(saveLog)	fprintf(logfile,"%s\n",strings[S_StartErase]);
	j=0;
	bufferU[j++]=ICSP8_LOAD;
	bufferU[j++]=LOAD_PC_ADDR;
	if(programID){	//0x8000 to erase UserID
		bufferU[j++]=0x80;
		bufferU[j++]=0x00;
	}
	else{
		bufferU[j++]=0x00;
		bufferU[j++]=0x00;
	}
	bufferU[j++]=ICSP8_SHORT;
	bufferU[j++]=BULK_ERASE_PROGRAM_MEM;
	bufferU[j++]=WAIT_T3;			// wait ~14ms
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=WAIT_T3;
	//separate EEPROM erase (undocumented on 16F184xx devices)
	bufferU[j++]=ICSP8_LOAD;
	bufferU[j++]=LOAD_PC_ADDR;
	bufferU[j++]=0xF0;
	bufferU[j++]=0x00;
	bufferU[j++]=ICSP8_SHORT;
	bufferU[j++]=BULK_ERASE_PROGRAM_MEM;
	bufferU[j++]=WAIT_T3;			// wait ~14ms
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=WAIT_T3;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(30);
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** write code ********************
	PrintMessage(strings[S_StartCodeProg]);	//"Write code ... "
	PrintStatusSetup();
	if(saveLog)	fprintf(logfile,"%s\n",strings[S_StartCodeProg]);
	fflush(logfile);
	int valid,inc;
	for(;dim>0&&memCODE_W[dim-1]>=0x3fff;dim--); //skip empty space at end
	if(dim%rowN) dim+=rowN-dim%rowN;		//grow to 32 word multiple
	for(i=k=0,j=0;i<dim;i+=rowN){
		for(valid=0;i<dim&&!valid;i+=valid?0:rowN){	//skip empty locations (32 words)
			valid=0;
			for(k=0;k<rowN;k++) if(memCODE_W[i+k]<0x3fff) valid=1;
		}
		bufferU[j++]=ICSP8_LOAD;
		bufferU[j++]=LOAD_PC_ADDR;	//update counter
		bufferU[j++]=i>>8;
		bufferU[j++]=i&0xFF;
		for(k=0;k<rowN&&i<dim;k++){	//load all latches
			bufferU[j++]=ICSP8_LOAD;
			bufferU[j++]=k<(rowN-1)?LOAD_NVM_INC:LOAD_NVM;
			bufferU[j++]=memCODE_W[i+k]>>8;  		//MSB
			bufferU[j++]=memCODE_W[i+k]&0xff;		//LSB
			if(j>DIMBUF-5){
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(3);
				j=0;
			}
		}
		if(i<dim){
			bufferU[j++]=ICSP8_SHORT;
			bufferU[j++]=BEGIN_INT_PROG;			//internally timed, T=2.8ms
			bufferU[j++]=WAIT_T3;
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(3);
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log7],i,i,0,0);	//"i=%d, k=%d 0=%d\n"
			}
			PrintStatus(strings[S_CodeWriting],i*100/dim,i);	//"Writing: %d%%, addr. %03X"
		}
	}
	PrintStatusEnd();
	PrintMessage(strings[S_Compl]);	//"completed\r\n"
//****************** verify code ********************
	PrintMessage(strings[S_CodeV]);	//"Verifying code ... "
	PrintStatusSetup();
	if(saveLog)	fprintf(logfile,"%s\n",strings[S_CodeV]);
	j=0;
	bufferU[j++]=ICSP8_LOAD;
	bufferU[j++]=LOAD_PC_ADDR;	//update counter
	bufferU[j++]=0x00;
	bufferU[j++]=0x00;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	j=0;
	for(i=k=0;i<dim;i++){
		if(j==0){				//skip empty locations (only at the beginning of a packet)
			for(k2=i;k2<dim&&memCODE_W[k2]>=0x3fff;k2++);
			if(k2>i+10){			//at least 10 skipped
				i=k2;
				k=i;
				bufferU[j++]=ICSP8_LOAD;
				bufferU[j++]=LOAD_PC_ADDR;	//update counter
				bufferU[j++]=(i>>8)&0xFF;
				bufferU[j++]=i&0xFF;
			}
		}
		bufferU[j++]=ICSP8_READ;
		bufferU[j++]=READ_NVM_INC;
		if((j+1)/2*3+3>DIMBUF||i==dim-1){		//2B cmd -> 3B data
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(5);
			for(z=0;z<DIMBUF-2;z++){
				if(bufferI[z]==ICSP8_READ){
					if(memCODE_W[k]!=(bufferI[z+1]<<8)+bufferI[z+2]){
						PrintMessage3(strings[S_CodeWError2],k,memCODE_W[k],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing address %3X: written %04X, read %04X\r\n"
						err++;
					}
					z+=2;
					k++;
				}
			}
			PrintStatus(strings[S_CodeV2],i*100/(dim+dim2),i);	//"Verify: %d%%, addr. %04X"
			j=0;
			if(saveLog){
				fprintf(logfile,strings[S_Log8],i,i,k,k,err);	//"i=%d, k=%d, errors=%d\n"
			}
			if(err>=max_err) i=dim;
		}
	}
	PrintStatusEnd();
	PrintMessage1(strings[S_ComplErr],err);	//"completed, %d errors\r\n"
	if(err>=max_err){
		PrintMessage1(strings[S_MaxErr],err);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
	}
//****************** write eeprom ********************
	else if(dim2&&err<max_err){
		int errEE=0;
		PrintMessage(strings[S_EEAreaW]);	//"Writing EEPROM ... "
		PrintStatusSetup();
		if(saveLog)	fprintf(logfile,"%s\n",strings[S_EEAreaW]);
		j=0;
		bufferU[j++]=SET_PARAMETER;
		bufferU[j++]=SET_T3;
		bufferU[j++]=5600>>8;		//5.6ms in datasheet?
		bufferU[j++]=5600&0xff;
		bufferU[j++]=ICSP8_LOAD;
		bufferU[j++]=LOAD_PC_ADDR;	//load address
		bufferU[j++]=0xF0;
		bufferU[j++]=0x00;
		for(w=i=k=0;i<dim2;i++){
			if(memEE[i]<0xff){
				bufferU[j++]=ICSP8_LOAD;
				bufferU[j++]=LOAD_NVM;
				bufferU[j++]=0;  		//MSB
				bufferU[j++]=memEE[i]&0xff;		//LSB
				bufferU[j++]=ICSP8_SHORT;
				bufferU[j++]=BEGIN_INT_PROG;		//internally timed
				bufferU[j++]=WAIT_T3;				//Tprogram XXms
				bufferU[j++]=ICSP8_READ;
				bufferU[j++]=READ_NVM_INC;
				w++;
			}
			else{
				bufferU[j++]=ICSP8_SHORT;
				bufferU[j++]=INC_ADDR8;
			}
			if(j>DIMBUF-10||i==dim2-1){
				PrintStatus(strings[S_CodeWriting],(i+dim)*100/(dim+dim2),i);	//"Writing: %d%%, add. %03X"
				bufferU[j++]=FLUSH;
				for(;j<DIMBUF;j++) bufferU[j]=0x0;
				PacketIO(w*6+2);
				j=0;
				w=0;
				for(z=0;z<DIMBUF-5;z++){
					if(bufferI[z]==ICSP8_SHORT&&memEE[k]>=0xff) k++;
					else if(bufferI[z]==ICSP8_LOAD&&bufferI[z+3]==ICSP8_READ){
						if(memEE[k]!=bufferI[z+5]){
							PrintMessage("\r\n");
							PrintMessage3(strings[S_CodeWError3],k,memEE[k],bufferI[z+5]);	//"Error writing address %4X: written %02X, read %02X\r\n"
							errEE++;
							if(max_err&&err+errEE>max_err){
								PrintMessage1(strings[S_MaxErr],err+errEE);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
								PrintMessage(strings[S_IntW]);	//"write interrupted"
								i=dim2;
								z=DIMBUF;
							}
						}
						k++;
						z+=5;
					}
				}
				if(saveLog){
					fprintf(logfile,strings[S_Log8],i,i,k,k,errEE);	//"i=%d, k=%d, errors=%d\n"
				}
			}
		}
		errEE+=i-k;
		PrintStatusEnd();
		PrintMessage1(strings[S_ComplErr],errEE);	//"completed, %d errors\r\n"
		if(max_err&&err+errEE>max_err){
			PrintMessage1(strings[S_MaxErr],err+errEE);	//"Exceeded maximum number of errors (%d), write interrupted\r\n"
		}
		err+=errEE;
	}
//****************** write ID, CONFIG, CALIB ********************
	if(max_err&&err<max_err){
		char t[256];
		PrintMessage(strings[S_ConfigAreaW]);	//"Writing CONFIG area ... "
		PrintMessage("\r\n");
		if(saveLog)	fprintf(logfile,"%s\n",strings[S_ConfigAreaW]);
		int err_c=0;
		bufferU[j++]=SET_PARAMETER;
		bufferU[j++]=SET_T3;
		bufferU[j++]=10000>>8;		//5.6ms in datasheet, but requires more in practice
		bufferU[j++]=10000&0xff;
		if(programID){
			bufferU[j++]=ICSP8_LOAD;
			bufferU[j++]=LOAD_PC_ADDR;	//load address
			bufferU[j++]=0x80;
			bufferU[j++]=0x00;
			for(i=0x8000;i<0x8004;i++){
				bufferU[j++]=ICSP8_LOAD;
				bufferU[j++]=LOAD_NVM;
				bufferU[j++]=memCODE_W[i]>>8;		//MSB
				bufferU[j++]=memCODE_W[i]&0xff;		//LSB
				bufferU[j++]=ICSP8_SHORT;
				bufferU[j++]=BEGIN_INT_PROG;		//internally timed
				bufferU[j++]=WAIT_T3;				//Tprogram 10ms
				bufferU[j++]=ICSP8_READ;
				bufferU[j++]=READ_NVM_INC;
			}
			bufferU[j++]=FLUSH;
			for(;j<DIMBUF;j++) bufferU[j]=0x0;
			PacketIO(45);
			for(i=0,z=0;i<4;i++){
				for(;z<DIMBUF-2&&bufferI[z]!=ICSP8_READ;z++);
				if (memCODE_W[0x8000+i]!=(bufferI[z+1]<<8)+bufferI[z+2]){
					PrintMessage3(strings[S_IDErr],i,memCODE_W[0x8000+i],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing ID%d: written %04X, read %04X\r\n"
					err_c++;
				}
				z+=6;
			}
			j=0;
		}
		bufferU[j++]=ICSP8_LOAD;
		bufferU[j++]=LOAD_PC_ADDR;	//load address
		bufferU[j++]=0x80;
		bufferU[j++]=0x07;
		for(i=0x8007;i<0x800C;i++){		//5 config words
			bufferU[j++]=ICSP8_LOAD;
			bufferU[j++]=LOAD_NVM;
			bufferU[j++]=memCODE_W[i]>>8;		//MSB
			bufferU[j++]=memCODE_W[i]&0xff;		//LSB
			bufferU[j++]=ICSP8_SHORT;
			bufferU[j++]=BEGIN_INT_PROG;		//internally timed
			bufferU[j++]=WAIT_T3;				//Tprogram 10ms
			bufferU[j++]=ICSP8_READ;
			bufferU[j++]=READ_NVM_INC;
		}
		bufferU[j++]=FLUSH;
		for(;j<DIMBUF;j++) bufferU[j]=0x0;
		PacketIO(55);
		for(i=0,z=0;i<5;i++){
			for(;z<DIMBUF-2&&bufferI[z]!=ICSP8_READ;z++);
			if (~memCODE_W[0x8007+i]&((bufferI[z+1]<<8)+bufferI[z+2])){	//error if written 0 and read 1 (~W&R)
				
				sprintf(t,"config%d",i+1);
				PrintMessage3(strings[S_WErr2],t,memCODE_W[0x8007+i],(bufferI[z+1]<<8)+bufferI[z+2]);	//"Error writing %s: written %04X, read %04X"
				err_c++;
			}
			z+=6;
		}
		err+=err_c;
		PrintMessage1(strings[S_ComplErr],err_c);	//"completed, %d errors\r\n"
		if(saveLog){
			fprintf(logfile,strings[S_Log9],err_c);	//"Config area 	errors=%d \n"
		}
	}
//****************** exit ********************
	j=0;
	bufferU[j++]=SET_PARAMETER;
	bufferU[j++]=SET_T3;
	bufferU[j++]=2000>>8;
	bufferU[j++]=2000&0xff;
	bufferU[j++]=NOP;				//exit program mode
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x1;
	bufferU[j++]=EN_VPP_VCC;
	bufferU[j++]=0x0;
	bufferU[j++]=SET_CK_D;
	bufferU[j++]=0x0;
	bufferU[j++]=FLUSH;
	for(;j<DIMBUF;j++) bufferU[j]=0x0;
	PacketIO(2);
	unsigned int stop=GetTickCount();
	sprintf(str,strings[S_EndErr],(stop-start)/1000.0,err,err!=1?strings[S_ErrPlur]:strings[S_ErrSing]);	//"\r\nEnd (%.2f s) %d %s\r\n\r\n"
	PrintMessage(str);
	if(saveLog){
		fprintf(logfile,str);
		CloseLogFile();
	}
	PrintStatusClear();			//clear status report
}

