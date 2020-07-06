#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEBUG 0
#define BUFFER_SZ 128
#define LENGTH_SZ 16
#define YY_SZ 8
#define DELIMITER ','
#define SPACE ' '
#define DATE_SEPARATOR '/'
#define EMPTY_VOTING_DETAIL ",,,,,,,,,,,"         /* 11 fields */
#define EMPTY_DIRECTOR_EXCEPTION_TOTAL ",,,,,,,"  /*  7 fields */
#define EMPTY_TOTAL ",,,,"                        /*  4 fields */

typedef struct
{
  char solicitor;
  char cusip;
  char client;
  char total;
} FLAG;

typedef struct
{
  FLAG flag;
  char ibuffer[BUFFER_SZ], yy[YY_SZ];
  int length[LENGTH_SZ], trailing_sign[LENGTH_SZ], date[LENGTH_SZ];
  int line;

  char solicitor[BUFFER_SZ];  /* 100 record */
  char cusip[BUFFER_SZ];      /* 200 record */
  char client[BUFFER_SZ];     /* 250 record */
  char obuffer[BUFFER_SZ];    /* 3xx, 4xx, 5xx records */
  char total[BUFFER_SZ];      /* 900 record */
} PROXY_TRANSMISSION;

void outta_here (FILE *fp_in, FILE *fp_out, int status)
{
  fclose (fp_in);
  fclose (fp_out);

  exit (status);
}

void delimit (char *source, char *destination, char *yy, int *length, int *trailing_sign, int *date)
{
  char *p, *q;

  for (p = source, q = destination; *length > 0; ++length, ++trailing_sign, ++date)
  {
    if (*p)  /* non-empty source field */
    {
#if (! DEBUG)
      if (*date)
      {
        strncpy (q, yy, 2);  /* yy */
        q += 2;
        strncpy (q, p, 2);  /* yy */
        p += 2;
        q += 2;
        *q = DATE_SEPARATOR;  /* / */
        ++q;
        strncpy (q, p, 2);  /* mm */
        p += 2;
        q += 2;
        *q = DATE_SEPARATOR;  /* / */
        ++q;
        strncpy (q, p, 2);  /* dd */
        p += 2;
        q += 2;
      }
      else
#endif
      {
        strncpy (q, p, *length);
        p += *length;
        q += *length;
      }
    }

#if (! DEBUG)
    if (*trailing_sign)
      --q;  /* strip trailing sign */

    for (; *(q - 1) == SPACE; --q)  /* strip trailing spaces */
      ;

    *q = DELIMITER;
    ++q;
#endif
    *q = '\0';
  }

#if (DEBUG)
  printf ("%s\n", destination);
#endif
}

