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

public class AuthorizationTest {

    public boolean isAuthorized(SessionTokenID sessionTokenId,
            String resourceName, String actionName) {
        boolean authDecision = false;
        AuthorizationService authZService = new AuthorizationServiceImpl();
        try {
            authDecision = authZService.isAuthorized(sessionTokenId,
                    resourceName, actionName);
        } catch (AuthXException e) {
            //
        }
        return authDecision;
    }

    public boolean isAuthorized(SessionTokenID sessionTokenId,
            String resourceName, String actionName,
            Map<String, List<String>> envParameters) {
        boolean authDecision = false;
        AuthorizationService authZService = new AuthorizationServiceImpl();
        try {
            authDecision = authZService.isAuthorized(sessionTokenId,
                    resourceName, actionName, envParameters);
        } catch (AuthXException e) {
            //
        }
        return authDecision;
    }

    public AuthorizationResponse authorize(SessionTokenID sessionTokenId,
            String resourceName, Set<String> actionNames,
            Map<String, List<String>> envParameters) {

        AuthorizationResponse authZResponse = null;
        AuthorizationService authZService = new AuthorizationServiceImpl();
        try {
            authZResponse = authZService.authorize(sessionTokenId,
                    resourceName, actionNames, envParameters);
        } catch (AuthXException e) {
            //
        }
        return authZResponse;
    }

    public AuthorizationResponse authorize(SessionTokenID sessionTokenId,
            Set<String> resourceNames, Set<String> actionNames,
            Map<String, List<String>> envParameters) {

        AuthorizationResponse authZResponse = null;
        AuthorizationService authZService = new AuthorizationServiceImpl();
        try {
            authZResponse = authZService.authorize(sessionTokenId,
                    resourceNames, actionNames, envParameters);
        } catch (AuthXException e) {
            //
        }
        return authZResponse;
    }

    public void analyzeResult(AuthorizationResponse authZResponse) {
        String resourceName;
        Map<String, ActionDecision> actionDecisions;
        Map<String, List<String>> attributeDecisions;
        Map<String, List<String>> adviceDecisions;

        if (authZResponse.getResponseCode() == Status.OK) {
            for (AuthorizationPolicyDecision apd : authZResponse
                    .getPolicyDecisions()) {
                resourceName = apd.getResourceName();
                System.out.println("Resource Name: " + resourceName);
                actionDecisions = apd.getActionDecisions();
                for (Map.Entry<String, ActionDecision> entry : actionDecisions
                        .entrySet()) {
                    System.out.println("Action : Value = " + entry.getKey()
                            + " : " + entry.getValue());
                }
                attributeDecisions = apd.getAttributeDecisions();
                System.out
                        .println("attributeDecisions : " + attributeDecisions);
                for (Map.Entry<String, List<String>> entry : attributeDecisions
                        .entrySet()) {
                    System.out.println("Attribute : Value = " + entry.getKey()
                            + " : " + entry.getValue());
                }
                adviceDecisions = apd.getAdviceDecisions();
                for (Map.Entry<String, List<String>> entry : adviceDecisions
                        .entrySet()) {
                    System.out.println("Advice : Value = " + entry.getKey()
                            + " : " + entry.getValue());
                }
            }
        } else {
            System.out.format("Authorization is failed: "
                    + authZResponse.getResponseCode());
        }
    }

    public static void main(String[] args) {
        AuthorizationTest authZTest = new AuthorizationTest();

        SessionTokenID sessionTokenID = new SessionTokenIDImpl(
                "AQIC5wM2LY4SfczntBbXvEAOuECbqMY3J4NW3byH6xwgkGE=@AAJTSQACMDE=#");
        String resourceName = "resource123";
        String actionName = "read";
        boolean result = authZTest.isAuthorized(sessionTokenID, resourceName,
                actionName);
        System.out.println("isAuthorized() result: " + result);
        System.out.println("\n");

        resourceName = "resource345";
        actionName = "write";
        Map<String, List<String>> envParameters = new HashMap<String, List<String>>();
        envParameters
                .put("VM-Max-Size", new LinkedList<>(Arrays.asList("500")));
        envParameters.put("VM-Max-Control-Plugin",
                new LinkedList<>(Arrays.asList("getMaxVMSize")));
        result = authZTest.isAuthorized(sessionTokenID, resourceName,
                actionName, envParameters);
        System.out.println("isAuthorized() result with env parameters: "
                + result);
        System.out.println("\n");

        resourceName = "resource345";
        envParameters = new HashMap<String, List<String>>();
        envParameters
                .put("VM-Max-Size", new LinkedList<>(Arrays.asList("500")));
        envParameters.put("VM-Max-Control-Plugin",
                new LinkedList<>(Arrays.asList("getMaxVMSize")));
        AuthorizationResponse authZResponse = authZTest.authorize(
                sessionTokenID, "resource345", null, envParameters);
        authZTest.analyzeResult(authZResponse);
        System.out.println("\n");

        Set<String> resourceNames = new HashSet<String>();
        resourceNames.add("resourceFirst");
        resourceNames.add("resourceSecond");
        envParameters = new HashMap<String, List<String>>();
        envParameters
                .put("VM-Max-Size", new LinkedList<>(Arrays.asList("500")));
        envParameters.put("VM-Max-Control-Plugin",
                new LinkedList<>(Arrays.asList("getMaxVMSize")));
        authZResponse = authZTest.authorize(sessionTokenID, resourceNames,
                null, envParameters);
        authZTest.analyzeResult(authZResponse);
    }

}
