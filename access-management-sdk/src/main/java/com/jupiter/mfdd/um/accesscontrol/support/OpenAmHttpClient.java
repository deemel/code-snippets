package com.jupiter.mfdd.um.accesscontrol.support;

import org.jboss.resteasy.client.jaxrs.ResteasyClient;
import org.jboss.resteasy.client.jaxrs.ResteasyClientBuilder;
import org.jboss.resteasy.client.jaxrs.ResteasyWebTarget;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;

public class OpenAmHttpClient {
    private final String umServerUrl;
    private final ResteasyClient client;
    private final ResteasyWebTarget target;
    private final OpenAmRest openAmRestProxy;

    public OpenAmHttpClient() throws AuthXException {
        this(OpenAmRestUtil.getUmServerUrl());
    }

    public OpenAmHttpClient(String umServerUrl) {
        this.umServerUrl = umServerUrl;
        client = new ResteasyClientBuilder().build();
        target = client.target(this.umServerUrl);
        openAmRestProxy = target.proxy(OpenAmRest.class);
    }

    public String getUmServerUrl() {
        return umServerUrl;
    }

    public ResteasyClient getClient() {
        return client;
    }

    public ResteasyWebTarget getTarget() {
        return target;
    }

    public OpenAmRest getOpenAmRestProxy() {
        return openAmRestProxy;
    }
}
