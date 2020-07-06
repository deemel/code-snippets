//bigint driver for ATT C++ 3.0

# include "bigint.h"
# include <iostream.h>
# include <strstream.h>
# include <string.h>
# include "cex_memory.h"
# include "cex_timer.h"


enum { BAD_STATUS = -1, GOOD_STATUS = 0 };
enum { FEEDBACK_INTERVAL = 100 };

# define ML       cex_MemTab::currentSourceLineNumber(__LINE__);
# define MF       cex_MemTab::currentSourceFileName(FILENAME);
# define M        MF ML
# define MS       M cex_MemTab::currentMemoryMode(0);
# define MU       M cex_MemTab::currentMemoryMode(1);

# define PRINT(X) cerr << "( X ) = \"" << (X) << '"' << endl;
# define ASSERT(X) { \
     if(!(X)) { cerr << "ERROR: assertion (X) failed at line " << __LINE__ \
             << "." << endl;  return BAD_STATUS; } \
}

# define EQ(X,Y) { \
    ostrstream buf; \
    buf << X << ends; \
    if(0 != strcmp(buf.str(), "Y")) { \
        cerr << "ERROR: assertion (" \
	     << buf.str() << " == Y) failed at line " << __LINE__ \
             << "." << endl; return BAD_STATUS; \
    } \
    delete buf.str(); \
}

int cex_MemTab::d_tracingEnabled = 0;
static const char * const FILENAME = __FILE__;

