package com.jupiter.boss.sdk.accesscontrol.authorization;

import java.util.List;
import java.util.Map;

/**
 * This interface represents the authorization policy decision object which
 * consists of Actions, Attributes and Advices.<br>
 * This object will be part of the AuthorizationResponse object.<br>
 * The PEP will make a decision based on the AuthorizationResponse result.<br>
 * The fields are set during the class instance construction to make the object
 * immutable. The fields should be defined as final and private.
 * 
 * <br>
 * 
 * @version 1.0.0
 */

public interface AuthorizationPolicyDecision {
    /**
     * Get a resource name.
     * 
     * @return a resource name
     */
    public String getResourceName();

    /**
     * Get action decisions.
     * 
     * @return an Action list
     */
    public Map<String, ActionDecision> getActionDecisions();

    /**
     * Get attribute decisions.
     * 
     * @return an Attribute list
     */
    public Map<String, List<String>> getAttributeDecisions();

    /**
     * Get advice decisions.
     * 
     * @return an Advice list
     */
    public Map<String, List<String>> getAdviceDecisions();
}
