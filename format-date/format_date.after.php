<?php

define("FORMAT_DATE_MODE_MD_HM",    "MD HM");
define("FORMAT_DATE_MODE_MDY",      "MDY");
define("FORMAT_DATE_MODE_SAMEDATE", "SAMEDATE");
define("DATE_UNKNOWN",              "???");

function format_date($dt1_in, $mode_in, $dt2_in = "")
{
    try {
        $dateTime1 = new DateTime($dt1_in);
    } catch(Exception $e) {
        return DATE_UNKNOWN;
    }

    switch ($mode_in) {
        case FORMAT_DATE_MODE_MD_HM:
            return $dateTime1->format('n-j g:i A');

        case FORMAT_DATE_MODE_MDY:
            return $dateTime1->format('n-j-Y');

        case FORMAT_DATE_MODE_SAMEDATE:
            if (empty($dt2_in)) {
                return DATE_UNKNOWN;
            }

            try {
                $dateTime2 = new DateTime($dt2_in);
            } catch(Exception $e) {
                return DATE_UNKNOWN;
            }

            $mdy1 = $dateTime1->format('n-j-Y');
            $mdy2 = $dateTime2->format('n-j-Y');

            if ($mdy1 === $mdy2) {
                return $dateTime2->format('g:i A');
            }

            return format_date($dt2_in, FORMAT_DATE_MODE_MD_HM);
    }

    return $dt1_in;
}

echo "\n";
$x = format_date("1/5/2010 9:57:00 PM", "MD HM");
echo $x;

echo "\n";
$x = format_date("1/5/2010 9:57:00 PM", "MDY");
echo $x;

echo "\n";
$x = format_date("1/5/2010 9:57:00 PM", "SAMEDATE");
echo $x;

echo "\n";
$x = format_date("1/5/2010 9:57:00 PM", "SAMEDATE", "1/5/2010 9:57:00 PM");
echo $x;

echo "\n";
$x = format_date("1/5/2010 9:57:00 PM", "SAMEDATE", "1/5/2010 10:57:00 PM");
echo $x;

// http://sandbox.onlinephpfunctions.com/
