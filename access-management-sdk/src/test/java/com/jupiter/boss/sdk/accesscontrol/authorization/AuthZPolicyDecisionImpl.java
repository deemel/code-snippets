package com.jupiter.boss.sdk.accesscontrol.authorization;

import java.util.List;
import java.util.Map;

/**
 * @author Byung-Woo Jun
 */

public class AuthZPolicyDecisionImpl implements AuthorizationPolicyDecision {

    String resourceName;
    Map<String, ActionDecision> actionDecisions;
    Map<String, List<String>> attributeDecisions;
    Map<String, List<String>> adviceDecisions;

    AuthZPolicyDecisionImpl(String resourceName,
            Map<String, ActionDecision> actionDecisions,
            Map<String, List<String>> attributeDecisions,
            Map<String, List<String>> adviceDecisions) {
        this.resourceName = resourceName;
        this.actionDecisions = actionDecisions;
        this.attributeDecisions = attributeDecisions;
        this.adviceDecisions = adviceDecisions;
    }

    /**
     * @return an Action list
     */
    public String getResourceName() {
        return resourceName;
    }

    /**
     * @return an Action list
     */
    public Map<String, ActionDecision> getActionDecisions() {
        return actionDecisions;
    }

    /**
     * @return an Attribute list
     */
    public Map<String, List<String>> getAttributeDecisions() {
        return attributeDecisions;
    }

    /**
     * @return an Advice list
     */
    public Map<String, List<String>> getAdviceDecisions() {
        return adviceDecisions;
    }
}
