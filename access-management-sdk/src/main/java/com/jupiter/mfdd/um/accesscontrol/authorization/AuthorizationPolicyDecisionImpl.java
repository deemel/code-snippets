package com.jupiter.mfdd.um.accesscontrol.authorization;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;
import com.jupiter.boss.sdk.accesscontrol.authorization.ActionDecision;
import com.jupiter.boss.sdk.accesscontrol.authorization.AuthorizationPolicyDecision;
import com.jupiter.mfdd.um.accesscontrol.authorization.support.ResponsePoliciesEvaluate;

/**
 * Implementation of authorization policy decision interface to support OpenAM
 * RESTful authorization services.
 */
public class AuthorizationPolicyDecisionImpl implements
        AuthorizationPolicyDecision {
    private final ResponsePoliciesEvaluate response;
    private final String resourceName;
    private final Map<String, ActionDecision> actionDecisions = new HashMap<String, ActionDecision>();
    private final Map<String, List<String>> attributeDecisions;
    private final Map<String, List<String>> adviceDecisions;

    /**
     * Constructs authorization policy decision with specified parameters.
     * 
     * @param response
     *            OpenAM policy evaluation
     * @param actionNames
     *            action names
     * @throws AuthXException
     *             if parameters are invalid or operation fails
     */
    public AuthorizationPolicyDecisionImpl(ResponsePoliciesEvaluate response,
            Set<String> actionNames) throws AuthXException {
        if (response == null)
            throw new AuthXException(
                    "Invalid authorization policy decision response <"
                            + response + ">.");

        this.response = response;
        resourceName = this.response.getResource();

        for (String i : this.response.getActions().keySet())
            if ((actionNames == null) || (actionNames.isEmpty())
                    || (actionNames.contains(i)))
                actionDecisions.put(i, ActionDecision.valueOf(this.response
                        .getActions().get(i)));

        attributeDecisions = this.response.getAttributes();
        adviceDecisions = this.response.getAdvices();
    }

    /**
     * Returns underlying OpenAM policy evaluation.
     * 
     * @return underlying OpenAM policy evaluation
     */
    public ResponsePoliciesEvaluate getResponse() {
        return response;
    }

    /**
     * Returns resource name.
     * 
     * @return resource name
     */
    @Override
    public String getResourceName() {
        return resourceName;
    }

    /**
     * Returns action decisions.
     * 
     * @return action decisions
     */
    @Override
    public Map<String, ActionDecision> getActionDecisions() {
        return actionDecisions;
    }

    /**
     * Returns attribute decisions.
     * 
     * @return attribute decisions
     */
    @Override
    public Map<String, List<String>> getAttributeDecisions() {
        return attributeDecisions;
    }

    /**
     * Returns advice decisions.
     * 
     * @return advice decisions
     */
    @Override
    public Map<String, List<String>> getAdviceDecisions() {
        return adviceDecisions;
    }

    /**
     * Returns string representation of authorization policy decision.
     * 
     * @return string representation of authorization policy decision
     */
    public String toString() {
        return "resourceName <" + resourceName + ">, actionDecisions <"
                + actionDecisions + ">, attributeDecisions <"
                + attributeDecisions + ">, adviceDecisions <" + adviceDecisions
                + ">";
    }
}
