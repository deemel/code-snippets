package com.jupiter.mfdd.um.accesscontrol.authentication.support;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.codehaus.jackson.JsonGenerationException;
import org.codehaus.jackson.map.JsonMappingException;
import org.codehaus.jackson.map.ObjectMapper;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;
import com.jupiter.mfdd.um.accesscontrol.support.OpenAmRestUtil;

public class DataSessionsSetProperty {
    Map<String, String> map;

    public DataSessionsSetProperty(String name, String value)
            throws AuthXException {
        OpenAmRestUtil.validatePropertyName(name);

        map = new HashMap<String, String>();
        map.put(name, value);
    }

    public DataSessionsSetProperty(Map<String, String> map) {
        this.map = map;
    }

    public Map<String, String> getMap() {
        return map;
    }

    public void setMap(Map<String, String> map) {
        this.map = map;
    }

    public String toJson() throws JsonGenerationException,
            JsonMappingException, IOException {
        return new ObjectMapper().writeValueAsString(map);
    }
}
