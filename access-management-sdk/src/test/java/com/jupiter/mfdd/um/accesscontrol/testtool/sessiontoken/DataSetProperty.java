package com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken;

import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.mfdd.um.accesscontrol.support.AccessControlFactoryImpl;
import com.jupiter.mfdd.um.accesscontrol.testtool.CallMethod;

public class DataSetProperty implements CallMethod {
    private String name;
    private String value;

    public String getName() {
        return name;
    }

    public String getValue() {
        return value;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setValue(String value) {
        this.value = value;
    }

    @Override
    public void callMethod(SessionTokenID sessionTokenId) {
        try {
            System.out.println();
            System.out.println("setProperty(" + name + ", " + value + "):");

            new AccessControlFactoryImpl().getAuthenticationService()
                    .getSessionToken(sessionTokenId).setProperty(name, value);

            System.out.println("rv <> (returns void)");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
