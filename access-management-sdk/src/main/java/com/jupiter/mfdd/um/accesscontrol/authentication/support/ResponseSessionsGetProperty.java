package com.jupiter.mfdd.um.accesscontrol.authentication.support;

import java.util.Map;

public class ResponseSessionsGetProperty {
    Map<String, String> map;

    public ResponseSessionsGetProperty(Map<String, String> map) {
        this.map = map;
    }

    public Map<String, String> getMap() {
        return map;
    }

    public void setMap(Map<String, String> map) {
        this.map = map;
    }

    public String get(String name) {
        return map.get(name);
    }
}
