package com.jupiter.boss.sdk.accesscontrol.authorization;

import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import javax.ws.rs.core.Response.Status;

import com.jupiter.boss.sdk.accesscontrol.AuthXException;
import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;

/**
 * @author Byung-Woo Jun
 */

public class AuthorizationServiceImpl implements AuthorizationService {

    public boolean isAuthorized(SessionTokenID sessionTokenId,
            String resourceName, String actionName) throws AuthXException {
        boolean authZResult = false;

        // call the PDP for authorization check-up and get a policy decision.
        // assume it got the following policy decision from the PDP
        // beginning of simulation
        Set<String> resourceNames = new HashSet<String>();
        resourceNames.add(resourceName);
        authZResult = analyzePolicyActionResponse(resourceName, actionName,
                simulatePDPResponse(resourceNames));
        // end of simulation
        return authZResult;
    };

    public boolean isAuthorized(SessionTokenID sessionTokenId,
            String resourceName, String actionName,
            Map<String, List<String>> envParameters) throws AuthXException {
        boolean authZResult = false;
        // ...
        // call the PDP for authorization check-up; get an authorization
        // decision.
        // assume it got the following policy decision from the PDP
        // beginning of simulation
        Set<String> resourceNames = new HashSet<String>();
        resourceNames.add(resourceName);
        authZResult = analyzePolicyActionResponse(resourceName, actionName,
                simulatePDPResponse(resourceNames));
        // end of simulation
        return authZResult;
    }

    public AuthorizationResponse authorize(SessionTokenID sessionTokenId,
            String resourceName, Set<String> actionNames,
            Map<String, List<String>> envParameters) throws AuthXException {
        // ...
        // call the PDP for authorization check-up; get an authorization
        // decision.
        // assume it got the following policy decision from the PDP
        // beginning of simulation
        Set<String> resourceNames = new HashSet<String>();
        resourceNames.add(resourceName);
        AuthorizationResponse pdpResponse = simulatePDPResponse(resourceNames);

        pdpResponse.getResponseCode();

        AuthorizationResponse authZResponse = new AuthorizationResponseImpl(
                pdpResponse.getResponseCode(), pdpResponse.getResponseUrl(),
                pdpResponse.getPolicyDecisions());
        // end of simulation
        return authZResponse;
    }

    public AuthorizationResponse authorize(SessionTokenID sessionTokenId,
            Set<String> resourceNames, Set<String> actionNames,
            Map<String, List<String>> envParameters) throws AuthXException {
        // ...
        // call the PDP for authorization check-up; get an authorization
        // decision.
        // assume it got the following policy decision from the PDP
        // beginning of simulation
        AuthorizationResponse pdpResponse = simulatePDPResponse(resourceNames);
        // end of simulation

        AuthorizationResponse authZResponse = new AuthorizationResponseImpl(
                pdpResponse.getResponseCode(), pdpResponse.getResponseUrl(),
                pdpResponse.getPolicyDecisions());

        return authZResponse;
    }

    private boolean analyzePolicyActionResponse(String resourceName,
            String actionName, AuthorizationResponse authZResponse) {
        boolean authZResult = false;
        // the following code is a simulation
        if (authZResponse.getResponseCode() != Status.OK) {
            System.out.println("responseCode is not 200");
            return authZResult;
        }

        Set<AuthorizationPolicyDecision> policyDecisions = authZResponse
                .getPolicyDecisions();

        for (AuthorizationPolicyDecision policyDecision : policyDecisions) {
            if (policyDecision.getResourceName().equals(resourceName)) {
                Map<String, ActionDecision> actionDecisions = policyDecision
                        .getActionDecisions();

                for (Map.Entry<String, ActionDecision> entry : actionDecisions
                        .entrySet()) {
                    // found the action name
                    if (entry.getKey().equals(actionName)) {
                        if (entry.getValue() == ActionDecision.PERMIT) {
                            authZResult = true;
                        } else {
                            authZResult = false;
                        }
                        return authZResult;
                    }
                }
            } else {
                continue;
            }
        }
        return authZResult;
    }

    private AuthorizationResponse simulatePDPResponse(Set<String> resourceNames) {
        Status responseCode = Status.OK;
        String responseUrl = "http://successful/site";
        Set<AuthorizationPolicyDecision> policyDecisions = new HashSet<AuthorizationPolicyDecision>();

        for (String resourceName : resourceNames) {

            Map<String, ActionDecision> actionDecisions = new HashMap<String, ActionDecision>();
            actionDecisions.put("read", ActionDecision.PERMIT);
            actionDecisions.put("write", ActionDecision.DENY);
            actionDecisions.put("execute", ActionDecision.DENY);

            Map<String, List<String>> attributeDecisions = new HashMap<String, List<String>>();
            attributeDecisions
                    .put("SSN", new LinkedList<>(Arrays.asList("NO")));
            attributeDecisions.put("Salary",
                    new LinkedList<>(Arrays.asList("NO")));
            attributeDecisions.put("Name",
                    new LinkedList<>(Arrays.asList("YES")));

            Map<String, List<String>> adviceDecisions = new HashMap<String, List<String>>();
            adviceDecisions.put("Authentication Level",
                    new LinkedList<>(Arrays.asList("3")));

            AuthorizationPolicyDecision policyDecision = new AuthZPolicyDecisionImpl(
                    resourceName, actionDecisions, attributeDecisions,
                    adviceDecisions);
            policyDecisions.add(policyDecision);

            actionDecisions = new HashMap<String, ActionDecision>();
            actionDecisions.put("create", ActionDecision.DENY);
            actionDecisions.put("update", ActionDecision.PERMIT);
            actionDecisions.put("delete", ActionDecision.DENY);
            actionDecisions.put("query", ActionDecision.PERMIT);

            attributeDecisions = new HashMap<String, List<String>>();
            attributeDecisions.put("VM Create",
                    new LinkedList<>(Arrays.asList("NO")));
            attributeDecisions.put("VM Query",
                    new LinkedList<>(Arrays.asList("YES")));
            attributeDecisions.put("VM Update",
                    new LinkedList<>(Arrays.asList("YES")));

            adviceDecisions = new HashMap<String, List<String>>();
            adviceDecisions.put("Authentication Level",
                    new LinkedList<>(Arrays.asList("2")));

            policyDecision = new AuthZPolicyDecisionImpl(resourceName,
                    actionDecisions, attributeDecisions, adviceDecisions);

            // add the decision for the resource #2.
            policyDecisions.add(policyDecision);
        }

        AuthorizationResponse authZResponse = new AuthorizationResponseImpl(
                responseCode, responseUrl, policyDecisions);
        return authZResponse;
    }

}
