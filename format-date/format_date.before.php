<?php

//-----------------------------------------------------------
// format dates for different display
//-----------------------------------------------------------
function format_date($dt1_in, $mode_in, $dt2_in = "")
{
    global $is_win32, $eol;
    
    switch ($mode_in) {
        case "MD HM":
            // stip the yyyy, strip the seconds -- we save six characters
            // 1/5/2010 9:57:00 PM  --> 1-5-2010 9:57 PM
            list($mm, $dd, $rest) = explode("/", $dt1_in); // mm,dd,yyyy hh:mm:ss PM
            list($yyyy, $time, $PM) = explode(" ", $rest); // yyyy, hh:mm:ss, PM
            $lc     = strrpos($time, ":"); // position of last colon (in time)
            $hhmm   = substr($time, 0, $lc); // strip the seconds :00
            $dt_out = $mm . "-" . $dd . "-" . $yyyy . "  " . $hhmm . $PM;
            break;
        case "MDY":
            // stip the yyyy, strip the seconds -- we save six characters
            // 1/5/2010 --> 1-5-2010
            list($mm, $dd, $yyyy) = explode("/", $dt1_in); // mm,dd,yyyy
            $dt_out = $mm . "-" . $dd . "-" . $yyyy;
            break;
        case "SAMEDATE":
            // compare date1 to date2, only return time if same date
            // 1/5/2010 9:57:00 PM
            if ($dt2_in > "") {
                $sp  = strpos($dt1_in, " "); // position of first space
                $dt1 = substr($dt1_in, 0, $sp);
                $sp  = strpos($dt2_in, " "); // position of first space
                $dt2 = substr($dt2_in, 0, $sp);
                if ($dt1 == $dt2) {
                    // same date, only return the time for date 2
                    list($mm, $dd, $rest) = explode("/", $dt2_in); // mm,dd,yyyy hh:mm:ss PM
                    list($yyyy, $time, $PM) = explode(" ", $rest); // yyyy, hh:mm:ss, PM
                    $lc     = strrpos($time, ":"); // position of last colon (in time)
                    $hhmm   = substr($time, 0, $lc); // strip the seconds
                    $dt_out = $hhmm . $PM;
                } else {
                    // diff date2, return cleaned up version of date 2
                    $dt_out = format_date($dt2_in, "MD HM");
                }
            } else {
                $dt_out = "???";
            }
            break;
        default:
            $dt_out = $dt1_in;
            break;
    } // end switch
    return $dt_out;
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

echo "\n";
$dateTime = new DateTime("1/5/2010 9:57:00 PM");
echo $dateTime->format('Y-m-d H:i:s');
