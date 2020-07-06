package com.jupiter.boss.sdk.accesscontrol.authorization;

import java.util.Set;

import javax.ws.rs.core.Response.Status;

/**
 * This interface represents the authorization response object.<br>
 * This interface includes SessionToken, AuthorizationPolicyDecision and other
 * attributes. The PEP will make a decision based on the AuthorizationResponse
 * result and a set of AuthorizationPolicyDecisions.<br>
 * The fields are set during the class instance construction to make the object
 * immutable, except the setProperty for extension purposes.<br>
 * The fields should be defined as final and private. <br>
 * 
 * @version 1.0.0
 */

public interface AuthorizationResponse {
    /**
     * @return the javax.ws.rs.core.Response.Status code<br>
     */
    public Status getResponseCode();

    /**
     * @return the response url
     */
    public String getResponseUrl();

    /**
     * Get resource authorization decisions based on the policy evaluation.
     * 
     * @return Set - returns a set of AuthorziationPolicyDecision
     */
    public Set<AuthorizationPolicyDecision> getPolicyDecisions();
}
