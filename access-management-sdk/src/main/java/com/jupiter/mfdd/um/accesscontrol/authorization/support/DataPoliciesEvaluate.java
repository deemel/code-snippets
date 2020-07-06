package com.jupiter.mfdd.um.accesscontrol.authorization.support;

import java.io.IOException;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.codehaus.jackson.JsonGenerationException;
import org.codehaus.jackson.map.JsonMappingException;
import org.codehaus.jackson.map.ObjectMapper;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;
import com.jupiter.mfdd.um.accesscontrol.support.OpenAmRestUtil;

public class DataPoliciesEvaluate {
    public static class Subject {
        /*
         * From OpenAM Developer's Guide, Version 13, 2.1.2.2.1 Requesting
         * Policy Decisions For Specific Resources: "The value is the SSO token
         * ID string for the subject, returned for example on successful
         * authentication."
         */
        private String ssoToken;

        /*
         * From OpenAM Developer's Guide, Version 13, 2.1.2.2.1 Requesting
         * Policy Decisions For Specific Resources: "The value is a JWT string."
         */
        private String jwt;

        /*
         * From OpenAM Developer's Guide, Version 13, 2.1.2.2.1 Requesting
         * Policy Decisions For Specific Resources: "The value is an object
         * (map) of JWT claims to their values."
         */
        private Map<String, String> claims;

        public Subject(String ssoToken, String jwt, Map<String, String> claims) {
            this.ssoToken = ssoToken;
            this.jwt = jwt;
            this.claims = claims;
        }

        public String getSsoToken() {
            return ssoToken;
        }

        public String getJwt() {
            return jwt;
        }

        public Map<String, String> getClaims() {
            return claims;
        }

        public void setSsoToken(String ssoToken) {
            this.ssoToken = ssoToken;
        }

        public void setJwt(String jwt) {
            this.jwt = jwt;
        }

        public void setClaims(Map<String, String> claims) {
            this.claims = claims;
        }
    }

    /*
     * From OpenAM Developer's Guide, Version 13, 2.1.2.2.1 Requesting Policy
     * Decisions For Specific Resources: "This required field specifies the list
     * of resources for which to return decisions."
     */
    private Set<String> resources;

    /*
     * From OpenAM Developer's Guide, Version 13, 2.1.2.2.1 Requesting Policy
     * Decisions For Specific Resources: "This field holds the name of the
     * policy set, and defaults to "iPlanetAMWebAgentService" if not specified."
     */
    private String application;

    /*
     * From OpenAM Developer's Guide, Version 13, 2.1.2.2.1 Requesting Policy
     * Decisions For Specific Resources: "This optional field holds an object
     * that represents the subject. You can specify one or more of the following
     * keys. If you specify multiple keys, the subject can have multiple
     * associated principals, and you can use subject conditions corresponding
     * to any type in the request. If you do not specify the subject, OpenAM
     * uses the SSO token ID of the subject making the request."
     */
    private Subject subject;

    /*
     * From OpenAM Developer's Guide, Version 13, 2.1.2.2.1 Requesting Policy
     * Decisions For Specific Resources: "This optional field holds a map of
     * keys to lists of values. If you do not specify the environment, the
     * default is an empty map."
     */
    private Map<String, List<String>> environment;

    public DataPoliciesEvaluate(Set<String> resources,
            Map<String, List<String>> envParameters) throws AuthXException {
        OpenAmRestUtil.validateResources(resources);

        this.resources = resources;
        this.environment = envParameters;
    }

    public DataPoliciesEvaluate(Set<String> resources, String application,
            Subject subject, Map<String, List<String>> environment)
            throws AuthXException {
        OpenAmRestUtil.validateResources(resources);

        this.resources = resources;
        this.application = application;
        this.subject = subject;
        this.environment = environment;
    }

    public Set<String> getResources() {
        return resources;
    }

    public String getApplication() {
        return application;
    }

    public Subject getSubject() {
        return subject;
    }

    public Map<String, List<String>> getEnvironment() {
        return environment;
    }

    public void setResources(Set<String> resources) {
        this.resources = resources;
    }

    public void setApplication(String application) {
        this.application = application;
    }

    public void setSubject(Subject subject) {
        this.subject = subject;
    }

    public void setEnvironment(Map<String, List<String>> environment) {
        this.environment = environment;
    }

    public String toJson() throws JsonGenerationException,
            JsonMappingException, IOException {
        return new ObjectMapper().writeValueAsString(this);
    }
}
