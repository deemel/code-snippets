package com.jupiter.mfdd.um.accesscontrol.authorization.support;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.ws.rs.core.GenericType;
import javax.ws.rs.core.Response;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;

public class ResponsePoliciesEvaluate {
    private String resource;
    private Map<String, Boolean> actions = new HashMap<String, Boolean>();
    private Map<String, List<String>> attributes = new HashMap<String, List<String>>();
    private Map<String, List<String>> advices = new HashMap<String, List<String>>();
    private Long ttl;

    public String getResource() {
        return resource;
    }

    public Map<String, Boolean> getActions() {
        return actions;
    }

    public Map<String, List<String>> getAttributes() {
        return attributes;
    }

    public Map<String, List<String>> getAdvices() {
        return advices;
    }

    public Long getTtl() {
        return ttl;
    }

    public void setResource(String resource) {
        this.resource = resource;
    }

    public void setActions(Map<String, Boolean> actions) {
        this.actions = actions;
    }

    public void setAttributes(Map<String, List<String>> attributes) {
        this.attributes = attributes;
    }

    public void setAdvices(Map<String, List<String>> advices) {
        this.advices = advices;
    }

    public void setTtl(Long ttl) {
        this.ttl = ttl;
    }

    public static boolean isAuthorized(Response response, String resourceName,
            String actionName) throws AuthXException {
        if (response == null)
            throw new AuthXException("Invalid authorization response <"
                    + response + ">.");

        List<ResponsePoliciesEvaluate> responseEntity = response
                .readEntity(new GenericType<List<ResponsePoliciesEvaluate>>() {
                });

        if ((responseEntity == null) || (responseEntity.size() < 1))
            return false;

        for (ResponsePoliciesEvaluate i : responseEntity) {
            if (!resourceName.matches(i.getResource())
                    && (!resourceName.equals(i.getResource())))
                continue;

            Map<String, Boolean> actions = i.getActions();
            if ((actions == null) || (actions.size() < 1))
                return false;

            Boolean isAllowed = actions.get(actionName);
            return ((isAllowed == null) ? (false) : (isAllowed));
        }

        return false;
    }
}
