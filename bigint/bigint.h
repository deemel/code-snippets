#ifndef INCLUDED_BIGINT_H
#define INCLUDED_BIGINT_H

class BigInt
{
      friend BigInt add_diff    (const BigInt &, const BigInt &);
      friend BigInt add_same    (const BigInt &, const BigInt &);
      friend int    compare     (const BigInt &, const BigInt &);
      friend int    abs_compare (const BigInt &, const BigInt &);

      friend BigInt operator+ (const BigInt &, const BigInt &);
      friend BigInt operator- (const BigInt &, const BigInt &);
      friend BigInt operator- (const BigInt &);
      friend BigInt abs       (const BigInt &);

      friend int operator== (const BigInt &, const BigInt &);
      friend int operator!= (const BigInt &, const BigInt &);
      friend int operator<  (const BigInt &, const BigInt &);
      friend int operator<= (const BigInt &, const BigInt &);
      friend int operator>  (const BigInt &, const BigInt &);
      friend int operator>= (const BigInt &, const BigInt &);

      friend class ostream &operator<< (ostream &, const BigInt &);

   private :

      int *k_head;
      int *head;
      int  sign;

   public :

      BigInt (const int = 0);
      BigInt (char *);
      BigInt (const BigInt &);

      ~BigInt ();

      BigInt &operator=  (const BigInt &);
      BigInt &operator++ ();
      BigInt &operator-- ();
      BigInt &operator+= (const BigInt &);
      BigInt &operator-= (const BigInt &);

      static const char *name ();
      static const char *id   ();
};

#endif
