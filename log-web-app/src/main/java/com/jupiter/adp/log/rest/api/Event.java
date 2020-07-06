package com.jupiter.adp.log.rest.api;

public class Event {
    public String logPlane;
    public String dateTime;
    public String source;
    public String sourceSyntax;
    public String level;
    public String message;

    public String toString() {
        return "dateTime <" + dateTime + ">, source <" + source
                + ">, sourceSyntax <" + sourceSyntax + ">, level <" + level
                + ">, message <" + message + ">";
    }

    public String toStringSendLogstash() {
        return "logPlane <" + logPlane + ">, " + toString();
    }
}
