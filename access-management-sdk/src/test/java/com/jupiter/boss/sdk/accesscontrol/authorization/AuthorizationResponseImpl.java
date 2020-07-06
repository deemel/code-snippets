package com.jupiter.boss.sdk.accesscontrol.authorization;

import java.util.Set;

import javax.ws.rs.core.Response.Status;

/**
 * @author Byung-Woo Jun
 */

public class AuthorizationResponseImpl implements AuthorizationResponse {
    // AuthXResponseCode responseCode;
    Status responseCode;
    String responseUrl;
    Set<AuthorizationPolicyDecision> policyDecisions;

    AuthorizationResponseImpl(Status responseCode, String responseUrl,
            Set<AuthorizationPolicyDecision> policyDecisions) {
        this.responseCode = responseCode;
        this.responseUrl = responseUrl;
        this.policyDecisions = policyDecisions;
    }

    public Status getResponseCode() {
        return responseCode;
    }

    public String getResponseUrl() {
        return responseUrl;
    }

    public Set<AuthorizationPolicyDecision> getPolicyDecisions() {
        return policyDecisions;
    }
}
