#include "bigint.h"
#include <iostream.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ABS(x) ((x < 0) ? -x : x)

enum {FALSE, TRUE};

const int    MAX_DIGITS = 9;
const int    MAX_N_ONE  = 1000000000;
const BigInt N_ONE      = -1;
const BigInt P_ONE      = 1;

BigInt::BigInt (const int number)
{
   k_head = new int [2];
   assert (k_head != 0);
   head = k_head;

   register int num = ABS (number);
   if (num / MAX_N_ONE)
   {
      head [0] = num / MAX_N_ONE;
      head [1] = num % MAX_N_ONE;
      sign     = (number < 0) ? -2 : 2;
   }
   else
   {
      head [0] = num;
      sign     = (number < 0) ? -1 : 1;
   }
}

BigInt::BigInt (char *number)
{
   register char *ptr = number;

   if (*ptr == '+')
   {
      sign = 1;
      ++ptr;
   }
   else
      if (*ptr == '-')
      {
         sign = -1;
         ++ptr;
      }
      else
         sign = 1;

   while (*ptr == '0')
      ++ptr;

   if (*ptr == '\0')
      --ptr;

   register int  length = strlen (ptr);
   register int  floor  = length / MAX_DIGITS;
   register int  end    = floor;
   register int  remain = length % MAX_DIGITS;
   register char temp_char;
   register int  info;
   register int  check_zero = TRUE;
   register int  zero_count = 0;
   register int  i          = 0;

   k_head = new int [floor + 1];
   assert (k_head != 0);
   head = k_head;

   if (remain)
   {
      temp_char    = ptr [remain];
      ptr [remain] = '\0';

      info = atoi (ptr);

      if (check_zero)
         if (info == 0)
            ++zero_count;
         else
         {
            head [0] = info;
            check_zero = FALSE;
         }
      else
         head [0] = info;

      ptr [remain] = temp_char;
      ptr = &ptr [remain];

      ++i;
   }
   else
      --end;

   if (floor != 0)
      for (; i <= end; ++i)
      {
         temp_char        = ptr [MAX_DIGITS];
         ptr [MAX_DIGITS] = '\0';

         info = atoi (ptr);

         if (check_zero)
            if (info == 0)
               ++zero_count;
            else
            {
               head [i] = info;
               check_zero = FALSE;
            }
         else
            head [i] = info;

         ptr [MAX_DIGITS] = temp_char;
         ptr = &ptr [MAX_DIGITS];
      }

   sign *= (end + 1 - zero_count);
   if (sign == 0)
   {
      head [0] = 0;
      sign     = 1;
   }
}

BigInt::BigInt (const BigInt &number)
{
   k_head = new int [ABS (number.sign)];
   assert (k_head != 0);
   head = k_head;

   memcpy (head, number.head, (ABS (number.sign) * sizeof (int)));
   sign   = number.sign;
}

BigInt::~BigInt ()
{
   delete [] k_head;
}

BigInt &BigInt::operator= (const BigInt &number)
{
   if (ABS (sign) < ABS (number.sign))
   {
      delete [] k_head;

      k_head = new int [ABS (number.sign)];
      assert (k_head != 0);
      head = k_head;
   }

   memcpy (head, number.head, (ABS (number.sign) * sizeof (int)));
   sign   = number.sign;

   return *this;
}

BigInt &BigInt::operator++ ()
{
   *this = *this + P_ONE;

   return *this;
}

BigInt &BigInt::operator-- ()
{
   *this = *this + N_ONE;

   return *this;
}

BigInt &BigInt::operator+= (const BigInt &number)
{
   *this = *this + number;

   return *this;
}

BigInt &BigInt::operator-= (const BigInt &number)
{
   *this = *this + -number;

   return *this;
}

const char*BigInt::name ()
{
   return ("Lee, Danny M.");
}

const char*BigInt::id ()
{
   return ("xxx-xx-xxxx");
}

BigInt add_diff (const BigInt &number_l, const BigInt &number_r)
{
   register int    abs_l = ABS (number_l.sign);
   register int    abs_r = ABS (number_r.sign);
   register BigInt answer;

   delete [] answer.k_head;
   answer.k_head = new int [abs_l];
   assert (answer.k_head != 0);

   register int *l_ptr  = &number_l.head [abs_l - 1];
   register int *r_ptr  = &number_r.head [abs_r - 1];
   register int *end    = &answer.k_head [abs_l - 1];
   register int  borrow = 0;
   register int  zero   = FALSE;
   register int *z_ptr;

   for (answer.head = &answer.k_head [abs_l - 1]; r_ptr >= number_r.head; --r_ptr, --l_ptr, --answer.head)
   {
      *answer.head = *l_ptr - *r_ptr - borrow;

      if (*answer.head < 0)
      {
         *answer.head += MAX_N_ONE;
         borrow = 1;
      }
      else
         borrow = 0;

      if (*answer.head == 0)
      {
         if (! zero)
            z_ptr = answer.head;

         zero = TRUE;
      }
      else
         zero = FALSE;
   }

   if (abs_l != abs_r)
      for (; l_ptr >= number_l.head; --l_ptr, --answer.head)
      {
         *answer.head = *l_ptr - borrow;

         if (*answer.head < 0)
         {
            *answer.head += MAX_N_ONE;
            borrow = 1;
         }
         else
            borrow = 0;

         if (*answer.head == 0)
         {
            if (! zero)
               z_ptr = answer.head;

            zero = TRUE;
         }
         else
            zero = FALSE;
      }

   ++answer.head;

   if (zero)
   {
      answer.head = (z_ptr != end) ? (z_ptr + 1) : end;

      if ((answer.head == end) && (*answer.head == 0))
         answer.sign = 1;
      else
         answer.sign = (number_l.sign < 0) ? -(end - answer.head + 1) : (end - answer.head + 1);
   }
   else
      answer.sign = (number_l.sign < 0) ? -(end - answer.head + 1) : (end - answer.head + 1);

   return answer;
}

