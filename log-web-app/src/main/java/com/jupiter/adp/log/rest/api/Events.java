package com.jupiter.adp.log.rest.api;

import java.util.List;

public class Events {
    public String logPlane;
    public List<Event> events;

    public String toString() {
        String eventsToString = null;
        if (events == null)
            eventsToString = null;
        else if (events.isEmpty())
            eventsToString = "[]";
        else {
            eventsToString = "[";
            for (Event i : events)
                eventsToString += ("<" + i + ">; ");
            eventsToString = eventsToString.substring(0,
                    (eventsToString.length() - 2))
                    + "]";
        }

        return "logPlane <" + logPlane + ">, events <" + eventsToString + ">";
    }
}