main () {
M   cex_Timer timer;
M   cerr << endl << "\t****** Beginning Test ******" << endl;
M   timer.start();
MU
M   cerr << endl << "Testing: CTORs, =" << endl;
M {
M   //BigInt(const char *)
M   BigInt s1 = "0";				EQ(s1,0)
M   BigInt s2 = "00000000000000000000";		EQ(s2,0)
M   BigInt s3 = "-000000000";			EQ(s3,0)
M   BigInt s4 = "+0000000000";			EQ(s4,0)
M   BigInt s5 = "-00000000000000";		EQ(s5,0)
M   BigInt w1 = "-0123456789";			EQ(w1,-123456789)
M   BigInt w2 = "000001";			EQ(w2,1)
M   BigInt w3 = "1000000000";			EQ(w3,1000000000)
M   BigInt s6 = "2147483647"; 			EQ(s6,2147483647)
M   BigInt s7 = "-2147483648"; 			EQ(s7,-2147483648)
M   BigInt s8 = "12345678901234567890"; 	EQ(s8,12345678901234567890)
M   BigInt s9 = "-98765432109876543210"; 	EQ(s9,-98765432109876543210)
M   BigInt w4 = "213890218308218038291803821038021830921839281308921903821038";
           EQ(w4,213890218308218038291803821038021830921839281308921903821038)

M   //BigInt(int)
M   BigInt i1 = -0; 				EQ(i1,0)
M   BigInt i2 = 1000; 				EQ(i2,1000)
M   BigInt i3 = -1234; 				EQ(i3,-1234)
M   BigInt w5 = -1000000001;			EQ(w5,-1000000001)
M   BigInt i4 = 2147483647; 			EQ(i4,2147483647)
M   BigInt i5 = -2147483647; 			EQ(i5,-2147483647)

M   //BigInt(const BigInt&)
M   BigInt t1 = s1; 				EQ(t1,0)
M   BigInt t2 = s2; 				EQ(t2,0)
M   BigInt t3 = s3; 				EQ(t3,0)
M   BigInt t4 = s4; 				EQ(t4,0)
M   BigInt t5 = s5; 				EQ(t5,0)
M   BigInt x1 = w1;				EQ(x1,-123456789)
M   BigInt x2 = w2;				EQ(x2,1);
M   BigInt x3 = w3;				EQ(x3,1000000000)
M   BigInt t6 = s6; 				EQ(t6,2147483647)
M   BigInt t7 = s7; 				EQ(t7,-2147483648)
M   BigInt t8 = s8; 				EQ(t8,12345678901234567890)
M   BigInt t9 = s9; 				EQ(t9,-98765432109876543210)

M   //BigInt& operator=(const BigInt&)
M   BigInt e1=0, e2=0, e3=0, e4=0, e5=0, e6=0, e7=0, e8=0, e9=0;
M   e1 = s1;				EQ(e1,0)
M   e2 = s2;				EQ(e2,0)
M   e3 = s3; 				EQ(e3,0)
M   e4 = s4; 				EQ(e4,0)
M   e5 = s5; 				EQ(e5,0)

M   x1 = x1;				EQ(x1,-123456789)
M   x2 = x2;				EQ(x2,1);
M   x3 = x3;				EQ(x3,1000000000)

M   e6 = s6; 				EQ(e6,2147483647)
M   e7 = s7; 				EQ(e7,-2147483648)
M   e8 = s8; 				EQ(e8,12345678901234567890)
M   e9 = s9; 				EQ(e9,-98765432109876543210)

M   t6 = -1;                            EQ(t6,-1)
M   t7 = "+39814821083821039210939210381231283821928120182182098138222832109";
     EQ(t7,39814821083821039210939210381231283821928120182182098138222832109)

M   t8 = 1;                             EQ(t8,1)
M   t9 = "-0";                          EQ(t9,0)
M   t6 = -(t7 = (t8 = t9));		EQ(t9,0) EQ(t8,0) EQ(t7,0) EQ(t6,0)
M }
M   cerr << "       user bytes: " << cex_MemTab::currentUserBytes() << endl;
M   cerr << "      user blocks: " << cex_MemTab::currentUserBlocks() << endl;
M   cerr << endl << "Testing: +=, -=, ++, and -- " << endl;
M {

M   //BigInt& operator+=(const BigInt&)
M   BigInt a1 = -12345678; 			EQ(a1,-12345678)
M          a1 += 12345678; 			EQ(a1,0)
M          a1 += 12345678; 			EQ(a1,12345678)
M          a1 += 90000000; 			EQ(a1,102345678)
M          a1 += -90000000; 			EQ(a1,12345678)
M          a1 += -12345678; 			EQ(a1,0)
M          a1 += -12345678; 			EQ(a1,-12345678)
M          a1 += a1; 				EQ(a1,-24691356)
M
M   //BigInt& operator-=(const BigInt&)
M          a1 = 12345678; 			EQ(a1,12345678)
M          a1 -= 12345678; 			EQ(a1,0)
M          a1 -= 12345678; 			EQ(a1,-12345678)
M          a1 -= 90000000; 			EQ(a1,-102345678)
M          a1 -= -90000000; 			EQ(a1,-12345678)
M          a1 -= -12345678; 			EQ(a1,0)
M          a1 -= -12345678; 			EQ(a1,12345678)
M          a1 -= a1; 				EQ(a1,0)
M
M   //BigInt& operator++()
M          a1 = "123456789012345"; 		EQ(a1,123456789012345)
M          ++a1; 				EQ(a1,123456789012346)
M          ++a1; 				EQ(a1,123456789012347)
M          a1 = "-123456789012345"; 		EQ(a1,-123456789012345)
M          ++a1; 				EQ(a1,-123456789012344)
M          ++a1; 				EQ(a1,-123456789012343)
M          a1 = -1;  ++a1;			EQ(a1,0)
M          ++ a1;				EQ(a1,1)
M          a1 = "9";  ++a1; ++a1;		EQ(a1,11)
M          a1 = -11;  ++a1; ++a1;		EQ(a1,-9)
M          a1 = "999999999";  ++a1, ++a1;	EQ(a1,1000000001)
M          a1 = "-1000000001";  ++a1, ++a1;	EQ(a1,-999999999)


M   //BigInt& operator--()
M          a1 = "123456789012345"; 		EQ(a1,123456789012345)
M          --a1; 				EQ(a1,123456789012344)
M          --a1; 				EQ(a1,123456789012343)
M          a1 = "-123456789012345"; 		EQ(a1,-123456789012345)
M          --a1; 				EQ(a1,-123456789012346)
M          --a1; 				EQ(a1,-123456789012347)

M          a1 = 1;  --a1;			EQ(a1,0)
M          -- a1;				EQ(a1,-1)
M          a1 = "11";  --a1; --a1;		EQ(a1,9)
M          a1 = -9;  --a1; --a1;		EQ(a1,-11)
// M          a1 = "1000000001";  --a1, --a1;	EQ(a1,999999999)
// M          a1 = "-999999999";  --a1, --a1;	EQ(a1,-1000000001)

M }
M   cerr << "       user bytes: " << cex_MemTab::currentUserBytes() << endl;
M   cerr << "      user blocks: " << cex_MemTab::currentUserBlocks() << endl;
M   cerr << endl << "Testing: +" << endl;
M {
M   //friend BigInt operator+(const BigInt&, const BigInt&)
M 	   BigInt a1 = "0";			EQ(a1,0)
M 	   BigInt a2 = "000000000000000000000";	EQ(a2,0)
M 	   BigInt a3 = "-00000000000000000000";	EQ(a3,0)
M 	   a1 = "12345678901234567890";		EQ(a1,12345678901234567890)
M 	   a2 = "98765432109876543210";		EQ(a2,98765432109876543210)
M 	   a3 = a1 + a2;			EQ(a3,111111111011111111100)
M 	   a1 = "-12345678901234567890";	EQ(a1,-12345678901234567890)
M 	   a2 = "-98765432109876543210";	EQ(a2,-98765432109876543210)
M 	   a3 = a1 + a2;			EQ(a3,-111111111011111111100)
M 	   a1 = "12345678901234567890";		EQ(a1,12345678901234567890)
M 	   a2 = "-98765432109876543210";	EQ(a2,-98765432109876543210)
M 	   a3 = a1 + a2;			EQ(a3,-86419753208641975320)
M 	   a1 = "-12345678901234567890";	EQ(a1,-12345678901234567890)
M 	   a2 = "98765432109876543210";		EQ(a2,98765432109876543210)
M 	   a3 = a1 + a2;			EQ(a3,86419753208641975320)

M 	   a1 = "12345678901234567890";		EQ(a1,12345678901234567890)
M 	   a2 = "98765432109876543210";		EQ(a2,98765432109876543210)
M 	   a3 = a2 + a1;			EQ(a3,111111111011111111100)
M 	   a1 = "-12345678901234567890";	EQ(a1,-12345678901234567890)
M 	   a2 = "-98765432109876543210";	EQ(a2,-98765432109876543210)
M 	   a3 = a2 + a1;			EQ(a3,-111111111011111111100)
M 	   a1 = "12345678901234567890";		EQ(a1,12345678901234567890)
M 	   a2 = "-98765432109876543210";	EQ(a2,-98765432109876543210)
M 	   a3 = a2 + a1;			EQ(a3,-86419753208641975320)
M 	   a1 = "-12345678901234567890";	EQ(a1,-12345678901234567890)
M 	   a2 = "98765432109876543210";		EQ(a2,98765432109876543210)
M 	   a3 = a2 + a1;			EQ(a3,86419753208641975320)

M          a1 = "999999999999999999";           EQ(a1,999999999999999999)
M          a2 = a1 + 1;				EQ(a2,1000000000000000000)
M          a3 = a1 + -a2;			EQ(a3,-1);
M          a2 = a2 + a2 + a2 + a1;		EQ(a2,3999999999999999999)

M }
M   cerr << "       user bytes: " << cex_MemTab::currentUserBytes() << endl;
M   cerr << "      user blocks: " << cex_MemTab::currentUserBlocks() << endl;
M   cerr << endl << "Testing: -" << endl;
M {
M   //friend BigInt operator-(const BigInt&, const BigInt&)
M 	   BigInt a1 = "0";			EQ(a1,0)
M 	   BigInt a2 = "000000000000000000000";	EQ(a2,0)
M 	   BigInt a3 = "-00000000000000000000";	EQ(a3,0)
M 	   a1 = "12345678901234567890";		EQ(a1,12345678901234567890)
M 	   a2 = "98765432109876543210";		EQ(a2,98765432109876543210)
M 	   a3 = a1 - a2;			EQ(a3,-86419753208641975320)
M 	   a1 = "-12345678901234567890";	EQ(a1,-12345678901234567890)
M 	   a2 = "-98765432109876543210";	EQ(a2,-98765432109876543210)
M 	   a3 = a1 - a2;			EQ(a3,86419753208641975320)
M 	   a1 = "12345678901234567890";		EQ(a1,12345678901234567890)
M 	   a2 = "-98765432109876543210";	EQ(a2,-98765432109876543210)
M 	   a3 = a1 - a2;			EQ(a3,111111111011111111100)
M 	   a1 = "-12345678901234567890";	EQ(a1,-12345678901234567890)
M 	   a2 = "98765432109876543210";		EQ(a2,98765432109876543210)
M 	   a3 = a1 - a2;			EQ(a3,-111111111011111111100)

M 	   a1 = "12345678901234567890";		EQ(a1,12345678901234567890)
M 	   a2 = "98765432109876543210";		EQ(a2,98765432109876543210)
M 	   a3 = a2 - a1;			EQ(a3,86419753208641975320)
M 	   a1 = "-12345678901234567890";	EQ(a1,-12345678901234567890)
M 	   a2 = "-98765432109876543210";	EQ(a2,-98765432109876543210)
M 	   a3 = a2 - a1;			EQ(a3,-86419753208641975320)
M 	   a1 = "12345678901234567890";		EQ(a1,12345678901234567890)
M 	   a2 = "-98765432109876543210";	EQ(a2,-98765432109876543210)
M 	   a3 = a2 - a1;			EQ(a3,-111111111011111111100)
M 	   a1 = "-12345678901234567890";	EQ(a1,-12345678901234567890)
M 	   a2 = "98765432109876543210";		EQ(a2,98765432109876543210)
M 	   a3 = a2 - a1;			EQ(a3,111111111011111111100)

M          a1 = - a1 - - a1;			EQ(a1,0)
M          a2 = a1 - a1 - a1;			EQ(a2,0)
M          a3 =
        BigInt("29192819218281281922299999999999999992188222222888888888882")
      - BigInt("29192819218281281922299999999999999992188222222888888888883")
      - BigInt(2);
                       EQ(a3,-3)
M }
M   cerr << "       user bytes: " << cex_MemTab::currentUserBytes() << endl;
M   cerr << "      user blocks: " << cex_MemTab::currentUserBlocks() << endl;
M   cerr << endl << "Testing: (unary) -, abs" << endl;
M {
M   //friend BigInt operator-(const BigInt&)
M 	   BigInt a1 = "0";			EQ(a1,0)
M 	   a1 = 0;				EQ(a1,0)
M 	   a1 = -a1;				EQ(a1,0)
M 	   a1 = -1;				EQ(a1,-1)
M 	   a1 = -a1;				EQ(a1,1)
M 	   a1 = -a1;				EQ(a1,-1)
M 	   a1 = - - a1;				EQ(a1,-1)
M 	   a1 = - - - a1;			EQ(a1,1)

M   //friend BigInt abs(const BigInt&)
M          a1 = "-0";				EQ(a1,0)
M 	   a1 = abs(a1);			EQ(a1,0)
M 	   a1 = -1;				EQ(a1,-1)
M 	   a1 = abs(a1);			EQ(a1,1)
M 	   a1 = "-98765432109876543210";	EQ(a1,-98765432109876543210)
M 	   a1 = "98765432109876543210";		EQ(a1,98765432109876543210)
M 	   a1 = abs(a1);;			EQ(a1,98765432109876543210)
M }
M   cerr << "       user bytes: " << cex_MemTab::currentUserBytes() << endl;
M   cerr << "      user blocks: " << cex_MemTab::currentUserBlocks() << endl;
M   cerr << endl << "Testing: ==, !=, >, >=, <, <=" << endl;
M {
M   const SIZE = 5;
M   BigInt *p[SIZE];
M   p[0] = new BigInt(-99999);
M   p[1] = new BigInt(-99998);
M   p[2] = new BigInt(0);
M   p[3] = new BigInt(99998);
M   p[4] = new BigInt(99999);

M   BigInt z = "0";
M   BigInt nz = "-0";
M   { BigInt i = 0;
M   i = z == nz; 			        EQ(i,1)
M   i = z != nz; 			        EQ(i,0)
M   i = z >= nz; 			        EQ(i,1)
M   i = z >  nz; 			        EQ(i,0)
M   i = z <= nz; 			        EQ(i,1)
M   i = z <  nz; 			        EQ(i,0)
M   i = nz == z; 			        EQ(i,1)
M   i = nz != z; 			        EQ(i,0)
M   i = nz >= z; 			        EQ(i,1)
M   i = nz >  z; 			        EQ(i,0)
M   i = nz <= z; 			        EQ(i,1)
M   i = nz <  z; 			        EQ(i,0)
M   }

M   

M   int i, j;

M   //friend int operator==(const BigInt&, const BigInt&)
M   for (i = 0; i < SIZE; i++) {
M       for (j = 0; j < SIZE; j++) {
M 		ASSERT ((i == j) == (*p[i] == *p[j]))
M       }
M   }
M   { BigInt i = 0;
M   i = *p[0] == *p[0];				EQ(i,1)
M   i = *p[0] == *p[1];				EQ(i,0)
M   i = *p[0] == *p[2];				EQ(i,0)
M   i = *p[0] == *p[3];				EQ(i,0)
M   i = *p[0] == *p[4];				EQ(i,0)

M   i = *p[1] == *p[0];				EQ(i,0)
M   i = *p[1] == *p[1];				EQ(i,1)
M   i = *p[1] == *p[2];				EQ(i,0)
M   i = *p[1] == *p[3];				EQ(i,0)
M   i = *p[1] == *p[4];				EQ(i,0)

M   i = *p[2] == *p[0];				EQ(i,0)
M   i = *p[2] == *p[1];				EQ(i,0)
M   i = *p[2] == *p[2];				EQ(i,1)
M   i = *p[2] == *p[3];				EQ(i,0)
M   i = *p[2] == *p[4];				EQ(i,0)

M   i = *p[3] == *p[0];				EQ(i,0)
M   i = *p[3] == *p[1];				EQ(i,0)
M   i = *p[3] == *p[2];				EQ(i,0)
M   i = *p[3] == *p[3];				EQ(i,1)
M   i = *p[3] == *p[4];				EQ(i,0)

M   i = *p[4] == *p[0];				EQ(i,0)
M   i = *p[4] == *p[1];				EQ(i,0)
M   i = *p[4] == *p[2];				EQ(i,0)
M   i = *p[4] == *p[3];				EQ(i,0)
M   i = *p[4] == *p[4];				EQ(i,1)
M   }

M   //friend int operator!=(const BigInt&, const BigInt&)
M   for (i = 0; i < SIZE; i++) {
M       for (j = 0; j < SIZE; j++) {
M 		ASSERT ((i != j) == (*p[i] != *p[j]))
M       }
M   }
M   //friend int operator< (const BigInt&, const BigInt&)
M   for (i = 0; i < SIZE; i++) {
M       for (j = 0; j < SIZE; j++) {
M 		ASSERT ((i < j) == (*p[i] < *p[j]))
M       }
M   }
M   //friend int operator<=(const BigInt&, const BigInt&)
M   for (i = 0; i < SIZE; i++) {
M       for (j = 0; j < SIZE; j++) {
M 		ASSERT ((i <= j) == (*p[i] <= *p[j]))
M       }
M   }
M   //friend int operator> (const BigInt&, const BigInt&)
M   for (i = 0; i < SIZE; i++) {
M       for (j = 0; j < SIZE; j++) {
M 		ASSERT ((i >  j) == (*p[i] >  *p[j]))
M       }
M   }
M   //friend int operator>=(const BigInt&, const BigInt&)
M   for (i = 0; i < SIZE; i++) {
M       for (j = 0; j < SIZE; j++) {
M 		ASSERT ((i >= j) == (*p[i] >= *p[j]))
M       }
M   }
M   for (i = 0; i < SIZE; i++) {
M 	delete p[i];
M   }
M   //~BigInt()
M }
M   cerr << "       user bytes: " << cex_MemTab::currentUserBytes() << endl;
M   cerr << "      user blocks: " << cex_MemTab::currentUserBlocks() << endl;
M   cerr << endl << "\t****** Extended Test 1 ******" << endl << endl;
M {
M   BigInt a = 0;
M   BigInt b = "1234567890123456789012345678901234567890";
M   const REPEAT_TIMES = 1000;
M   int i;
M   cerr << "BEGIN";
M   for (i = 0; i < REPEAT_TIMES; i++) {
M       if (0 == i % FEEDBACK_INTERVAL) {
M           cerr << '.';
M       }
M       a += b;
M       ++a;
M   }
M   EQ(a,1234567890123456789012345678901234567891000)
M   for (i = 0; i < REPEAT_TIMES; i++) {
M       if (0 == i % FEEDBACK_INTERVAL) {
M           cerr << '.';
M       }
M       a -= b;
M       --a;
M   }
M   EQ(a,0)
M   for (i = 0; i < REPEAT_TIMES; i++) {
M       if (0 == i % FEEDBACK_INTERVAL) {
M           cerr << '.';
M       }
M       a -= b;
M       --a;
M   }
M   EQ(a,-1234567890123456789012345678901234567891000)
M   for (i = 0; i < REPEAT_TIMES; i++) {
M       if (0 == i % FEEDBACK_INTERVAL) {
M           cerr << '.';
M       }
M       a += b;
M       ++a;
M   }
M   EQ(a,0)
M   cerr << "DONE" << endl;
M }
M   cerr << endl;
M   cerr << "       user bytes: " << cex_MemTab::currentUserBytes() << endl;
M   cerr << "      user blocks: " << cex_MemTab::currentUserBlocks() << endl;
M   cerr << endl << "\t****** Extended Test 2 ******" << endl << endl;
M {
M   BigInt a = 0;
M   BigInt b = "1234567890123456789012345678901234567890";
M   const REPEAT_TIMES = 1000;
M   int i;
M   cerr << "BEGIN";
M   for (i = 0; i < REPEAT_TIMES; i++) {
M       if (0 == i % FEEDBACK_INTERVAL) {
M           cerr << '.';
M       }
M       a = a + b;
M       ++a;
M   }
M   EQ(a,1234567890123456789012345678901234567891000)
M   for (i = 0; i < REPEAT_TIMES; i++) {
M       if (0 == i % FEEDBACK_INTERVAL) {
M           cerr << '.';
M       }
M       a = a - b;
M       --a;
M   }
M   EQ(a,0)
M   for (i = 0; i < REPEAT_TIMES; i++) {
M       if (0 == i % FEEDBACK_INTERVAL) {
M           cerr << '.';
M       }
M       a = a - b;
M       --a;
M   }
M   EQ(a,-1234567890123456789012345678901234567891000)
M   for (i = 0; i < REPEAT_TIMES; i++) {
M       if (0 == i % FEEDBACK_INTERVAL) {
M           cerr << '.';
M       }
M       a = a + b;
M       ++a;
M   }
M   EQ(a,0)
M   cerr << "DONE" << endl;

M   cerr << endl << "        ****** FIBONACCI TESTS ******" << endl;

   {        // FIBONACCI 1000
M   BigInt  left = 0, right = 1;

M   for( BigInt i=0; i < 1000 ; ++i ) {
M     BigInt temp = right;          // temporary var to do parallel bind below
M     right = left + right;
M     left = temp;
    }

M    EQ(left,43466557686937456435688527675040625802564660517371780402481729089536555417949051890403879840079255169295922593080322634775209689623239873322471161642996440906533187938298969649928516003704476137795166849228875)
   }

   {
M    BigInt fibonacci(const BigInt &);
M    BigInt fib = fibonacci(20);		EQ(fib,6765)
   }

M   cerr << endl << "DONE" << endl << endl;
M }
M   cerr << "        ****** Memory Usage status ******" << endl << endl;
M   cerr << "       user bytes: " << cex_MemTab::currentUserBytes() << endl;
M   cerr << "      user blocks: " << cex_MemTab::currentUserBlocks() << endl;
M   cerr << endl << endl;
MS
M   timer.stop();
M   if (cex_MemTab::currentUserBlocks() > 0) {
M       for (cex_MemTabIter it; it; ++it) {
M           if (it().memoryMode() > 0) {
M               cerr << "Memory Leak: ";
M               cerr << it().size();
M               cerr << " bytes at line ";
M               cerr << it().sourceLineNumber();
M               cerr << " of ";
M               cerr << it().sourceFileName();
M               cerr << ".";
M               cerr << endl;
M           }
M       }
M   }
M   
M   cerr << endl;
M   cerr << "ID: " << BigInt::id() << ", NAME: " << BigInt::name() << endl;
M   cerr << endl;
M   cerr << "         cpu time: " << timer.cpu() << " seconds." << endl;
M   cerr << "        wall time: " << timer.wall() << " seconds." << endl;
M   cerr << endl;
M   cerr << "       user bytes: " << cex_MemTab::currentUserBytes() << endl;
M   cerr << "      user blocks: " << cex_MemTab::currentUserBlocks() << endl;
M   cerr << "     system bytes: " << cex_MemTab::currentSystemBytes() << endl;
M   cerr << "    system blocks: " << cex_MemTab::currentSystemBlocks() << endl;
M   cerr << endl;
M   cerr << "    sizeof BigInt: " << sizeof (BigInt) << endl;
M   cerr << endl;
M   long status =  cex_MemTab::status() ? cex_MemTab::status() :
                        cex_MemTab::currentUserBlocks() > 0 ? -2 : 0;
M
M   if (status || ! status) return status; // eliminate spurious warning msg

}


BigInt fibonacci(const BigInt& i)
{
  if( i == 0 )
    return 0;
  if( i == 1 || i == 2 )
    return 1;
  return  fibonacci(i-1) + fibonacci(i-2);
}
