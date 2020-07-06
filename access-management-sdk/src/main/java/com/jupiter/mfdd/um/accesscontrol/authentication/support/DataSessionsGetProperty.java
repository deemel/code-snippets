package com.jupiter.mfdd.um.accesscontrol.authentication.support;

import java.io.IOException;
import java.util.HashSet;
import java.util.Set;

import org.codehaus.jackson.JsonGenerationException;
import org.codehaus.jackson.map.JsonMappingException;
import org.codehaus.jackson.map.ObjectMapper;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;
import com.jupiter.mfdd.um.accesscontrol.support.OpenAmRestUtil;

public class DataSessionsGetProperty {
    private Set<String> properties;

    public DataSessionsGetProperty() {
    }

    public DataSessionsGetProperty(String name) throws AuthXException {
        OpenAmRestUtil.validatePropertyName(name);

        properties = new HashSet<String>();
        properties.add(name);
    }

    public DataSessionsGetProperty(Set<String> properties) {
        this.properties = properties;
    }

    public Set<String> getProperties() {
        return properties;
    }

    public void setProperties(Set<String> properties) {
        this.properties = properties;
    }

    public void addProperty(String name) throws AuthXException {
        OpenAmRestUtil.validatePropertyName(name);

        if (properties == null)
            properties = new HashSet<String>();

        properties.add(name);
    }

    public String toJson() throws JsonGenerationException,
            JsonMappingException, IOException {
        return new ObjectMapper().writeValueAsString(this);
    }
}
