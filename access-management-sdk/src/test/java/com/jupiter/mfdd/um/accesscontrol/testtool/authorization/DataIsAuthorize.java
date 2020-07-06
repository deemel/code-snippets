package com.jupiter.mfdd.um.accesscontrol.testtool.authorization;

import java.util.List;
import java.util.Map;

import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.mfdd.um.accesscontrol.support.AccessControlFactoryImpl;
import com.jupiter.mfdd.um.accesscontrol.testtool.CallMethod;

public class DataIsAuthorize implements CallMethod {
    private String resourceName;
    private String actionName;
    private Map<String, List<String>> envParameters;

    public String getResourceName() {
        return resourceName;
    }

    public String getActionName() {
        return actionName;
    }

    public Map<String, List<String>> getEnvParameters() {
        return envParameters;
    }

    public void setResourceName(String resourceName) {
        this.resourceName = resourceName;
    }

    public void setActionName(String actionName) {
        this.actionName = actionName;
    }

    public void setEnvParameters(Map<String, List<String>> envParameters) {
        this.envParameters = envParameters;
    }

    @Override
    public void callMethod(SessionTokenID sessionTokenId) {
        try {
            boolean rv;

            System.out.println();
            if (envParameters == null) {
                System.out.println("isAuthorize(<sessionTokenId>, "
                        + resourceName + ", " + actionName + "):");

                rv = new AccessControlFactoryImpl().getAuthorizationService()
                        .isAuthorized(sessionTokenId, resourceName, actionName);
            } else {
                System.out.println("isAuthorize(<sessionTokenId>, "
                        + resourceName + ", " + actionName + ", "
                        + envParameters + "):");

                rv = new AccessControlFactoryImpl().getAuthorizationService()
                        .isAuthorized(sessionTokenId, resourceName, actionName,
                                envParameters);
            }

            System.out.println("rv <" + rv + ">");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
