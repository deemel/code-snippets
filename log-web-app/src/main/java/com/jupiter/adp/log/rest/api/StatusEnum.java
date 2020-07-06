package com.jupiter.adp.log.rest.api;

public enum StatusEnum {
    red("service is not healthy"), yellow(
            "service is in the process of making the transition to green"), green(
            "service is healthy and ready to ingest events");

    private String description;

    private StatusEnum(String description) {
        this.description = description;
    }

    public String getDescription() {
        return description;
    }
}
