package com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken;

import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.mfdd.um.accesscontrol.support.AccessControlFactoryImpl;
import com.jupiter.mfdd.um.accesscontrol.testtool.CallMethod;

public class DataGetMaxSessionTime implements CallMethod {
    @Override
    public void callMethod(SessionTokenID sessionTokenId) {
        try {
            System.out.println();
            System.out.println("getMaxSessionTime():");

            long rv = new AccessControlFactoryImpl().getAuthenticationService()
                    .getSessionToken(sessionTokenId).getMaxSessionTime();

            System.out.println("rv <" + rv + ">");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
