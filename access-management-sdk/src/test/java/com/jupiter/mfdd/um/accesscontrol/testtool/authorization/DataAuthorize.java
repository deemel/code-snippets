package com.jupiter.mfdd.um.accesscontrol.testtool.authorization;

import java.util.List;
import java.util.Map;
import java.util.Set;

import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.boss.sdk.accesscontrol.authorization.AuthorizationResponse;
import com.jupiter.mfdd.um.accesscontrol.support.AccessControlFactoryImpl;
import com.jupiter.mfdd.um.accesscontrol.testtool.CallMethod;

public class DataAuthorize implements CallMethod {
    private String resourceName;
    private Set<String> resourceNames;
    private Set<String> actionNames;
    private Map<String, List<String>> envParameters;

    public String getResourceName() {
        return resourceName;
    }

    public Set<String> getResourceNames() {
        return resourceNames;
    }

    public Set<String> getActionNames() {
        return actionNames;
    }

    public Map<String, List<String>> getEnvParameters() {
        return envParameters;
    }

    public void setResourceName(String resourceName) {
        this.resourceName = resourceName;
    }

    public void setResourceNames(Set<String> resourceNames) {
        this.resourceNames = resourceNames;
    }

    public void setActionNames(Set<String> actionNames) {
        this.actionNames = actionNames;
    }

    public void setEnvParameters(Map<String, List<String>> envParameters) {
        this.envParameters = envParameters;
    }

    @Override
    public void callMethod(SessionTokenID sessionTokenId) {
        try {
            AuthorizationResponse rv;

            System.out.println();
            if (resourceName != null) {
                System.out.println("authorize(<sessionTokenId>, "
                        + resourceName + ", " + actionNames + ", "
                        + envParameters + "):");

                rv = new AccessControlFactoryImpl().getAuthorizationService()
                        .authorize(sessionTokenId, resourceName, actionNames,
                                envParameters);
            } else {
                System.out.println("authorize(<sessionTokenId>, "
                        + resourceNames + ", " + actionNames + ", "
                        + envParameters + "):");

                rv = new AccessControlFactoryImpl().getAuthorizationService()
                        .authorize(sessionTokenId, resourceNames, actionNames,
                                envParameters);
            }

            System.out.println("rv <" + rv + ">");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