int main (int argc, char **argv)
{
  FILE *fp_in, *fp_out;
  PROXY_TRANSMISSION pt;
  char c;
  int i, j, *p, *q, *r;
  time_t time_current;
  struct tm *time_local;

  fp_in = fp_out = NULL;

  if (argc != 3)
  {
    printf ("Usage: %s <input file> <output file>\n", argv[0]);
    printf ("Example: %s S457_TESTFILE_30708.TXT S457_TESTFILE_30708.CSV\n", argv[0]);
    outta_here (fp_in, fp_out, EXIT_FAILURE);
  }

  if (! (fp_in = fopen (argv[1], "r")))
  {
    printf ("Unable to open \"%s\" for read.  Exiting.\n", argv[1]);
    outta_here (fp_in, fp_out, EXIT_FAILURE);
  }

#if 0
  if (fp_out = fopen (argv[2], "r"))
  {
    printf ("\"%s\" already exist.  Exiting.\n", argv[2]);
    outta_here (fp_in, fp_out, EXIT_FAILURE);
  }
#endif

  if (! (fp_out = fopen (argv[2], "w")))
  {
    printf ("Unable to open \"%s\" for write.  Exiting.\n", argv[2]);
    outta_here (fp_in, fp_out, EXIT_FAILURE);
  }

  memset (&pt, 0, sizeof (pt));
  pt.line = 1;

  time_current = time (NULL);
  time_local = localtime (&time_current);
  strftime (pt.yy, YY_SZ, "%Y", time_local);
  pt.yy[2] = '\0';

  while (fgets (pt.ibuffer, sizeof (pt.ibuffer), fp_in))
  {
    /*
     * Strip trailing CRLF (\r\n)
     */

    for (i = strlen (pt.ibuffer), j = 1; j <= 2; ++j)
      if ((pt.ibuffer[i - j] == '\r') || (pt.ibuffer[i - j] == '\n'))
        pt.ibuffer[i - j] = '\0';

    switch (pt.ibuffer[0])
    {
      case '1':

        if (! strncmp (&pt.ibuffer[1], "00", 2))  /* 100 (solicitor) record */
        {
          if (pt.flag.solicitor)
          {
            printf ("%s\n", pt.ibuffer);
            printf ("Multiple solicitor record found in line %d.  Exiting.\n", pt.line);
            outta_here (fp_in, fp_out, EXIT_FAILURE);
          }

          p = pt.length;  q = pt.trailing_sign;  r = pt.date;
          *p++ =  3;      *q++ = 0;              *r++ = 0;
          *p++ =  6;      *q++ = 0;              *r++ = 0;
          *p++ =  5;      *q++ = 0;              *r++ = 0;
          *p++ = 40;      *q++ = 0;              *r++ = 0;
          *p++ =  6;      *q++ = 0;              *r++ = 1;
          *p++ =  6;      *q++ = 0;              *r++ = 0;
          *p++ =  3;      *q++ = 0;              *r++ = 0;
          *p++ =  7;      *q++ = 0;              *r++ = 0;
          *p++ =  4;      *q++ = 0;              *r++ = 0;
          *p++ =  0;      *q++ = 0;              *r++ = 0;
          delimit (pt.ibuffer, pt.solicitor, pt.yy, pt.length, pt.trailing_sign, pt.date);

          pt.flag.solicitor = 1;
          break;
        }

      case '2':

        if (! strncmp (&pt.ibuffer[1], "00", 2))  /* 200 (CUSIP) record */
        {
          if (! pt.flag.solicitor)
          {
            printf ("%s\n", pt.ibuffer);
            printf ("No solicitor record found prior to CUSIP record in line %d.  Exiting.\n", pt.line);
            outta_here (fp_in, fp_out, EXIT_FAILURE);
          }

          p = pt.length;  q = pt.trailing_sign;  r = pt.date;
          *p++ =  3;      *q++ = 0;              *r++ = 0;
          *p++ =  2;      *q++ = 0;              *r++ = 0;
          *p++ =  9;      *q++ = 0;              *r++ = 0;
          *p++ =  1;      *q++ = 0;              *r++ = 0;
          *p++ = 33;      *q++ = 0;              *r++ = 0;
          *p++ =  7;      *q++ = 0;              *r++ = 0;
          *p++ =  6;      *q++ = 0;              *r++ = 0;
          *p++ =  6;      *q++ = 0;              *r++ = 1;
          *p++ =  6;      *q++ = 0;              *r++ = 1;
          *p++ =  6;      *q++ = 0;              *r++ = 1;
          *p++ =  1;      *q++ = 0;              *r++ = 1;
          *p++ =  0;      *q++ = 0;              *r++ = 0;
          delimit (pt.ibuffer, pt.cusip, pt.yy, pt.length, pt.trailing_sign, pt.date);

          pt.flag.cusip = 1;
          pt.flag.client = 0;
          break;
        }

        if (! strncmp (&pt.ibuffer[1], "50", 2))  /* 250 (client) record */
        {
          if (! pt.flag.cusip)
          {
            printf ("%s\n", pt.ibuffer);
            printf ("No CUSIP record found prior to client record in line %d.  Exiting.\n", pt.line);
            outta_here (fp_in, fp_out, EXIT_FAILURE);
          }

          p = pt.length;  q = pt.trailing_sign;  r = pt.date;
          *p++ =  3;      *q++ = 0;              *r++ = 0;
          *p++ =  2;      *q++ = 0;              *r++ = 0;
          *p++ =  3;      *q++ = 0;              *r++ = 0;
          *p++ = 10;      *q++ = 0;              *r++ = 0;
          *p++ = 20;      *q++ = 0;              *r++ = 0;
          *p++ = 34;      *q++ = 0;              *r++ = 0;
          *p++ =  5;      *q++ = 0;              *r++ = 0;
          *p++ =  3;      *q++ = 0;              *r++ = 0;
          *p++ =  0;      *q++ = 0;              *r++ = 0;
          delimit (pt.ibuffer, pt.client, pt.yy, pt.length, pt.trailing_sign, pt.date);

          pt.flag.client = 1;
          break;
        }

      case '3':
      case '4':

        c = pt.ibuffer[3];
        pt.ibuffer[3] = '\0';
        i = atoi (&pt.ibuffer[1]);
        pt.ibuffer[3] = c;

        if ((i >= 1) && (i <= 20))  /* 301-320 (initial) or 401-420 (revised) voting detail record */
        {
          if (! pt.flag.client)
          {
            printf ("%s\n", pt.ibuffer);
            printf ("No client record found prior to %s voting detail record in line %d.  Exiting.\n", ((pt.ibuffer[0] == '3') ? ("initial") : ("revised")), pt.line);
            outta_here (fp_in, fp_out, EXIT_FAILURE);
          }

          p = pt.length;  q = pt.trailing_sign;  r = pt.date;
          *p++ =  1;      *q++ = 0;              *r++ = 0;
          *p++ =  2;      *q++ = 0;              *r++ = 0;
          *p++ =  2;      *q++ = 0;              *r++ = 0;
          *p++ =  3;      *q++ = 0;              *r++ = 0;
          *p++ = 10;      *q++ = 0;              *r++ = 0;
          *p++ = 15;      *q++ = 1;              *r++ = 0;
          *p++ =  1;      *q++ = 0;              *r++ = 0;
          *p++ = 15;      *q++ = 1;              *r++ = 0;
          *p++ = 15;      *q++ = 1;              *r++ = 0;
          *p++ = 15;      *q++ = 1;              *r++ = 0;
          *p++ =  1;      *q++ = 0;              *r++ = 0;
          *p++ =  0;      *q++ = 0;              *r++ = 0;
          delimit (pt.ibuffer, pt.obuffer, pt.yy, pt.length, pt.trailing_sign, pt.date);

#if (! DEBUG)
          if (pt.ibuffer[0] == '3')
            fprintf (fp_out, "%s%s%s%s%s%s%s\n", pt.solicitor, ((pt.flag.total) ? (pt.total) : (EMPTY_TOTAL)), pt.cusip, pt.client, pt.obuffer, EMPTY_VOTING_DETAIL, EMPTY_DIRECTOR_EXCEPTION_TOTAL);
          else
            fprintf (fp_out, "%s%s%s%s%s%s%s\n", pt.solicitor, ((pt.flag.total) ? (pt.total) : (EMPTY_TOTAL)), pt.cusip, pt.client, EMPTY_VOTING_DETAIL, pt.obuffer, EMPTY_DIRECTOR_EXCEPTION_TOTAL);
#endif

          break;
        }

      case '5':

        c = pt.ibuffer[3];
        pt.ibuffer[3] = '\0';
        i = atoi (&pt.ibuffer[1]);
        pt.ibuffer[3] = c;

        if ((i >= 1) && (i <= 40))  /* 501-540 (director exception total) record */
        {
          if (! pt.flag.client)
          {
            printf ("%s\n", pt.ibuffer);
            printf ("No client record found prior to director exception total record in line %d.  Exiting.\n", pt.line);
            outta_here (fp_in, fp_out, EXIT_FAILURE);
          }

          p = pt.length;  q = pt.trailing_sign;  r = pt.date;
          *p++ =  1;      *q++ = 0;              *r++ = 0;
          *p++ =  2;      *q++ = 0;              *r++ = 0;
          *p++ =  3;      *q++ = 0;              *r++ = 0;
          *p++ = 20;      *q++ = 0;              *r++ = 0;
          *p++ =  9;      *q++ = 0;              *r++ = 0;
          *p++ = 15;      *q++ = 1;              *r++ = 0;
          *p++ = 30;      *q++ = 0;              *r++ = 0;
          *p++ =  0;      *q++ = 0;              *r++ = 0;
          delimit (pt.ibuffer, pt.obuffer, pt.yy, pt.length, pt.trailing_sign, pt.date);

#if (! DEBUG)
          fprintf (fp_out, "%s%s%s%s%s%s%s\n", pt.solicitor, ((pt.flag.total) ? (pt.total) : (EMPTY_TOTAL)), pt.cusip, pt.client, EMPTY_VOTING_DETAIL, EMPTY_VOTING_DETAIL, pt.obuffer);
#endif

          break;
        }

      case '9':

        if (! strncmp (&pt.ibuffer[1], "00", 2))  /* 900 (total) record */
        {
          if (pt.flag.total)
          {
            printf ("%s\n", pt.ibuffer);
            printf ("Multiple total record found in line %d.  Exiting.\n", pt.line);
            outta_here (fp_in, fp_out, EXIT_FAILURE);
          }

          p = pt.length;  q = pt.trailing_sign;  r = pt.date;
          *p++ =  3;      *q++ = 0;              *r++ = 0;
          *p++ =  6;      *q++ = 0;              *r++ = 0;
          *p++ =  9;      *q++ = 0;              *r++ = 0;
          *p++ = 62;      *q++ = 0;              *r++ = 0;
          *p++ =  0;      *q++ = 0;              *r++ = 0;
          delimit (pt.ibuffer, pt.total, pt.yy, pt.length, pt.trailing_sign, pt.date);

          pt.flag.total = 1;
          break;
        }

      default:

        printf ("%s\n", pt.ibuffer);
        printf ("Unrecognized record ID in line %d.  Exiting.\n", pt.line);
        outta_here (fp_in, fp_out, EXIT_FAILURE);
    }

    memset (pt.ibuffer, 0, sizeof (pt.ibuffer));
    memset (pt.obuffer, 0, sizeof (pt.obuffer));
    ++pt.line;
  }

  outta_here (fp_in, fp_out, EXIT_SUCCESS);
  return EXIT_SUCCESS;
}
