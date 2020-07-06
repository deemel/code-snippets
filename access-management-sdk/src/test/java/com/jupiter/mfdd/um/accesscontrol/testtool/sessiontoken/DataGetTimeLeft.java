package com.jupiter.mfdd.um.accesscontrol.testtool.sessiontoken;

import com.jupiter.boss.sdk.accesscontrol.SessionTokenID;
import com.jupiter.mfdd.um.accesscontrol.support.AccessControlFactoryImpl;
import com.jupiter.mfdd.um.accesscontrol.testtool.CallMethod;

public class DataGetTimeLeft implements CallMethod {
    @Override
    public void callMethod(SessionTokenID sessionTokenId) {
        try {
            System.out.println();
            System.out.println("getTimeLeft():");

            long rv = new AccessControlFactoryImpl().getAuthenticationService()
                    .getSessionToken(sessionTokenId).getTimeLeft();

            System.out.println("rv <" + rv + ">");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
