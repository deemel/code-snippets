package com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken;

import java.security.Principal;

import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.mfdd.um.accesscontrol.support.AccessControlFactoryImpl;
import com.jupiter.mfdd.um.accesscontrol.testtool.CallMethod;

public class DataGetPrincipal implements CallMethod {
    @Override
    public void callMethod(SessionTokenID sessionTokenId) {
        try {
            System.out.println();
            System.out.println("getPrincipal():");

            Principal rv = new AccessControlFactoryImpl()
                    .getAuthenticationService().getSessionToken(sessionTokenId)
                    .getPrincipal();

            System.out.println("rv <" + rv + ">");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