BigInt add_same (const BigInt &number_l, const BigInt &number_r)
{
   register int    abs_l = ABS (number_l.sign);
   register int    abs_r = ABS (number_r.sign);
   register BigInt answer;

   delete [] answer.k_head;
   answer.k_head = new int [abs_l + 1];
   assert (answer.k_head != 0);

   register int *l_ptr = &number_l.head [abs_l - 1];
   register int *r_ptr = &number_r.head [abs_r - 1];
   register int  carry = 0;

   for (answer.head = &answer.k_head [abs_l]; r_ptr >= number_r.head; --r_ptr, --l_ptr, --answer.head)
   {
      *answer.head = *l_ptr + *r_ptr + carry;

      carry = *answer.head / MAX_N_ONE;
      if (carry)
         *answer.head %= MAX_N_ONE;
   }

   if (abs_l != abs_r)
      for (; l_ptr >= number_l.head; --l_ptr, --answer.head)
      {
         *answer.head = *l_ptr + carry;

         carry = *answer.head / MAX_N_ONE;
         if (carry)
            *answer.head %= MAX_N_ONE;
      }

   ++answer.head;

   if (carry)
   {
      --answer.head;
      *answer.head = carry;
      answer.sign = (number_l.sign < 0) ? -(abs_l + 1) : (abs_l + 1);
   }
   else
      answer.sign = (number_l.sign < 0) ? -abs_l : abs_l;

   return answer;
}

int compare (const BigInt &number_l, const BigInt &number_r)
{
   register int sign_l = (number_l.sign < 0) ? -1 : 1;
            int sign_r = (number_r.sign < 0) ? -1 : 1;

   if ((sign_l * sign_r) < 0)
      return sign_l;
   else
      if (sign_l < 0)
         return (-abs_compare (number_l, number_r));
      else
         return (abs_compare (number_l, number_r));
}

int abs_compare (const BigInt &number_l, const BigInt &number_r)
{
   register int abs_l = ABS (number_l.sign);
   register int abs_r = ABS (number_r.sign);

   if (abs_l < abs_r)
      return -1;

   if (abs_l > abs_r)
      return 1;

   if (memcmp (number_l.head, number_r.head, (abs_l * sizeof (int))) == 0)
      return 0;

   for (register int i = 0; i < abs_l; ++i)
   {
      if (number_l.head [i] < number_r.head [i])
         return -1;

      if (number_l.head [i] > number_r.head [i])
         return 1;
   }

   return 0;
}

BigInt operator+ (const BigInt &number_l, const BigInt &number_r)
{
   int sign_l = (number_l.sign < 0) ? -1 : 1;
   int sign_r = (number_r.sign < 0) ? -1 : 1;

   if ((sign_l * sign_r) < 0)
      if (abs (number_l) > abs (number_r))
         return (add_diff (number_l, number_r));
      else
         return (add_diff (number_r, number_l));
   else
      if (abs (number_l) > abs (number_r))
         return (add_same (number_l, number_r));
      else
         return (add_same (number_r, number_l));
}

BigInt operator- (const BigInt &number_l, const BigInt &number_r)
{
   return (number_l + -number_r);
}

BigInt operator- (const BigInt &number)
{
   BigInt answer = number;

   if (*answer.head != 0)
      answer.sign *= -1;

   return answer;
}

BigInt abs (const BigInt &number)
{
   BigInt answer = number;

   answer.sign = ABS (answer.sign);

   return answer;
}

int operator== (const BigInt &number_l, const BigInt &number_r)
{
   if (compare (number_l, number_r) == 0)
      return TRUE;
   else
      return FALSE;
}

int operator!= (const BigInt &number_l, const BigInt &number_r)
{
   if (compare (number_l, number_r) != 0)
      return TRUE;
   else
      return FALSE;
}

int operator< (const BigInt &number_l, const BigInt &number_r)
{
   if (compare (number_l, number_r) < 0)
      return TRUE;
   else
      return FALSE;
}

int operator<= (const BigInt &number_l, const BigInt &number_r)
{
   if (compare (number_l, number_r) <= 0)
      return TRUE;
   else
      return FALSE;
}

int operator> (const BigInt &number_l, const BigInt &number_r)
{
   if (compare (number_l, number_r) > 0)
      return TRUE;
   else
      return FALSE;
}

int operator>= (const BigInt &number_l, const BigInt &number_r)
{
   if (compare (number_l, number_r) >= 0)
      return TRUE;
   else
      return FALSE;
}

ostream &operator<< (ostream &out, const BigInt &number)
{
   if (number.sign < 0)
      out << '-';

   out << *number.head;

   register char buffer [MAX_DIGITS + 1];

   for (register int i = 1; i < ABS (number.sign); ++i)
   {
      sprintf (buffer, "%09d", number.head [i]);
      out << buffer;
   }

   return out;
}
