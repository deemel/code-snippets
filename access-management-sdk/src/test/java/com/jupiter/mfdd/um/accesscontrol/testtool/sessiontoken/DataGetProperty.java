package com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken;

import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.mfdd.um.accesscontrol.support.AccessControlFactoryImpl;
import com.jupiter.mfdd.um.accesscontrol.testtool.CallMethod;

public class DataGetProperty implements CallMethod {
    private String name;
    private Boolean ignoreState;

    public String getName() {
        return name;
    }

    public Boolean isIgnoreState() {
        return ignoreState;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setIgnoreState(Boolean ignoreState) {
        this.ignoreState = ignoreState;
    }

    @Override
    public void callMethod(SessionTokenID sessionTokenId) {
        try {
            String rv;

            System.out.println();
            if (ignoreState == null) {
                System.out.println("getProperty(" + name + "):");

                rv = new AccessControlFactoryImpl().getAuthenticationService()
                        .getSessionToken(sessionTokenId).getProperty(name);
            } else {
                System.out.println("getProperty(" + name + ", " + ignoreState
                        + "):");

                rv = new AccessControlFactoryImpl().getAuthenticationService()
                        .getSessionToken(sessionTokenId)
                        .getProperty(name, ignoreState);
            }

            System.out.println("rv <" + rv + ">");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
